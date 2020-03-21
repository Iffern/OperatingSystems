#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "filedir.h"
#include "filenftw.h"

char* remove_first_char(char*string){
    int size = strlen(string);
    char* new_string = calloc(size,sizeof(char));
    for(int i=1;i<size;i++){
        new_string[i-1]=string[i];
    }
    new_string[size-1]='\0';
    return new_string;
}

void date_plus_days( struct tm* date, int days )
{
    const time_t ONE_DAY = 24 * 60 * 60 ;
    time_t date_seconds = mktime( date ) + (days * ONE_DAY);
    *date =*localtime( &date_seconds );
}

void print_statistics(char* file, struct stat * statistics){
    char * type = calloc(50,sizeof(char));
    char * bufor = calloc(70,sizeof(char));

    if(S_ISDIR(statistics->st_mode)!=0) strcpy(type,"katalog");
    else if(S_ISCHR(statistics->st_mode)!=0) strcpy(type,"urzadzenie znakowe");
    else if(S_ISBLK(statistics->st_mode)!=0) strcpy(type,"urzadzenie blokowe");
    else if(S_ISREG(statistics->st_mode)!=0) strcpy(type,"zwykly plik");
    else if(S_ISFIFO(statistics->st_mode)!=0) strcpy(type,"potok nazwany");
    else if(S_ISLNK(statistics->st_mode)!=0) strcpy(type,"dowiazanie symboliczne");
    else if(S_ISSOCK(statistics->st_mode)!=0) strcpy(type,"socket");

    struct tm* time_m = localtime((const time_t *) &statistics->st_mtime);
    struct tm* time_a = localtime((const time_t *) &statistics->st_atime);

    printf("\nSciezka:  %s\n"
               "Liczba dowiazan:  %ld\n"
               "Rodzaj pliku:  %s\n"
               "Rozmiar:  %ld  bajtow\n" ,
               file,
               statistics->st_nlink,
               type,
               statistics->st_size);
        strftime (bufor, 50 ,"Ostatnia modyfikacja:  %d.%m.%Y\n", time_m);
        printf("%s", bufor);
        strftime (bufor, 50 ,"Ostatni dostep:  %d.%m.%Y\n", time_a);
        printf("%s", bufor);
        free(type);
        free(bufor);
}


int main(int argc, char **argv){
    int i=1;

    if(argc<2){
        fprintf(stderr,"Nieprawidlowa liczba argumentow!\n");
        return 1;
    }

    char* path = argv[i];
    i++;

    char* mode = argv[i];
    if(strcmp(mode,"dir")!=0 && strcmp(mode,"nftw")!=0){
        fprintf(stderr,"Dostepne tryby dzialania to dir i nftw\n");
        return 1;
    }
    i++;

    bool maxdepth = false;
    int depth =0;
    bool atime = false;
    char mode_a = '0';
    int count_a = 0;
    bool mtime = false;
    char mode_m = '0';
    int count_m = 0;

    while(i<argc){
        if(strcmp(argv[i],"-maxdepth")==0){
        if(i+1<argc){
            i++;
            char* count = argv[i];
            if(count[0]=='-'){
                fprintf(stderr,"Glebokosc poszukiwan musi byc dodatnia!\n");
                return 1;
            }
            maxdepth=true;
            depth = atoi(argv[i]);
            i++;
        }
        }
        else if(strcmp(argv[i],"-atime")==0){
            if(i+1<argc){
                i++;
                char* arg=argv[i];
                if(arg[0]=='-'){
                mode_a='-';
                count_a=atoi(remove_first_char(arg));
                }
                else if(arg[0]=='+'){
                mode_a='+';
                count_a=atoi(remove_first_char(arg));
                }
                else if(arg[0]>='0' && arg[0]<='9'){
                mode_a='=';
                count_a=atoi(arg);
                }
                else{
                fprintf(stderr,"Podano nieprawidlowy argument opcji atime\n");
                return 1;
                }
                atime=true;
                i++;
            }
            }
        else if(strcmp(argv[i],"-mtime")==0){
            if(i+1<argc){
                i++;
                char* arg=argv[i];
                if(arg[0]=='-'){
                mode_m='-';
                count_m=atoi(remove_first_char(arg));
                }
                else if(arg[0]=='+'){
                mode_m='+';
                count_m=atoi(remove_first_char(arg));
                }
                else if(arg[0]>='0' && arg[0]<='9'){
                mode_m='=';
                count_m=atoi(arg);
                }
                else{
                fprintf(stderr,"Podano nieprawidlowy argument opcji mtime\n");
                return 1;
                }
                mtime=true;
                i++;
            }
        }
        else{
            fprintf(stderr,"Podana opcja nie jest prawidlowa, dostepne opcje: maxdepth, mtime, atime\n");
        }
    }


        if(strcmp(mode,"dir")==0){
            find_dir(path,maxdepth,depth,atime,mode_a,count_a,mtime,mode_m,count_m);
        }
        else if(strcmp(mode,"nftw")==0){
            find_nftw(path,maxdepth,depth,atime,mode_a,count_a,mtime,mode_m,count_m);
        }

        return 0;
    }
