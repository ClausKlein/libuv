#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define DEFAULT_PORT 9123
#define DEFAULT_BACKLOG 128
#define DEFAULT_TIMEOUT 10000
#define USE_TIMER


typedef struct {
    uv_timer_t *timer_handle;
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t *)req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(
    uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = malloc(suggested_size);
    buf->len  = suggested_size;
}

void on_close(uv_handle_t *handle) {
#ifdef USE_TIMER
    if (handle->data) {
        uv_timer_t *timer_handle = handle->data;
        if ((timer_handle->type == UV_TIMER)
            && (uv_is_active((uv_handle_t *)timer_handle))) {
            uv_timer_stop(timer_handle);
            uv_close((uv_handle_t *)timer_handle, on_close);
        }
        timer_handle->data = NULL;
    }
#endif
    free(handle);
}

void on_timeout(uv_timer_t *handle) {
    uv_buf_t buf;
    buf.base            = "\0";
    buf.len             = 0;
    uv_stream_t *client = (uv_stream_t *)handle->data;
    int r               = uv_try_write(client, &buf, 1);
    if (r < 0) {
        fprintf(
            stderr, "on_timeout() uv_try_write error: %s\n", uv_err_name(r));
        if (uv_is_active((uv_handle_t *)client)) {
            uv_close((uv_handle_t *)client, on_close);
        }
    }
}

void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    } else {
#ifdef USE_TIMER
        uv_timer_t *timer_handle = ((write_req_t *)req)->timer_handle;
        uv_timer_again(timer_handle);
#endif
    }
    free_write_req(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        write_req_t *req  = malloc(sizeof(write_req_t));
        req->buf          = uv_buf_init(buf->base, nread);
        req->timer_handle = client->data;
        uv_write((uv_write_t *)req, client, &req->buf, 1, echo_write);
        return;

    } else if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t *)client, on_close);
    }

    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(server->loop, client);
    if (uv_accept(server, (uv_stream_t *)client) == 0) {
        uv_read_start((uv_stream_t *)client, alloc_buffer, echo_read);
#ifdef USE_TIMER
        uv_timer_t *timer_handle = malloc(sizeof(uv_timer_t));
        client->data             = timer_handle;
        timer_handle->data       = client;
        uv_timer_init(server->loop, timer_handle);
        uv_timer_start(
            timer_handle, on_timeout, DEFAULT_TIMEOUT, DEFAULT_TIMEOUT);
#endif
        uv_tcp_keepalive(client, 1, DEFAULT_TIMEOUT);
    } else {
        // error!
        uv_close((uv_handle_t *)client, on_close);
    }
}

int main() {
    uv_loop_t *loop = uv_default_loop();
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", DEFAULT_PORT, &addr);

    uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
    int r = uv_listen(
        (uv_stream_t *)&server, DEFAULT_BACKLOG, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
