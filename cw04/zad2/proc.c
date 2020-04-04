#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define SIGNAL SIGUSR1

int main(int argc, char** argv[]){
    if(strcmp(argv[1],"ignore")==0 || strcmp(argv[1],"mask")==0){
            raise(SIGNAL);
        }

    if(strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
            sigset_t set_mask;
            sigpending(&set_mask);
            char* is_pending = calloc(10,sizeof(char));
            if(sigismember(&set_mask,SIGNAL)) is_pending="tak";
            else is_pending = "nie";
            printf("Sygnal oczekuje: %s\n",is_pending);
        }

}
