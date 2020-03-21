#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include <ftw.h>

time_t time_a_G;
time_t time_m_G;
bool maxdepth_G;
int depth_G;
bool atime_G;
char mode_a_G;
bool mtime_G;
char mode_m_G;

int nftw_fn(const char *filepath, const struct stat *statistics, int typeflag, struct FTW *ftwbuf){
    bool time_m_flag = false;
    bool time_a_flag = false;

    if(maxdepth_G==true && ftwbuf->level > depth_G){
        return 0;
    }
            if(mtime_G==true){
                time_t modif_time = (time_t) statistics->st_mtime;
                int diff_days = difftime(time_m_G,modif_time)/(60*60*24);
                if(difftime(time_m_G,modif_time)<=60*60*24) diff_days=0;
                if(mode_m_G=='+' && diff_days>0) time_m_flag=true;
                else if(mode_m_G=='-' && diff_days<0) time_m_flag=true;
                else if(mode_m_G=='=' && diff_days==0) time_m_flag=true;
            }
            if(atime_G==true){
                time_t modif_time = (time_t) statistics->st_atime;
                int diff_days = difftime(time_a_G,modif_time)/(60*60*24);
                if(difftime(time_a_G,modif_time)<=60*60*24 && diff_days>=0) diff_days=0;
                if(mode_a_G=='+' && diff_days>0) time_a_flag=true;
                else if(mode_a_G=='-' && diff_days<0) time_a_flag=true;
                else if(mode_a_G=='=' && diff_days==0) time_a_flag=true;
            }
            if(atime_G==time_a_flag && mtime_G==time_m_flag){
                print_statistics(filepath,statistics);
            }
        return 0;
}


void find_nftw(char *path, bool maxdepth, int depth, bool atime, char mode_a, int count_a, bool mtime, char mode_m,int count_m){
    int flag = FTW_PHYS;
    int limit = 10;
    maxdepth_G = maxdepth;
    depth_G = depth;
    atime_G = atime;
    mode_a_G = mode_a;
    mtime_G = mtime;
    mode_m_G = mode_m;


    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo_a;
    struct tm *timeinfo_m;

    if(mode_a!='0' && atime==true){
    timeinfo_a = localtime(&rawtime);
    date_plus_days(timeinfo_a,-count_a);
    if(mode_a=='+'){
    date_plus_days(timeinfo_a,-1);
    }
    time_a_G = mktime(timeinfo_a);
    }
    if(mode_m!='0' && mtime==true){
    timeinfo_m = localtime(&rawtime);
    date_plus_days(timeinfo_m,-count_m);
    if(mode_m=='+'){
    date_plus_days(timeinfo_m,-1);
    }
    time_m_G=mktime(timeinfo_m);
    }

    nftw(path,nftw_fn,limit,flag);

}
