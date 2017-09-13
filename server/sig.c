//
// Created by zhy on 2/21/17.
//

#include "sig.h"
#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int listen_fd;
extern threadpool_t *threadpool;
void sig(int sig) {
    close(listen_fd);
    threadpool->shutdown = 1;
    printf("\nbye\n");
    pthread_cond_broadcast(threadpool->cond);
    for (int i = 0; i < threadpool->thread_count; i++) {
        printf("i join\n");
        pthread_join((threadpool->pthreads)[i], NULL);
    }
    free(threadpool->lock);
    free(threadpool->cond);
    free(threadpool->job_head);
    free(threadpool->pthreads);
    free(threadpool);
    exit(0);
}
