#ifdef DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...) ;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include"pipe.h"

#define INTER_BUFF_SIZE 4096


size_t rcv (Pipe* self, pipeType parent) {

    if (parent)
    {
        dprintf ("rcv parent\n");
        self->len = read(self->fd_back[0], self->data, INTER_BUFF_SIZE);
        dprintf ("rcved child\n");
        return self->len;
    }
    else 
    {
        dprintf ("rcv child\n");
        self->len = read(self->fd_direct[0], self->data, INTER_BUFF_SIZE);
        dprintf ("rcved child\n");
        return self->len;
    }
}

size_t snd (Pipe* self, pipeType parent) {
    if (parent)
        return write(self->fd_direct[1], self->data, self->len);
    else
        return write(self->fd_back[1], self->data, self->len);
}

size_t pipe_ctor (Pipe* self) {
    self->data = calloc(INTER_BUFF_SIZE, sizeof(char));
    if (self->data == NULL)
    {
        fprintf (stderr, "Unable to allocate buffer\n");
        return 1;
    }

    pipe(self->fd_direct);
    pipe(self->fd_back);

    self->len  = INTER_BUFF_SIZE;
    self->actions.rcv = &rcv;
    self->actions.snd = &snd;

    return 0;
}

void pipe_dtor (Pipe *self) {
    free(self->data);
}
