#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

int sig_count = 0;
int sender;
bool sig_catching = true;

void sig_counter(int sig_no){
    sig_count++;
}

void usr2_handler(int sig_no, siginfo_t* info, void* ucontext){
    sender = info->si_pid;
    sig_catching = false;
}

int main(int argc, char** argv){

    if(argc!=2){
        fprintf(stderr,"Bledna ilosc argumentow funkcji\n");
        return -1;
    }

    printf("PID programu catcher: %d\n",getpid());

    if(strcmp(argv[1],"kill")==0 || strcmp(argv[1],"sigqueue")==0){
        struct sigaction action;

        action.sa_flags = SA_SIGINFO;

        action.sa_sigaction = usr2_handler;

        signal(SIGUSR1,sig_counter);
        sigaction(SIGUSR2,&action,NULL);
    }
    else if(strcmp(argv[1],"sigrt")==0){
        struct sigaction action;

        action.sa_flags = SA_SIGINFO;

        action.sa_sigaction = usr2_handler;

        signal(SIGRTMIN,sig_counter);
        sigaction(SIGRTMAX,&action,NULL);
    }
    else{
        fprintf(stderr,"Nieprawidlowy argument programu catcher; dozwolone argumenty: kill, sigqueue, sigrt\n");
        return -1;
    }

    while(sig_catching==true){
        pause();
    }

    if(strcmp(argv[1],"kill")==0){
        for(int i=0;i<sig_count;i++){
            kill(sender,SIGUSR1);
        }
        kill(sender,SIGUSR2);
    }
    else if(strcmp(argv[1],"sigqueue")==0){
        union sigval sigval;
        for(int i=0;i<sig_count;i++){
            sigval.sival_int = i+1;
            sigqueue(sender,SIGUSR1,sigval);
        }
        sigval.sival_int = sig_count;
        sigqueue(sender,SIGUSR2,sigval);
    }
    else if(strcmp(argv[1],"sigrt")==0){
        for(int i=0;i<sig_count;i++){
            kill(sender,SIGRTMIN);
        }
        kill(sender,SIGRTMAX);
    }

    printf("Liczba sygnalow odebranych przez catcher: %d\n",sig_count);

    return 0;
}
