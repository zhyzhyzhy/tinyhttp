//
// Created by 朱逸尘 on 2017/7/31.
//

#ifndef TINYHTTP_THREADPOOL_H
#define TINYHTTP_THREADPOOL_H

#include <pthread.h>


struct libevent_thread_t {
    struct event_base *base;
    struct event *event;
    int read_fd;
};

typedef struct libevent_thread_t libevent_thread_t;

int libevent_threadpool_init(int nthread, struct event_base *main_base);

#endif //TINYHTTP_THREADPOOL_H
