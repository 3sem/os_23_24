#include <string.h>
#ifdef DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...) ;
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pipe.h"

#define MAX_BUF_SIZE 4096

void parent_process(Pipe* pipe, FILE* input, FILE* output);
void child_process(Pipe* pipe);


int main() {

    FILE* input  = fopen ("input.txt", "r");
    FILE* output = fopen ("output.txt", "w");
    assert (input != NULL);
    assert (output != NULL);

    Pipe my_pipe = {};
    pipe_ctor(&my_pipe);

    pid_t pid = fork();

    if (pid == 0)
    {
        child_process(&my_pipe);
    }
    else if (pid == -1)
    {
        fprintf(stderr, "Can't fork\n");
    }
    else 
    {
        parent_process(&my_pipe, input, output);
    }

    pipe_dtor(&my_pipe);

    fclose(input);
    fclose(output);
}

void child_process(Pipe* my_pipe) {
    while (1) {
            while (!my_pipe->actions.rcv(my_pipe, Child)) {}

            if (strcmp(my_pipe->data, "exit") == 0) {
                return;
            }

            my_pipe->actions.snd(my_pipe, Child);

            my_pipe->len = MAX_BUF_SIZE;
        }
}

void parent_process(Pipe* my_pipe, FILE* input, FILE* output) {

    if (input == NULL || output == NULL) {
            printf ("Can't open file\n");
            return;
    }

    int num;
    while (num = fread(my_pipe->data, sizeof(char), MAX_BUF_SIZE, input)) {

        my_pipe->data[num] = '\0';

        if (num != MAX_BUF_SIZE) {
            my_pipe->len = strlen(my_pipe->data);
        }
        int send = my_pipe->actions.snd(my_pipe, Parent);

        while (!my_pipe->actions.rcv(my_pipe, Parent)) {}

        fwrite(my_pipe->data, sizeof(char), my_pipe->len, output);
        my_pipe->len = MAX_BUF_SIZE;
    }

    strcpy(my_pipe->data, "exit");
    my_pipe->actions.snd(my_pipe, Parent);
}
