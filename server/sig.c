//
// Created by zhy on 2/21/17.
//

#include "sig.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int listen_fd;
void sig(int sig) {
    close(listen_fd);
    printf("\nbye\n");
    exit(0);
}
