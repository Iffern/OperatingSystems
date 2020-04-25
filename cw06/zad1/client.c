#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include "shared.h"

key_t queue_key;
int id;
int server_queue;
int client_q_id;



int init(){
    msg* message = calloc(1,sizeof(msg));
    message->client_id = getpid();
    message->mtype = INIT;
    message->queue_key=queue_key;

    if(msgsnd(server_queue,message,MSG_SIZE,0)==-1){
        fprintf(stderr,"Nie udalo sie wyslac wiadomosci inicjujacej\n");
    }

    struct msg* received = calloc(1,sizeof(msg));

    if(msgrcv(client_q_id,received,MSG_SIZE,INIT,0)==-1){
        fprintf(stderr,"Nie otrzymano wiadomosci zwrotnej z numerem id\n");
    }
    int received_id;
    sscanf(received->message,"%d",&received_id);
    return received_id;
}

void exit_handler_cl(int signo){
    msg* message = calloc(1,sizeof(msg));
    message->mtype=STOP;
    message->client_id=id;

    if(msgsnd(server_queue,message,MSG_SIZE,0)==-1){
        fprintf(stderr,"Nie udalo sie wyslac wiadomosci STOP na serwer\n");
    }
    if(msgctl(client_q_id,IPC_RMID,0)==-1){
        fprintf(stderr,"Nie udalo sie zamknac kolejki\n");
    }
    printf("Klient %d konczy prace\n",id);
    exit(0);
}

void comm_stop(){
    printf("Wykonywanie polecenia STOP\n");
    msg* message = calloc(1,sizeof(msg));
    message->client_id=id;
    message->mtype=STOP;
    if(msgsnd(server_queue,message,MSG_SIZE,0)==-1){
        fprintf(stderr,"Nie udalo sie wyslac polecenia STOP\n");
    }
    if(msgctl(client_q_id,IPC_RMID,0)==-1){
        fprintf(stderr,"Nie udalo sie zamknac kolejki\n");
    }
    printf("Klient %d konczy prace\n",id);
    exit(0);
}

void enter_chat(int second_id, int second_queue){
    printf("Rozpoczeto chat z uzytkownikiem %d\n",second_id);
    char* command=NULL;
    size_t length =0;
    size_t readed=0;
    msg* message = calloc(1,sizeof(msg));
    while(1){
        printf("Wyslij wiadomosc lub uzyj DISCONNECT, aby sie rozlaczyc: \n");

    if(msgrcv(client_q_id, message, MSG_SIZE, STOP, IPC_NOWAIT) >= 0){
        comm_stop();
    }

    if(msgrcv(client_q_id,message,MSG_SIZE, DISCONNECT,IPC_NOWAIT)>=0){
        printf("Twoj rozmowca sie rozlaczyl \n");
        return;
    }

    while(msgrcv(client_q_id, message, MSG_SIZE, 0, IPC_NOWAIT) >= 0) {
            printf("[%d]: %s\n", second_id, message->message);
    }

    readed = getline(&command,&length,stdin);
    command[readed-1]='\0';

    if(strcmp(command,"DISCONNECT")==0){
        message->client_id=id;
        message->mtype=DISCONNECT;
        char* bufor = calloc(20,sizeof(char));
        sprintf(bufor,"%d",second_id);
        strcpy(message->message,bufor);
        if(msgsnd(server_queue,message,MSG_SIZE,0)==-1){
            fprintf(stderr,"Nie udalo sie wyslac DISCONNECT na serwer\n");
        }
        printf("Wpisz polecenie: \n");
        return;
    }
    else if(strcmp(command,"")!=0){
        message->mtype=CONNECT;
        strcpy(message->message,command);
        if(msgsnd(second_queue,message,MSG_SIZE,0)==-1){
            fprintf(stderr,"Nie udalo sie wyslac wiadomosci do klienta %d\n",second_id);
        }
    }
    }
}

void comm_list(){
    printf("Wykonywanie polecenia LIST\n");
    msg* message = calloc(1,sizeof(msg));
    message->client_id=id;
    message->mtype=LIST;
    if(msgsnd(server_queue,message,MSG_SIZE,0)==-1){
        fprintf(stderr,"Nie udalo sie wyslac polecenia LIST\n");
    }
    if(msgrcv(client_q_id,message,MSG_SIZE,LIST,0)==-1){
        fprintf(stderr,"Nie otrzymano odpowiedzi zwrotnej na LIST\n");
    }
    printf("List aktywnych uzytkownikow: \n %s",message->message);
}

void comm_connect(char* second_id){
    printf("Wykonywanie polecenia CONNECT z uzytkownikiem %s\n",second_id);
    msg* message = calloc(1,sizeof(msg));
    message->client_id=id;
    message->mtype=CONNECT;
    strcpy(message->message,second_id);
    if(msgsnd(server_queue,message,MSG_SIZE,0)==-1){
        fprintf(stderr,"Nie udalo sie wyslac polecenia CONNECT z uztkownikiem %s\n",second_id);
    }
    if(msgrcv(client_q_id,message,MSG_SIZE,CONNECT,0)==-1){
        fprintf(stderr,"Nie otrzymano wiadomosci zwrotnej od serwera po poleceniu CONNECT z uzytkownikiem %s\n",second_id);
    }
    int second_queue = atoi(message->message);
    if(second_queue==-1){
        fprintf(stderr,"Bledny identyfikator kontaktowy\n");
        return;
    }
    else if(second_queue==-2){
        fprintf(stderr, "Klient o  numerze ID %s jest nieosiagalny\n",second_id);
        return;
    }
    enter_chat(atoi(second_id),atoi(message->message));
}


void get_server_communicat(){
    msg* message = calloc(1,sizeof(msg));
    if(msgrcv(client_q_id,message,MSG_SIZE,0,IPC_NOWAIT)>=0){
        if(message->mtype==CONNECT){
            int second_queue;
            int second_id;
            sscanf(message->message,"%d:%d",&second_id,&second_queue);
            if(second_queue<0){
                fprintf(stderr,"Otrzymano bledny numer kolejki do polaczenia \n");
            }
            enter_chat(second_id,second_queue);
        }
    }
}

int main(int argc, char** argv){
    srand(time(NULL));

    queue_key = ftok(getenv("HOME"), random()%255+1);

    client_q_id = msgget(queue_key,IPC_CREAT | 0666);

    char* home_path = getenv("HOME");
    if(home_path==NULL){
        fprintf(stderr,"Nie udalo sie pobrac sciezki do katalogu $HOME\n");
    }

    key_t server_key = ftok(home_path,PROJ_ID);
    if(server_key==-1){
        fprintf(stderr,"Nie udalo sie prawidlowo wygenerowac klucza glownego\n");
    }

    server_queue=msgget(server_key,0);
    if(server_queue<0){
        fprintf(stderr,"Problem z dostepem do kolejki serwera\n");
         exit(0);
    }

    id = init();

    signal(SIGINT,exit_handler_cl);

    char* command = NULL;
    size_t length = 0;
    size_t readed = 0;

    while(1){
        printf("Wpisz polecenie: \n");

        get_server_communicat();
        readed = getline(&command,&length,stdin);
        command[readed-1]='\0';

        if(strcmp(command,"")==0) continue;

        char* comm = strtok(command, " ");

        if(strcmp(comm,"LIST")==0) comm_list();
        else if(strcmp(comm,"CONNECT")==0){
            comm = strtok(NULL," ");
            comm_connect(comm);
        }
        else if(strcmp(comm,"STOP")==0) comm_stop();
        else{
            fprintf(stderr,"Niepoprawne polecenie\n");
        }
    }
    return 0;
}

