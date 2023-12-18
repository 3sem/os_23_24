#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

#define BUF_SZ (4096 / sizeof(int))

void handler(int signo, siginfo_t *info, void *context) { return; }

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("The argument should be name input file\n");
        return 0;
    }

    FILE *input = fopen(argv[1], "rb");
    if (!input)
    {
        perror("fopen");
        return 0;
    }

    int rec_pid;

    FILE *pid_file = fopen("pid", "rb+");
    while(fread(&rec_pid, sizeof(int), 1, pid_file) != 1);
    fclose(pid_file);

    int data[BUF_SZ] = {0};

    union sigval val;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    sigaction(SIGINT, &sa, NULL);

    val.sival_int = getpid();
    sigqueue(rec_pid, SIGINT, val);

    size_t cnt_read = 0;
    while ((cnt_read = fread(data, sizeof(int), BUF_SZ, input)) > 0)
    {
        for (size_t i = 0; i < cnt_read; i++)
        {
            val.sival_int = data[i];
            sigqueue(rec_pid, SIGUSR1, val);
            pause();
        }
    }
    kill(rec_pid, SIGUSR2);

    fclose(input);
    return 0;
}