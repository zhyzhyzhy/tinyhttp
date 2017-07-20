//
// Created by zhy on 2/12/17.
//

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void usage() {
    fprintf(stderr, "usage :: WebServer host port index_home\n");
}
