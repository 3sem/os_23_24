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

        case SIGUSR1: // Записывает в файл
            fwrite(&(info->si_value.sival_int), sizeof(int), 1, output);
            break;

        case SIGUSR2: // Прекращает запись
            cond = 0;
            break;

        default: // Что-то пошло не так...
            fprintf(stderr, "Something went wrong, signo: %d\n", signo);
            return;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) // Нам нужно имя файла для хранения полученных данных
    {
        fprintf(stderr, "The argument should be name output file\n");
        
        return -1;
    }

    output = fopen(argv[1], "wb+");

    if (!output)
    {
        fprintf(stderr, "Can't open file\n");

        return -1;
    }

    int self_pid = getpid(); // Создаём файлик с id процесса получателся

    FILE *pid_file = fopen("pid", "wb+");
    fwrite(&self_pid, sizeof(int), 1, pid_file);
    fclose(pid_file);

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler; // Устанавливаем свой обработчик для определенных сигналов

    sigaction(SIGUSR1, &sa, NULL); // Говорим программе, что делать при обнаружении определенных сигналов
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);

    while (cond)
    {
        pause();

        kill(sender_pid, SIGINT);    
    }

    kill(sender_pid, SIGINT); // Посылаем сигнал остановки процессу отправителю

    fclose(output);

    return 0;
}