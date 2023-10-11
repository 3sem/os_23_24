#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "duplex.h"

const int BUFFER_SIZE = 4 * 1024;

const char* const END_MARKER = "HALT";

void pipe_ctor(Pipe *ppipe) 
{
    if (!ppipe)
    {
        return;
    }

    ppipe->len = BUFFER_SIZE;
    ppipe->data = (char*) calloc(ppipe->len, sizeof(char));

    pipe(ppipe->fd_direct);
    pipe(ppipe->fd_back);

    ppipe->actions.snd = &send;
    ppipe->actions.rcv = &receive;

    return;
}

void pipe_dtor(Pipe *ppipe) 
{
    if (!ppipe)
    {
        return;
    }

    free(ppipe->data);

    return;
}

void parent(Pipe *ppipe, char *src_name, char *dst_name) 
{
    if (!ppipe || !src_name || !dst_name)
    {
        return;
    }

    FILE *src_file = fopen(src_name, "r");
    FILE *dst_file = fopen(dst_name, "w");

    if (!src_file || !dst_file) 
    {
        return;
    }

    int chars_read = fread(ppipe->data, sizeof(char), BUFFER_SIZE, src_file);

    while (chars_read)
    {
        *(ppipe->data + chars_read) = '\0';

        ppipe->actions.snd(ppipe, ppipe->fd_direct[1]);

        if (chars_read != BUFFER_SIZE)
        {
            ppipe->len = strlen(ppipe->data);
        }

        while (!(ppipe->actions.rcv(ppipe, ppipe->fd_back[0])));

        fwrite(ppipe->data, sizeof(char), ppipe->len, dst_file);

        ppipe->len = BUFFER_SIZE;

        chars_read = fread(ppipe->data, sizeof(char), BUFFER_SIZE, src_file);
    }

    strcpy(ppipe->data, END_MARKER);
    ppipe->actions.snd(ppipe, ppipe->fd_direct[1]);

    fclose(src_file);
    fclose(dst_file);

    return;
}

void child(Pipe *ppipe) 
{
    if (!ppipe)
    {
        return;
    }

    while (1) 
    {
        while (!(ppipe->actions.rcv(ppipe, ppipe->fd_direct[0])));

        if (!strcmp(ppipe->data, END_MARKER)) 
        {
            break;
        }

        ppipe->actions.snd(ppipe, ppipe->fd_back[1]);

        ppipe->len = BUFFER_SIZE;
    }

    return;
}

size_t send(Pipe *ppipe, int fd) 
{
    if (!ppipe)
    {
        return 0;
    }

    return write(fd, ppipe->data, ppipe->len);
}

size_t receive(Pipe *ppipe, int fd) 
{
    if (!ppipe)
    {
        return 0;
    }

    return read(fd, ppipe->data, ppipe->len);
}