#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/types.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define MAX_TOKENS_NUMBER 80

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char **split_command(char *command)
{
    char *tmp = NULL;
    char **tokens = malloc(MAX_TOKENS_NUMBER * sizeof(*tokens));

    int token_number = 0;
    for (tmp = strtok(command, " "); tmp != NULL; tmp = strtok(NULL, " "))
    {
        tokens[token_number] = malloc(strlen(tmp) + 1);

        strcpy(tokens[token_number], tmp);

        token_number++;
    }

    tokens[token_number] = NULL; // execvp нужно, чтобы последний аргумент был выставлен в 0

    return tokens;
}

char ***tokenize(char *commands)
{
    char ***tokens = malloc(MAX_TOKENS_NUMBER * sizeof(*tokens));
    size_t commands_length = strlen(commands);

    size_t command_offset = 0, token_number = 0;
    for (size_t symbol_index = 0; symbol_index < commands_length; symbol_index++)
        if (commands[symbol_index] == '|')
        {
            commands[symbol_index] = '\0';

            tokens[token_number] = split_command(commands + command_offset);

            command_offset = symbol_index + 1;

            token_number++;
        }
    
    tokens[token_number] = NULL; // Терминируем массив токенов, чтобы понимать, где его конец

    return tokens;
}

void tokens_destructor(char ***tokens)
{
    for (size_t token_number = 0; tokens[token_number] != NULL; token_number++)
    {
        for (size_t word_number = 0; tokens[token_number][word_number] != NULL; word_number++)
            free(tokens[token_number][word_number]);

        free(tokens[token_number]);
    }

    free(tokens);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void pipe_sequence(char ***tokens)
{
    int pipefd[2] = {0};
    int fd_in = 0;

    int token_number = 0;
    while (tokens[token_number] != NULL) 
    {
        pipe(pipefd);

        pid_t pid = fork();

        if (pid < 0) 
        {
            fprintf(stderr, "Can't create child process\n");

            exit(pid);
        }

        else if (pid == 0) 
        {
            if (token_number > 0)
                dup2(fd_in, 0);

            if (tokens[token_number + 1] != NULL)
                dup2(pipefd[1], 1);

            close(pipefd[0]);

            execvp(tokens[token_number][0], tokens[token_number]);

            exit(-1); // Если команда верная, то execvp заместит текущий процесс, и мы не дойдем до этой строки
        } 

        else 
        {
            wait(NULL); // Ждём пока процесс ребёнка умрет

            close(pipefd[1]);

            if (token_number > 0)
                close(fd_in);

            fd_in = pipefd[0];

            token_number++;
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void run(char *commands)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Can't create a new process\n");

        exit(-1);
    }

    if (pid)
    {
        int status = 0;

        waitpid(pid, &status, 0);

        printf("Return code: %d\n", WEXITSTATUS(status));

        return;
    }

    char ***tokens = tokenize(commands);

    pipe_sequence(tokens);

    tokens_destructor(tokens);

    exit(0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
