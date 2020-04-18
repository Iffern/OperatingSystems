#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

const int MAX_LINE = 500;
const int MAX_COMM = 7;
const int MAX_ARG = 7;
const int MAX_ARG_L = 50;



void excecute_line(char*** comm_line){
    pid_t* pids = calloc(MAX_COMM,sizeof(pid_t));

    int fd[2];
    pipe(fd);

    pid_t child = fork();
    if(child==0){
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(comm_line[0][0], comm_line[0]);
    }
    pids[0]=child;

    int prev_fd[2];
    prev_fd[0] = fd[0];
    prev_fd[1] = fd[1];

    int index=1;
    while(comm_line[index]!=NULL){
        prev_fd[0] = fd[0];
        prev_fd[1] = fd[1];
        pipe(fd);

        pid_t child = fork();
        if(child==0){
            dup2(prev_fd[0],STDIN_FILENO);
            if(comm_line[index+1]!=NULL){
            dup2(fd[1],STDOUT_FILENO);}
            close(fd[0]);
            close(fd[1]);
            close(prev_fd[0]);
            close(prev_fd[1]);
            execvp(comm_line[index][0],comm_line[index]);
        }
        close(prev_fd[0]);
        close(prev_fd[1]);
        pids[index]=child;
        index++;
    }

    for(int i=0;i<index;i++){
        waitpid(pids[i],NULL,0);
    }

    close(fd[0]);
    close(fd[1]);
    free(pids);
}


int main(int argc, char **argv){
    if(argc!=2){
        fprintf(stderr,"Nieprawidlowa ilosc argumentow programu\n");
        return -1;
    }

    FILE * file = fopen(argv[1],"r");
    if(file==NULL){
        fprintf(stderr,"Nie udalo sie otworzyc pliku\n");
        return -1;
    }

    char * line = calloc(MAX_LINE,sizeof(char));

    while(fgets(line,MAX_LINE,file)!=NULL){
        line[strcspn(line,"\n")]=0;
        char *** comm_list = calloc(MAX_COMM,sizeof(char**));

        char* bufor_1;
        char* command = strtok_r(line,"|",&bufor_1);

        int index = 0;
        while(command!=NULL){
        char* bufor_2;
        char* arg = strtok_r(command," ",&bufor_2);
        int a_index = 0;
        comm_list[index]=calloc(MAX_ARG,sizeof(char));
        while(arg!=NULL){

            comm_list[index][a_index]=calloc(MAX_ARG_L,sizeof(char));
            strcpy(comm_list[index][a_index],arg);
            arg = strtok_r(NULL," ",&bufor_2);
            a_index++;
        }
        command=strtok_r(NULL,"|",&bufor_1);
        index++;
        }


        excecute_line(comm_list);


        int i=0;
        while(comm_list[i]!=NULL){
            int j=0;
            while(comm_list[i][j]!=NULL){
                free(comm_list[i][j]);
                j++;
            }
             free(comm_list[i]);
            i++;
        }
        free(comm_list);
    }

    free(line);
    fclose(file);
    return 0;
}
