#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr,"Nieprawidlowa liczba argumentow/n");
        return -1;
    }

    char* stream_n = calloc(10,sizeof(char));
    strcpy(stream_n, argv[1]);

    char* out_file = calloc(10,sizeof(char));
    strcpy(out_file,argv[2]);

    int N = atoi(argv[3]);

    FILE* stream = fopen(stream_n,"r");
    FILE* result = fopen(out_file,"w+");

    char* bufor = calloc(N+1, sizeof(char));
    while(fread(bufor,1,N,stream)>0){
        fwrite(bufor,1,strlen(bufor),result);
        for(int i=0;i<strlen(bufor);i++){
            bufor[i]=0;
        }
    }

    fclose(stream);
    fclose(result);
    free(stream_n);
    free(out_file);
    free(bufor);
    return 0;
}
