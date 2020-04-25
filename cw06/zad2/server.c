#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <mqueue.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "shared.h"

extern int errno ;

typedef struct client{
    int pid;
    mqd_t queue;
    char* queue_n;
    bool available;
}client;

client clients[MAX_CLIENTS];
int current_clients = 0;
mqd_t main_queue = -1;

void exit_handler(int signo){

    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].pid!=-1){
        if(mq_close(clients[i].queue)==-1){
            fprintf(stderr,"Blad podczas zamykania kolejki klienta %d\n",i);
        }
        kill(clients[i].pid,SIGINT);
        }
    }

    if(mq_close(main_queue)==-1){
        fprintf(stderr,"Blad podczas zamykania glownej kolejki\n");
    }
    if(mq_unlink(SERVER_QUEUE_NAME)==-1){
        fprintf(stderr,"Blad podczas usuwania glownej kolejki\n");
    }
    exit(0);
}

void stop(char* message){
    int id = atoi(message);
    if(id>=0 && id<MAX_CLIENTS){
        clients[id].available=false;
        clients[id].pid=-1;
        current_clients-=1;
        if(mq_close(clients[id].queue)==-1){
            fprintf(stderr,"Blad podczas zamykania kolejki klienta %d\n",id);
        }
        if(current_clients==0){
            if(mq_close(main_queue)==-1){
                fprintf(stderr,"Blad podczas zamykania glownej kolejki\n");
            }
            if(mq_unlink(SERVER_QUEUE_NAME)==-1){
                fprintf(stderr,"Blad podczas usuwania glownej kolejki\n");
            }
            exit(0);
        }
    }
}

void disconnect(char* message){
    int id;
    int second_id;
    sscanf(message,"%d:%d",&id,&second_id);
    if(id>=0 && id<MAX_CLIENTS){
        clients[id].available=true;
    }
    if(second_id>=0 && second_id<MAX_CLIENTS){
        clients[second_id].available=true;
        if(mq_send(clients[second_id].queue,message,MAX_MESSAGE,DISCONNECT)==-1){
            fprintf(stderr,"Nie udalo sie wyslac informacji DISCONNECT do klienta %d",second_id);
        }
    }
}

void list(char* message){
    int id = atoi(message);
    if(id>=0 && id<MAX_CLIENTS){
    char list[MAX_MESSAGE];
    char bufor[MAX_MESSAGE];
    char available[4];
    strcpy(list,"");
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].pid!=-1){
        if(clients[i].available==true) strcpy(available,"TAK");
        else strcpy(available,"NIE");
        sprintf(bufor,"ID: %d, DOSTEPNY: %s\n",i,available);
        strcat(list,bufor);}
    }
    if(mq_send(clients[id].queue,list,MAX_MESSAGE,LIST)==-1){
        fprintf(stderr,"Nie udalo sie wyslac listy dostepnych klientow\n");
    }
}
}

void init(char* message){
    char* name = calloc(MAX_NAME,sizeof(char));
    int pid;

    sscanf(message,"%s %d",name,&pid);

    int new_id;
    for(new_id=0;new_id<MAX_CLIENTS;new_id++){
        if(clients[new_id].pid==-1) break;
    }

    if(new_id>=MAX_CLIENTS){
        fprintf(stderr,"Zbyt duza ilosc klientow na serwerze\n");
    }

    mqd_t queue_desc = mq_open(name,O_RDWR);

    if(queue_desc==-1){
        fprintf(stderr,"Blad podczas odczytu indentyfikatora kolejki\n");
        return;
    }

    clients[new_id].pid=pid;
    clients[new_id].queue=queue_desc;
    clients[new_id].queue_n=name;
    clients[new_id].available=true;

    char msg[MAX_MESSAGE];
    sprintf(msg,"%d",new_id);

    if(mq_send(queue_desc,msg,MAX_MESSAGE,new_id)==-1){
        fprintf(stderr,"Blad podczas wysylania identyfikatora do klienta\n");
    }
}

void connect(char* message){
    int client_id;
    int second_id;
    char* msg = calloc(MAX_MESSAGE,sizeof(char));
    sscanf(message,"%d:%d",&client_id,&second_id);
    if(second_id<0 || second_id>=MAX_CLIENTS || client_id==second_id){
        fprintf(stderr,"Bledny identyfikator kontaktowy\n");
        sprintf(msg,"%d",-1);
        if(mq_send(clients[client_id].queue,msg,MAX_MESSAGE,CONNECT)==-1){
            fprintf(stderr,"Nie udalo sie wyslac wiadomosci CONNECT do klienta %d\n",client_id);
        }
        return;
    }
    if(clients[second_id].available==false || clients[second_id].pid==-1){
        fprintf(stderr, "Klient o  numerze ID %d jest nieosiagalny\n",second_id);
        sprintf(msg,"%d",-1);
        if(mq_send(clients[client_id].queue,msg,MAX_MESSAGE,CONNECT)==-1){
            fprintf(stderr,"Nie udalo sie wyslac wiadomosci CONNECT do klienta %d\n",client_id);
        }
        return;
    }

    char* client_queue = clients[client_id].queue_n;
    char* second_queue = clients[second_id].queue_n;

    if(mq_send(clients[client_id].queue,second_queue,MAX_MESSAGE,CONNECT)==-1){
            fprintf(stderr,"Nie udalo sie wyslac wiadomosci CONNECT do klienta %d\n",client_id);
        }
    sprintf(msg,"%d:%s",client_id,client_queue);
    if(mq_send(clients[second_id].queue,msg,MAX_MESSAGE,CONNECT)==-1){
            fprintf(stderr,"Nie udalo sie wyslac wiadomosci CONNECT do klienta %d\n",second_id);
        }

    clients[client_id].available=false;
    clients[second_id].available=false;
}


void public_queue_handler(unsigned int type,char* message){
    if(message==NULL) return;
    switch(type){
        case STOP:
            stop(message);
            break;
        case DISCONNECT:
            disconnect(message);
            break;
        case LIST:
            list(message);
            break;
        case INIT:
            init(message);
            break;
        case CONNECT:
            connect(message);
            break;
        default:
            break;
    }
}


int main(int argc, char** argv){

    for(int i=0;i<MAX_CLIENTS;i++){
        clients[i].pid=-1;
        clients[i].available=true;
    }

    signal(SIGINT,exit_handler);

    struct mq_attr qinfo;
    qinfo.mq_maxmsg = MAX_QUEUE_SIZE;
    qinfo.mq_msgsize = MAX_MESSAGE;

    main_queue=mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, 0666, &qinfo);

    if(main_queue==-1){
        fprintf(stderr,"Nie udalo sie stworzyc kolejki glownej komunikatow\n");
        exit(1);
    }

    char* bufor = calloc(MAX_MESSAGE,sizeof(char));
    unsigned int type;
    while(1){
        if (mq_receive(main_queue,bufor,MAX_MESSAGE,&type) == -1){
            fprintf(stderr,"Nie udalo sie odebrac komunikatu\n");
        }
        public_queue_handler(type,bufor);
    }
    return 0;
}
