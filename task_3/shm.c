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
#include <sys/mman.h>
#include <sys/stat.h>


#define KEY 1234
#define SIZE 4096



void client()
{
    sem_t* sem;
    sem = sem_open("/sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    FILE* in = fopen("total.txt", "r");
    int num = 1;
    /* name of the shared memory object */
    const char* name = "OS";
    
    char BUFF[SIZE];

    /* shared memory file descriptor */
    int shm_fd;
 
    /* pointer to shared memory object */
    void* ptr;
 
    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
 
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    while(1)
    {
        num = fread(BUFF, sizeof(char), SIZE - 1, in);
        if(num == 0)
        {
            sprintf(ptr, "%s", "quit");
            return;
        }
        BUFF[num] = '\0';
        /* write to the shared memory object */
        sprintf(ptr, "%s", BUFF);
        ((char*) ptr)[SIZE - 1] = '\0';
    }
}

void server()
{
    sem_t* sem;
    sem = sem_open("/sem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    FILE* out = fopen("resshm.txt", "w+");
 
    /* name of the shared memory object */
    const char* name = "OS";
 
    /* shared memory file descriptor */
    int shm_fd;
 
    /* pointer to shared memory object */
    void* ptr;
 
    /* open the shared memory object */
    shm_fd = shm_open(name, O_RDONLY, 0666);
    /* memory map the shared memory object */
    
    
    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    while(1)
    {
        /* read from the shared memory object */
        if(!strcmp((char*) ptr, "quit")){break;}
        while(ptr == NULL);
        fwrite((char*)ptr, sizeof(char), SIZE, out);  
    }
    /* remove the shared memory object */
    shm_unlink(name);
    return;
}

int main()
{
    FILE *timeshm = fopen("tempshm", "a+");

    struct timeval ts, te;
    double elapsed;
    gettimeofday(&ts, NULL);


    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork()");
        return 1;
    }
    if (pid)
    {
        client();
    }
    else
    {
        server();
    }
    gettimeofday(&te, NULL);
    elapsed = (te.tv_sec - ts.tv_sec) * 1000;
    elapsed += (te.tv_usec - ts.tv_usec) / 1000;
    fprintf(timeshm, "%lf\n", elapsed);
    return 0;
}