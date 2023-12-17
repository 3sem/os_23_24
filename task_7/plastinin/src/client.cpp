#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 4096
#define FIFO_OUT "register.fifo"
#define FIFO_IN "answer.fifo"

int main()
{
    int fd_out = open(FIFO_OUT, O_RDWR | O_NONBLOCK);
    mkfifo(FIFO_IN, 0666);

    int size;
    char buf[BUFFERSIZE];
    char ans[BUFFERSIZE];

    sprintf(buf, "REGISTER 1.fifo 2.fifo");
    write(fd_out, buf, strlen(buf));
    int fd_in = open(FIFO_IN, O_RDONLY);

    while((size = read(fd_in, ans, BUFFERSIZE)) == 0) {}

    if (strcmp(ans, "ACK") != 0) {
        printf("Registration went wrong!\n");
        printf("Try again\n");

        return 0;
    }

    sleep(2);

    sprintf(buf, "GET test");
    int fifo_out = open("1.fifo", O_WRONLY);
    write(fifo_out, buf, strlen(buf));

    FILE* result = fopen("recieved_file", "w");

    int fifo_in = open("2.fifo", O_RDONLY);
    while((size = read(fifo_in, buf, BUFFERSIZE - 1)) > 0) {
        buf[size] = '\0';
        fwrite(buf, size, sizeof(char), result);
    }

    return 0;
}
