#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

#define MAX_COM_SIZE 1024

void run_cmd (char* cmd);

void parse_cmd (char* cmd, char** args);

int main (int argc, char* argv[]) {

    char* comLineInput = (char*) calloc (MAX_COM_SIZE, sizeof (char));
    assert (comLineInput != NULL);

    while (true) {

        int inputSize = 0;

        if ((inputSize = read (0, comLineInput, MAX_COM_SIZE)) == -1) {

            perror ("some read error i guess\n");
            return -1;
        }

        run_cmd (comLineInput);
    }
}

void run_cmd (char* cmd) {

    assert (cmd != NULL);

    pid_t pid = fork ();

    if (pid < 0) {

        perror ("fail on fork childing or some stuff idk\n");
        exit (-1);
    }
    if (pid) {

        waitpid (pid, NULL, 0);
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