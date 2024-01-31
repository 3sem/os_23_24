
#include <stdio.h> 
#include <pthread.h> 
#include <math.h> 
#include <stdlib.h> 
#include <time.h> 
#include <unistd.h> 
 
#define NUM_PTS 10000000 
 
#define eps 0.00000001 
 
#define UNIQUE_NAME  
 
#define BILLION  1000000000L 
 
typedef struct data { 
    double a; 
    double b; 
    double s; 
    double points; 
} data; 
 
double func(double x) { 
    return x; 
} 
 
void *sq_cell (void *arg) { 
 
    data *cell_data = (data *)arg; 
    double a = cell_data->a; 
    double b = cell_data->b; 
    double points = cell_data->points;  
    double s = 0; 
    double x; 
    double y; 
    //printf("thread %lf %lf %lf %lf\n", a, b, s, points); 
     
    srand((unsigned)time(NULL)); 
    for (int i = 0; i < points; i++)  
    { 
        x = (double)rand()/RAND_MAX*(b-a)+a; //генерируем x в интервале [0,1] 
        y = (double)rand()/RAND_MAX*2; //генерируем y в интервале [0,2] 
        if(fabs((double)y)-func(x) < 0) s += 1; 
        //printf("%lf %lf %lf %lf %lf\n", s, x, y, a, b); 
    } 
 
    s=(s/points) * (b-a) * (b-a) * NUM_PTS / points; 
 
    cell_data->s = s; 
 
    //printf("thread %lf %lf %lf\n", a, b, s); 
 
    //pthread_exit(NULL); 
    return NULL; 
} 
 
/*bool thread_exists (pthread_t thread_id) 
{ 
    char path[100]; 
    char thread_name[16]; 
    FILE *fp; 
    bool  thread_exists = false; 
 
    // If the /proc/<pid>/task/<lwp>/comm file exists and it's contents match the "unique name" the 
    // thread exists, and it's the original thread (TID has NOT been reused). 
 
    sprintf(path, "/proc/%d/task/%d/comm", getpid(), subthread_lwp); 
 
    fp = fopen(path, "r"); 
 
    if( fp != NULL ) { 
 
        fgets(thread_name, 16, fp); 
        fclose(fp); 
 
        // Need to trim off the newline 
        thread_name[strlen(thread_name)-1] = '\0'; 
 
        if( strcmp(UNIQUE_NAME, thread_name) == 0 ) { 
            thread_exists = true; 
        } 
    } 
 
    if( thread_exists ) { 
        printf("thread exists\n"); 
    } else { 
        printf("thread does NOT exist\n"); 
    } 
 
    return thread_exists; 
} 
*/ 
int main(int argc, char *argv[]) { 
    int j; 
    FILE* fdata = fopen(argv[2], "a"); 
 
    for (j = 1; j <= atoi(argv[1]); j++) { 
        for (int l = 0; l < 6; l++) { 
 
            int n_th = j; 
            printf("%d\n", n_th); 
            double a = 0.0; 
            double b = 2.0; 
            int i; 
            double sq = 0.0; 
            pthread_t thread[n_th]; 
            data st_data[n_th]; 
 
             
 
            struct timespec start, stop; 
            clock_gettime( CLOCK_MONOTONIC, &start); 
 
            for (i = 0; i < n_th; i++) { 
 
                st_data[i].a = (double)2/n_th*i; 
                st_data[i].b = (double)2/n_th*(i+1); 
                st_data[i].s = 0.0; 
                st_data[i].points = (double)NUM_PTS/n_th; 
                int check = pthread_create(&thread[i], NULL, sq_cell, (void *)&(st_data[i])); 
                 
                //usleep(1000); 
                //while (!thread_exists(thread[i])); 
        //        sleep(1); 
                //printf("%d %lf\n", i, st_data[i].s); 
 
            } 
            //printf("1\n"); 
            for (i = 0; i < n_th; i++) { 
             
                int check2 = pthread_join(thread[i], NULL); 
                 
                //printf("%d\n", check2); 
                sq += st_data[i].s; 
                //printf("%lf %lf\n", sq, st_data[i].s); 
            } 
            //printf("2\n"); 
 
            clock_gettime( CLOCK_MONOTONIC, &stop); 
            printf("%d %lf\n", n_th, ( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec ) /BILLION); 
            fprintf(fdata, "%d %lf\n", n_th, ( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec ) /BILLION); 
        } 
    } 
 
    fclose(fdata); 
 
    return 0; 
}
