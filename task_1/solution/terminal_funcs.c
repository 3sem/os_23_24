#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

const size_t MAX_CMD_TOKENS = 80;

const int CHILD_ERROR_CODE = 1;

const size_t FD_SIZE = 2;

char **parse_cmd(char *arg)
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

char ***prep_cmds(char *arg)
{
    char ***cmds = malloc(MAX_CMD_TOKENS * sizeof(char**));

    int len = strlen(arg);

    int prev = 0, cmds_found = 0;

    for (int i = 0; i < len; i++)
    {
        if (arg[i] == '|')
        {
            arg[i] = '\0';

            cmds[cmds_found] = parse_cmd(arg + prev);

            prev = i + 1;
            cmds_found++;
        }
    }

    cmds[cmds_found] = NULL;

    return cmds;
}

void seq_pipe(char ***cmds)
{
    int fd[FD_SIZE];

    pid_t pid = 0;
    int fd_in = 0;
    int i = 0;

    while (cmds[i] != NULL) 
    {
        pipe(fd);

        if ((pid = fork()) == -1) 
        {
            exit(1);
        } 
        else if (pid == 0) 
        {
            if (i > 0)
            {
                dup2(fd_in, 0); //stdin <- read from fd_in (dup / dup2(int oldfd, int newfd) newfd <--copy( oldfd ) )
            }

            if (cmds[i + 1] != NULL)
            {
                dup2(fd[1], 1); //stdout -> write to pipe
            }

            close(fd[0]);

            execvp((cmds)[i][0], cmds[i]);
            
            exit(2);
        } 
        else 
        {
            wait(NULL);

            close(fd[1]);

            if (i > 0)
            {
                close(fd_in); // old pipe from previous step is not used further, and can be destroyed
            }

            fd_in = fd[0]; //fd_in <--read from pipe

            i++;
        }
    }

  return;
}

void run_cmds(char *arg)
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

    char ***cmds = prep_cmds(arg);

    seq_pipe(cmds);

    exit(0);
}
