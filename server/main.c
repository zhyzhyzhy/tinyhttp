//
// Created by zhy on 2/20/17.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include "mysocket.h"
#include "log.h"
#include "handle.h"
#include "sig.h"
#include "threadpool.h"
#include "subreactor.h"

char* index_home = "";
int listen_fd;
struct event_base *base;
int *notify;
libevent_thread_t *libevent_threads;
int main(int argc, char *argv[])
{
    signal(SIGINT, sig);
    signal(SIGKILL, sig);
    signal(SIGTERM, sig);
    signal(SIGPIPE, SIG_IGN);

//    if (argc != 4) {
//        usage();
//        exit(1);
//    }

    argv[1] = "127.0.0.1";
    argv[2] = "4001";
    argv[3] = "/Users/zhuyichen/fortest/tinydemo/v3.bootcss.com/";

    //change the dir
    char* index_home = argv[3];
    log_info("index home : %s", index_home);
    if (chdir(index_home) == -1) {
        log_err_by_errno;
        exit(1);
    }

    listen_fd = socket_bind_listen(argv[1], argv[2]);

    set_no_blocking(listen_fd);


    log_info("start listen in host %s port %s ...", argv[1], argv[2]);


    base = event_base_new();
    libevent_threadpool_init(8, base);
    struct event* ev_listen = event_new(base, listen_fd, EV_READ | EV_PERSIST, on_accept, NULL);
    event_base_set(base, ev_listen);
    event_add(ev_listen, NULL);

    event_base_dispatch(base);

    event_base_free(base);
    event_free(ev_listen);

}
