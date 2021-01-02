//
// Created by powerdi on 2021/1/2.
//

#ifndef SHAREMEMORY_SHMFIFO_H
#define SHAREMEMORY_SHMFIFO_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct shmhead {

    int rd_idx; //读数据索引
    int wr_idx; //写数据索引
    int blocks;
    int blksz;
}shmhead_t;

typedef struct shmfifo{

    shmhead_t *p_head; //共享内存的起始地址
    char *p_payload; //有效数据
    int shmid;
    int sem_mutex;
    int sem_empty; //还剩多少个可以消费
    int sem_full; //剩余多少个地方可以生产

}shmfifo_t;

shmfifo_t *shmfifo_init(key_t key, int blocks, int blksz);

void shmfifo_put(shmfifo_t *fifo, const void *buf);

void shmfifo_get(shmfifo_t *fifo, void *buf);

void shmfifo_destroy(shmfifo_t *fifo);

#endif //SHAREMEMORY_SHMFIFO_H
