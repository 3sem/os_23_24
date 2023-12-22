#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main ()
{
    pid_t pid = getpid();

    printf("Process id: '%d'\n", pid);

    while (1) {}

    return 0;
}