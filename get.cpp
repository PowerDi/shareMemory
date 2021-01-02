#include "shmfifo.h"
#include <unistd.h>

typedef struct Products{
    int id;
    char pro_name[10];
}Pro;

int main()
{
    shmfifo_t* fifo = shmfifo_init(12345, 4, sizeof(Pro));
    Pro p;

    while( 1){
        memset(&p, 0x00, sizeof(p));
        shmfifo_get(fifo, &p);
        printf("id:%d, 产品名：%s\n", p.id, p.pro_name);
        sleep(1);
    }
    shmfifo_destroy(fifo);
}


