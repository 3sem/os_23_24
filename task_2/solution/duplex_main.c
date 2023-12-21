#include "duplex.h"

const int NAME_SIZE = 100;

int main() 
{
    char *src_name = (char*) calloc(NAME_SIZE, sizeof(char));
    printf("Please enter source file name: ");
    scanf("%s", src_name);

    char *dst_name = (char*) calloc(NAME_SIZE, sizeof(char));
    printf("Please enter destination file name: ");
    scanf("%s", dst_name);

    Pipe ppipe;
    pipe_ctor(&ppipe);

    pid_t pid = fork();

    if (pid) 
    {
        parent(&ppipe, src_name, dst_name);
    } 
    else 
    {
        child(&ppipe);
    }

    free(src_name);
    free(dst_name);
    pipe_dtor(&ppipe);

    return 0;
}
