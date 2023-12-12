#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

#define BUF_SIZE (4096 / sizeof(int))

void handler(int signo, siginfo_t *info, void *context) 
{ 
    return; 
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "The argument should be name input file\n");

        return -1;
    }

    FILE *input = fopen(argv[1], "rb");

    if (!input)
    {
        fprintf(stderr, "Can't open input file\n");

        return -1;
    }

    FILE *pid_file = fopen("pid", "rb+");

    if (!input)
    {
        fprintf(stderr, "Can't open pid file\n");

        return -1;
    }

    int rec_pid; // Узнаем процесс получателя
    while(fread(&rec_pid, sizeof(int), 1, pid_file) != 1);
    fclose(pid_file);

    struct sigaction sa = {0};
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    sigaction(SIGINT, &sa, NULL); // Не обрабатываем SIGINT, т.к. handler ничего не делает

    union sigval val = {0};
    val.sival_int = getpid();
    sigqueue(rec_pid, SIGINT, val); // Отправляем SIGINT получателю, чтобы получатель смог узнать его pid

    int data[BUF_SIZE] = {0};
    size_t cnt_read = 0;

    while ((cnt_read = fread(data, sizeof(int), BUF_SIZE, input)) > 0)
    {
        for (size_t i = 0; i < cnt_read; i++)
        {
            val.sival_int = data[i];

            sigqueue(rec_pid, SIGUSR1, val); // Говорим получателю продолжать читать

            pause();
        }
    }

    kill(rec_pid, SIGUSR2); // Убиваем процесс получателя

    fclose(input);

    return 0;
}