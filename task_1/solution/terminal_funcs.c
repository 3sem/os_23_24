#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

const size_t MAX_CMD_TOKENS = 80;

const int CHILD_ERROR_CODE = 1;

char **parse_cmd(const char *arg)
{
    char **tokens = malloc(MAX_CMD_TOKENS * sizeof(char*));

    char *tmp;

    char delim[] = " ";

    int i = 0;
    for (tmp = strtok(arg, delim); tmp != NULL; tmp = strtok(NULL, delim))
    {
        tokens[i] = malloc(strlen(tmp) + 1);

        strcpy(tokens[i], tmp);

        i++;
    }

    tokens[i] = NULL; // set last token to NULL for execvp

    return tokens;
}

void run_cmd(const char *arg)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("run_cmd failed\n");

        exit(-1);
    }

    if (pid)
    {
        int status = 0;

        waitpid(pid, &status, 0);

        printf("Child status = %d\n", status);
        printf("Recieved return code from child: %d\n\n", WEXITSTATUS(status));

        return;
    }

    char **args = parse_cmd(arg);

    execvp(arg, args);

    printf("Exec failed\n");

    exit(CHILD_ERROR_CODE);
}