#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include "../C-Thread-Pool/thpool.h"

#define LOG1(...)   fopen(serv.log_name,"a");                          \
                    fprintf(serv.log, __VA_ARGS__);                    \
                    fclose(serv.log);

#define LOG2(...)   fopen(serv->log_name,"a");                         \
                    fprintf(serv->log, __VA_ARGS__);                   \
                    fclose(serv->log);

#define COMMA ,

#define REGISTER_FIFO "register.fifo"
#define ANSWER_FIFO "answer.fifo"
#define BUFFERSIZE 4096
#define STRSIZE 200

struct Client {
    int in_fd;
    char* out_name;
};

struct Server {
    Client clients[64];
    threadpool thpool;
    int clients_num;
    int reg_fd;
    FILE* log;
    const char* log_name;
    bool off;
};

struct Arg {
    char* fifo_name;
    char file_name[STRSIZE];
    Server* serv;
};

int server_ctor(Server* serv);

int server_dtor(Server* serv);

int handle_cmd(Server* serv);

int initialize_rfds(Server* serv, fd_set* rfds);

int handle_request(Server* serv, int client_num);

void send_file(void* args);

#endif // SERVER_H
