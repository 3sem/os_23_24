#include <signal.h>
#include <stdio.h>
#include <errno.h>

#define SIZE 4096

void h(int signo){}

int main(int argc, const char **argv)
{   
    sleep(1);
    char data[4096];
    FILE *in = fopen("first.txt","rb");
    FILE *pid_u = fopen("pid", "rb");
    int PID[1];
    fread(PID, sizeof(int), 1, pid_u);
    printf("%d\n", PID[0]);
    
    union sigval val;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = h;
    int num;
    while((num = fread(data, sizeof(char), SIZE, in)) > 0)
    {
        for(int i = 0; i < num; i++)
        {
            val.sival_int = data[i];
            printf("data %d \n", data[i]);
            sigqueue(PID[0], SIGRTMIN, val);
            sigaction(SIGRTMIN, &sa, NULL);
            pause();
        }
    }
    val.sival_int = -1;
    sigqueue(PID[0], SIGRTMIN, val);

    return 0;
}