#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h> 

#define BUFSZ 6400

typedef struct pPipe Pipe;

typedef struct op_table Ops;

int f_len (Pipe *self);

size_t f_snd_0(Pipe *self);
size_t f_rcv_0(Pipe *self);
size_t f_snd_1(Pipe *self);
size_t f_rcv_1(Pipe *self);

typedef struct op_table  {
    size_t (*rcvd)(Pipe *self); 
    size_t (*sndd)(Pipe *self);
    size_t (*rcvb)(Pipe *self); 
    size_t (*sndb)(Pipe *self);
    int (*len)(Pipe *self);
} Ops;

typedef struct pPipe {
    char* data; // intermediate buffer
    int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
    int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
    size_t len; // data length in intermediate buffer
    Ops actions;
} Pipe;

Pipe *constructPipe(int n) {
    Pipe *str = calloc(1, sizeof(Pipe));
    str->data = calloc(n, sizeof(char));
    
    str->len = n;
    str->actions.len = f_len;
    str->actions.rcvd = f_rcv_0;
    str->actions.sndd = f_snd_0;
    str->actions.rcvb = f_rcv_1;
    str->actions.sndb = f_snd_1;
    str->data[0] = 0;

    return str;
}


int f_len(Pipe *self) {
    return strlen(self->data);
}

size_t f_snd_0(Pipe *p) {

    size_t size = p->actions.len(p);
    //printf("snd worked\n");
    
    //size = read(p->fd_direct[1], p->data, p->len-1) - 1;
    //printf("snd p->data: %s, sz %ld, %ld, %d\n", p->data, size, p->len, p->fd_direct[1]); 
    //p->data[size] = '\0'; // the text string data is expected 
    //printf("snd p->data: %s, sz %ld, %d\n", p->data, size, p->data[size]); 
    write(p->fd_direct[1], p->data, size); 
    
    //printf("\nSend: %s\n", p->data);
    return size;
}

size_t f_rcv_0(Pipe *p) {
    size_t size = 0;
    
    size = read(p->fd_direct[0], p->data, p->len-1);
    //printf("rcv p->data: %s, sz %ld, %d\n", p->data, size, p->data[size]); 
    p->data[size] = '\0'; // the text string data is expected
    //printf("rcv p->data: %s, sz %ld, %d\n", p->data, size, p->data[size]); 
    //printf("Received from parent: %s\n", p->data);
    return size;
}

size_t f_snd_1(Pipe *p) {

    size_t size = p->actions.len(p);
    //printf("snd worked\n");
    
    //size = read(p->fd_back[1], p->data, p->actions.len(p)) - 1;
    //printf("snd p->data: %s, sz %d, %d\n", p->data, size, p->data[size]); 
    p->data[size] = '\0'; // the text string data is expected 
    //printf("snd p->data: %s, sz %d, %d\n", p->data, size, p->data[size]); 
    write(p->fd_back[1], p->data, size); 
    
    //printf("\nSend1: %s\n", p->data);
    return size;
}

size_t f_rcv_1(Pipe *p) {
    size_t size = 0;
    
    size = read(p->fd_back[0], p->data, p->len-1);
    //printf("rcv p->data: %s, sz %d, %d\n", p->data, size, p->data[size]); 
    //p->data[size] = '\0'; // the text string data is expected
    //printf("rcv p->data: %s, sz %d, %d\n", p->data, size, p->data[size]); 
    //printf("Received from parent1: %s\n", p->data);
    return size;
}

void release (Pipe *p) {

    free (p->data);
    free (p);
}

 
int main(int argc, char *argv[]) { 

    if (argc != 3) {
        printf("please, input names of two files: the sender and the receiver:\n");
        scanf("%s %s", argv[1], argv[2]);
    }
    
    struct timespec start, stop; 
    clock_gettime( CLOCK_MONOTONIC, &start); 

    Pipe *p = constructPipe(BUFSZ);
 
    if (pipe(p->fd_direct) < 0) { 
        printf ("Pipe creation failed1!\n"); 
        return -1; 
    }
    if (pipe(p->fd_back) < 0) { 
        printf ("Pipe creation failed2!\n"); 
        return -1; 
    } 
 
    const pid_t pid = fork(); //child appearance 
    if (pid < 0) { 
        printf("fork failed!\n"); 
        return -1; 
    } 
 
    int size = 0; 
    ////////////////////////////////////////////////////////////////
    if (pid != 0) { //parent transmits the data to child 
        //printf("if\n");
        FILE* inpf = fopen(argv[1], "r");
        FILE* outf = fopen(argv[2], "w");

        if (inpf == NULL) {
            return -1;
        }
        if (outf == NULL) {
            return -1;
        }

        while ((size = fread(p->data, sizeof(char), BUFSZ, inpf))) {
            *(p->data + size) = '\0';
            p->actions.sndd(p);

            if (size != BUFSZ) {
                p->len = strlen(p->data);
            }
            
            while (1) {
                size = p->actions.rcvb(p);
                if (size != 0)
                    break;
            }

            fwrite(p->data, sizeof(char), p->len, outf);
            p->len = BUFSZ;
        }

        strcpy(p->data, "quit");
        p->actions.sndd(p);
        fclose(inpf);
        fclose(outf);
        wait(NULL);
        printf("endparent\n");
    } 
     
    else { 
        //sleep(5);
        //printf("10\n");
        while (1) {
        
            while (1) {
                size = p->actions.rcvd(p);
                //printf("101\n");
                if (size != 0)
                    break;
            }

            if (strcmp(p->data, "quit") == 0) {
                break;
            }

            p->actions.sndb(p);
            p->len = BUFSZ;
            printf("endchild\n");
        }
    }     
    release(p);
    
    clock_gettime( CLOCK_MONOTONIC, &stop); 
 
    printf("%lf\n", ( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec ) * 1e-6);
    return 0; 
}
