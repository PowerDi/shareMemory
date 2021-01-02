//
// Created by powerdi on 2021/1/2.
//

#include "shmfifo.h"

typedef struct Product
{
    int id;
    char pro_name[10];
}Pro;

int main()
{
    shmfifo_t *fifo = shmfifo_init(12345, 4, sizeof(Pro));
    Pro p;

    for (int i=0; i<20; ++i)
    {
        memset(&p, 0x00, sizeof(p));
        sprintf(p.pro_name, "iphone%d", i);
        p.id = i+1;
        shmfifo_put(fifo, &p);
        printf("put %d ok\n", i);
    }
}
