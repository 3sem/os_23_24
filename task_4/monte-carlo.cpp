#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <math.h>

#include <sys/time.h>
#include <chrono>
#include <ctime>

double func(double x)
{
    return x*x;
}

double integral;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;



struct args_struct
{
    double begin;
    double end;
    double (*math_func)(double);
    unsigned points_amount;
};

void* f(void* arguments)
{
    args_struct args = *((args_struct*)arguments);

    double x, y;
    unsigned s = 0;

    unsigned seed = (unsigned) time(NULL);

    for (int i = 0; i < args.points_amount; i++)
    {
        x = ((double) rand_r(&seed)/RAND_MAX) * (args.end - args.begin) + args.begin;
        y = (double) rand_r(&seed)/RAND_MAX;

        if (y < args.math_func(x))
        {
            s += 1;
        }
    }

    double result = (args.end - args.begin) * (((double) s) / ((double) args.points_amount));

    // std::cout << "thread " << pthread_self() << ": s = " << s << ", result = " << result << "\n";

    pthread_mutex_lock(&m);
    integral += result;
    pthread_mutex_unlock(&m);

    return NULL;
}

double run_calculations(unsigned points_amount, unsigned threads_amount)
{
    double a = 0.0, b = 1.0;
    // unsigned points_amount = 1000000000;
    // unsigned threads_amount = 4;




    struct timeval time_now{};
    gettimeofday(&time_now, nullptr);
    time_t timer_begin = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);


    std::vector<args_struct> settings;

    double step_size = (b - a) / threads_amount;
    for (int i = 0; i < threads_amount; ++i)
    {
        args_struct tmp;
        tmp.begin = a + step_size * i;
        tmp.end = a + step_size * (i+1);
        tmp.math_func = &func;
        tmp.points_amount = points_amount / threads_amount;

        settings.push_back(tmp);
    }

    std::vector<pthread_t> threads;

    for (int i = 0; i < threads_amount; ++i)
    {
        pthread_t tmp;
        pthread_create(&tmp, NULL, &f, &settings[i]);

        threads.push_back(tmp);
    }

    while (threads.size() != 0)
    {
        pthread_join(threads.back(), NULL);
        threads.pop_back();
    }


    gettimeofday(&time_now, nullptr);
    time_t timer_end = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

    double time_spent = ((double)(timer_end - timer_begin))/1000;

    // std::cout << "\nintegral = " << integral << "\n";
    // std::cout << "time spent = " << time_spent << "\n\n";

    return time_spent;
}


int main()
{
    // unsigned points_amount = 1000000000;

    // std::cout << "Enter points amount: ";
    // std::cin >> points_amount;
    // std::cout << "Enter threads amount: ";
    // std::cin >> threads_amount;
    // std::cout << "points amount = " << points_amount << "\nthreads amount = " << threads_amount << "\n\n";

    std::cout << "threads: time:\n";
    for (int threads_amount = 1; threads_amount <= 20; ++threads_amount)
    {
        std::cout << threads_amount << " " << run_calculations(1000000000, threads_amount) << "\n";
    }
    return 0;
}
