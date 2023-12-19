#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main () {

    printf ("I'll sleep for next 5 seconds\n");

    sleep (5);

    printf ("Leaving with exit code 42\n");

    return 42;
}