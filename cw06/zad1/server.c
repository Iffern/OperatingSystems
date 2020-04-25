#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdbool.h>
#include "shared.h"

typedef struct client{
    int pid;
    int queue_id;
    bool available;
}client;

client clients[MAX_CLIENTS];
int current_clients = 0;
int main_queue_id = -1;

void send_message(enum mtype type, int client_id, char* msg){
    if(client_id>MAX_CLIENTS || client_id<0 || sizeof(msg)>MAX_MESSAGE){
        fprintf(stderr,"Nie mozna wyslac wiadomosci do klienta\n");
    }

    struct msg new_msg;
    new_msg.mtype=type;
    new_msg.client_id=getpid();
    strcpy(new_msg.message,msg);

    if(msgsnd(clients[client_id].queue_id,&new_msg,MSG_SIZE,0)==-1){
        fprintf(stderr,"Blad podczas wysylania widomosci do klienta %d\n",client_id);
    }
}

void exit_handler(int signo){
    msg* message = calloc(1,sizeof(msg));
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].pid!=-1){
        kill(clients[i].pid,SIGINT);
        if(msgrcv(main_queue_id,message,MSG_SIZE,STOP,0)==-1){
            fprintf(stderr,"Nie otrzymano wiadomosci zwrotnej od klienta %d\n",i);
        }
        }
    }

    if(msgctl(main_queue_id,IPC_RMID,NULL)==-1){
        fprintf(stderr,"Blad podczas zamykania glownej kolejki\n");
    }
    exit(0);
}

void stop(struct msg *message){
    int id = message->client_id;
    if(id>=0 && id<MAX_CLIENTS){
        clients[id].available=false;
        clients[id].pid=-1;
        clients[id].queue_id=-1;
        current_clients-=1;
        if(current_clients==0){
            if(msgctl(main_queue_id,IPC_RMID,NULL)==-1){
                fprintf(stderr,"Blad podczas zamykania glownej kolejki\n");
            }
            exit(0);
        }
    }
}

void disconnect(struct msg *message){
    int id=message->client_id;
    int second_id;
    sscanf(message->message,"%d",&second_id);
    if(id>=0 && id<MAX_CLIENTS){
        clients[id].available=true;
    }
    if(second_id>=0 && second_id<MAX_CLIENTS){
        clients[second_id].available=true;
        send_message(DISCONNECT,second_id,"");
    }
}

void list(struct msg *messages){
    if(messages->client_id>=0 && messages->client_id<MAX_CLIENTS){
    char list[MAX_MESSAGE];
    char bufor[MAX_MESSAGE];
    char available[4];
    strcpy(list,"");
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].pid!=-1){
        if(clients[i].available==true) strcpy(available,"TAK");
        else strcpy(available,"NIE");
        sprintf(bufor,"ID: %d, ID KOLEJKI: %d, DOSTEPNY: %s\n",i,clients[i].queue_id,available);
        strcat(list,bufor);}
    }
    send_message(LIST,messages->client_id,list);
}
}

void init(struct msg *message){
    int client_pid = message->client_id;
    int new_id;

    for(new_id=0;new_id<MAX_CLIENTS;new_id++){
        if(clients[new_id].pid==-1) break;
    }

    if(new_id>=MAX_CLIENTS){
        fprintf(stderr,"Zbyt duza ilosc klientow na serwerze\n");
    }

    key_t client_queue_key=message->queue_key;
    int queue_id = msgget(client_queue_key,0);

    if(queue_id==-1){
        fprintf(stderr,"Blad podczas odczytu indentyfikatora kolejki\n");
    }

    clients[new_id].pid=client_pid;
    clients[new_id].queue_id=queue_id;
    clients[new_id].available=true;

    char msg[MAX_MESSAGE];
    sprintf(msg,"%d",new_id);
    struct msg* new_msg = calloc(1,sizeof(msg));
    new_msg->client_id = getpid();
    strcpy(new_msg->message,msg);
    new_msg->mtype=INIT;

    if(msgsnd(queue_id,new_msg,MSG_SIZE,0)==-1){
        fprintf(stderr,"Blad podczas wysylania identyfikatora do klienta\n");
    }
}

void connect(struct msg* message){
    int client_id = message->client_id;
    int second_id;
    sscanf(message->message,"%d",&second_id);
    if(second_id<0 || second_id>=MAX_CLIENTS || client_id==second_id){
        fprintf(stderr,"Bledny identyfikator kontaktowy\n");
        send_message(CONNECT,client_id,"-1");
        return;
    }
    if(clients[second_id].available==false || clients[second_id].pid==-1){
        fprintf(stderr, "Klient o  numerze ID %d jest nieosiagalny\n",second_id);
        send_message(CONNECT,client_id,"-2");
        return;
    }
    char msg[MAX_MESSAGE];
    char msg2[MAX_MESSAGE];
    int client_queue = clients[client_id].queue_id;
    int second_queue = clients[second_id].queue_id;
    sprintf(msg,"%d",second_queue);
    send_message(CONNECT,client_id,msg);
    sprintf(msg2,"%d:%d",client_id,client_queue);
    send_message(CONNECT,second_id,msg2);

    clients[client_id].available=false;
    clients[second_id].available=false;
}


void public_queue_handler(struct msg* message){
    if(message==NULL) return;
    switch(message->mtype){
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

    char* home_path = getenv("HOME");
    if(home_path==NULL){
        fprintf(stderr,"Nie udalo sie pobrac sciezki do katalogu $HOME\n");
        exit(0);
    }

    key_t main_key = ftok(home_path,PROJ_ID);
    if(main_key==-1){
        fprintf(stderr,"Nie udalo sie prawidlowo wygenerowac klucza glownego\n");
        exit(0);
    }

    main_queue_id=msgget(main_key, IPC_CREAT |IPC_EXCL | 0666);
    if(main_queue_id==-1){
        fprintf(stderr,"Nie udalo sie stworzyc kolejki glownej komunikatow\n");
        exit(0);
    }


    msg* bufor = calloc(1,sizeof(msg));
    while(1){
        if (msgrcv(main_queue_id, bufor, MSG_SIZE, -COMMANDS_COUNT, 0) == -1){
            fprintf(stderr,"Nie udalo sie odebrac komunikatu\n");
        }
        public_queue_handler(bufor);
    }
}
