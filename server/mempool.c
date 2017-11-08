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

/**
 * init the memnode of startaddr
 * @param startaddr the start addr of this node
 * @param next      the addr of next node
 * @return          the current node
 */
void *memnode_init(char *startaddr, memnode_t *next) {
    memnode_t *node = (memnode_t *) startaddr;
    node->next = next;
    node->data = startaddr + sizeof(memnode_t);
    return node;
}

/**
 *
 * @param block  the point for the block
 * @param nodesize the type size
 * @param num the init capacity the block hold
 * @return
 */
void *memblock_init(memblock_t *block, int nodesize, int num) {
    block->nodesize = nodesize;
    block->b_start_addr = malloc((size_t) (nodesize + sizeof(memnode_t)) * num);
    block->b_end_addr = block->b_start_addr + (nodesize + sizeof(memnode_t)) * num;
    block->freenum = num;
    block->toalnum = num;
    block->next = NULL;

    if (block->b_start_addr == NULL) {
        log_err("can not init block");
        exit(2);
    }

    //init the block lock
    block->openListLock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(block->openListLock, NULL);

    //init memnode head
    memnode_t *head = memnode_init(block->b_start_addr, NULL);
    block->openList = head;

    memnode_t *p = head;
    for (int i = 1; i < num; i++) {
        memnode_t *next = memnode_init((char *) p + sizeof(memnode_t) + nodesize, p);
        p->next = next;
        p = p->next;
        p->next = NULL;
    }

//    memnode_t* q = block->openList;
//    while (q != NULL) {
//        printf("%p\n", q->data);
//        q = q->next;
//    }
//    printf("\n\n");
}

/**
 *
 * @param num the type that memory pool should hold
 * @param blocksizenums size of each type
 * @param ...
 * @return
 */
void *mempool_init(int num, int blocksizenums, ...) {
    va_list blocks;
    va_start(blocks, blocksizenums);

    mempool = malloc(sizeof(mempool_t));
    if (mempool == NULL) {
        log_err("can not malloc for memory poll!\nexiting...");
        exit(1);
    }
    mempool->memblocks = malloc(sizeof(memblock_t) * blocksizenums);
    if (mempool->memblocks == NULL) {
        log_err("can not malloc for memory blocks\nexiting...");
        exit(1);
    }
    mempool->blocksize = blocksizenums;

    //初始化各个memblocks
    for (int i = 0; i < blocksizenums; i++) {
        memblock_init((mempool->memblocks) + i, va_arg(blocks, int), num);
    }
    return NULL;

}

/**
 * find one node from the target block
 * if not enough, then malloc new one block
 * @param block
 * @return
 */
void *bmalloc(memblock_t *block) {
    memblock_t *blockhead = block;
    memblock_t *tailblock = block;
    //check if has enough node
    while (block != NULL) {
        tailblock = block;
        pthread_mutex_lock(block->openListLock);
        if (block->freenum == 0) {
            pthread_mutex_unlock(block->openListLock);
            block = block->next;
        } else {
            //if has enough node
            memnode_t *node = block->openList;
            block->openList = node->next;
            block->freenum--;
            pthread_mutex_unlock(block->openListLock);
            return node->data;
        }
    }

    //not enough node, then malloc new one block
    pthread_mutex_lock(tailblock->openListLock);
    //new one block of target size
    block = malloc(sizeof(memblock_t));
    memblock_t *newblock = memblock_init(block, blockhead->nodesize, blockhead->toalnum);
    tailblock->next = newblock;
    pthread_mutex_unlock(tailblock->openListLock);
    //then bmalloc again
    return bmalloc(blockhead);
}

/**
 * malloc size byte from the memory pool
 * @param size  the size
 * @return  the start addr
 */
void *mmalloc(int size) {
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

/**
 * free the memnode in the target block
 * @param block  the target block
 * @param ptr    the target ptr
 */
void bfree(memblock_t *block, void *ptr) {
    pthread_mutex_lock(block->openListLock);
    memnode_t *node = (memnode_t *) ((char *) ptr - sizeof(memnode_t));
    debug("bfree = %p", ptr);
    node->next = block->openList;
    block->openList = node;
    block->freenum++;
    pthread_mutex_unlock(block->openListLock);
}

/**
 * free the target addr
 * @param ptr the target addr
 */
void mfree(void *ptr) {
    memblock_t *blocks = mempool->memblocks;
    //if is in memory pool
    for (int i = 0; i < mempool->blocksize; i++) {
        memblock_t *headblock = blocks + i;
        while (headblock != NULL) {
            if (ptr > headblock->b_start_addr && ptr < (void *) headblock->b_end_addr) {
                bfree(blocks + i, ptr);
                return;
            }
            headblock = headblock->next;
        }
    }
    //if not, free directly
    free(ptr);
}

//free the lock in the block
void memblock_destroy(memblock_t *block) {
    free(block->openListLock);
    free(block->b_start_addr);
}

/**
 * destroy the memory pool
 */
void mempool_destroy() {
    memblock_t *blocks = mempool->memblocks;
    for (int i = 0; i < mempool->blocksize; i++) {
        memblock_t *headblock = blocks + i;
        while (headblock != NULL) {
            memblock_t *temp = headblock;
            headblock = headblock->next;
            memblock_destroy(temp);
        }
    }
    free(mempool->memblocks);
    free(mempool);
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
