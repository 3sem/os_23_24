#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include "../include/fifo.h"
#include "../include/shm.h"
#include "../include/queue.h"

void parse_args(bool* shm, bool* fifo, bool* queue, char* cmd);

int main(int argc, char** argv) {
    char* input;
    bool shm = true, fifo = true, queue = true;
    if (argc == 1) {
        printf("Please, specify input file, and programm mode if you want\n");
        return 0;
    } else if (argc > 3) {
        printf("Please, specify input file, and programm mode if you want\n");
        return 0;
    } else if (argc == 3) {
        if (argv[1][0] == '-' && argv[1][1] == '-') {
            parse_args(&shm, &fifo, &queue, argv[1]);
            input = argv[2];
        } else {
            parse_args(&shm, &fifo, &queue, argv[2]);
            input = argv[1];
        }
    }

    pid_t pid = fork();
    if (pid == -1) {
        return 0;
    }

    if (shm) {
        if (pid == 0) {
            shm_sender(input);
        } else {
            shm_receiver();
        }
    } else if (fifo) {
        if (pid == 0) {
            fifo_sender(input);
        } else {
            fifo_receiver();
        }
    } else if (queue) {
        if (pid == 0) {
            queue_sender(input);
        } else {
            queue_receiver();
        }
    }
}

void parse_args(bool* shm, bool* fifo, bool* queue, char* cmd) {
    if (strcmp(cmd, "--queue") == 0) {
        *shm = false;
        *queue = true;
        *fifo = false;
    } else if (strcmp(cmd, "--fifo") == 0) {
        *shm = false;
        *queue = false;
        *fifo = true;
    } else if (strcmp(cmd, "--shm") == 0) {
        *shm = true;
        *queue = false;
        *fifo = false;
    } else {
        printf("There is no such mode: %s\n", cmd);
        *fifo = false;
        *queue = false;
        *shm = false;
    }
}
