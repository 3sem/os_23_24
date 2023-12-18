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
    // initialize all sems 
    sem_t* full_sem = sem_open ("full_sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    sem_t* empty_sem = sem_open ("empty_sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    sem_t* mutex = sem_open ("mutex", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);

    FILE* output = fopen("output.txt", "w");
    
    // configuring shmem object
    const int SIZE = 1024;
    const char* name = "OS";
    int shm_fd;
    void* ptr;
    shm_fd = shm_open(name, O_RDONLY, 0666);
    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
 
    while (1) {
        sem_wait(empty_sem);
        sem_wait(mutex);
        printf ("Server critical zone\n");

        if (strcmp((char*) ptr, "quit") == 0)
            break;

        fwrite((char*) ptr, sizeof(char), SIZE, output);

        sem_post(mutex);
        sem_post(full_sem);
    }
 
    /* remove the shared memory object */
    shm_unlink(name);
    return 0;
}