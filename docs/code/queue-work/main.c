#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Attention: sleep() used! CK

#include <uv.h>

#define FIB_UNTIL 25
uv_loop_t *loop;

size_t fib_(size_t t) {
    if (t == 0 || t == 1)
        return 1;
    else
        return fib_(t-1) + fib_(t-2);
}

void fib(uv_work_t *req) {
    size_t n = *(size_t *) req->data;
    if (random() % 2)
        sleep(1);
    else
        sleep(2);
    size_t fib = fib_(n);
    fprintf(stderr, "%zuth fibonacci is %zu\n", n, fib);
}

void after_fib(uv_work_t *req, int status) {
    fprintf(stderr, "Done calculating %zuth fibonacci\n", *(size_t *) req->data);
}

int main() {
    loop = uv_default_loop();

    size_t data[FIB_UNTIL];
    uv_work_t req[FIB_UNTIL];
    size_t i;
    for (i = 0; i < FIB_UNTIL; i++) {
        data[i] = i;
        req[i].data = (void *) &data[i];
        uv_queue_work(loop, &req[i], fib, after_fib);
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
