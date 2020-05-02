#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "shared.h"


sem_t** semaphores;
pid_t* workers;
orders* ord;

void clean(){
    for(int i=0;i<4;i++){
        if(sem_close(semaphores[i])==-1){
            fprintf(stderr,"Nie udalo sie zamknac semafora %d dla glownego programu\n",i);
        }
    }
    for(int i=0;i<4;i++){
        if(sem_unlink(NAMES[i])==-1){
            fprintf(stderr,"Nie udalo sie usunac semafora %d dla glownego programu\n",i);
        }
    }
    if(munmap(ord,sizeof(orders))==-1){
        fprintf(stderr,"Nie udalos sie odloczyc pamieci wspolnej od przestrzeni adresowej\n");
    }
    if(shm_unlink(SHARED)==-1){
        fprintf(stderr,"Nie udalo sie usunac pamieci wspolnej\n");
    }
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
    semaphores = calloc(4,sizeof(sem_t*));
    ord = calloc(1,sizeof(orders));

    for(int i=0;i<3;i++){
        semaphores[i]= sem_open(NAMES[i], O_RDWR | O_CREAT, 0666 , 0);
        if(semaphores[i]==SEM_FAILED){
            fprintf(stderr,"Nie udalo sie utorzyc semafora %s w funkcji glownej\n",NAMES[i]);
            exit(1);
        }
    }

    semaphores[3] = sem_open(NAMES[3], O_RDWR | O_CREAT, 0666 , 1);
    if(semaphores[3]==SEM_FAILED){
            fprintf(stderr,"Nie udalo sie utorzyc semafora %s w funkcji glownej\n",NAMES[3]);
            exit(1);
        }

//    Wykorzystywane semafory:
//    0 - indeks kolejnego zamowienia do przygotowania
//    1 - indeks kolejnego zamowienia do wyslania
//    2 - pierwszy wolny indeks w tablicy
//    3 - czy tablica jest obecnie dostepna (1-tak ,0-nie)

    int shared_des = shm_open(SHARED, O_RDWR | O_CREAT, 0666);
    if(shared_des==-1){
        fprintf(stderr,"Problem z utworzeniem pamieci wspolnej\n");
        exit(2);
    }

    if(ftruncate(shared_des,sizeof(orders))==-1){
        fprintf(stderr,"Nie udalo sie zaalokowac pamieci wspolnej\n");
        exit(3);
    }

    ord = mmap(NULL,sizeof(orders),PROT_READ | PROT_WRITE| PROT_EXEC, MAP_SHARED, shared_des,0);
    if(ord == (void *) -1){
        fprintf(stderr,"Nie udalo sie dolaczy pamieci wspolnej do przestrzeni adresowej procesu glownego\n");
    }

    ord->to_pack=0;
    ord->to_send=0;

    activate_workers();

    for(int i=0;i<MAX_WORKERS;i++) wait(NULL);

    clean();

    return 0;
}
