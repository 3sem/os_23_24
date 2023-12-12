#include <stdlib.h>
#include <stdio.h>
#include "integral.h"

const int MAX_THREADS = 16;

int main()
{
    for (int n_threads = 1; n_threads <= MAX_THREADS; n_threads++)
    {
        printf("%d: %lf\n", n_threads, calculate_time(n_threads));
    }
}