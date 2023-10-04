#include <stdio.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

#define KEY 1234
#define BUFFERSIZE 4096

int shm_receiver() {
    timeval start_time;
    gettimeofday(&start_time, NULL);

    FILE* output = fopen("output_shm", "wb+");
    FILE* data = fopen("data_shm.csv", "a");

    sem_t* sem;
    sem = sem_open("/sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    sem_post(sem);
    int res = shmget(KEY, BUFFERSIZE, 0644 | IPC_CREAT);
    if (res == -1) {
        perror("shmget");
        return 1;
    }
    void* ptr = shmat(res, NULL, 0);

    // sem_wait(&sem);
    ((char*) ptr)[0] = '\0';


    while (true) {
        sem_wait(sem);
        if (strcmp((char*)ptr, "quit") == 0)
            break;
        if (((char*)ptr)[BUFFERSIZE - 1] == '\0') {
            int len = strlen((char*) ptr);
            fwrite((char*)ptr, sizeof(char), len, output);
            ((char*)ptr)[BUFFERSIZE - 1] = '1';
        }
        sem_post(sem);
    }

    shmdt(ptr);
    shmctl(KEY, IPC_RMID, NULL);

    timeval end_time;
    gettimeofday(&end_time, NULL);

    fprintf(data, "%lf,\n", (end_time.tv_sec - end_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6);

    fclose(output);
    fclose(data);
    return 0;
}

int shm_sender(char* input_name) {
    FILE* input = fopen(input_name, "r");
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
            num = fread(data, sizeof(char), BUFFERSIZE - 1, input);
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
