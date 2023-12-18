#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUF_SIZE 4097

typedef struct Double_linked_pipe dlpipe;
int child_actions(dlpipe a);
int main_actions1(dlpipe a);
int main_actions2(dlpipe a);

struct Double_linked_pipe
{
    int fd_direct[2];
    int retd;
    int fd_back[2];
    int retb;
    FILE *f1, *f2;
};

int child_actions(dlpipe a)
{
    char buf[BUF_SIZE];

    read(a.fd_direct[0], buf, sizeof(buf) - 1);
    write(a.fd_back[1], buf, sizeof(buf) - 1);
    return 3;
}

int main_actions1(dlpipe a)
{
    char buf[BUF_SIZE];
    if (fread(buf, 1, sizeof(buf) - 1, a.f1) == 0)
    {
        return 4;
    };
    write(a.fd_direct[1], buf, sizeof(buf) - 1);
    return 2;
}

int main_actions2(dlpipe a)
{
    char buf[BUF_SIZE];

    read(a.fd_back[0], buf, sizeof(buf) - 1);
    fwrite(buf, 1, sizeof(buf) - 1, a.f2);
    return 1;
}

// void actions();

int main()
{
    dlpipe a;

    key_t key = ftok(".", 'R');                             // Генерация ключа для доступа к разделяемой памяти
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666); // Создание сегмента разделяемой памяти

    a.f1 = fopen("file.txt", "r");
    a.f2 = fopen("new_file.txt", "w");
    a.retb = pipe(a.fd_back);
    a.retd = pipe(a.fd_direct);

    int *flag = (int *)shmat(shmid, NULL, 0); // Присоединение сегмента разделяемой памяти
    *flag = 1;

    pid_t pid = fork();
    while (*flag != 4)
    {
        if (pid != 0)
        {
            // main process
            while (*flag != 1)
            {
                if (*flag == 4)
                {
                    break;
                };
            }
            *flag = main_actions1(a);
            if (*flag == 4)
            {
                wait(0);
                break;
            };
            while (*flag != 3)
            {
                if (*flag == 4)
                {
                    break;
                }
            };
            if (*flag == 4)
                break;
            *flag = main_actions2(a);
        }
        else
        {
            // child process
            while (*flag != 2)
            {
                if (*flag == 4)
                {
                    break;
                }
            };
            if (*flag == 4)
            {
                break;
            }
            *flag = child_actions(a);
        }
    }

    return 0;
}