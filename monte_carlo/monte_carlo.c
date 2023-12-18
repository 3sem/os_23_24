#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_PTS 100000000

double func(double x);
void* square_of_one_square(void* arg);
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

double res_square = 0;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf ("Only two params\n");
    }

    // bounds
    double x1 = 0;
    double y1 = 0;
    double x2 = 3;
    double y2 = 3;
    int n_threads = atoi(argv[1]);
    pthread_t tids[n_threads];
    double args[n_threads][5];

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < n_threads; i++) {

        args[i][0] = (x2 - x1) / n_threads * i + x1;
        args[i][1] = y1;
        args[i][2] = (x2 - x1) / n_threads * (i + 1) + x1;
        args[i][3] = y2;
        args[i][4] = (double) NUM_PTS / n_threads;

        pthread_create(&tids[i], NULL, &square_of_one_square, args[i]);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(tids[i], NULL);
    }

    
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    printf("%lf;", (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6);

    //printf("%lf\n", res_square);

    return 0;
} 

double func(double x) {
    return x;
}

void* square_of_one_square(void* arg) {
    double* d_arg = (double*) arg;
    double x1 = d_arg[0];
    double y1 = d_arg[1];
    double x2 = d_arg[2];
    double y2 = d_arg[3];
    long long points_num = (long long) d_arg[4];

    int i;
    double square = 0.0;
    double x, y;
    unsigned int seed = time(NULL);


    for (i = 0; i < points_num; i++) {
        x = x1 + (double)rand_r(&seed)/ RAND_MAX * (x2 - x1);
        y = y1 + (double)rand_r(&seed)/ RAND_MAX * (y2 - y1);

        if (y < func(x)) 
            square += 1;
    }

    square = ((square / (double) points_num)) * (x2 - x1) * (y2 - y1);

    pthread_mutex_lock(&m);
    res_square += square;
    pthread_mutex_unlock(&m);

    return NULL;
}
