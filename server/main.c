//
// Created by zhy on 2/20/17.
//
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include "handle.h"
#include "sig.h"
#include "mysocket.h"
#include "helper.h"

#define MAXEPOLL 1024
char* index_home = "";
int server_fd;

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_int);
    signal(SIGKILL, sig_int);
    signal(SIGTERM, sig_int);

    if (argc != 4) {
        usage();
        exit(1);
    }


    index_home = argv[3];
    printf(index_home);
    if (chdir(index_home) == -1) {
        perror("index_home : ");
        exit(1);
    }

    server_fd = socket_bind_listen(argv[1], argv[2]);
    if(server_fd == -1) {
        exit(1);
    }

    set_no_blocking(server_fd);

    int epollfd = epoll_create(10);
    struct epoll_event events[MAXEPOLL];
    memset(events, 0, sizeof(events));

    struct epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &event);

    while(1) {
        int ret = epoll_wait(epollfd, events, MAXEPOLL, -1);
        if(ret == -1) {
            perror("ret : ");
            exit(1);
        } else if(ret == 0) {
            continue;
        }
        else {
            for (int i = 0; i < ret; ++i) {
                if(events[i].events & EPOLLIN) {
                    if(events[i].data.fd == server_fd) {
                        while(1) {
                            int newClientfd = accept(server_fd, NULL,0);
                            if (newClientfd < 0) {
                                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                                    break;
                                } else {
                                    exit(3);
                                }
                            }
                            set_no_blocking(newClientfd);
                            struct epoll_event event;
                            event.data.fd = newClientfd;
                            event.events = EPOLLIN;
                            epoll_ctl(epollfd, EPOLL_CTL_ADD, newClientfd, &event);
                        }
                    } else {
                        response(events[i].data.fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                    }
                } else {
                    continue;
                }
            }
        }
    }
}
