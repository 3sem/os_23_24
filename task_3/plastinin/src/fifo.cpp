#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define FIFO "fifo_example"

int fifo_receiver() {
    timeval start_time;
    gettimeofday(&start_time, NULL);
    FILE* output = fopen("output_fifo", "w");
    FILE* data = fopen("data_fifo.csv", "a");

    mknod(FIFO, S_IFIFO | 0666, 0);
    int fd = open(FIFO, O_RDONLY);

    int size;
    char buf[4096];

    while (( size = read(fd, buf, sizeof(buf)-1)) > 0) {
        buf[size] = '\0';
        fwrite(buf, sizeof(char), size, output);
    }

    timeval end_time;
    gettimeofday(&end_time, NULL);

    fprintf(data, "%lf,\n", (end_time.tv_sec - end_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6);

    fclose(output);
    fclose(data);

    return 0;
}

int fifo_sender(char* input_name) {
    FILE* input = fopen(input_name, "r");

    mknod(FIFO, S_IFIFO | 0666, 0);
    int fd = open(FIFO, O_WRONLY);

    int size;
    char buf[4096];

    while (( size = fread(buf, sizeof(char), sizeof(buf)-1, input)) > 0) {
        buf[size] = '\0';
        write(fd, buf, size);
    }

    return 0;
}

