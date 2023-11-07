#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <math.h>

#define N_POINTS 1000000 // Общее количество точек для генерации
#define N_THREADS 1// Число потоков

double foo(double x) {
    return x*x; // Возведение в квадрат
}

void *monte_carlo(void *param) {
    double *count = (double *)param;
    *count = 0;

    for (int i = 0; i < N_POINTS / N_THREADS; i++) {
        double x = (double)rand() / RAND_MAX; // Генерация случайной x-координаты
        double y = (double)rand() / RAND_MAX; // Генерация случайной y-координаты

        if (y <= foo(x)) {
            (*count)++;
        }
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N_THREADS];
    double counts[N_THREADS];

    srand(time(NULL));

    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, monte_carlo, (void *)&counts[i]);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double total_count = 0;
    for (int i = 0; i < N_THREADS; i++) {
        total_count += counts[i];
    }

    double integral = total_count / N_POINTS;

    printf("Значение интеграла: %f\n", integral);

    return 0;
}