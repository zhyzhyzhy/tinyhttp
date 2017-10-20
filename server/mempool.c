//
// Created by 朱逸尘 on 2017/10/20.
//


/*
 *
 * 看了一下，主要是struct httpRequest的分配和job的分配较多
 * 所以考虑的是固定大小的分配规则
 * 采用可利用空间表freeList
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "mempool.h"
#include "log.h"

extern mempool_t* mempool;

void* memnode_init(char* startaddr, memnode_t* next) {
    memnode_t* node = (memnode_t*)startaddr;
    node->next = next;
    node->data = startaddr + sizeof(memnode_t);
    return node;
}

void* memblock_init(memblock_t* block, int nodesize, int num) {
    block->nodesize = nodesize;
    block->b_start_addr = malloc((size_t)(nodesize+sizeof(memnode_t))*num);
    block->b_end_addr = block->b_start_addr + (nodesize+sizeof(memnode_t))*num;
    block->freenum = num;
    block->toalnum = num;
    block->next = NULL;

    if (block->b_start_addr == NULL) {
        log_err("can not init block");
        exit(2);
    }

    block->openListLock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(block->openListLock, NULL);

    memnode_t* head = memnode_init(block->b_start_addr, NULL);
    block->openList = head;

    memnode_t* p = head;
    for (int i = 1; i < num; i++) {
        memnode_t* next = memnode_init((char*)p + sizeof(memnode_t) + nodesize, p);
        p->next = next;
        p = p->next;
        p->next = NULL;
    }

    memnode_t* q = block->openList;
    while (q != NULL) {
        printf("%p\n", q->data);
        q = q->next;
    }
    printf("\n\n");
}


void* mempool_init(int num, int blocksizenums, ...) {
    va_list blocks;
    va_start(blocks, blocksizenums);

    mempool = malloc(sizeof(mempool_t));
    mempool->memblocks = malloc(sizeof(memblock_t) * blocksizenums);
    mempool->blocksize = blocksizenums;
    if (mempool == NULL) {
        log_err("can not malloc mempool");
        exit(2);
    }

    //初始化各个memblocks
    for (int i = 0; i < blocksizenums; i++) {
        memblock_init((mempool->memblocks) + i, va_arg(blocks, int), num);
    }

}

void* bmalloc(memblock_t* block) {
    memblock_t* blockhead = block;
    memblock_t* tempblock = block;
    while (block != NULL) {
        tempblock = block;
        pthread_mutex_lock(block->openListLock);
        if (block->freenum == 0) {
            block = block->next;
            pthread_mutex_unlock(tempblock->openListLock);
        }
        else {
            pthread_mutex_unlock(tempblock->openListLock);
            break;
        }
    }

    if (block == NULL) {
        pthread_mutex_lock(blockhead->openListLock);
        memblock_t* newblock = memblock_init(blockhead, blockhead->nodesize, blockhead->toalnum);
        tempblock->next = newblock;
        pthread_mutex_unlock(blockhead->openListLock);
        return bmalloc(blockhead);
    }
    pthread_mutex_lock(block->openListLock);
    memnode_t* node = block->openList;
    block->openList = node->next;
    block->freenum--;
    pthread_mutex_unlock(block->openListLock);
    printf("bmalloc = %p\n", (void*)node->data);
    return node->data;
}

void* mmalloc(int size) {
    memblock_t *block = mempool->memblocks;
    for (int i = 0; i < mempool->blocksize; i++) {
        //找到匹配的block大小
        if (block[i].nodesize == size) {
            return bmalloc(block + i);
        }
    }
    //没有匹配的大小的blocks，返回NULL
    return NULL;
}

void bfree(memblock_t* block, void* ptr) {
    pthread_mutex_lock(block->openListLock);
    memnode_t* node = (memnode_t*)((char*)ptr - sizeof(memnode_t));
    printf("bfree =   %p\n", ptr);
    node->next = block->openList;
    block->openList = node;
    block->freenum++;
    pthread_mutex_unlock(block->openListLock);
}

void mfree(void* ptr) {
    memblock_t *blocks = mempool->memblocks;
    for (int i = 0; i < mempool->blocksize; i++) {
        memblock_t* headblock = blocks + i;
        while (headblock != NULL) {
            if (ptr > headblock->b_start_addr && ptr < (void*)headblock->b_end_addr) {
                bfree(blocks + i, ptr);
                return;
            }
            headblock = headblock->next;
        }
    }
    free(ptr);
}
void memblock_destroy(memblock_t* block) {
    free(block->openListLock);
}
void mempool_destroy() {
    memblock_t *blocks = mempool->memblocks;
    for (int i = 0; i < mempool->blocksize; i++) {
        memblock_t* headblock = blocks + i;
        while (headblock != NULL) {
            memblock_t* temp = headblock;
            headblock = headblock->next;
            memblock_destroy(temp);
        }
    }
    free(mempool->memblocks);
}

//int main()
//{
//    mempool_init(5, 5, 1,2,3,4,5);
//    for (int i = 0; i < 6; i++) {
//        char* demo = mmalloc(3);
//        printf("demo    = %p\n", (void*)demo);
//        mfree(demo);
//    }
//
//}
