//
// Created by zhy on 2/20/17.
//
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include "helper.h"

//内核监听队列的最大长度
#define BACKLOG 1024


int main(int argc, char *argv[])
{
    signal(SIGINT, sig_int);
    if (argc != 3) {
        usage();
//        exit(1);
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
    port = (unsigned short)atoi("8080");

    memset(&addr_s, 0, sizeof(addr_c));
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_s.sin_port = htons(port);

    int ret = bind(server_fd, (struct sockaddr*)&addr_s, sizeof(addr_c));
    if(ret == -1) {
        perror("bind :");
        close(server_fd);
        exit(1);
    }

    fd_set readfd;
    fd_set readfd_bak;
    FD_ZERO(&readfd);
    FD_ZERO(&readfd_bak);
    FD_SET(server_fd, &readfd_bak);
    struct timeval timeout;
    int maxfd = server_fd;
    int addr_clen = sizeof(addr_c);
    if(listen(server_fd, BACKLOG) == -1) {
        perror("listen : ");
        exit(1);
    }
    while(1) {
        readfd = readfd_bak;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int res = select(maxfd + 1, &readfd, NULL, NULL, &timeout);
        if(res < 0) {
            perror("select : ");
            exit(1);
        }
        else if(res == 0)
            continue;
        else {
            for (int i = 0; i < maxfd + 1; ++i) {
                if(FD_ISSET(i, &readfd)) {
                    if(i == server_fd) {
                        int newClient = accept(server_fd, NULL, 0);
                        if(newClient > maxfd) {
                            maxfd = newClient;
                        }
                        FD_SET(newClient, &readfd_bak);
                    } else {
                        response(i);
                        FD_CLR(i, &readfd_bak);
                    }
                } else {
                    continue;
                }
            }
        }
    }
    return 0;
}
