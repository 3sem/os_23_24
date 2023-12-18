#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

FILE *output = NULL;
int sender_pid = 0;
unsigned cond = 1;

void handler(int signo, siginfo_t *info, void *context)
{
    switch (signo)
    {
    case SIGINT:
        sender_pid = info->si_value.sival_int;
        break;
    case SIGUSR1: //write
        fwrite(&(info->si_value.sival_int), sizeof(int), 1, output);
        break;
    case SIGUSR2: //end
        cond = 0;
        break;
    }
    return;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("The argument should be name ouput file\n");
        return 0;
    }

    output = fopen(argv[1], "wb+");
    if (!output)
    {
        perror("fopen");
        return 0;
    }

    int self_pid = getpid();

    FILE *pid_file = fopen("pid", "wb+");
    fwrite(&self_pid, sizeof(int), 1, pid_file);
    fclose(pid_file);

    union sigval val;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    while (cond)
    {
        pause();
        kill(sender_pid, SIGINT);    
    }
    kill(sender_pid, SIGINT);
    fclose(output);
    return 0;
}