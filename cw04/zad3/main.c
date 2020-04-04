#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void divide_handler(int sig_no, siginfo_t* info, void* ucontext) {
    printf("Numer sygnalu: %d\n",info->si_signo);
    printf("Numer PID procesu wysylajacego: %d\n",info->si_pid);
    printf("Kod sygnalu: %d\n",info->si_code);
    if(info->si_code==1) printf("Probowano dzielic przez 0\n");
    exit(0);
}

void time_handler(int sig_no, siginfo_t* info, void* ucontext){
    printf("Numer sygnalu: %d\n",info->si_signo);
    printf("Numer PID procesu wysylajacego: %d\n",info->si_pid);
    printf("Czas systemowy: %f [s]\n Czas uzytkownika: %f [s]\n",(double) info->si_stime/sysconf(_SC_CLK_TCK),(double) info->si_utime/sysconf(_SC_CLK_TCK));
}

void exit_handler(int sig_no, siginfo_t* info, void* ucontext) {
    printf("Numer sygnalu: %d\n",info->si_signo);
    printf("Numer PID procesu wysylajacego: %d\n",info->si_pid);
    printf("Status wyjscia: %d\n",info->si_status);
}

int main(int argc, char** argv[]){
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    if(argc!=2){
        fprintf(stderr,"Nieprawidlowa liczba argumentow");
        return -1;
    }

    if(strcmp(argv[1],"divide")==0){
        action.sa_sigaction=divide_handler;
        sigaction(SIGFPE, &action, NULL);
        double a = 1/0;
    }
    else if(strcmp(argv[1],"time")==0){
        action.sa_sigaction=time_handler;
        sigaction(SIGCHLD, &action, NULL);

        pid_t child = fork();

        if(child==0){
            int a=1;
            for(int i=0;i<1000000;i++){
            FILE* file=fopen("test.txt","a+");
            fprintf(file,"%d\n",i);
            fprintf(file,"%d\n",i*2+i*3+i*4);
            fclose(file);
            }
        }
        wait(NULL);
    }
    else if(strcmp(argv[1],"exit")==0){
        action.sa_sigaction=exit_handler;
        sigaction(SIGCHLD, &action, NULL);

        pid_t child = fork();

        if(child==0){
            exit(21);
        }
        wait(NULL);
    }
    else{
        fprintf(stderr,"Dostepne scenariusze to divide, time i exit");
        return -1;
    }
    return 0;
}
