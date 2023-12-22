#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Переменные для функции calculate

double Result = 0;

const double XStart = 0;
const double XEnd   = 1;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex для синхронизации в calculate

double function(double x) // Наша функция
{
    return x;
}

typedef struct function_info // Структура с параметрами для потоков
{
    double start;
    double end;

    double (*function)(double);

    int points_count;
} function_info;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void *monte_carlo(void* arguments) // Функция в которой поток будет считать свой интеграл
{
    function_info func_info = *((function_info *) arguments);

    double x = 0, y = 0, s = 0;

    unsigned seed = (unsigned) time(NULL);

    for (int i = 0; i < func_info.points_count; i++)
    {
        // Вычисляем точку методом Монте-Карло
        x = ((double) rand_r(&seed) / RAND_MAX) * (func_info.end - func_info.start) + func_info.start;
        y = (double) rand_r(&seed) / RAND_MAX;

        if (y < func_info.function(x))
            s += 1;
    }

    double result = (func_info.end - func_info.start) * (s / func_info.points_count);

    pthread_mutex_lock(&mutex);
    Result += result;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

double calculate(int threads_count, int max_point_number) // Функция, которая введет расчет результата и времени
{
    struct timeval time_start;
    gettimeofday(&time_start, NULL);

    function_info *arguments = (function_info*) calloc (threads_count, sizeof(function_info));

    double step = (XEnd - XStart) / threads_count;
    
    for (int i = 0; i < threads_count; i++)
    {
        arguments[i].start = XStart + i * step;
        arguments[i].end = XStart + (i + 1) * step;
        arguments[i].function = &function;
        arguments[i].points_count = max_point_number / threads_count;
    }

    pthread_t *threads = (pthread_t*) calloc (threads_count, sizeof(pthread_t));
    
    for (int i = 0; i < threads_count; i++)
        pthread_create(&threads[i], NULL, &monte_carlo, &arguments[i]);
    
    for (int i = 0; i < threads_count; i++)
        pthread_join(threads[i], NULL);

    struct timeval time_end;
    gettimeofday(&time_end, NULL);

    free(arguments);
    free(threads);

    return (time_end.tv_sec - time_start.tv_sec) + (time_end.tv_usec - time_start.tv_usec) * 1e-6;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
    int max_thread_number = 0;
    printf("Enter number of threads: ");
    scanf("%d", &max_thread_number);

    int max_point_number = 0;
    printf("Enter number of points: ");
    scanf("%d", &max_point_number);

    printf("\nXStart: %lf;\nXEnd: %lf;\n", XStart, XEnd);

    printf("\nThreads stepping with point count: %d;\n", max_point_number);

    for (int threads_count = 1; threads_count <= max_thread_number; threads_count++)
        printf("\nThreads number %d, Time: %lf s;\n", threads_count, calculate(threads_count, max_point_number));

    printf("\nMean result: %lf;\n", Result / max_thread_number);

    double mean_result = 0;
    int step_number = 0;

    printf("Points stepping with thread count: %d;\n", max_thread_number);

    for (int point_number = 10; point_number <= max_point_number; point_number += 100)
    {
        Result = 0;

        calculate(max_thread_number, point_number);

        mean_result += Result;
        step_number++;

        printf("\nPoints number: %d, Result: %lf;\n", point_number, Result);
    }

    printf("\nMean result: %lf;\n", mean_result / step_number);

    return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
