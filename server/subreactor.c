//
// Created by 朱逸尘 on 2017/9/11.
//

#include "subreactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <event.h>
#include <event2/thread.h>
#include <unistd.h>
#include "handle.h"
#include "mempool.h"
#include "log.h"

extern int *notify;
extern libevent_reactor_t *libevent_reactors;

//to save the num of sub_reactors for clear
static int sub_reactors;
/**
 * the callback in the handle#on_accept#non_blocking_write
 * @param notify_fd the fd that receive connect fd
 * @param events    useless
 * @param arg       the libevent_reactor_t point
 */
void on_notify(int notify_fd, short events, void *arg) {

    libevent_reactor_t *libevent_thread = (libevent_reactor_t*)arg;
    int conn_fd;
    if (recv(notify_fd, &conn_fd, sizeof(int), 0) > 0) {
        struct http_request *request = (struct http_request *) mmalloc(sizeof(struct http_request));
        struct event *pread = event_new(libevent_thread->base, conn_fd, EV_READ | EV_PERSIST | EV_ET, on_read, request);

        //set the timeout event util 20 mins if the target fd is silent
        struct event* timeout = evtimer_new(libevent_thread->base, on_timeout, request);
        struct timeval* tv = mmalloc(sizeof(struct timeval));
        tv->tv_sec = 20*60;
        tv->tv_usec = 0;
        evtimer_add(timeout, tv);

        request->timeout = timeout;
        request->tv = tv;

        request->pread = pread;
        request->base = libevent_thread->base;
        request->connfd = conn_fd;

        // event_set(pread, conn_fd, EV_READ | EV_PERSIST | EV_ET, on_read, request);
        event_base_set(libevent_thread->base, pread);
        event_add(pread, NULL);
    }
}

/**
 * create subReactor and start dispatch
 * @param arg  libevent_reactor_t point
 * @return NULL
 */
void* start_dispatch(void *arg) {
    libevent_reactor_t *libevent_thread = (libevent_reactor_t* )arg;
    //new one base_event
    evthread_use_pthreads();
    libevent_thread->base = event_base_new();
    libevent_thread->event = event_new(libevent_thread->base, libevent_thread->read_fd, EV_READ | EV_PERSIST, on_notify, libevent_thread);
    event_add(libevent_thread->event, NULL);
    event_base_dispatch(libevent_thread->base);

    event_base_free(libevent_thread->base);
    event_del(libevent_thread->event);

    return NULL;
}

/**
 *
 * @param nreactor the num of the subReactors
 * @param main_base the main base event
 * @return  useless
 */
int libevent_reactors_init(int nreactor, struct event_base *main_base) {
    libevent_reactors = malloc(sizeof(libevent_reactor_t) * nreactor);
    notify = malloc(sizeof(int) * nreactor);
    sub_reactors = nreactor;
    if (notify == NULL) {
        log_err("can not malloc for subReactor\nexiting...");
        exit(1);
    }
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
/**
 * clear sub reactors resources
 */
void libevent_reactors_destroy() {
    for (int i = 0; i < sub_reactors; i++) {
        close(libevent_reactors[i].read_fd);
        close(notify[i]);
    }
    free(notify);
    free(libevent_reactors);
}