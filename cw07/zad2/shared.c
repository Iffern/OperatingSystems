#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "shared.h"

const int MAX_WORKERS = MAX_PACKER+MAX_RECEIVER+MAX_SENDER;

const char NAMES[4][12] = {"/NEXTPACK","/NEXTSEND","/INDEX","/AVILABLE"};

const char* SHARED = "/SHARED";

char* get_time(){
    char* result = calloc(200,sizeof(char));
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME,&spec);
    double miliseconds = spec.tv_nsec/1.0e6;
    sprintf(result,"%f ms",miliseconds);
    return result;
}

