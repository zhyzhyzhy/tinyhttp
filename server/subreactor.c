//
// Created by 朱逸尘 on 2017/9/11.
//

#include "subreactor.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <event.h>
#include "threadpool.h"
#include "handle.h"

extern int *notify;
int count;
extern libevent_thread_t *libevent_threads;
void on_notify(int notify_fd, short events, void *arg) {
    libevent_thread_t *libevent_thread = (libevent_thread_t*)arg;
    int conn_fd;
    if (recv(notify_fd, &conn_fd, sizeof(int), 0) > 0) {
        struct http_request *request = (struct http_request *) malloc(sizeof(struct http_request));
        struct event *pread = event_new(libevent_thread->base, conn_fd, EV_READ | EV_PERSIST, on_read, request);

//        request->pwrite = event_new(libevent_thread->base, conn_fd, EV_WRITE, on_write, request);
        request->pread = pread;
        request->base = libevent_thread->base;
        request->connfd = conn_fd;

        event_set(pread, conn_fd, EV_READ | EV_PERSIST, on_read, request);
        event_base_set(libevent_thread->base, pread);
        event_add(pread, NULL);
    }
}

void* start_dispatch(void *arg) {
    libevent_thread_t *libevent_thread = (libevent_thread_t*)arg;
    libevent_thread->base = event_base_new();
    libevent_thread->event = event_new(libevent_thread->base, libevent_thread->read_fd, EV_READ | EV_PERSIST, on_notify, libevent_thread);
    event_base_set(libevent_thread->base, libevent_thread->event);
    event_add(libevent_thread->event, NULL);
    event_base_dispatch(libevent_thread->base);
    return "vvv";
}

int libevent_threadpool_init(int nthread, struct event_base *main_base) {
    libevent_threads = malloc(sizeof(libevent_thread_t) * nthread);
    notify = malloc(sizeof(int) * nthread);
    for (int i = 0; i < nthread; i++) {
        int fd[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        libevent_thread_t *libevent_thread = &(libevent_threads[i]);
        libevent_thread->read_fd = fd[0];
        notify[i] = fd[1];
        pthread_t t;
        pthread_create(&t, NULL, start_dispatch, libevent_thread);
    }
    return 1;
}
