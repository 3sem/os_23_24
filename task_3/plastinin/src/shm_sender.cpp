#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>

#define KEY 1234
#define BUFFERSIZE 4096


int main() {
    sem_t* sem;
    sem = sem_open("/sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);

    int res = shmget(KEY, BUFFERSIZE, 0644 | IPC_CREAT);
    if (res == -1) {
        perror("shmget");
        return 1;
    }
    char data[BUFFERSIZE];
    void* ptr = shmat(res, NULL, 0);

    while (true) {
        int num = 1;
        sem_wait(sem);
        if (((char*) ptr)[BUFFERSIZE - 1] == '1') {
            num = fread(data, sizeof(char), BUFFERSIZE - 1, stdin);
            data[num] = '\0';
            sprintf((char*) ptr, "%s", data);
            ((char*) ptr)[BUFFERSIZE - 1] = '\0';
        }
        sem_post(sem);
        if (num == 0) {
            break;
        }
    }

    sem_wait(sem);
    sprintf((char*)ptr, "quit");
    sem_post(sem);

    shmdt(ptr);
    shmctl(KEY, IPC_RMID, NULL);

    return 0;
}
