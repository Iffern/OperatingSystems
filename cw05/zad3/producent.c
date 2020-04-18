#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr,"Nieprawidlowa liczba argumentow\n");
        return -1;
    }

    char* stream_n = calloc(10,sizeof(char));
    strcpy(stream_n,argv[1]);

    char* file = calloc(20,sizeof(char));
    strcpy(file,argv[2]);

    int N = atoi(argv[3]);

    FILE* input = fopen(file,"r");
    if(input==NULL){
        fprintf(stderr,"Nie mozna otworzyc pliku %s\n",file);
        return -1;
    }

    FILE* stream = fopen(stream_n,"w");
    if(stream==NULL){
        fprintf(stderr,"Nie mozna otworzyc potoku\n");
        return -1;
    }

    char* bufor = calloc(N+1,sizeof(char));
    char* bufor_2 = calloc(50,sizeof(char));

    pid_t pid = getpid();

    while(fread(bufor,1,N,input)>0){
        sleep(rand()%3 + 1);
        sprintf(bufor_2,"#%d#%s\n",pid,bufor);
        fwrite(bufor_2,1,strlen(bufor_2),stream);

        for(int i=0;i<strlen(bufor);i++){
            bufor[i]=0;
        }

        for(int j=0;j<strlen(bufor_2);j++){
            bufor_2[j]=0;
        }
    }
    fclose(input);
    fclose(stream);
    free(bufor);
    free(bufor_2);
    free(file);
    free(stream_n);

    return 0;
}
