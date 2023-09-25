#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "../include/task_2.h"

int BUFFERSIZE = 4096;

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        printf("Please, specify only input file and output file\n");
        return 0;
    }

    Pipe ppipe = {};
    pipe_ctor(&ppipe);

    char* input_name = argv[1];
    char* output_name = argv[2];

    pid_t pid = fork();
    if (pid == -1) {
        exit(1);
    } else if (pid) {
        parent_process(&ppipe, input_name, output_name);
    } else {
        child_process(&ppipe);
    }

    pipe_dtor(&ppipe);

    return 0;
}

void pipe_ctor(Pipe* ppipe) {
    ppipe->len = BUFFERSIZE;
    ppipe->data = (char*) calloc(ppipe->len, sizeof(char));

    pipe(ppipe->fd_direct);
    pipe(ppipe->fd_back);

    ppipe->actions = {};
    ppipe->actions.snd = &send;
    ppipe->actions.rcv = &receive;
}

void pipe_dtor(Pipe* ppipe) {
    ppipe->len = 0;
    free(ppipe->data);
    ppipe->data = NULL;

    ppipe->actions.rcv = NULL;
    ppipe->actions.snd = NULL;
}

long file_size(FILE* fp) {
    struct stat buf;
    fstat(fileno(fp), &buf);

    return buf.st_size;
}

void parent_process(Pipe* ppipe, char* input_name, char* output_name) {
    FILE* input = fopen(input_name, "r");
    FILE* output = fopen(output_name, "w");

    if (input == NULL || output == NULL) {
        return;
    }

    while (int num = fread(ppipe->data, sizeof(char), BUFFERSIZE, input)) {
        *(ppipe->data + num) = '\0';
        int send = ppipe->actions.snd(ppipe, ppipe->fd_direct[1]);

        if (num != BUFFERSIZE) {
            ppipe->len = strlen(ppipe->data);
        }
        while (!ppipe->actions.rcv(ppipe, ppipe->fd_back[0])) {}

        fwrite(ppipe->data, sizeof(char), ppipe->len, output);
        ppipe->len = BUFFERSIZE;
    }

    strcpy(ppipe->data, "quit");
    ppipe->actions.snd(ppipe, ppipe->fd_direct[1]);
}

void child_process(Pipe* ppipe) {
    while (true) {
        while (!ppipe->actions.rcv(ppipe, ppipe->fd_direct[0])) {}

        if (strcmp(ppipe->data, "quit") == 0) {
            break;
        }

        ppipe->actions.snd(ppipe, ppipe->fd_back[1]);

        ppipe->len = BUFFERSIZE;
    }
}

size_t send(Pipe* ppipe, int fd) {
    int num = write(fd, ppipe->data, ppipe->len);

    return num;
}

size_t receive(Pipe* ppipe, int fd) {
    size_t num = read(fd, ppipe->data, ppipe->len);

    return num;
}
