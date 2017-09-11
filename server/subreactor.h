//
// Created by 朱逸尘 on 2017/9/11.
//

#ifndef TINYHTTP_SUBREACTOR_H
#define TINYHTTP_SUBREACTOR_H
#include <pthread.h>


struct libevent_thread_t {
    struct event_base *base;
    struct event *event;
    struct event *timeout;
    int read_fd;
};

typedef struct libevent_thread_t libevent_thread_t;

int libevent_threadpool_init(int nthread, struct event_base *main_base);

#endif //TINYHTTP_SUBREACTOR_H
