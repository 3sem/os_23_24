#include "shell-wrapper-4.20.hpp"

void run_cmd (char* cmd) {

    FILE* log_out = fopen ("logout", "w");

    assert (cmd != NULL);

    Command commands[MAX_CMD_CNT];
    char* args[MAX_COM_SIZE + 1] = {0};
    int sequenceSize = parse_cmd (cmd, commands);
    // printf ("seq size = %d\n", sequenceSize);

    int stdoutSave = dup (STDOUT_FILENO);
    int stdinSave = dup (STDIN_FILENO);
    int fd[2] = {0};
    int fd_in = 0;

    for (int i = 0; i < sequenceSize; i++) {

        if (pipe (fd) < 0) {

            perror ("Error during pipe allocation\n");
            exit (-1);
        }

        pid_t pid = fork ();

        if (pid < 0) {

            perror ("fail on fork or stuff");
            exit (-1);
        }
        if (pid) {

            int forkRetVal = 0;

            if (waitpid (pid, &forkRetVal, 0) == -1) {

                perror ("Some error during waitpid\n");
                exit (-1);
            }

            close (fd[1]);

            if (commands[i].first == 0) close (fd_in);

            fd_in = fd[0];

            printf ("fork returned : %d", WEXITSTATUS(forkRetVal));
            printf ("\n");

            continue;
        }

        if (commands[i].first == 0) dup2 (fd_in, STDIN_FILENO);

        if (commands[i].last == 0) dup2 (fd[1], STDOUT_FILENO);

        close (fd[0]);

        execvp (commands[i].seq[0], commands[i].seq);
        exit (42);
    }

}

int parse_cmd (char* cmd, Command* output) {

    assert (cmd != NULL);
    assert (output != NULL);

    const char pipeDelim[] = "|\n";
    const char delim[] = " \n";
    int comCnt = 0;

    output[0].seq[0] = strtok (cmd, pipeDelim);


    for (comCnt = 1;
        comCnt < MAX_CMD_CNT
        and (output[comCnt].seq[0] = strtok (NULL, pipeDelim)) != NULL;
        comCnt++) {}

    // output[comCnt].seq[0] = NULL;

    for (int i = 0; i < comCnt; i++) {

        printf ("%d : [%s]\n", i, output[i].seq[0]);
    }

    for (int i = 0; i < comCnt; i++) {

        output[i].seq[0] = strtok (output[i].seq[0], delim);

        for (int j = 1; j < CMD_SEQ_SIZE and (output[i].seq[j] = strtok (NULL, delim)) != NULL; j++) {}
    }

    for (int i = 0; i < comCnt; i++) {

        printf ("%d:\n", i);
        for (int j = 0; output[i].seq[j] != NULL; j++) {

            printf ("\t%d : [%s]\n", j, output[i].seq[j]);
        }
    }

    output[0].first = 1;
    output[comCnt - 1].last = 1;
    return comCnt;
}

// int parse_cmd (char* cmd, char** args, comTok* cmdTokens) {

//     assert (cmd != NULL);
//     assert (args != NULL);
//     assert (cmdTokens != NULL);

//     const char pipeDelim[] = "|";
//     const char delim[] = " \n";
//     int tokenCnt = 0;

//     cmdTokens[0] = strtok (cmd, delim);

//     for (tokenCnt = 1; tokenCnt < MAX_COM_SIZE and (cmdTokens[tokenCnt].begin = strtok (NULL, pipeDelim)) != NULL; tokenCnt++) {

//         // cmdTokens[tokenCnt - 1].end = cmdTokens[tokenCnt].begin;
//     }



//     int sequenceSize = 0;
//     cmdTokens[0].begin = 0;

//     char delim[] = "\n ";

//     args[0] = strtok (cmd, delim);

//     int i = 0;

//     for (i = 1; i < MAX_COM_SIZE and (args[i] = strtok (NULL, delim)) != NULL; i++) {

//         printf ("%s\n", cmd);
//     }

//     args[i] = NULL;

//     for (int j = 0; j < MAX_COM_SIZE / 4;j++) printf ("%d : [%s]\n", j, args[j]);

//     cmdTokens[sequenceSize].begin = -1;

//     for (int j = 0; j < i; j++) {

//         if (args[j][0] == '|') {

//             cmdTokens[sequenceSize++].end = j;
//             cmdTokens[sequenceSize].begin = j + 1;
//         }
//     }

//     cmdTokens[sequenceSize++].end = -1;

//     return sequenceSize;
// }