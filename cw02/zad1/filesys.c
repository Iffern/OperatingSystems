#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "filesys.h"

#define SEEK_SET 0

int sortSys(char * filepath, int number, int length){
    int file = open(filepath,O_RDWR);
    if(file<0){
        fprintf(stderr,"Nie mozna otworzyc pliku!");
        return -1;
    }
    if(number==1){
        return 0;
    }
    int offset = (length+1)*sizeof(char);

    int result=quicksortSys(file,0,number-1,offset,length);

    if(result!=0){
        fprintf(stderr,"Nie udalo sie posortowac tablicy-blad w Quicksorcie: blad %d\n",result);
        return -1;
    }
    close(file);
    return 0;
}

int quicksortSys(int file, int l, int r, int offset,int length){
    int first = l;
    int last = r;
    if(l<r){
        lseek(file,l*offset,0);
        char* pivot = malloc(offset);
        if(read(file,pivot,offset)!= offset) return 1;
        if(length>1024){
                pivot[1025]='\0';
            }
        lseek(file,l*offset,0);
        while(l<=r){
            char*left=malloc(offset);
            lseek(file,l*offset,0);
            if(read(file,left,offset)!= offset) return 8;
            if(length>1024){
                left[1025]='\0';
            }
            if(length==1){
                while(pivot[0]>left[0]){
                l+=1;
                lseek(file,l*offset,0);
                if(read(file,left,offset)!= offset) return 1;
                }
                }
             else
             {while(strcmp(pivot,left)>0){
                l+=1;
                lseek(file,l*offset,0);
                if(read(file,left,offset)!= offset) return 1;
                if(length>1024){
                left[1025]='\0';
            }
                }
                }
            char *right = malloc(offset);
            lseek(file,r*offset,0);
            if(read(file,right,offset)!= offset) return 2;
            if(length>1024){
                right[1025]='\0';
            }
            if(length==1){
                while(pivot[0]<right[0]){
                r-=1;
                lseek(file,r*offset,0);
                if(read(file,right,offset)!= offset) return 1;
                }
                }
                else{
                while(strcmp(pivot,right)<0){
                r=r-1;
                lseek(file,r*offset,0);
                if(read(file,right,offset)!= offset) return 3;
                if(length>1024){
                right[1025]='\0';
            }
                }
                }
            if(l<=r){
                lseek(file,l*offset,0);
                if(read(file,left,offset)!= offset) return 4;
                lseek(file,r*offset,0);
                if(read(file,right,offset)!= offset) return 5;
                lseek(file,l*offset,0);
                if(write(file,right,offset)!=offset) return 6;
                lseek(file,r*offset,0);
                if(write(file,left,offset)!=offset) return 7;
                l+=1;
                r-=1;
            }
            free(left);
            free(right);
        }
        quicksortSys(file,first,r,offset,length);
        quicksortSys(file,l,last,offset,length);
        return 0;
    }
}

int copySys(char* source, char* destiny,int number,int length){
    int sourceDes = open(source,O_RDONLY);
    int destinyDes= open(destiny,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
    if(sourceDes<0 || destinyDes<0){
        fprintf(stderr,"Wystapil blad podczas odczytywania zrodla plikow\n");
        return 1;
    }
    char * bufor = malloc((length+1)*sizeof(char));
    for(int i=0;i<number;i++){
        if(read(sourceDes,bufor,(length+1)*sizeof(char))!=(length+1)*sizeof(char)) return 1;
        if(write(destinyDes,bufor,(length+1)*sizeof(char))!=(length+1)*sizeof(char)) return 1;
    }
    close(sourceDes);
    close(destinyDes);
    free(bufor);
    return 0;
}

