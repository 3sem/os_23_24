#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
 
int main()
{
    /* the size (in bytes) of shared memory object */
    const int SIZE = 1024;

    char inter_buf[1024] = {};

    // initialize all sems 
    sem_t* full_sem = sem_open ("full_sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    sem_t* empty_sem = sem_open ("empty_sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    sem_t* mutex = sem_open ("mutex", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);

    // configuring shmmem obj
    const char* name = "OS";
    int shm_fd;
    void* ptr;
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // need to change
    FILE* input = fopen("input.txt", "r");

    int num_of_read;
    while ((num_of_read = fread(inter_buf, sizeof(char), SIZE, input)) != 0) {
        printf ("readed %d\n", num_of_read);
        sem_wait(full_sem);
        sem_wait(mutex);
        printf ("Client critical zone\n");

        strncpy(ptr, inter_buf, num_of_read);

        sem_post(mutex);
        sem_post(empty_sem);
    }

    sem_wait(full_sem);
    printf ("Client 2nd critical zone\n");
    sem_wait(mutex);
    sprintf(ptr, "quit");
    sem_post(mutex);
    sem_post(empty_sem);

    return 0;
}