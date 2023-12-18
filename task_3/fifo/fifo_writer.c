#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define FIFO "fifo_example"

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("usage ./writer [file]");
        exit(1);
    }

    FILE* input = fopen(argv[1], "r");
    assert (input != NULL);

    mknod(FIFO, S_IFIFO | 0666, 0);
    int fd = open(FIFO, O_WRONLY);


    int size;
    char buf[1024];

    while (1) {
        size = fread(buf, sizeof(char), sizeof(buf) - 1, input);

        if (size == 0) {
            strcpy(buf, "quit");
            size = 4;
            break;
        }
        write(fd, buf, size);
    }

    fclose(input);
    close(fd);

    return 0;
}