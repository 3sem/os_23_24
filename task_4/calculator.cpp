#include <cstdlib>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include "common.hpp"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

double Square = 0.0;

double f (double x) {

    return x * cos(x);
}

void* routine (void* args_);

int main (int argc, char* argv[]) {

    const double x_l = 0.0;
    const double x_r = 10.0;
    const double y_l = 0.0;
    const double y_r = 10.0;
    const double PT_CNT = 1e7;

    int threadCnt = 1;

    if (argc != 2) {

        perror ("Wrong number of args");
        exit (42);
    }
    else if (argc == 2) {

        threadCnt = atoi (argv[1]);
    }

    printf ("Starting test\nFunc : x * cos(x)\nThread count : %d\n", threadCnt);

    FILE* output = fopen (OUTPUT_FILE_NAME, "a");
    assert (output != NULL);

    double** args = (double**) calloc (threadCnt, sizeof (double*));
    assert (args != NULL);
    args[0] = (double*) calloc (threadCnt * 5, sizeof (double));
    assert (args[0] != NULL);
    for (int i = 0; i < threadCnt; i++) args[i] = args[0] + i * 5;

    pthread_t* thr_ids = (pthread_t*) calloc (threadCnt, sizeof (pthread_t));
    assert (thr_ids != NULL);

    for (int i = 0; i < threadCnt; i++) {

        args[i][0] = x_l + i * (x_r - x_l) / threadCnt;
        args[i][1] = y_l;
        args[i][2] = x_l + (i + 1) * (x_r - x_l) / threadCnt;
        args[i][3] = y_r;
        args[i][4] = PT_CNT / (double) threadCnt;
    }

    timeval start;
    gettimeofday (&start, NULL);

    for (int i = 0; i < threadCnt; i++)
        pthread_create (thr_ids + i, NULL, &routine, (void*) args[i]);

    for (int i = 0; i < threadCnt; i++)
        pthread_join (thr_ids[i], NULL);

    timeval finish;
    gettimeofday (&finish, NULL);

    printf ("Finished test\nSquare = %lf\n", Square);
    fprintf (output, "%d %lf\n", threadCnt, (finish.tv_sec - start.tv_sec) + (finish.tv_usec - start.tv_usec) * 1e-6);
}

void* routine (void* args_) {

    assert (args_ != NULL);

    double* args = (double*) args_;
    double x_l = args[0];
    double y_l = args[1];
    double x_r = args[2];
    double y_r = args[3];
    long long pts = floor (args[4]);
    // printf ("%ld\n", pts);

    double s = 0.0;

    unsigned int seed = (unsigned int) time(NULL);
    for (int i = 0; i < pts; i++) {

        double x = x_l + (double) rand_r (&seed) / RAND_MAX * (x_r - x_l);
        double y = y_l + (double) rand_r (&seed) / RAND_MAX * (y_r - y_l);

        if (y < f (x)) s+=1;
    }

    s = (s / (double) (pts)) * (x_r - x_l) * (y_r - y_l);

    pthread_mutex_lock (&mut);
    Square += s;
    pthread_mutex_unlock (&mut);

    return NULL;
}