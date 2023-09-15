#include "shell-wrapper-4.20.hpp"

void run_cmd (char* cmd) {

    assert (cmd != NULL);

    pid_t pid = fork ();

    if (pid < 0) {

        perror ("fail on fork childing or some stuff idk\n");
        exit (-1);
    }
    if (pid) {

        int forkRetVal = 0;

        if (waitpid (pid, &forkRetVal, 0) == -1) printf ("какая-то хуйня бля\n");
        printf ("fork ret val: %d", WEXITSTATUS (forkRetVal));
        return;
    }

    char* args[MAX_COM_SIZE] = {0};

    parse_cmd (cmd, args);

    execvp (args[0], args);

    return;
}

void parse_cmd (char* cmd, char** args) {

    assert (cmd != NULL);

    char delim[] = "\n ";

    args[0] = strtok (cmd, delim);

    for (int i = 1; i < MAX_COM_SIZE and (args[i] = strtok (NULL, delim)) != NULL; i++);

    return;
}