#ifndef ORDERS_SHARED_POS
#define ORDERS_SHARED_POS

#define MAX_RECEIVER 3
#define MAX_PACKER 3
#define MAX_SENDER 3

#define MAX_ORDERS 10


typedef struct orders{
    int orders[MAX_ORDERS];
    int to_pack;
    int to_send;
}orders;

const char* SHARED;
const char NAMES[4][12];
const int MAX_WORKERS;
char* get_time();

#endif
