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
#include "mempool.h"

extern int *notify;
int count;
extern libevent_reactor_t *libevent_reactors;
void on_notify(int notify_fd, short events, void *arg) {
    libevent_reactor_t *libevent_thread = (libevent_reactor_t*)arg;
    int conn_fd;
    if (recv(notify_fd, &conn_fd, sizeof(int), 0) > 0) {
        struct http_request *request = (struct http_request *) mmalloc(sizeof(struct http_request));
        struct event *pread = event_new(libevent_thread->base, conn_fd, EV_READ | EV_PERSIST, on_read, request);

        struct event* timeout = evtimer_new(libevent_thread->base, on_timeout, request);

        struct timeval* tv = malloc(sizeof(struct tv));
        tv->tv_sec = 20*60;
        tv->tv_usec = 0;
        evtimer_add(timeout, tv);

        request->timeout = timeout;
        request->tv = tv;

        request->pread = pread;
        request->base = libevent_thread->base;
        request->connfd = conn_fd;

        event_set(pread, conn_fd, EV_READ | EV_PERSIST, on_read, request);
        event_base_set(libevent_thread->base, pread);
        event_add(pread, NULL);
    }
}

void* start_dispatch(void *arg) {
    libevent_reactor_t *libevent_thread = (libevent_reactor_t* )arg;
    libevent_thread->base = event_base_new();
    libevent_thread->event = event_new(libevent_thread->base, libevent_thread->read_fd, EV_READ | EV_PERSIST, on_notify, libevent_thread);
    event_base_set(libevent_thread->base, libevent_thread->event);
    event_add(libevent_thread->event, NULL);
    event_base_dispatch(libevent_thread->base);
    return (char*)NULL;
}

int libevent_reactors_init(int nreactor, struct event_base *main_base) {
    libevent_reactors = malloc(sizeof(libevent_reactor_t) * nreactor);
    notify = malloc(sizeof(int) * nreactor);
    for (int i = 0; i < nreactor; i++) {
        int fd[2];
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        libevent_reactor_t *libevent_thread = &(libevent_reactors[i]);
        libevent_thread->read_fd = fd[0];
        notify[i] = fd[1];
        pthread_t t;
        pthread_create(&t, NULL, start_dispatch, libevent_thread);
    }
    return 1;
}
