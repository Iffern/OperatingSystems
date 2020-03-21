#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "filelib.h"

int sortLib(char * filepath, int number, int length){
    FILE* file = fopen(filepath,"r+");
    if(!file){
        fprintf(stderr,"Nie mozna otworzyc pliku!");
        return -1;
    }
    if(number==1){
        return 0;
    }
    int offset = (length+1)*sizeof(char);

    int result=quicksortLib(file,0,number-1,offset,length);

    if(result!=0){
        fprintf(stderr,"Nie udalo sie posortowac tablicy-blad w Quicksorcie: blad %d\n",result);
        return -1;
    }
    fclose(file);
    return 0;
}

int quicksortLib(FILE* file, int l, int r, int offset, int length){
    int first = l;
    int last = r;
    if(l<r){
        fseek(file,l*offset,0);
        char* pivot = malloc(offset);
        if(fread(pivot,sizeof(char),length+1,file)!= length+1) return 1;
        if(length>1024){
            pivot[1025]='\0';
        }
        fseek(file,l*offset,0);
        while(l<=r){
            char*left=malloc(offset);
            fseek(file,l*offset,0);
            if(fread(left,sizeof(char),length+1,file)!= length+1) return 8;
            if(length>1024){
                left[1025]='\0';
            }
            if(length==1){
                while(pivot[0] > left[0]){
                l+=1;
                fseek(file,l*offset,0);
                if(fread(left,sizeof(char),length+1,file)!= length+1) return 1;
                }
            }
            else{
             while(strcmp(pivot,left)>0){
                l+=1;
                fseek(file,l*offset,0);
                if(fread(left,sizeof(char),length+1,file)!= length+1) return 1;
                if(length>1024){
                left[1025]='\0';
            }
                }
                }
            char *right = malloc(offset);
            fseek(file,r*offset,0);
            if(fread(right,sizeof(char),length+1,file)!= length+1) return 2;
            if(length>1024){
                right[1025]='\0';
            }
            if(length==1){
            while(pivot[0]<right[0]){
                r-=1;
                fseek(file,r*offset,0);
                if(fread(right,sizeof(char),length+1,file)!= length+1) return 1;
                }
            }
            else{
             while(strcmp(pivot,right)<0){
                r=r-1;
                fseek(file,r*offset,0);
                if(fread(right,sizeof(char),length+1,file)!= length+1) return 3;
                if(length>1024){
                right[1025]='\0';
            }
                }
                }
            if(l<=r){
                fseek(file,l*offset,0);
                if(fread(left,sizeof(char),length+1,file)!= length+1) return 4;
                fseek(file,r*offset,0);
                if(fread(right,sizeof(char),length+1,file)!= length+1) return 5;
                fseek(file,l*offset,0);
                if(fwrite(right,sizeof(char),length+1,file)!=length+1) return 6;
                fseek(file,r*offset,0);
                if(fwrite(left,sizeof(char),length+1,file)!=length+1) return 7;
                l+=1;
                r-=1;
            }
            free(left);
            free(right);
        }
        quicksortLib(file,first,r,offset,length);
        quicksortLib(file,l,last,offset,length);
        return 0;
    }
}

int copyLib(char* source, char* destiny,int number,int length){
    FILE* sourceF = fopen(source,"r+");
    FILE* destinyF = fopen(destiny,"w+");
    if(sourceF==NULL || destinyF==NULL){
        fprintf(stderr,"Wystapil blad podczas odczytywania zrodla plikow\n");
        return 1;
    }
    char * bufor = malloc((length+1)*sizeof(char));
    for(int i=0;i<number;i++){
        if(fread(bufor,sizeof(char),length+1,sourceF)!=length+1) return 1;
        if(fwrite(bufor,sizeof(char),length+1,destinyF)!=length+1) return 1;
    }
    fclose(sourceF);
    fclose(destinyF);
    free(bufor);
    return 0;
}
