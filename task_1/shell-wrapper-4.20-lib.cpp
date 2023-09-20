#include "shell-wrapper-4.20.hpp"

void run_cmd (char* cmd) {

    assert (cmd != NULL);

    comTok cmdTokens[MAX_COM_SIZE + 1]; // plus one for last redirect in case a max limit of commands was reached
    char* args[MAX_COM_SIZE + 1] = {0};
    int sequenceSize = parse_cmd (cmd, args, cmdTokens);
    printf ("seq size = %d\n", sequenceSize);

    int stdoutSave = dup (STDOUT_FILENO);
    int stdinSave = dup (STDIN_FILENO);

    int fd[2] = {0};

    if (pipe (fd) < 0) {

        perror ("хуйня пайп пошел нахуй\n");
        exit (-1);
    }


    for (int i = 0; i < sequenceSize; i++) {

        pid_t pid = fork ();

        if (pid < 0) {

            perror ("fail on fork or stuff");
            exit (-1);
        }
        if (pid) {

            int forkRetVal = 0;

            if (waitpid (pid, &forkRetVal, 0) == -1) {

                perror ("какаято хуйня бля\n");
                exit (-1);
            }

            dup2 (stdoutSave, STDOUT_FILENO);
            dup2 (stdinSave, STDIN_FILENO);

            printf ("fork returned : %d", WEXITSTATUS(forkRetVal));
            printf ("\n");

            continue;
        }

        // Read from 0 write to 1

        if (cmdTokens[i].begin != -1)
            dup2 (fd[0], STDIN_FILENO);

        if (cmdTokens[i].end != -1)
            dup2 (fd[1], STDOUT_FILENO);

        if (cmdTokens[i].end != -1) args[cmdTokens[i].end] = NULL;

        execvp (args[cmdTokens[i].begin == -1 ? 0 : cmdTokens[i].begin], args + (cmdTokens[i].begin == -1 ? 0 : cmdTokens[i].begin));
    }

}

int parse_cmd (char* cmd, char** args, comTok* cmdTokens) {

    assert (cmd != NULL);
    assert (args != NULL);
    assert (cmdTokens != NULL);

    int sequenceSize = 0;
    cmdTokens[0].begin = 0;

    char delim[] = "\n ";

    args[0] = strtok (cmd, delim);

    int i = 0;

    for (i = 1; i < MAX_COM_SIZE and (args[i] = strtok (NULL, delim)) != NULL; i++);

    args[i] = NULL;

    cmdTokens[sequenceSize].begin = -1;

    for (int j = 0; j < i; j++) {

        if (args[j][0] == '|') {

            cmdTokens[sequenceSize++].end = j;
            cmdTokens[sequenceSize].begin = j + 1;
        }
    }

    cmdTokens[sequenceSize++].end = -1;

    // printf ("seq Size : %d\n", sequenceSize);

    // for (int j = 0; j < i; j++) {

    //     printf ("%d : <%s>\n", j, args[j]);
    // }

    // printf ("----------------\n");

    // for (int j = 0; j < sequenceSize; j++) {

    //     printf ("%d : %d - %d\n", j, cmdTokens[j].begin, cmdTokens[j].end);
    // }

    return sequenceSize;
}










































// void run_cmd (char* cmd) {

//     assert (cmd != NULL);

//     pid_t pid = fork ();

//     if (pid < 0) {

//         perror ("fail on fork childing or some stuff idk\n");
//         exit (-1);
//     }
//     if (pid) {

//         int forkRetVal = 0;

//         if (waitpid (pid, &forkRetVal, 0) == -1) {

//             perror ("какая-то хуйня бля\n");
//             exit (-1);
//         }
//         printf ("fork ret val: %d", WEXITSTATUS (forkRetVal));
//         printf ("\n");
//         return;
//     }

//     char* args[MAX_COM_SIZE] = {0};

//     parse_cmd (cmd, args);

//     execvp (args[0], args);

//     return;
// }

// void parse_cmd (char* cmd, char** args) {

//     assert (cmd != NULL);

//     char delim[] = "\n ";

//     args[0] = strtok (cmd, delim);

//     int i = 0;

//     for (i = 1; i < MAX_COM_SIZE and (args[i] = strtok (NULL, delim)) != NULL; i++);

//     for (int j = 0; j < i ; j++) {

//         printf ("%d : <%s>\n", j, args[j]);
//     }

//     return;
// }