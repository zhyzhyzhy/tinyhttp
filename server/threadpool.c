//
// Created by 朱逸尘 on 2017/7/31.
//

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"
#include "log.h"
#include "mempool.h"
#include "handle.h"

/**
 * the work method that threads do
 * @param arg  the threadpool_t point
 * @return  NULL
 */
void* work(void *arg) {
    threadpool_t *pool = (threadpool_t*)arg;
    for(;;) {
        //lock
        pthread_mutex_lock(pool->lock);
        //if job_count == 0, then wait
        while(pool->job_count == 0) {
            pthread_cond_wait(pool->cond, pool->lock);
            //if the server is shutting down then exit
            if (pool->shutdown == 1) {
                pthread_mutex_unlock(pool->lock);
                pthread_exit(arg);
            }
        }
        thread_job *job = pool->job_head;
        pool->job_head = pool->job_head->next;
        pool->job_count--;

        //unlock
        pthread_mutex_unlock(pool->lock);
        //process the action
        job->func(job->arg);
        mfree(job);

        if (pool->shutdown == 1) {
            break;
        }
    }
    return NULL;
}
/**
 *
 * @param thread_num  the num of threads in the thread pool
 * @return            one thread pool
 */
threadpool_t* threadpool_init(int thread_num) {

    log_info("init thread pool with thread num %d", thread_num);

    threadpool_t *pool = (threadpool_t*)malloc(sizeof(threadpool_t));
    if (pool == NULL) {
        log_err("can not malloc for thread pool\nexiting...");
        exit(1);
    }
    pool->pthreads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    if (pool->pthreads == NULL) {
        log_err("can not malloc for threads\nexiting...");
        exit(1);
    }
    pool->job_head = (thread_job*)malloc(sizeof(thread_job));
    pool->job_count = 0;
    pool->thread_count = thread_num;
    pool->shutdown = 0;
    pool->cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    pool->lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

    if (pool->cond == NULL || pool->lock == NULL) {
        log_err("can not malloc for lock!\nexiting...");
        exit(1);
    }

    pthread_mutex_init(pool->lock, NULL);
    pthread_cond_init(pool->cond, NULL);
    for (int i = 0; i < thread_num; i++) {
        pthread_create(&(pool->pthreads)[i], NULL, work, pool);
    }
    return pool;
}

/**
 * add job into the thread pool
 * @param pool the target thread pool
 * @param job  the thread_job need to add to pool
 */
void add_job(threadpool_t *pool, thread_job* job) {
    pthread_mutex_lock(pool->lock);
    job->next = pool->job_head;
    pool->job_head = job;
    pool->job_count++;
    pthread_mutex_unlock(pool->lock);
    pthread_cond_signal(pool->cond);
}

/**
 * stop the threads in target thread pool and clear the resource
 * @param pool the thread pool that need to destroy
 */
void threadpool_destroy(threadpool_t *pool) {
    //set thread pool state to shutdown
    pool->shutdown = 1;
    pthread_cond_broadcast(pool->cond);
    for (int i = 0; i < pool->thread_count; i++) {
        debug("thread %d join", i);
        pthread_join((pool->pthreads)[i], NULL);
    }
    free(pool->lock);
    free(pool->cond);
    free(pool->pthreads);
    //free all waiting job
    thread_job* head = pool->job_head;
    while (head != NULL) {
        thread_job* p = head->next;
        struct http_request *request = (struct http_request*)head->arg;
        if (request != NULL) {
            close(request->connfd);
            mfree(request);
        }
        mfree(head);
        head= p;
    }
    free(pool);
}