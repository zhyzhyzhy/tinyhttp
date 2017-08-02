//
// Created by zhy on 2/21/17.
//

#include "sig.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int server_fd;
void sig_int(int sig) {
    close(server_fd);
    printf("\nbye\n");
    exit(0);
}
