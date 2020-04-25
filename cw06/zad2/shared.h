#ifndef SIMPLE_CHAT_CHAT_H
#define SIMPLE_CHAT_CHAT_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <stddef.h>
#include <string.h>

#define MAX_CLIENTS 100
#define MAX_MESSAGE 200
#define MAX_NAME 20
#define MAX_QUEUE_SIZE 9

typedef enum mtype{
    STOP = 1,
    DISCONNECT = 2,
    LIST = 3,
    INIT = 4,
    CONNECT = 5
}mtype;

const char* SERVER_QUEUE_NAME = "/SERVER";

#endif
