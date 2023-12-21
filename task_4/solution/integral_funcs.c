#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include "integral.h"

const int N_POINTS = 1000000000;

const double X_START = 0, X_END = 1;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

double area = 0;

double func(double x)
{
    return x;
}

void *delta(void* arguments)
{
    struct args_struct args = *((struct args_struct*) arguments);

    double x, y;
    double s = 0;

    unsigned seed = (unsigned) time(NULL);

    for (int i = 0; i < args.n_points; i++)
    {
        x = ((double) rand_r(&seed)/RAND_MAX) * (args.end - args.start) + args.start;
        y = (double) rand_r(&seed)/RAND_MAX;

        if (y < args.func(x))
        {
            s += 1;
        }
    }

    double result = (args.end - args.start) * (s / args.n_points);

    pthread_mutex_lock(&m);
    area += result;
    pthread_mutex_unlock(&m);

    return NULL;
}

double calculate_time(int n_threads)
{
    struct timeval time_start;
    gettimeofday(&time_start, NULL);

    struct args_struct *arguments = (struct args_struct*) calloc(n_threads, sizeof(struct args_struct));

    double step = (X_END - X_START) / n_threads;
    
    for (int i = 0; i < n_threads; i++)
    {
        struct args_struct current;

        current.start = X_START + i * step;
        current.end = X_START + (i + 1) * step;
        current.func = &func;
        current.n_points = N_POINTS / n_threads;

        arguments[i] = current;
    }

    pthread_t *threads = (pthread_t*) calloc(n_threads, sizeof(pthread_t));
    
    for (int i = 0; i < n_threads; i++)
    {
        pthread_t current;
        pthread_create(&current, NULL, &delta, &arguments[i]);

        threads[i] = current;
    }
    
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    struct timeval time_end;
    gettimeofday(&time_end, NULL);

    free(arguments);
    free(threads);

    return (time_end.tv_sec - time_start.tv_sec) + (time_end.tv_usec - time_start.tv_usec) * 1e-6;
}