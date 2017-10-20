//
// Created by zhy on 2/20/17.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <signal.h>
#include "mysocket.h"
#include "log.h"
#include "handle.h"
#include "sig.h"
#include "threadpool.h"
#include "subreactor.h"
#include "config.h"
#include "mempool.h"

int listen_fd;
struct event_base *base;
int *notify;
libevent_reactor_t *libevent_reactors;
threadpool_t *threadpool;
server_config config;
int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sig);
    signal(SIGKILL, sig);
    signal(SIGTERM, sig);

    mempool_init(20, 2, sizeof(struct http_request), sizeof(job));

    read_config("/Users/zhuyichen/fortest/config.txt", &config);

    log_info("index_file_name: %s", config.index_file_name);
    log_info("target_dir: %s", config.target_dir);
    log_info("thread_num: %d", config.thread_num);
    log_info("sub_reactor: %d", config.sub_reactor);
    log_info("listen_host: %s", config.host);
    log_info("listen_post: %s", config.port);


    //change the dir
    if (chdir(config.target_dir) == -1) {
        log_err_by_errno;
        exit(1);
    }

    //bind and listen
    listen_fd = socket_bind_listen(config.host, config.port);
    set_no_blocking(listen_fd);

    //init threadpool
    threadpool = threadpool_init(config.thread_num);

    log_info("start listen in host %s port %s ...", config.host, config.port);

    //main reactor
    base = event_base_new();
    libevent_reactors_init(config.sub_reactor, base);
    struct event* ev_listen = event_new(base, listen_fd, EV_READ | EV_PERSIST, on_accept, NULL);
    event_base_set(base, ev_listen);
    event_add(ev_listen, NULL);


    event_base_dispatch(base);
    event_base_free(base);
    event_free(ev_listen);

}
