#include <signal.h>
#include <stdio.h>

int main(int argc, const char **argv)
{   
    union sigval val;
    val.sival_int = 5;
    sigqueue(atoi(argv[1]), SIGUSR1, val);
    //kill(-1, SIGKILL);


    return 0;
}