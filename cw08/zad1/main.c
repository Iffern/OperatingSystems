#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#define M 255

int num_of_threads;
int** image;
int* histogram;
int width;
int height;

suseconds_t time_diff(struct timeval time1, struct timeval time2){
     return (time2.tv_sec*1e6+time2.tv_usec - time1.tv_usec-time1.tv_sec*1e6);
}

void read_image(char* image_name){
    FILE* image_file = fopen(image_name,"r");
    if(image_file==NULL){
        fprintf(stderr,"Nie udalo sie otworzyc pliku z obrazem\n");
        exit(2);
    }

    char* bufor = NULL;
    size_t n = 0;

    int line_number= 0;
    int pixel_number = 0;

    while(getline(&bufor,&n,image_file)!=-1){
        if(line_number==1){
            char* size = strtok(bufor," ");
            width = atoi(size);
            size = strtok(NULL," \n");
            height = atoi(size);

            image = calloc(width+1, sizeof(int*));
            for(int i=0;i<width;i++){
                image[i] = calloc(height+1,sizeof(int));
            }
        }
        if(line_number>3){
            char* pixel = strtok(bufor," ");
            while(pixel!=NULL){
                image[pixel_number%width][pixel_number/height] = atoi(pixel);
                pixel_number++;
                pixel = strtok(NULL, " \n");
            }
        }
        line_number++;
    }
}

void* get_histogram_sign(void* num){
    int k = *((int*) num);
    struct timeval start;
    gettimeofday(&start,NULL);

    int start_int = (k-1)*ceil((M+1)/num_of_threads);
    int end_int = fmin(k*ceil((M+1)/num_of_threads)-1,M);

    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            if(image[i][j]>=start_int && image[i][j]<=end_int){
                histogram[image[i][j]]++;
            }
        }
    }

    struct timeval stop;
    gettimeofday(&stop,NULL);
    long* diff  = calloc(1, sizeof(long));
    *diff = time_diff(start,stop);
    pthread_exit((void*) diff);
}

void* get_histogram_block(void* num){
    int k = *((int*) num);
    struct timeval start;
    gettimeofday(&start,NULL);

    int start_x = (k-1)*ceil(width/num_of_threads);
    int end_x = fmin(k*ceil(width/num_of_threads)-1,width-1);

    for(int i=start_x;i<=end_x;i++){
        for(int j=0;j<height;j++){
                histogram[image[i][j]]++;
            }
        }

    struct timeval stop;
    gettimeofday(&stop,NULL);
    long* diff  = calloc(1, sizeof(long));
    *diff = time_diff(start,stop);
    pthread_exit((void*) diff);
}

void* get_histogram_interleaved(void* num){
    int k = *((int*) num);
    struct timeval start;
    gettimeofday(&start,NULL);

    int start_point=k-1;

    for(int i=start_point;i<width;i+=num_of_threads){
        for(int j=0;j<height;j++){
                histogram[image[i][j]]++;
            }
        }

    struct timeval stop;
    gettimeofday(&stop,NULL);
    long* diff  = calloc(1, sizeof(long));
    *diff = time_diff(start,stop);
    pthread_exit((void*) diff);
}

void save_result(char* result_name){
    FILE* result = fopen(result_name,"w+");
    if(result==NULL){
        fprintf(stderr, "Nie udalo sie otworzyc pliku wynikowego\n");
    }

    fprintf(result, "Histogram:\n");
    for(int i=0;i<=M;i++){
        fprintf(result,"%d %d\n",i,histogram[i]);
    }
}

int main(int argc, char** argv){

    if(argc!=5){
        fprintf(stderr,"Niewlasciwa liczba argumentow programu\n");
        exit(1);
    }

    char* option = calloc(15,sizeof(char));
    char* image_file = calloc(15,sizeof(char));
    char* result_file = calloc(15,sizeof(char));

    num_of_threads=atoi(argv[1]);
    option = argv[2];
    image_file = argv[3];
    result_file = argv[4];

    read_image(image_file);

    histogram = calloc(M+2,sizeof(int));

    long* times = calloc(num_of_threads+1,sizeof(long));

    struct timeval start;
    gettimeofday(&start, NULL);

    pthread_t* pthreads_id = calloc(num_of_threads, sizeof(pthread_t));
    void* function;
    if(strcmp(option,"sign")==0) function = get_histogram_sign;
    else if(strcmp(option,"block")==0) function = get_histogram_block;
    else if(strcmp(option,"interleaved")==0) function = get_histogram_interleaved;
    else{
        fprintf(stderr,"Dostepne opcje: sign, block, interleaved \n");
        exit(3);
    }

    for(int i=0;i<num_of_threads;i++){
        int* k = calloc(1,sizeof(int));
        *k = i+1;
        pthread_create(&pthreads_id[i],NULL,function,(void *) k);
    }

    for(int i=0;i<num_of_threads;i++){
        void* return_val;
        if(pthread_join(pthreads_id[i],&return_val)!=0){
            fprintf(stderr,"Nie udalo sie pobrac wartosci zwroconej przez watek %d",i+1);
        }
        times[i]=*((long*) return_val);
    }

    struct timeval end;
    gettimeofday(&end, NULL);

    FILE * time_file = fopen("Times.txt","a+");

    printf("Liczba watkow: %d\nWariant: %s\n",num_of_threads,option);
    fprintf(time_file,"Liczba watkow: %d\nWariant: %s\n",num_of_threads,option);

    for(int i=0;i<num_of_threads;i++){
        printf("Identyfiaktor watku: %ld, czas spedzony na wykonaniu zadania: %ld us\n",pthreads_id[i],times[i]);
        fprintf(time_file,"Identyfiaktor watku: %ld, czas spedzony na wykonaniu zadania: %ld us\n",pthreads_id[i],times[i]);
    }

    printf("Calkowity czas spedzony na wykonaniu zadania: %ld us\n",time_diff(start,end));
    fprintf(time_file,"Calkowity czas spedzony na wykonaniu zadania: %ld us\n",time_diff(start,end));

    save_result(result_file);

    for(int i=0;i<width;i++) free(image[i]);
    free(image);
    free(histogram);

    return 0;
}
