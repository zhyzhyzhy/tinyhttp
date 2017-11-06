//
// Created by 朱逸尘 on 2017/7/31.
//

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "threadpool.h"
#include "log.h"
#include "mempool.h"


void* work(void *arg) {
    threadpool_t *pool = (threadpool_t*)arg;

    while (1) {
        pthread_mutex_lock(pool->lock);
        while(pool->job_count == 0) {
            pthread_cond_wait(pool->cond, pool->lock);
            if (pool->shutdown == 1) {
                pthread_mutex_unlock(pool->lock);
                pthread_exit(arg);
            }
        }
        job *job1 = pool->job_head;
        pool->job_head = pool->job_head->next;
        pool->job_count--;

        pthread_mutex_unlock(pool->lock);

        job1->func(job1->arg);

        if (pool->shutdown == 1) {
            break;
        }

        mfree(job1);
    }
}

threadpool_t* threadpool_init(int thread_num) {

    log_info("init threadpool with thread num %d", thread_num);

    threadpool_t *pool = (threadpool_t*)malloc(sizeof(threadpool_t));

    pool->pthreads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    pool->job_head = (job*)malloc(sizeof(job));
    pool->job_count = 0;
    pool->thread_count = thread_num;
    pool->shutdown = 0;
    pool->cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    pool->lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(pool->lock, NULL);
    pthread_cond_init(pool->cond, NULL);


    for (int i = 0; i < thread_num; i++) {
        pthread_create(&(pool->pthreads)[i], NULL, work, pool);
    }
    return pool;
}

void add_job(threadpool_t *pool, job* job) {
    pthread_mutex_lock(pool->lock);
    job->next = pool->job_head;
    pool->job_head = job;
    pool->job_count++;
    pthread_mutex_unlock(pool->lock);
    pthread_cond_signal(pool->cond);
}

void thread_stop(threadpool_t *pool) {

}