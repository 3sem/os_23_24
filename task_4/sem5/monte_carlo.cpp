#include <cstdlib>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

long long NUM_PTS = 1000000000;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
double SQUARE = 0;

void* one_square(void* args);

double func(double x) {
    return x;
}

int main(int argc, char** argv) {
    double x1  = 0.0;
    double y1 = 0.0;
    double x2 = 2.0;
    double y2 = 2.0;
    int n_thread;

    if (argc < 2)
        n_thread = 1;
    else
        n_thread = atoi(argv[1]);

    double args[n_thread][5];
    pthread_t tids[n_thread];

    for (int i = 0; i < n_thread; i++) {
        args[i][0] = x1 + i * (x2-x1) / n_thread;
        args[i][1] = y1;
        args[i][2] = x1 + (i+1) * (x2-x1) / n_thread;
        args[i][3] = y2;
        args[i][4] = (double)NUM_PTS / (double)n_thread;
    }

    timeval start_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < n_thread; i++) {
        pthread_create(&tids[i], NULL, &one_square, (void*)args[i]);
    }

    for (int i = 0; i < n_thread; i++) {
        pthread_join(tids[i], NULL);
    }

    timeval end_time;
    gettimeofday(&end_time, NULL);

    printf("Integral = %lf\n", SQUARE);
    printf("Elapsed time = %lf\n", (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6);


    return 0;
}

void* one_square(void* args) {
    double* arr = (double*) args;
    double x1 = arr[0];
    double y1 = arr[1];
    double x2 = arr[2];
    double y2 = arr[3];

    int points = (int)arr[4];
    double s = 0.0;
    double x,y;

    unsigned seed = (unsigned) time(NULL);
    for (int i = 0; i < points; i++) {
        x = x1 + (double) rand_r(&seed) / RAND_MAX * (x2 - x1);
        y = y1 + (double) rand_r(&seed) / RAND_MAX * (y2 - y1);
        if (y < func(x))
            s += 1;
    }

    s = (s / (double)(points)) * (x2 - x1) * (y2 - y1);

    pthread_mutex_lock(&m);
    SQUARE += s;
    pthread_mutex_unlock(&m);

    return NULL;
}
