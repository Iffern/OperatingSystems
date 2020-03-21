#include "difflib.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

double periodOfTime(clock_t time1,clock_t time2){
    return ((double)(time1 - time2)/sysconf(_SC_CLK_TCK));
}

int main(int argc, char** argv){

    FILE* timeResult = fopen("raport3b.txt","a");
    void * library = dlopen("./libdifflib.so", RTLD_LAZY);
    if(!library) fprintf(stderr,"Biblioteka nie zostala poprawnie otworzona");

    struct tms* time1[argc];
    clock_t time2[argc];
    for(int i=0;i<argc;i++){
    time1[i]=calloc(1,sizeof(struct tms*));
    time2[i]=0;
    }

    if(argc<=1){
    printf("Dostepne polecenia: create_table compare_pairs remove_block remove_operation\n");
    return 0;
    }

    struct MainTable* (*createMainTable)(int);
    createMainTable = (struct MainTable* (*)(int))dlsym(library,"createMainTable");
    struct BlockTable* (*createBlock)(int);
    createBlock = (struct BlockTable* (*)(int))dlsym(library,"createBlock");
    void (*diffFiles)(const char*,const char*, const char*);
    diffFiles = (void (*)(const char*,const char*, const char*))dlsym(library,"diffFiles");
    int (*parseTmpFile)(const char*, struct MainTable*);
    parseTmpFile = (int (*)(const char*, struct MainTable*))dlsym(library,"parseTmpFile");
    int (*numberOfOperations)(struct MainTable*, int);
    numberOfOperations = (int (*)(struct MainTable*, int))dlsym(library,"numberOfOperations");
    void (*removeEditingOperation)(struct MainTable*, int, int);
    removeEditingOperation = (void (*)(struct MainTable*, int, int))dlsym(library,"removeEditingOperation");
    void (*removeOperationBlock)(struct MainTable*, int);
    removeOperationBlock = (void (*)(struct MainTable*, int))dlsym(library,"removeOperationBlock");
    if(dlerror()!=NULL) printf("Blad przy wczytywaniu funkcji");

    struct MainTable* mainTab=createMainTable(atoi(argv[1]));

    int currTime=0;

    for(int i=2;i<argc;){

    time2[currTime]=times(time1[currTime]);
    currTime+=1;
    int counter=0;

    const char* sign = ":";
    if(strcmp(argv[i],"compare_pairs")==0){
        i++;
        while(i<argc && strchr(argv[i],':')!=NULL && counter<atoi(argv[1])){
            char* first_file=strtok(argv[i],sign);
            char* second_file=strtok(NULL,sign);
            diffFiles(first_file,second_file,"tmp.txt");
            parseTmpFile("tmp.txt",mainTab);
            i++;
            counter++;
        }
        if(counter==atoi(argv[1])){
        fprintf(stderr,"Podano zbyt duzo argumentow w stosunku do zadeklarowanej ilosci\n");
        return 1;
        }
        }
        else if(strcmp(argv[i],"create_table")==0){
            if(i+1<argc){
            int size = atoi(argv[i+1]);
            createMainTable(size);
            i+=2;}
            else{
                printf("Bledne argumenty funkcji\n");
                return 1;
            }
        }
        else if(strcmp(argv[i],"remove_block")==0){
            if(i+1<argc){
            int index = atoi(argv[i+1]);
            removeOperationBlock(mainTab,index);
            i+=2;}
            else{
                printf("Bledne argumenty funkcji\n");
                return 1;
            }
        }
        else if(strcmp(argv[i],"remove_operation")==0){
            if(i+2<argc){
            int blockIdx = atoi(argv[i+1]);
            int operationIdx = atoi(argv[i+2]);
            removeEditingOperation(mainTab,blockIdx,operationIdx);
            i+=3;}
            else{
                printf("Bledne argumenty funkcji\n");
                return 1;
            }
        }
        else{
        printf("Bledne argumenty funkcji\n");
        return 1;
        }

        time2[currTime]=times(time1[currTime]);

        printf("Czas rzeczywisty: %f\n",periodOfTime(time2[currTime],time2[currTime-1]));
        printf("Czas systemowy: %f\n",periodOfTime(time1[currTime]->tms_cstime,time1[currTime-1]->tms_cstime));
        printf("Czas uzytkownika: %f\n",periodOfTime(time1[currTime]->tms_cutime,time1[currTime-1]->tms_cutime));

        fprintf(timeResult,"Czas rzeczywisty: %f\n",periodOfTime(time2[currTime],time2[currTime-1]));
        fprintf(timeResult,"Czas systemowy: %f\n",periodOfTime(time1[currTime]->tms_cstime,time1[currTime-1]->tms_cstime));
        fprintf(timeResult,"Czas uzytkownika: %f\n",periodOfTime(time1[currTime]->tms_cutime,time1[currTime-1]->tms_cutime));
        currTime+=1;
}
fclose(timeResult);
dlclose(library);
return 0;
}
