//
// Created by 朱逸尘 on 2017/9/11.
//

#ifndef TINYHTTP_SUBREACTOR_H
#define TINYHTTP_SUBREACTOR_H
#include <pthread.h>

struct libevent_reactor_t {
    struct event_base *base;
    struct event *event;
    int read_fd;
};

typedef struct libevent_reactor_t libevent_reactor_t;
int libevent_reactors_init(int nreactor, struct event_base *main_base);
void libevent_reactors_destroy();

#endif //TINYHTTP_SUBREACTOR_H
