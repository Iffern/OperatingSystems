#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "shared.h"

extern int errno ;

char* queue_name;
int id;
mqd_t server_queue;
mqd_t client_queue;



int init(){
    char* message = calloc(MAX_MESSAGE,sizeof(char));

    sprintf(message,"%s %d",queue_name,getpid());

    if(mq_send(server_queue,message,MAX_MESSAGE,INIT)==-1){
        fprintf(stderr,"Nie udalo sie wyslac wiadomosci inicjujacej\n");
    }

    unsigned int received_id;
    if(mq_receive(client_queue,message,MAX_MESSAGE,&received_id)==-1){
        fprintf(stderr,"Nie otrzymano wiadomosci zwrotnej z numerem id\n");
    }

    return received_id;
}

void exit_handler_cl(int signo){
    char* message = calloc(MAX_MESSAGE,sizeof(char));

    sprintf(message,"%d",id);

    if(mq_send(server_queue,message,MAX_MESSAGE,STOP)==-1){
        fprintf(stderr,"Nie udalo sie wyslac wiadomosci STOP na serwer\n");
    }
    if(mq_close(client_queue)==-1){
        fprintf(stderr,"Nie udalo sie zamknac kolejki\n");
    }

    if(mq_close(server_queue)==-1){
        fprintf(stderr,"Nie udalo sie zamknac kolejki serwera\n");
    }
    if(mq_unlink(queue_name)==-1){
        fprintf(stderr,"Nie udalo sie usunac kolejki z systemu\n");
    }
    printf("Klient %d konczy prace\n",id);
    exit(0);
}

void comm_stop(){
    printf("Wykonywanie polecenia STOP\n");
    char* message = calloc(MAX_MESSAGE,sizeof(char));

    sprintf(message,"%d",id);

    if(mq_send(server_queue,message,MAX_MESSAGE,STOP)==-1){
        fprintf(stderr,"Nie udalo sie wyslac polecenia STOP\n");
    }
    if(mq_close(client_queue)==-1){
        fprintf(stderr,"Nie udalo sie zamknac kolejki\n");
    }
    if(mq_unlink(queue_name)==-1){
        fprintf(stderr,"Nie udalo sie usunac kolejki z systemu\n");
    }
    printf("Klient %d konczy prace\n",id);
    exit(0);
}

void enter_chat(int second_id, mqd_t second_queue){
    printf("Rozpoczeto chat z uzytkownikiem %d\n",second_id);
    char* command=NULL;
    size_t length =0;
    size_t readed=0;
    char* message = calloc(MAX_MESSAGE,sizeof(char));

    while(1){
    printf("Wyslij wiadomosc lub uzyj DISCONNECT, aby sie rozlaczyc: \n");

    unsigned int type;
    struct mq_attr queue_info;
    mq_getattr(client_queue, &queue_info);

    if(queue_info.mq_curmsgs>0){
    if(mq_receive(client_queue,message,MAX_MESSAGE,&type)>=0){
        if(type==STOP){
            comm_stop();
        }
        else if(type==DISCONNECT){
            printf("Twoj rozmowca sie rozlaczyl\n");
            return;
        }
        else{
            printf("[%d]: %s\n", second_id, message);
            printf("Wyslij wiadomosc lub uzyj DISCONNECT, aby sie rozlaczyc: \n");
        }
    }
    }

    readed = getline(&command,&length,stdin);
    command[readed-1]='\0';

    if(strcmp(command,"DISCONNECT")==0){
        sprintf(message,"%d:%d",id,second_id);
        if(mq_send(server_queue,message,MAX_MESSAGE,DISCONNECT)==-1){
            fprintf(stderr,"Nie udalo sie wyslac DISCONNECT na serwer\n");
        }
        printf("Wpisz polecenie: \n");
        return;
    }
    else if(strcmp(command,"")!=0){
        strcpy(message,command);
        if(mq_send(second_queue,message,MAX_MESSAGE,CONNECT)){
            fprintf(stderr,"Nie udalo sie wyslac wiadomosci do klienta %d\n",second_id);
        }
    }
    }
}

void comm_list(){
    printf("Wykonywanie polecenia LIST\n");
    char* message = calloc(MAX_MESSAGE,sizeof(char));
    sprintf(message,"%d",id);
    if(mq_send(server_queue,message,MAX_MESSAGE,LIST)==-1){
        fprintf(stderr,"Nie udalo sie wyslac polecenia LIST\n");
    }
    unsigned int type;
    if(mq_receive(client_queue,message,MAX_MESSAGE,&type)==-1){
        fprintf(stderr,"Nie otrzymano odpowiedzi zwrotnej na LIST\n");
    }
    printf("List aktywnych uzytkownikow: \n %s",message);
}

void comm_connect(char* second_id){
    printf("Wykonywanie polecenia CONNECT z uzytkownikiem %s\n",second_id);
    char* message = calloc(MAX_MESSAGE,sizeof(char));
    sprintf(message,"%d:%d",id,atoi(second_id));
    if(mq_send(server_queue,message,MAX_MESSAGE,CONNECT)==-1){
        fprintf(stderr,"Nie udalo sie wyslac polecenia CONNECT z uztkownikiem %s\n",second_id);
    }
    unsigned int type;
    if(mq_receive(client_queue,message,MAX_MESSAGE,&type)==-1){
        fprintf(stderr,"Nie otrzymano wiadomosci zwrotnej od serwera po poleceniu CONNECT z uzytkownikiem %s\n",second_id);
    }
    char* second_queue_name = message;
    if(atoi(second_queue_name)==-1){
        fprintf(stderr,"Bledny identyfikator kontaktowy\n");
        return;
    }
    else if(atoi(second_queue_name)==-2){
        fprintf(stderr, "Klient o  numerze ID %s jest nieosiagalny\n",second_id);
        return;
    }
    mqd_t second_queue= mq_open(second_queue_name,O_RDWR);
    enter_chat(atoi(second_id),second_queue);
}


void get_server_communicat(){
    char* message = calloc(MAX_MESSAGE,sizeof(char));
    unsigned int type;

    struct mq_attr queue_info;
    mq_getattr(client_queue, &queue_info);
    if(queue_info.mq_curmsgs==0) return;

    if(mq_receive(client_queue,message,MAX_MESSAGE,&type)>=0){
        if(type==CONNECT){
            int second_id;
            char* second_queue_name = calloc(MAX_NAME,sizeof(char));
            sscanf(message,"%d:%s",&second_id,second_queue_name);
            mqd_t second_queue = mq_open(second_queue_name,O_RDWR);
            if(second_queue==-1){
                fprintf(stderr,"Bledny deskryptor kolejki\n");
            }
            enter_chat(second_id,second_queue);
        }
    }
}

char random_char(){
    return random()%('Z'-'A'+1)+'A';
}

char* generate_name(){
    char* name = calloc(MAX_NAME,sizeof(char));
    strcpy(name,"/CLIENT");
    for(int i=7;i<MAX_NAME-1;i++) name[i]=random_char();

    return name;
}

int main(int argc, char** argv){
    srand(time(NULL));

    queue_name=generate_name();

    server_queue = mq_open(SERVER_QUEUE_NAME,O_RDWR);
    if(server_queue<0){
        fprintf(stderr,"Nie mozna otworzyk kolejki serwera\n");
        exit(1);
    }

    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    queue_attr.mq_msgsize = MAX_MESSAGE;

    client_queue = mq_open(queue_name, O_RDWR | O_CREAT, 0666, &queue_attr);
    if(client_queue<0){
        fprintf(stderr,"Nie mozna otworzyc kolejki klienta\n");
        exit(1);
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
