#include "filelib.h"
#include "filesys.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/times.h>
#include <time.h>

double periodOfTime(clock_t time1,clock_t time2){
    return ((double)(time1 - time2)/sysconf(_SC_CLK_TCK));
}


int generate(char* filename, int num_records, int length){
    FILE* file = fopen(filename,"w+");
    FILE* random_file = fopen("/dev/urandom","r");
    char* tmp = malloc((length+1)*sizeof(char));
    int i,j;
    for(i=0; i < num_records; i++){
        if(fread(tmp,sizeof(char),length+1,random_file) != length+1) return -1;
        for(j=0; j < length; j++) tmp[j] = abs(tmp[j] % 26) + 97;
        tmp[length] = '\n';
        if(fwrite(tmp,sizeof(char),length+1,file) != length+1) return -1;
    }
    free(tmp);
    fclose(file);
    fclose(random_file);
    return 0;
}

int main(int argc, char** argv){

    FILE* timeResult = fopen("wyniki.txt","a");

    struct tms* time1[argc];
    for(int i=0;i<argc;i++){
    time1[i]=calloc(1,sizeof(struct tms*));
    }

    if(argc<=1){
    printf("Dostepne polecenia: generate copy sort\n");
    return 0;
    }

    int currTime=0;

    int i=1;
    while(i<argc){
    times(time1[currTime]);
    currTime+=1;
    if(strcmp(argv[i],"generate")==0){
        if(i+3 > argc){
            printf("Podano za mala ilosc argumentow");
            return -1;}
        else{
        i+=1;
        char *file=argv[i];
        i+=1;
        int num_records = atoi(argv[i]);
        i+=1;
        int size=atoi(argv[i]);
        i+=1;
        int res = generate(file,num_records,size);
        if(res!=0){
            fprintf(stderr,"Nie udalo sie wygenerowac pliku\n");
            return -1;
        }
        }
    }
    else if(strcmp(argv[i],"sort")==0){
         if(i+4 > argc){
            printf("Podano za mala ilosc argumentow\n");
            return -1;}
        else{
        i+=1;
        char *file=argv[i];
        i+=1;
        int num_records = atoi(argv[i]);
        i+=1;
        int size=atoi(argv[i]);
        i+=1;
        char * option = argv[i];
        if(strcmp(option,"sys")!=0 && strcmp(option,"lib")!=0){
            printf("Mozliwymi opcjami sa sys i lib\n");
        }
        i+=1;
        int res=1;
        if(strcmp(option,"sys")==0){
        res = sortSys(file,num_records,size);}
        else if(strcmp(option,"lib")==0){
        res =sortLib(file,num_records,size);}
        if(res!=0){
            fprintf(stderr,"Nie udalo sie posortowac pliku\n");
            return -1;
        }
    }
    }
    else if(strcmp(argv[i],"copy")==0){
        if(i+5 > argc){
            printf("Podano za mala ilosc argumentow\n");
        }
        else{
        i+=1;
        char*file1 = argv[i];
        i+=1;
        char*file2 = argv[i];
        i+=1;
        int num_records = atoi(argv[i]);
        i+=1;
        int size=atoi(argv[i]);
        i+=1;
        char * option = argv[i];
        if(strcmp(option,"sys")!=0 && strcmp(option,"lib")!=0){
            printf("Mozliwymi opcjami sa sys i lib\n");
        }
        i+=1;
        int res=1;
        if(strcmp(option,"sys")==0){
        res = copySys(file1,file2,num_records,size);}
        else if(strcmp(option,"lib")==0){
        res =copyLib(file1,file2,num_records,size);}
        if(res!=0){
            fprintf(stderr,"Nie udalo sie skopiowac pliku\n");
            return -1;
        }
        }
    }
        times(time1[currTime]);

        printf("Czas systemowy: %f\n",periodOfTime(time1[currTime]->tms_stime,time1[currTime-1]->tms_stime));
        printf("Czas uzytkownika: %f\n",periodOfTime(time1[currTime]->tms_utime,time1[currTime-1]->tms_utime));

        fprintf(timeResult,"Czas systemowy: %f\n",periodOfTime(time1[currTime]->tms_stime,time1[currTime-1]->tms_stime));
        fprintf(timeResult,"Czas uzytkownika: %f\n",periodOfTime(time1[currTime]->tms_utime,time1[currTime-1]->tms_utime));
        currTime+=1;
}
    fclose(timeResult);
    return 0;
}
