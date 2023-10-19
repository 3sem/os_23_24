#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_PTS 10000000

unsigned int seed = 0;

int NUM_THREADS = 10000;

typedef struct{
    double a;
    double step;
    double sum;
} pthrData;


double func(double x)
{
    return x;
}

void *solver(void *threadData) {
    pthrData *data = (pthrData*) threadData;
    int i;
    int points = NUM_PTS/NUM_THREADS;
    double s = 0.0;
    double x,y;
	for(i = 0; i < points; i++)
    {
        x = data->a + (double)rand_r(&seed)*(data->step)/(RAND_MAX);
        y = (double)rand_r(&seed)/(RAND_MAX);
        if(y < func(x)) s+=1;

    }
    s = (s/(double)(points)) * data->step;
    data->sum = s;
    return NULL;
}


int main()
{
    
    FILE* out = fopen("res.csv", "a+");
    perror("opening:");
    for(NUM_THREADS = 1; NUM_THREADS <= 10000; NUM_THREADS += 10)
    {
        double restime = 0;
        double midres = 0;
        for(int accuracy = 0; accuracy < 5; accuracy++)
        {
            struct timeval ts, te;
            double elapsed;
            gettimeofday(&ts, NULL);
            pthread_t thread[NUM_THREADS];
            double result = 0.0;
            seed = (unsigned)time(NULL);
            pthrData dat[NUM_THREADS];
            double length = 0.0;
            double step = 1.0 / NUM_THREADS;
            for(int i = 0; length < 1.0; i++)
            {
                dat[i].a = length;
                dat[i].step = step;
                length += step;
            }
            dat[NUM_THREADS-1].step = 1.0 - dat[NUM_THREADS-1].a;
            for(int i = 0; i < NUM_THREADS; i++)
            {
                pthread_create(&thread[i], NULL, solver, &dat[i]);
            }
	        for(int i = 0; i < NUM_THREADS; i++)
		        pthread_join(thread[i], NULL);
            for(int i = 0; i < NUM_THREADS; i++)
		        result += dat[i].sum;
            midres += result;
            gettimeofday(&te, NULL);
            elapsed = (te.tv_sec - ts.tv_sec) * 1000;
            elapsed += (te.tv_usec - ts.tv_usec) / 1000;
            restime += elapsed;
        }
        fprintf(out, "%d, %lf\n", NUM_THREADS, restime/5);
        perror("fprintf:");
        printf("%lf, %d, %lf\n", midres/5, NUM_THREADS, restime/5);
        
    }
    return 0;
}