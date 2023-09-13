#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "../include/task_1.h"

#define STDIN_FD 0
#define STDOUT_FD 1
#define STRSIZE 2097152


int main(){
    char str[STRSIZE];

    while(strcmp(str, "exit") != 0) {
        if (fgets(str, STRSIZE, stdin) == NULL)
            return 0;
        *strchr(str, '\n') = '\0';
        char*** args = parse_cmd(str);
        run_cmd(args);
    }

    return 1;
}

void run_cmd(char*** args){
    int pipefd[2];
    int fd_in = 0;
    int i = 0;

    while (args[i] != NULL) {
        if (pipe(pipefd) < 0)
            exit(3);
        pid_t pid = fork();
        if (pid == -1) {
            exit(1);
        } else if (pid == 0) {
            if (i > 0)
                dup2(fd_in, STDIN_FD);

            if (args[i+1] != NULL)
                dup2(pipefd[1], STDOUT_FD);

            close(pipefd[0]);
            execvp(args[i][0], args[i]);
            exit(2);
        } else {
            int status;
            waitpid(pid, &status, 0);
            close(pipefd[1]);

            if (i > 0)
                close(fd_in);

            fd_in = pipefd[0];
            i++;
        }
    }
    return;
}

char*** parse_cmd(char* arg){
    int pipes = pipes_num(arg);
    char*** args = (char***) calloc(pipes, sizeof(char**));
    char** cmds = (char**) calloc(pipes, sizeof(char*));
    cmds[0] = arg;

    for (int i = 1; *arg != 0; arg++) {
        if (*arg == '|') {
            *arg = '\0';
            cmds[i] = arg + 1;
            i++;
        }
    }

    for (int i = 0; i < pipes; i++) {
        if (cmds[i] != NULL) {
            int tokens = tokens_num(cmds[i]);
            args[i] = (char**) calloc(tokens, sizeof(char*));
            parse_tokens(args[i], cmds[i]);
        }
    }
    free(cmds);
    return args;
}

void parse_tokens(char** args, char* str) {
    char delimeters[] = " ";
    int i = 0;
    char* token;

    for (token = strtok(str, delimeters); token != NULL; token = strtok(NULL, delimeters)) {
        args[i] = (char*) calloc(strlen(token) + 1, sizeof(char));
        strcpy(args[i], token);
        i += 1;
    }
}

int pipes_num(char* str) {
    int pipes = 2;

    while (*str != '\0') {
        if (*str == '|') {
            pipes++;
        }
        str++;
    }

    return pipes;
}

int tokens_num(char* str) {
    int tokens = 2;

    while (*str != '\0') {
        if (*str == ' ') {
            tokens++;
        }

        str++;
    }

    return tokens;
}
