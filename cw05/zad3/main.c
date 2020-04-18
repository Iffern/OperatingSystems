#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

const int NUM_CHARS = 50;
const int PROD_NUM = 5;

int main(int argc, char **argv){
    if(mkfifo("stream",S_IRWXG | S_IRWXO | S_IRWXU)==-1){
        fprintf(stderr,"Blad przy tworzeniu potoku nazwanego\n");
        return -1;
    }

    char * prod_file = calloc(6,sizeof(char*));

    for(int i=0;i<PROD_NUM;i++){
        sprintf(prod_file,"prod%d",i);
        FILE * file = fopen(prod_file,"w+");

        char ch = '0' + rand()%43;

        for(int i=0;i<NUM_CHARS;i++){
            fwrite(&ch,1,1,file);
        }

        if(fork()==0){
        execl("./producent","./producent","stream",prod_file,"5",NULL);}

        }
        if(fork()==0){
        execl("./consumer","./consumer","stream","result","5", NULL);}

        return 0;
    }
