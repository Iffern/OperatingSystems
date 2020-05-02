#ifndef ORDERS_SHARED
#define ORDERS_SHARED

#define MAX_RECEIVER 3
#define MAX_PACKER 3
#define MAX_SENDER 3

#define MAX_ORDERS 10

#define SEM_NUM 1
#define SHARED_NUM 2

typedef struct orders{
    int orders[MAX_ORDERS];
    int to_pack;
    int to_send;
}orders;

union semun {
             int val;                  /* wartość dla SETVAL */
             struct semid_ds *buf;     /* bufor dla IPC_STAT i IPC_SET */
             unsigned short *array;    /* tablica dla GETALL i SETALL */
                                       /* Część specyficzna dla Linuksa: */
             struct seminfo *__buf;    /* bufor dla IPC_INFO */
}semun;

const int MAX_WORKERS;
char* get_time();

#endif
