//
// Created by powerdi on 2021/1/2.
//

#include "shmfifo.h"

typedef union semun{

    int val;
}semun;

shmfifo_t *shmfifo_init(key_t key, int blocks, int blksz) {

    shmfifo_t *p = (shmfifo_t*)malloc(sizeof(shmfifo_t));

    int shmid = shmget(key, 0, 0);
    int len = sizeof(shmhead_t) + blocks*blksz;

    if(shmid== -1)
    {
        shmid = shmget(key, len, IPC_CREAT|0644);
        if(shmid == -1) perror("shmget"),exit(1);

        p->p_head = (shmhead_t*)shmat(shmid, NULL, 0);
        p->p_head->rd_idx = 0;
        p->p_head->wr_idx = 0;
        p->p_head->blocks = blocks;
        p->p_head->blksz = blksz;

        //初始化后段
        p->p_payload = (char*)(p->p_head+1);
        p->shmid = shmid;
        p->sem_mutex = semget(key, 1, IPC_CREAT|0644); //第二个参数表示我创建了一个信号集,里面有一个信号量,如果是获得就传两个0
        p->sem_empty = semget(key+1, 1, IPC_CREAT|0644);
        p->sem_full = semget(key+2, 1, IPC_CREAT|0644);

        semun su = {1};
        semctl(p->sem_mutex, 0, SETVAL, su);

        su.val = blocks;
        semctl(p->sem_empty, 0 , SETVAL, su);

        su.val = 0;
        semctl(p->sem_full, 0, SETVAL,su);

    } else
    {
        p->p_head = (shmhead_t*)shmat(shmid,  NULL, 0);
        p->p_payload = (char*)(p->p_head+1);
        p->shmid = shmid;
        p->sem_mutex = semget(key,   0, 0); //创建要传入1的个数,获取就不需要指定信号集的个数了
        p->sem_empty = semget(key+1, 0, 0);
        p->sem_full  = semget(key+2, 0, 0);
    }


    return p;
}

static void P(int id)
{

    struct sembuf sb[1] = {0,-1,0};
    semop(id, sb,1); //表示sembuf有几个数组,即多少个操作,可以组合嘛

}

static void V(int id)
{

    struct sembuf sb[1] = {0,1,0};
    semop(id, sb,1); //表示sembuf有几个数组,即多少个操作,可以组合嘛

}

void shmfifo_put(shmfifo_t *fifo, const void *buf)
{
    P(fifo->sem_empty);  //有多少地方可供生产,确保有空位生产
    P(fifo->sem_mutex); //保证进程互斥
    memcpy(fifo->p_payload + fifo->p_head->wr_idx * fifo->p_head->blksz, //写入位置
           buf,
           fifo->p_head->blksz);  //每次写入一个数据块大小

    fifo->p_head->wr_idx = (fifo->p_head->wr_idx+1)
                           %fifo->p_head->blocks;  //取模，保证数据存满时，转从payload处写数据

    V(fifo->sem_full);
    V(fifo->sem_mutex);
}

void shmfifo_get(shmfifo_t* pFifo, void *buf)
{
    P(pFifo->sem_full);  //确保有数据可取
    P(pFifo->sem_mutex);
    //从内存段读取，拷入buf中
    memcpy(buf,
           pFifo->p_payload + pFifo->p_head->rd_idx* pFifo->p_head->blksz,
           pFifo->p_head->blksz);

    pFifo->p_head->rd_idx = (pFifo->p_head->rd_idx+1)
                            %pFifo->p_head->blocks;  //取模，保证数据存满时，转从payload处取数据

    V(pFifo->sem_empty);
    V(pFifo->sem_mutex);
}

void shmfifo_destroy(shmfifo_t *fifo) {

    shmdt(fifo->p_head); //取消挂载
    shmctl(fifo->shmid, IPC_RMID, 0 );
    semctl(fifo->sem_mutex, 0, IPC_RMID,0); //信号量所有函数都需要一个 信号量集合中的个数,一般只要创建传入创建一个的标记,其他时候这个集合都传0
    semctl(fifo->sem_empty, 0, IPC_RMID,0); //信号量所有函数都需要一个 信号量集合中的个数,一般只要创建传入创建一个的标记,其他时候这个集合都传0
    semctl(fifo->sem_full, 0, IPC_RMID,0); //信号量所有函数都需要一个 信号量集合中的个数,一般只要创建传入创建一个的标记,其他时候这个集合都传0

    free(fifo);

}

