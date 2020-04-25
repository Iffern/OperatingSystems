#ifndef SIMPLE_CHAT_CHAT_H
#define SIMPLE_CHAT_CHAT_H


#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stddef.h>
#include <string.h>

#define MAX_CLIENTS 100
#define MAX_MESSAGE 1000
#define PROJ_ID 0x099
#define COMMANDS_COUNT 6

typedef enum mtype{
    STOP = 1,
    DISCONNECT = 2,
    LIST = 3,
    INIT = 4,
    CONNECT = 5
}mtype;

typedef struct msg{
    long mtype;
    int client_id;
    char message[MAX_MESSAGE];
    key_t queue_key;
}msg;


const size_t MSG_SIZE = sizeof(msg)-sizeof(long);

#endif
