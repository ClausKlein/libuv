include_directories(${PROJECT_SOURCE_DIR}/include)

if(NOT WIN32)
    add_executable(tick code/cgi/tick.c)
    add_executable(cgi code/cgi/main.c)
    target_link_libraries(cgi uv)
endif()

add_executable(detach code/detach/main.c)
target_link_libraries(detach uv)

add_executable(dns code/dns/main.c)
target_link_libraries(dns uv)

add_executable(helloworld code/helloworld/main.c)
target_link_libraries(helloworld uv)

add_executable(idle-basic code/idle-basic/main.c)
target_link_libraries(idle-basic uv)

add_executable(idle-compute code/idle-compute/main.c)
target_link_libraries(idle-compute uv)

if(NOT WIN32)
    add_executable(interfaces code/interfaces/main.c)
    target_link_libraries(interfaces uv)
endif()

add_executable(locks code/locks/main.c)
target_link_libraries(locks uv)

add_executable(worker code/multi-echo-server/worker.c)
target_link_libraries(worker uv)
add_executable(multi-echo-server code/multi-echo-server/main.c)
target_link_libraries(multi-echo-server uv)

add_executable(onchange code/onchange/main.c)
target_link_libraries(onchange uv)

add_executable(pipe-echo-server code/pipe-echo-server/main.c)
target_link_libraries(pipe-echo-server uv)

add_executable(plugin code/plugin/plugin.h code/plugin/hello.c code/plugin/main.c)
target_link_libraries(plugin uv)

add_executable(proc-streams code/proc-streams/main.c)
target_link_libraries(proc-streams uv)

if(NOT WIN32)
    add_executable(progress code/progress/main.c)
    target_link_libraries(progress uv)

    add_executable(queue-cancel code/queue-cancel/main.c)
    target_link_libraries(queue-cancel uv)

    add_executable(queue-work code/queue-work/main.c)
    target_link_libraries(queue-work uv)
endif()

add_executable(ref-timer code/ref-timer/main.c)
target_link_libraries(ref-timer uv)

if(NOT WIN32)
    add_executable(signal code/signal/main.c)
    target_link_libraries(signal uv)
endif()

add_executable(spawn code/spawn/main.c)
target_link_libraries(spawn uv)

add_executable(tcp-echo-server code/tcp-echo-server/main.c)
target_link_libraries(tcp-echo-server uv)

if(NOT WIN32)
    add_executable(thread-create code/thread-create/main.c)
    target_link_libraries(thread-create uv)

    add_executable(tty code/tty/main.c)
    target_link_libraries(tty uv)

    add_executable(tty-gravity code/tty-gravity/main.c)
    target_link_libraries(tty-gravity uv)

    add_executable(udp-dhcp code/udp-dhcp/main.c)
    target_link_libraries(udp-dhcp uv)
endif()

add_executable(uvcat code/uvcat/main.c)
target_link_libraries(uvcat uv)

add_executable(uvstop code/uvstop/main.c)
target_link_libraries(uvstop uv)

add_executable(uvtee code/uvtee/main.c)
target_link_libraries(uvtee uv)

find_package(CURL)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_executable(uvget code/uvwget/main.c)
    target_link_libraries(uvget uv ${CURL_LIBRARIES})
endif()
