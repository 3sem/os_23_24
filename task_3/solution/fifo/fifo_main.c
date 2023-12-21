#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "fifo.h"

const int NAME_SIZE = 100;

int main() 
{
    char *src_name = (char*) calloc(NAME_SIZE, sizeof(char));
    printf("Please enter source file name: ");
    scanf("%s", src_name);

    pid_t pid = fork();

    if (pid == 0) 
    {
        fifo_sender(src_name);
    } 
    else 
    {
        fifo_receiver();
    } 
    
    free(src_name);
}
