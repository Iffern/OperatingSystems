#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "shared.h"


int sem_id;
int shared_id;
pid_t* workers;

void clean(){
    semctl(sem_id,0,IPC_RMID);
    shmctl(shared_id,IPC_RMID,NULL);
}

void exit_handler(int signo){
    for(int i=0;i<MAX_WORKERS;i++){
        kill(workers[i],SIGINT);
    }
    clean();
    exit(0);
}

void activate_workers(){
    for(int i=0;i<MAX_RECEIVER;i++){
        pid_t child = fork();
        if(child==0){
            execlp("./receiver","receiver",NULL);
        }
        workers[i]=child;
    }
    for(int i=0;i<MAX_PACKER;i++){
        pid_t child=fork();
        if(child==0){
            execlp("./packer","packer",NULL);
        }
        workers[MAX_RECEIVER+i]=child;
    }
    for(int i=0;i<MAX_SENDER;i++){
        pid_t child=fork();
        if(child==0){
            execlp("./sender","sender",NULL);
        }
        workers[MAX_RECEIVER+MAX_PACKER+i]=child;
    }
}


int main(int argc, char** argv){
    signal(SIGINT,exit_handler);

    workers = calloc(MAX_WORKERS,sizeof(pid_t));

    key_t sem_key = ftok(getenv("HOME"),SEM_NUM);
    sem_id=semget(sem_key,4,IPC_CREAT |IPC_EXCL | 0666);
    if(sem_id==-1){
        fprintf(stderr,"Problem z utworzeniem zbioru semaforow\n");
        exit(1);
    }

//    Wykorzystywane semafory:
//    0 - indeks kolejnego zamowienia do przygotowania
//    1 - indeks kolejnego zamowienia do wyslania
//    2 - pierwszy wolny indeks w tablicy
//    3 - czy tablica jest obecnie dostepna (1-tak ,0-nie)

    key_t shared_key = ftok(getenv("HOME"),SHARED_NUM);
    shared_id = shmget(shared_key,sizeof(orders),IPC_CREAT |IPC_EXCL | 0666);
    if(shared_id==-1){
        fprintf(stderr,"Problem z utworzeniem pamieci wspoldzielonej\n");
        exit(2);
    }
    orders* orders = shmat(shared_id,NULL,0);
    if(orders == (void *) -1){
        fprintf(stderr,"Nie udalo sie dolaczy pamieci wspolnej do przestrzeni adresowej procesu %d\n",getpid());
    }

    orders->to_pack=0;
    orders->to_send=0;

    union semun initial;
    initial.val=0;

    for(int i=0;i<=2;i++){
        semctl(sem_id,i,SETVAL,initial);
    }
    initial.val=1;
    semctl(sem_id,3,SETVAL,initial);

    activate_workers();

    for(int i=0;i<MAX_WORKERS;i++) wait(NULL);

    clean();

    return 0;
}
