#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define SIGNAL SIGUSR1

void sig_handler(int sig_no){
    printf("Otrzymano sygnal %d\n",sig_no);
}

int main(int argc, char** argv[]){
    if(argc!=2){
        fprintf(stderr,"Nieprawidlowa liczba parametrow programu\n");
        return -1;
    }
    if(strcmp(argv[1],"ignore")==0){
        signal(SIGNAL,SIG_IGN);
    }
    else if(strcmp(argv[1],"handler")==0){
        signal(SIGNAL,sig_handler);
    }
    else if(strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
        sigset_t new_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGNAL);
        sigprocmask(SIG_BLOCK,&new_mask,NULL);
    }
    else{
        fprintf(stderr,"Podano nieprawidlowy argument\n lista mozliwych argumentow: ignore, handler, mask, pending\n");
        return -1;
    }

    raise(SIGNAL);

    sigset_t set_mask;

    if(strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
        sigpending(&set_mask);
        char* is_pending = calloc(10,sizeof(char));
        if(sigismember(&set_mask,SIGNAL)) is_pending="tak";
        else is_pending = "nie";
        printf("Sygnal oczekuje: %s\n",is_pending);
    }

    if(strcmp(argv[1],"ignore")==0 || strcmp(argv[1],"handler")==0 || strcmp(argv[1],"mask")==0){
        pid_t child = fork();
        if(child==0){
            raise(SIGNAL);
        }
        if(strcmp(argv[1],"mask")==0 && child==0){
            sigpending(&set_mask);
            char* is_pending = calloc(10,sizeof(char));
            if(sigismember(&set_mask,SIGNAL)) is_pending="tak";
            else is_pending = "nie";
            printf("Sygnal oczekuje: %s\n",is_pending);
        }
    }

    if(strcmp(argv[1],"pending")==0){
        pid_t child = fork();
        if(child==0){
            sigpending(&set_mask);
            char* is_pending = calloc(10,sizeof(char));
            if(sigismember(&set_mask,SIGNAL)) is_pending="tak";
            else is_pending = "nie";
            printf("Sygnal oczekuje: %s\n",is_pending);
        }
    }
    return 0;
}
