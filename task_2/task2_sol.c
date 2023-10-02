#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <string.h>

#include <errno.h>

#include <sys/stat.h>

typedef struct pPipe Pipe;



typedef struct pPipe {
        char* data; // intermediate buffer
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        size_t len; // data length in intermediate buffer

} Pipe;


size_t send(Pipe* pipe, int fd) {
    int num = write(fd, pipe->data, pipe->len);

    return num;
}

size_t receive(Pipe* pipe, int fd) {
    size_t num = read(fd, pipe->data, pipe->len);

    return num;
}


int main(int argc, char * argv[])
{
    char *inname;
    printf("Enter the name of the file:");
    scanf("%s", inname);
    int64_t file_size = 0;
	struct stat buff;
    FILE* in = fopen(inname,"r");
    fstat(fileno(in), &buff);
	file_size = buff.st_size;
    Pipe p;
    p.len = 4096;
    p.data = (char*)calloc(file_size, sizeof(char));
    pipe(p.fd_back);
    pipe(p.fd_direct);
    pid_t pid = fork();
    if (pid == -1) {
        exit(1);
    } else if (pid) {
        FILE* out = fopen("total.txt","w+");
        long long num = 4096;
        while(num = fread(p.data, sizeof(char), 4096, in))
        {
        
            p.data[num] = '\0';
            send(&p, p.fd_direct[1]);
           if(num != 4096)
            {
                p.len = strlen(p.data);
            }
            receive(&p, p.fd_back[0]);
            fwrite(p.data, sizeof(char), p.len, out);
            p.len = 4096;
        }
        
    } else {
        while(receive(&p, p.fd_direct[0]) > 0)
        {
            send(&p, p.fd_back[1]);
        }
    }
    free(p.data);
    return 0;
}



