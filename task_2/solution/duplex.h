#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define FD_SIZE 2 // cant use const int since array size must be known at compile time

typedef struct pPipe Pipe;

typedef struct Op_table Ops;

struct Op_table 
{
    size_t (*rcv) (Pipe *self, int fd);
    size_t (*snd) (Pipe *self, int fd);
};

struct pPipe 
{
        char *data; // intermediate buffer
        int fd_direct[FD_SIZE]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[FD_SIZE]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        size_t len; // data length in intermediate buffer
        Ops actions;
};

void pipe_ctor(Pipe *ppipe);

void pipe_dtor(Pipe *ppipe);

void parent(Pipe *ppipe, char *src_name, char *dst_name);

void child(Pipe *ppipe);

size_t send(Pipe *ppipe, int fd);

size_t receive(Pipe *ppipe, int fd);
