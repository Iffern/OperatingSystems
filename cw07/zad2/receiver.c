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
#include <time.h>
#include "shared.h"
#define MAX_RAND 100

sem_t** semaphores;
pid_t* workers;
orders* ord;

void exit_worker(int signo){
    for(int i=0;i<4;i++){
        if(sem_close(semaphores[i])==-1){
            fprintf(stderr,"Nie udalo sie zamknac semafora %d dla programu %d\n",getpid());
        }
    }
    if(munmap(ord,sizeof(orders))==-1){
        fprintf(stderr,"Nie udalos sie odloczyc pamieci wspolnej od przestrzeni adresowej programu %d\n",getpid());
    }
    exit(0);
}

int random_num(){
    return rand() % MAX_RAND + 1;
}

void add_order(){
    if(ord->to_pack+ord->to_send>=MAX_ORDERS) return;

    sem_wait(semaphores[3]);

    int index;
    sem_getvalue(semaphores[2],&index);
    int order = random_num();
    ord->orders[index % MAX_ORDERS]=order;
    ord->to_pack+=1;

    printf("(%d %s) Dodalem liczbe : %d. Liczba zamownien do przygotowania: %d. Liczba zamownien do wyslania: %d\n",getpid(),get_time(),order,ord->to_pack,ord->to_send);

    sem_post(semaphores[2]);

    sem_post(semaphores[3]);
}

int main(int argc, char**argv){
    srand(time(NULL));
    signal(SIGINT,exit_worker);

    workers = calloc(MAX_WORKERS,sizeof(pid_t));
    semaphores = calloc(4,sizeof(sem_t*));
    ord = calloc(1,sizeof(orders));

    for(int i=0;i<4;i++){
        semaphores[i]= sem_open(NAMES[i], O_RDWR);
        if(semaphores[i]==SEM_FAILED){
            fprintf(stderr,"Nie udalo sie otworzyc semafora %s w programie %d\n",NAMES[i],getpid());
        }
    }

    int shared_des = shm_open(SHARED, O_RDWR | O_CREAT, 0666);
    if(shared_des==-1){
        fprintf(stderr,"Problem z otworzeniem pamieci wspolnej dla procesu %d\n",getpid());
    }

    ord = mmap(NULL,sizeof(orders),PROT_READ | PROT_WRITE| PROT_EXEC, MAP_SHARED, shared_des,0);
    if(ord == (void *) -1){
        fprintf(stderr,"Nie udalo sie dolaczy pamieci wspolnej do przestrzeni adresowej procesu %d\n",getpid());
    }

    while(1){
        int val;
        sem_getvalue(semaphores[3],&val);
        if(ord->to_pack+ord->to_send < MAX_ORDERS && val==1){
            add_order();
        }
        sleep(0.5);
    }
}

