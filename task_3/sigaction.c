#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile int sigPid = -1;

void sighandler(int signo, siginfo_t *info, void *context)
{
    sigPid = info->si_vslue.sival_int;
}

int main()
{
    printf("PID: %d\n", getpid());
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    sigaction(SIGUSR1, &sa, NULL);
    pause();
    printf("PID2: %d\n", sigPid);
    return 0;
}
