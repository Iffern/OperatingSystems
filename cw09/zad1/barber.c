#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#define MAX_SEC 2

int num_of_chairs;
int num_of_clients;

pthread_t* waiting_room;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

int awaiting_num = 0;
bool barber_working = true;
pthread_t curr_client;
int first_free_place = 0;

unsigned int rand_time(){
    return random()%MAX_SEC+1;
}

void* barber_function(){
    int served_clients = 0;
    int first_to_shave = 0;
    while(served_clients<num_of_clients){
        pthread_mutex_lock(&mutex);
        if(awaiting_num==0){
            printf("Golibroda: ide spac\n");
            barber_working=false;
            pthread_cond_wait(&condition,&mutex);
            barber_working=true;
        }
        else if(awaiting_num>0){
            awaiting_num--;
            curr_client = waiting_room[first_to_shave];
            first_to_shave = (first_to_shave+1)%num_of_chairs;
        }
        printf("Golibroda: czeka %d klientow, gole klienta %ld\n",awaiting_num,curr_client);
        served_clients++;
        pthread_mutex_unlock(&mutex);

        sleep(rand_time()+2);
    }
    pthread_exit((void *) 0);
}

void* client_function(){
    pthread_t pthread_id = pthread_self();
    bool is_served = false;

    while(!is_served){
        pthread_mutex_lock(&mutex);

        if(awaiting_num==num_of_chairs){
            printf("Zajete; %ld\n",pthread_id);
        }
        else if(awaiting_num<num_of_chairs && barber_working){
            waiting_room[first_free_place] = pthread_id;
            first_free_place = (first_free_place+1)%num_of_chairs;
            awaiting_num++;
            printf("Poczekalnia, wolne miejsca: %d; %ld\n",num_of_chairs-awaiting_num,pthread_id);
            is_served=true;
        }
        else if(!barber_working){
            printf("Budze golibrode; %ld\n",pthread_id);
            curr_client = pthread_id;
            pthread_cond_broadcast(&condition);
            is_served=true;
        }
        pthread_mutex_unlock(&mutex);
        sleep(rand_time());
    }

    pthread_exit((void *) 0);
}

int main(int argc, char** argv){
    srand(time(NULL));

    if(argc!=3){
        fprintf(stderr,"Niewlasciwa liczba argumentow programu\n");
        exit(1);
    }

    num_of_chairs = atoi(argv[1]);
    num_of_clients = atoi(argv[2]);
    waiting_room = calloc(num_of_chairs+1,sizeof(pthread_t));

    if(pthread_mutex_init(&mutex,NULL)!=0){
        fprintf(stderr,"Nie udalo sie zainicjowac mutex\n");
        exit(2);
    }
    if(pthread_cond_init(&condition,NULL)!=0){
        fprintf(stderr,"Nie udalo sie zainicjowac condition\n");
        exit(3);
    }

    pthread_t* threads = calloc(num_of_clients+2,sizeof(pthread_t));

    if(pthread_create(&threads[0],NULL,barber_function,NULL)!=0){
        fprintf(stderr,"Nie udalo sie zainicjowac watku dla golibrody\n");
        exit(4);
    }

    for(int i=1;i<=num_of_clients;i++){
        if(pthread_create(&threads[i],NULL,client_function,NULL)!=0){
        fprintf(stderr,"Nie udalo sie zainicjowac watku dla klienta numer %d\n",i);
        exit(5);
    }
    sleep(rand_time());
    }

    void* return_val;
    for(int i=0;i<=num_of_clients;i++){
        pthread_join(threads[i],&return_val);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}
