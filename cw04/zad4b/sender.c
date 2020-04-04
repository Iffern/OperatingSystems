#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

int count_s = 0;
bool catched_last = false;

void sig_counter_s(int sig_no){
    count_s++;
}

void usr2_handler_s(int sig_no){
    catched_last = true;
}

void queue_info(int sig_no, siginfo_t* info, void* ucontext){
    count_s++;
    printf("Numer otrzymanego sygnalu: %d\n",info->si_value.sival_int);
}

void queue_info_2(int sig_no, siginfo_t* info, void* ucontext){
    catched_last = true;
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

        signal(SIGUSR1,sig_counter_s);

        for(int i=0;i<number_sig;i++){
            kill(catcher,SIGUSR1);
            while(count_s==i){
                pause();
            }
        }

        kill(catcher,SIGUSR2);
        signal(SIGUSR2,usr2_handler_s);
        while(catched_last==false){
            pause();
        }
    }
    else if(strcmp(mode,"sigqueue")==0){
        union sigval sigval;

        struct sigaction action1;
        action1.sa_flags = SA_SIGINFO;
        action1.sa_sigaction = queue_info;

        struct sigaction action2;
        action2.sa_flags = SA_SIGINFO;
        action2.sa_sigaction = queue_info_2;

        sigaction(SIGUSR1,&action1,NULL);
        sigaction(SIGUSR2,&action2,NULL);

        for(int i=0;i<number_sig;i++){
            sigval.sival_int = i+1;
            sigqueue(catcher,SIGUSR1,sigval);
            while(count_s==i){
                pause();
            }
        }
        sigval.sival_int = number_sig;

        sigqueue(catcher,SIGUSR2,sigval);

        while(catched_last==false){
            pause();
        }
    }
    else{
        fprintf(stderr,"Nieprawidlowy tryb programu sender; dozwolone tryby: kill, sigqueue\n");
        return -1;
    }

    printf("Sender powinien otrzymac %d sygnalow\n", number_sig);
    printf("Sender otrzymal %d sygnalow\n", count_s);

    return 0;
}

