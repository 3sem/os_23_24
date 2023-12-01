#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/server.h"

int server_ctor(Server* serv) {
    serv->clients_num = 0;

    serv->log_name = "server_log";
    serv->log = fopen(serv->log_name, "w");
    fclose(serv->log);

    serv->off = false;

    return 0;
}

int handle_cmd(Server* serv) {
    char buffer[BUFFERSIZE];
    char cmd[STRSIZE];
    char fifo_in[STRSIZE];
    char fifo_out[STRSIZE];

    int num = read(serv->reg_fd, buffer, BUFFERSIZE);
    sscanf(buffer, "%s %s %s", cmd, fifo_in, fifo_out);

    if (strcmp(cmd, "REGISTER") != 0) {
        if (strcmp(cmd, "OFF") == 0) {
            LOG2("Server is shutting down\n");
            serv->off = true;

            return 0;
        }
        sprintf(buffer, "INVALID COMMAND");
        LOG2("Client gave wrong cmd(register)\n");
        mkfifo(ANSWER_FIFO, 0666);
        int ans_fd = open(ANSWER_FIFO, O_WRONLY);

        if (ans_fd == -1) {
            LOG2("Cannot open file: %s\n", ANSWER_FIFO);

            return 0;
        }

        write(ans_fd, buffer, strlen(buffer) + 1);

        close(ans_fd);

        return 0;
    }

    mknod(fifo_in, S_IFIFO | 0666, 0);
    int in_fd = open(fifo_in, O_RDWR);
    if (in_fd == -1) {
        LOG2("Cannot open file: %s\n", fifo_out);

        return 0;
    }
    serv->clients[serv->clients_num].in_fd = in_fd;
    LOG2("Created fifo in: %s\n\n" COMMA fifo_in);

    mknod(fifo_out, S_IFIFO | 0666, 0);
    serv->clients[serv->clients_num].out_name = (char*) calloc(STRSIZE, sizeof(char));
    sprintf(serv->clients[serv->clients_num].out_name, "%s", fifo_out);
    int out_fd = open(fifo_out, O_RDWR);
    if (out_fd == -1) {
        LOG2("Cannot open file: %s\n", fifo_out);

        return 0;
    }
    close(out_fd);
    LOG2("Created fifo out: %s\n\n" COMMA fifo_out);

    serv->clients_num += 1;

    mkfifo(ANSWER_FIFO, 0666);
    int ans_fd = open(ANSWER_FIFO, O_WRONLY);

    if (ans_fd == -1) {
        LOG2("Cannot open file: %s\n", ANSWER_FIFO);
    }

    char ans[5];
    sprintf(ans, "ACK");
    write(ans_fd, ans, strlen(ans) + 1);

    close(ans_fd);

    return 0;
}

int initialize_rfds(Server* serv, fd_set* rfds) {
    FD_ZERO(rfds);

    FD_SET(serv->reg_fd, rfds);
    for (int i = 0; i < serv->clients_num; i++) {
        FD_SET(serv->clients[i].in_fd, rfds);
    }

    return 0;
}

int handle_request(Server* serv, int client_num) {
    Arg arg = {};

    char buffer[BUFFERSIZE];
    char cmd[BUFFERSIZE];
    char file_name[BUFFERSIZE];

    int num = read(serv->clients[client_num].in_fd, buffer, BUFFERSIZE);
    sscanf(buffer, "%s %s", cmd, arg.file_name);

    if (strcmp(cmd, "GET") != 0) {
        sprintf(buffer, "INVALID COMMAND");
        LOG2("Client gave wrong cmd(get)\n");
        int ans_fd = open(serv->clients[client_num].out_name, O_WRONLY);

        if (ans_fd == -1) {
            LOG2("Cannot open file: %s\n", serv->clients[client_num].out_name);

            return 0;
        }

        write(ans_fd, buffer, strlen(buffer) + 1);

        close(ans_fd);

        return 0;
    }

    LOG2("Going to send file: %s\n", arg.file_name);

    arg.fifo_name = serv->clients[client_num].out_name;
    arg.serv = serv;

    pthread_t tid;
    pthread_create(&tid, NULL, &send_file, (void*)(&arg));

    return 0;
}

void* send_file(void* args) {
    Arg* arg = (Arg*) args;
    Server* serv = arg->serv;

    char buffer[BUFFERSIZE];
    int num = 0;

    FILE* file = fopen(arg->file_name, "r");

    if (file == NULL) {
        return NULL;
    }

    int fifo_fd = open(arg->fifo_name, O_WRONLY);

    if (fifo_fd == -1) {
        return NULL;
    }

    while((num = fread(buffer, sizeof(char), BUFFERSIZE - 1, file)) > 0) {
        buffer[num] = '\0';
        write(fifo_fd, buffer, num);
    }

    close(fifo_fd);

    return NULL;
}

int server_dtor(Server* serv) {
    serv->log = NULL;
    serv->log_name = NULL;
    serv->reg_fd = -1;

    for (int i = 0; i < serv->clients_num; i++) {
        free(serv->clients[i].out_name);
    }

    return 0;
}
