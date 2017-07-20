//
// Created by zhy on 2/20/17.
//
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include "helper.h"
#include <sys/epoll.h>
#include <string.h>

//内核监听队列的最大长度
#define BACKLOG 1024


int main(int argc, char *argv[])
{
    signal(SIGINT, sig_int);
    if (argc != 3) {
        usage();
        exit(1);
    }

    int server_fd, conn_fd;
    unsigned short port;
    struct sockaddr_in addr_s;
    struct sockaddr_in addr_c;
    //AF_INET ipv4
    //SOCK_STREAM 以流的方式传递
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket :");
        exit(1);
    }

    //get port
    port = (unsigned short)atoi(argv[2]);

    memset(&addr_s, 0, sizeof(addr_c));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr(argv[1]);
    addr_s.sin_port = htons(port);

    int ret = bind(server_fd, (struct sockaddr*)&addr_s, sizeof(addr_c));
    if(ret == -1) {
        perror("bind :");
        exit(1);
    }

    if(listen(server_fd, BACKLOG) == -1) {
        perror("listen : ");
        exit(1);
    }

    int epollfd = epoll_create(10);

    struct epoll_event events[1024];
    memset(events, 0, sizeof(events));

    struct epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &event);

    while(1) {
        int ret = epoll_wait(epollfd, events, 1024, 2);
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
                        int newClientfd = accept(server_fd, NULL,0);
                        struct epoll_event event;
                        event.data.fd = newClientfd;
                        event.events = EPOLLIN;
                        epoll_ctl(epollfd, EPOLL_CTL_ADD, newClientfd, &event);
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
