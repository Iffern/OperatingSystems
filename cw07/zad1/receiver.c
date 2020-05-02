#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include "shared.h"
#define MAX_RAND 100

int sem_id;
int shared_id;
orders* ord;

void exit_worker(int signo){
    shmdt(ord);
    exit(0);
}

int random_num(){
    return rand() % MAX_RAND + 1;
}

void add_order(){
    struct sembuf* buffor = calloc(1, sizeof(struct sembuf));
    if(ord->to_pack+ord->to_send>=MAX_ORDERS) return;

    buffor->sem_num=3;
    buffor->sem_op=-1;
    buffor->sem_flg=0;

    semop(sem_id,buffor,1);

    int index = semctl(sem_id,2,GETVAL);

    int order = random_num();
    ord->orders[index]=order;
    ord->to_pack+=1;

    printf("(%d %s) Dodalem liczbe : %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n",getpid(),get_time(),order,ord->to_pack,ord->to_send);

    union semun arg;
    arg.val=(index+1)%MAX_ORDERS;

    semctl(sem_id,2,SETVAL,arg);

    buffor->sem_num=3;
    buffor->sem_op=1;
    buffor->sem_flg=0;

    semop(sem_id,buffor,1);
}

int main(int argc, char**argv){
    srand(time(NULL));
    signal(SIGINT,exit_worker);

    key_t sem_key = ftok(getenv("HOME"),SEM_NUM);
    sem_id=semget(sem_key,0,0);
    if(sem_id==-1){
        fprintf(stderr,"Problem z otworzeniem zbioru semaforow dla procesu %d\n",getpid());
    }

    key_t shared_key = ftok(getenv("HOME"),SHARED_NUM);
    shared_id = shmget(shared_key,0,0);
    if(shared_id==-1){
        fprintf(stderr,"Problem z otworzeniem pamieci wspoldzielonej procesu %d\n",getpid());
    }

    ord = shmat(shared_id,NULL,0);
    if(ord == (void *) -1){
        fprintf(stderr,"Nie udalo sie dolaczy pamieci wspolnej do przestrzeni adresowej procesu %d\n",getpid());
    }

    while(1){
        if(ord->to_pack+ord->to_send < MAX_ORDERS && semctl(sem_id,3,GETVAL)==1){
            add_order();
        }
        sleep(0.5);
    }
}
