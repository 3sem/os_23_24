#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile int sigPid = -2;
int sender = 0;

void sighandler(int signo, siginfo_t *info, void *context)
{
    sigPid = info->si_value.sival_int;
    sender = info->si_pid;
}

int main()
{
    FILE *out = fopen("second.txt", "wb+");
    FILE *pid = fopen("pid", "wb+");
    printf("PID: %d\n", getpid());
    int p = getpid();
    fwrite(&p, sizeof(int), 1, pid);
    fclose(pid);
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    while(1)
    {
        sigaction(SIGRTMIN, &sa, NULL);
        pause();
        if(sigPid == -1){ kill(sender, SIGRTMIN); return 0; }
        printf("PID2: %d\n", sigPid);
        fwrite(&sigPid, sizeof(char), 1, out);
        kill(sender, SIGRTMIN);
    }
    return 0;
}
