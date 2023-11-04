#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FIFO "fifo_example"

void reader()
{
    FILE* out = fopen("resfifo.txt", "w+");
    mkfifo(FIFO, 0666);
    //printf("Waiting for a writer\n");
    int fd = open(FIFO, O_RDONLY);
    //printf("A writer is connected\n");

    int size;
    char buf[4096];
    while(size = read(fd, buf, 4096))
    {
        buf[size] = '\0';
        //printf("Send: %s", buf);
        fwrite(buf, sizeof(char), size, out);
    }
}
void writer()
{
    FILE* in = fopen("total.txt","r");
    mkfifo(FIFO, 0666);
    //printf("Waiting for a reader\n");
    int fd = open(FIFO, O_WRONLY);
    //printf("A reader is connected\n");

    int size;
    char buf[4096];
    while(size = fread(buf, sizeof(char), 4096, in))
    {
        buf[size] = '\0';

        //printf("Send: %s",  buf);
        write(fd, buf, size);
    }
}

int main()
{
    FILE *timeFifo = fopen("tempFF", "a+");

    struct timeval ts, te;
    double elapsed;
    gettimeofday(&ts, NULL);
    pid_t pid = fork();
    if (pid == -1) {
        return 0;
    }
    if(pid == 0)
    {
        writer();
    }
    else
    { 
        reader();
    }
    gettimeofday(&te, NULL);
    elapsed = (te.tv_sec - ts.tv_sec) * 1000;
    elapsed += (te.tv_usec - ts.tv_usec) / 1000;
    fprintf(timeFifo, "%lf\n", elapsed);
    return 0;
}