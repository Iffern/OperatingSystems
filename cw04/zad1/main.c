#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int awaiting = 0;

void handleSTP(int sig_no){
    if(awaiting==1){
        awaiting = 0;
        return;
    }
    printf("\nOczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    awaiting = 1;

}

void handleINT(int sig_no){
    printf("\nOdebrano sygnal SIGINT\n");
    exit(EXIT_SUCCESS);
}

int main(){
    struct sigaction action;
    action.sa_handler = handleSTP;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);

    sigaction(SIGTSTP, &action, NULL);
    signal(SIGINT, handleINT);

    char* comm = calloc(30,sizeof(char));
    comm = "ls -l";

    while(1){

        if(awaiting==0){
            system(comm);
            sleep(1);
        }
        else{
            pause();
        }
    }
    return 0;
}
