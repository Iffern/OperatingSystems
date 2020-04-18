#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
        if(argc!=2){
           fprintf(stderr,"Nieprawidlowa liczba argumentow\n");
           return -1;
        }
        FILE * file = fopen(argv[1],"r");
        if(file==NULL){
            fprintf(stderr,"Bledna nazwa pliku: %s\n",argv[1]);
            return -1;
        }
        char * line = calloc(500,sizeof(char));
        FILE * sort = popen("sort","w");
        while(fgets(line,500,file)!=NULL){
            fputs(line,sort);
        }
        pclose(sort);
        fclose(file);

        return 0;
}
