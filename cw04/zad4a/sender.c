#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

int count_s = 0;
bool catching_s = true;

void sig_counter_s(int sig_no){
    count_s++;
}

void usr2_handler_s(int sig_no){
    catching_s = false;
}

void queue_info(int sig_no, siginfo_t* info, void* ucontext){
    count_s++;
    printf("Numer otrzymanego sygnalu: %d\n",info->si_value.sival_int);
}

void queue_info_2(int sig_no, siginfo_t* info, void* ucontext){
    catching_s = false;
    printf("W sumie catcher wyslal %d sygnalow\n",info->si_value.sival_int);
}

int main(int argc, char** argv){

    if(argc!=4){
        fprintf(stderr,"Bledna liczba argumentow wywolania programu\n");
        return -1;
    }

    int catcher = atoi(argv[1]);
    int number_sig = atoi(argv[2]);
    char *mode = calloc(20,sizeof(char));
    mode=argv[3];

    if(strcmp(mode,"kill")==0){
        for(int i=0;i<number_sig;i++){
            kill(catcher,SIGUSR1);
        }
        kill(catcher,SIGUSR2);
    }
    else if(strcmp(mode,"sigqueue")==0){
        union sigval sigval;
        for(int i=0;i<number_sig;i++){
            sigval.sival_int = i+1;
            sigqueue(catcher,SIGUSR1,sigval);
        }
        sigval.sival_int = number_sig;
        sigqueue(catcher,SIGUSR2,sigval);
    }
    else if(strcmp(mode,"sigrt")==0){
        for(int i=0;i<number_sig;i++){
            kill(catcher,SIGRTMIN);
        }
        kill(catcher,SIGRTMAX);
    }
    else{
        fprintf(stderr,"Nieprawidlowy tryb programu sender; dozwolone tryby: kill, sigqueue, sigrt\n");
        return -1;
    }



    if(strcmp(mode,"kill")==0){
        signal(SIGUSR1,sig_counter_s);
        signal(SIGUSR2,usr2_handler_s);
    }
    else if(strcmp(mode,"sigqueue")==0){
        struct sigaction action1;
        action1.sa_flags = SA_SIGINFO;
        action1.sa_sigaction = queue_info;

        struct sigaction action2;
        action2.sa_flags = SA_SIGINFO;
        action2.sa_sigaction = queue_info_2;

        sigaction(SIGUSR1,&action1,NULL);
        sigaction(SIGUSR2,&action2,NULL);
    }
    else if(strcmp(mode,"sigrt")==0){
        signal(SIGRTMIN,sig_counter_s);
        signal(SIGRTMAX,usr2_handler_s);
    }

    while(catching_s==true){
        pause();
    }

    printf("Sender powinien otrzymac %d sygnalow\n", number_sig);
    printf("Sender otrzymal %d sygnalow\n", count_s);

    return 0;
}
