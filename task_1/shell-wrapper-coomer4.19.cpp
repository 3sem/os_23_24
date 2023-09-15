#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// | - разделитель; ./a | ./b - запустить а и перенаправить вывод в б



void parse_cmd (const char* arg) {

    char* str = strdup(arg);
}

void run_cmd(const char* arg) {

    pid_t pid = fork ();
    if (pid < 0) {

        perror ("Runc_cmd failed\n");
        exit (1);
    }
    if (pid) {

        waitpid (pid, NULL, 0); // no status check here, check if necessary
        return;
    }

    char **args = parse_cmd (arg); //command cooking method somehow no care in the world

    execvp (args[0], args + 1);

    return;
}

int main () {

    char str[256]; //set size as macros

    while (1) { // add another condition

        scanf ("%s", str); //command input from console

        run_cmd (str);
    }

    return 0;
}