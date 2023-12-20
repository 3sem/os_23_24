#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "integral.h"

#define NUM_THREADS 4 // Количество потоков
#define NUM_POINTS 1000000 // Количество точек для метода Монте-Карло

double foo(double x) {
    x*x;
}

double integral = 0.0;
pthread_mutex_t mutex;

struct ThreadData {
    double x1;
    double x2;
};

void* monteCarlo(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    double x_range = data->x2 - data->x1;
    int points_per_thread = NUM_POINTS / NUM_THREADS;
    int points_count = 0;
    double partial_integral = 0.0;
    
    // Генерация случайных точек и вычисление частичного интеграла
    for (int i = 0; i < points_per_thread; i++) {
        double x = data->x1 + (double)rand() / RAND_MAX * x_range;
        double y = foo(x);
        
        if (y > 0) { // Проверка, принадлежит ли точка области под графиком
            points_count++;
            partial_integral += y;
        }
    }
    
    pthread_mutex_lock(&mutex);
    integral += partial_integral;
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

double monte_carlo(double x1, double x2) {
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];
    
    pthread_mutex_init(&mutex, NULL);
    
    double x_range = x2 - x1;
    
    // Создание потоков
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].x1 = x1;
        thread_data[i].x2 = x2;
        pthread_create(&threads[i], NULL, monteCarlo, &thread_data[i]);
    }
    
    // Ожидание завершения потоков
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double result = (integral / NUM_POINTS) * x_range;
    
    pthread_mutex_destroy(&mutex);
    
    return result;
}