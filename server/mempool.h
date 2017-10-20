//
// Created by 朱逸尘 on 2017/10/20.
//

#ifndef TINYHTTP_MEMPOOL_H
#define TINYHTTP_MEMPOOL_H

#include <pthread.h>

struct memnode_t {
    struct memnode_t *next;
    char *data;
};

typedef struct memnode_t memnode_t;

struct memblock_t {
    char* b_start_addr;
    char* b_end_addr;
    int nodesize;
    int freenum;
    int toalnum;
    memnode_t *openList;
    struct memblock_t *next;
    pthread_mutex_t *openListLock;
};
typedef struct memblock_t memblock_t;

struct mempool_t {
    char* p_start_addr;
    char* p_end_addr;
    int blocksize;
    memblock_t *memblocks;
};

typedef struct mempool_t mempool_t;

mempool_t* mempool;

void* bmalloc(memblock_t* block);
void bfree(memblock_t* block, void* ptr);
void* memblock_init(memblock_t * block, int blocksize, int num);
void* memnode_init(char* startaddr, memnode_t* next);
void* mempool_init(int num, int blocksize, ...);
void* mmalloc(int size);
void mfree(void* ptr);
void mempool_destroy();
void memblock_destroy(memblock_t* block);
#endif //TINYHTTP_MEMPOOL_H
