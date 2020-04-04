#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

int sig_count = 0;
int sender;
bool sig_catching = true;

void sig_counter(int sig_no, siginfo_t* info, void* ucontext){
    sender=info->si_pid;
    sig_count++;
    kill(sender,SIGUSR1);
}

void usr2_handler(int sig_no, siginfo_t* info, void* ucontext){
    sig_catching = false;
    kill(sender,SIGUSR2);
}

void sig_counter_qu(int sig_no, siginfo_t* info, void* ucontext){
    union sigval sigval;
    sender=info->si_pid;
    sig_count++;
    sigval.sival_int = sig_count;
    sigqueue(sender,SIGUSR1,sigval);
}

void usr2_handler_qu(int sig_no, siginfo_t* info, void* ucontext){
    union sigval sigval;
    sig_catching = false;
    sigval.sival_int = sig_count;
    sigqueue(sender,SIGUSR2,sigval);
}

int main(int argc, char** argv){

    if(argc!=2){
        fprintf(stderr,"Bledna ilosc argumentow funkcji\n");
        return -1;
    }

    printf("PID programu catcher: %d\n",getpid());

    if(strcmp(argv[1],"kill")==0){
        struct sigaction action;

        action.sa_flags = SA_SIGINFO;

        action.sa_sigaction = usr2_handler;

        struct sigaction action2;

        action2.sa_flags = SA_SIGINFO;

        action2.sa_sigaction = sig_counter;

        sigaction(SIGUSR1,&action2,NULL);
        sigaction(SIGUSR2,&action,NULL);
    }
    else if(strcmp(argv[1],"sigqueue")==0){
        struct sigaction action;

        action.sa_flags = SA_SIGINFO;

        action.sa_sigaction = usr2_handler_qu;

        struct sigaction action2;

        action2.sa_flags = SA_SIGINFO;

        action2.sa_sigaction = sig_counter_qu;

        sigaction(SIGUSR1,&action2,NULL);
        sigaction(SIGUSR2,&action,NULL);
    }
    else{
        fprintf(stderr,"Nieprawidlowy argument programu catcher; dozwolone argumenty: kill, sigqueue");
        return -1;
    }

    while(sig_catching==true){
        pause();
    }

    printf("Liczba sygnalow odebranych przez catcher: %d\n",sig_count);

    return 0;
}
