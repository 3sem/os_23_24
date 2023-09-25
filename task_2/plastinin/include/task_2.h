#ifndef TASK_2
#define TASK_2

#include <stdio.h>
#include <stdlib.h>

struct Pipe;

struct Op_table  {
    size_t (*rcv)(Pipe *self, int fd);
    size_t (*snd)(Pipe *self, int fd);
};

struct Pipe {
        char* data; // intermediate buffer
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        size_t len; // data length in intermediate buffer
        Op_table actions;
};

void pipe_ctor(Pipe* pipe);

void pipe_dtor(Pipe* ppipe);

void parent_process(Pipe* ppipe, char* input_name, char* output_name);

void child_process(Pipe* ppipe);

long file_size(FILE* fp);

size_t receive(Pipe* ppipe, int fd);

size_t send(Pipe* ppipe, int fd);


#endif // TASK_2
