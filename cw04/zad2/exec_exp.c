#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define SIGNAL SIGUSR1

int main(int argc, char** argv[]){
    if(argc!=2){
        fprintf(stderr,"Nieprawidlowa liczba parametrow programu");
        return -1;
    }
    if(strcmp(argv[1],"ignore")==0){
        signal(SIGNAL,SIG_IGN);
    }
    else if(strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
        sigset_t new_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGNAL);
        sigprocmask(SIG_BLOCK,&new_mask,NULL);
    }
    else{
        fprintf(stderr,"Podano nieprawidlowy argument\n lista mozliwych argumentow: ignore, mask, pending\n");
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

    if(execl("./proc","./proc",argv[1],NULL)==-1){
        printf("Wystapil blad przy wykonywaniu programu proc\n");
    };

    return 0;
}
