//
// Created by 朱逸尘 on 2017/7/31.
//

#ifndef TINYHTTP_THREADPOOL_H
#define TINYHTTP_THREADPOOL_H

#include <pthread.h>

struct thread_job {
    void (*func)(void *arg);
    void *arg;
    struct thread_job* next;
};

typedef struct thread_job thread_job;

struct threadpool_t {
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
    int job_count;
    thread_job* job_head;
    thread_job* job_tail;
    pthread_t *pthreads;
    int thread_count;
    int shutdown;
};
typedef struct threadpool_t threadpool_t;

threadpool_t* threadpool_init(int thread_num);
void add_job(threadpool_t *pool, thread_job* job);
void threadpool_destroy(threadpool_t *pool);

#endif //TINYHTTP_THREADPOOL_H
