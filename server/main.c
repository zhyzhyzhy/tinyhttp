//
// Created by zhy on 2/20/17.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include "mysocket.h"
#include "log.h"
#include "handle.h"
#include "sig.h"
#include "helper.h"

#define MAXEPOLL 1024
char* index_home = "";
int server_fd;
struct event_base *base;

int count = 0;
int main(int argc, char *argv[])
{
    signal(SIGINT, sig_int);
    signal(SIGKILL, sig_int);
    signal(SIGTERM, sig_int);
    signal(SIGPIPE, SIG_IGN);

    if (argc != 4) {
        usage();
        exit(1);
    }

//    argv[1] = "127.0.0.1";
//    argv[2] = "4000";
//    argv[3] = "/Users/zhuyichen/fortest/tinydemo/v3.bootcss.com/";
    index_home = argv[3];
    if (chdir(index_home) == -1) {
        perror("index_home : ");
        exit(1);
    }

    log_info("index home : %s\n", index_home);

    int listener;
    listener = socket_bind_listen(argv[1], argv[2]);
    if (listener == -1) {
        exit(1);
    }

    set_no_blocking(listener);
    log_info("start listen in host %s port %s ...\n", argv[1], argv[2]);

    base = event_base_new();

    struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, on_accept, NULL);

    event_base_set(base, ev_listen);
    event_add(ev_listen, NULL);
    event_base_dispatch(base);
    event_base_free(base);

}
