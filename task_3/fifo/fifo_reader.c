#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIFO "fifo_example"

int main()
{
    FILE* output = fopen("output.txt", "w");

    mknod(FIFO, S_IFIFO | 0666, 0);
    printf("Waiting for a writer\n");
    int fd = open(FIFO, O_RDONLY);
    printf("A writer is connected\n");

    int size;
    char buf[1024];

    while (( size = read(fd, buf, sizeof(buf)-1)) > 0) {
        if (size == 4) {
            if (strcmp(buf, "quit") == 0) {
                break;
            }
        }
        fwrite(buf, sizeof(char), size, output);
    }

    return 0;
}