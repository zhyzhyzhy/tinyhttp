//
// Created by zhy on 2/21/17.
//

#include "sig.h"
#include "threadpool.h"
#include "log.h"
#include "subreactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

extern int listen_fd;
extern threadpool_t *threadpool;

/**
 * response for the signal SIGPIPE SIGINT SIGKILL SIGTERM
 * free the memory pool and thread pool and close the listen fd
 * @param sig
 */
void sig(int sig) {
    //close the listen fd
    close(listen_fd);

    //clear thread pool
    threadpool_destroy(threadpool);

    //clear sub reactors
    libevent_reactors_destroy();
    //log bye!
    log_info("\nbye\n");
    exit(0);
}
