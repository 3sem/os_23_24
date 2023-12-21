#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 4096

const char *NAME = "fifo_name";

const double USEC_MULTIPLIER = 1e-6;

void fifo_sender(char *src_name) 
{
    FILE *input = fopen(src_name, "r");

    mknod(NAME, S_IFIFO | 0666, 0);
    int fd = open(NAME, O_WRONLY);

    int size = 0;
    char buf[BUF_SIZE];

    while (( size = fread(buf, sizeof(char), sizeof(buf) - 1, input)) > 0) 
    {
        buf[size] = '\0';
        write(fd, buf, size);
    }

    return;
}

void fifo_receiver() 
{    
    FILE *output = fopen("fifo_output.txt", "w");
    FILE *data = fopen("fifo_data.csv", "a");

    int size = 0;

    char buf[BUF_SIZE];

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    mknod(NAME, S_IFIFO | 0666, 0);

    int fd = open(NAME, O_RDONLY);

    while (( size = read(fd, buf, sizeof(buf) - 1)) > 0)
    {
        buf[size] = '\0';
        fwrite(buf, sizeof(char), size, output);
    }

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    fprintf(data, "%lf,\n", (end_time.tv_sec - end_time.tv_sec) + USEC_MULTIPLIER * (end_time.tv_usec - start_time.tv_usec));

    fclose(output);
    fclose(data);

    return;
}
