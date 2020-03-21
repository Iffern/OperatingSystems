#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include <sys/stat.h>

void find_dir(char *path, bool maxdepth, int depth, bool atime, char mode_a, int count_a, bool mtime, char mode_m,int count_m){
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo_a;
    struct tm *timeinfo_m;
    time_t time_a;
    time_t time_m;
    bool time_m_flag = false;
    bool time_a_flag = false;

    if(mode_a!='0' && atime==true){
    timeinfo_a = localtime(&rawtime);
    date_plus_days(timeinfo_a,-count_a);
    if(mode_a=='+'){
    date_plus_days(timeinfo_a,-1);
    }
    time_a = mktime(timeinfo_a);
    }
    if(mode_m!='0' && mtime==true){
    timeinfo_m = localtime(&rawtime);
    date_plus_days(timeinfo_m,-count_m);
    if(mode_m=='+'){
    date_plus_days(timeinfo_m,-1);
    }
    time_m=mktime(timeinfo_m);
    }

    DIR* dir = opendir(path);
    if(dir==NULL){
        fprintf(stderr,"\nNie udalo sie otworzyc katalogu ze sciezki %s\n",path);
        return;
    }

    struct dirent* file;

    char * new_path = calloc(256,sizeof(char));


    while((file=readdir(dir))!=NULL){

        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);

        struct stat statistics;

        if(lstat(new_path,&statistics)<0){
            fprintf(stderr,"Nie udalo sie uzyskac statystyk dla %s\n",new_path);
        }

        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
            if(maxdepth==true && depth>0 && S_ISLNK(statistics.st_mode)==0){
                find_dir(new_path,maxdepth,depth-1,atime,mode_a,count_a,mtime,mode_m,count_m);
            }

            if(mtime==true){
                time_t modif_time = (time_t) statistics.st_mtim.tv_sec;
                int diff_days = difftime(time_m,modif_time)/(60*60*24);
                if(difftime(time_m,modif_time)<60*60*24) diff_days=0;
                if(mode_m=='+' && diff_days>0) time_m_flag=true;
                else if(mode_m=='-' && diff_days<0) time_m_flag=true;
                else if(mode_m=='=' && diff_days==0) time_m_flag=true;
            }
            if(atime==true){
                time_t modif_time = (time_t) statistics.st_atim.tv_sec;
                int diff_days = difftime(time_a,modif_time)/(60*60*24);
                if(difftime(time_a,modif_time)<60*60*24) diff_days=0;
                if(mode_a=='+' && diff_days>0) time_a_flag=true;
                else if(mode_a=='-' && diff_days<0) time_a_flag=true;
                else if(mode_a=='=' && diff_days==0) time_a_flag=true;
            }
            if(atime==time_a_flag && mtime==time_m_flag){
                print_statistics(new_path,&statistics);
            }
        }
    }
    closedir(dir);
}
