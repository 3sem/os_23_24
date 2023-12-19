#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _NO_PIPE_ERR -1
#define _NO_FORK_ERR -2

int main () {

    char buf[4096] = {0};
    int size = 0;
    int fd[2] = {0};

    if (pipe (fd) < 0) {

        printf ("Pipe is no\n go fork urself\n");
        return _NO_PIPE_ERR;
    }

    pid_t pid = fork ();

    if (pid < 0) {

        printf ("no fork 4 u 2day\n");
        return _NO_FORK_ERR;
    }

    if (pid) { // incase paren

        // close (fd[0]);

        while ((size = read (0, buf, sizeof (buf) - 1)) > 0) {

            buf[size] = '\0';
            write (fd[1], buf, size);
            printf ("sent: %s", buf);

            for (int i = 0; i < 4096; i++) buf[i] = 0;

            size = read (fd[0], buf, sizeof (buf) - 1);
            printf ("Got returned %d bytes: %s", size, buf);

            sleep (1);
        }
    }
    else {

        // close (fd[1]);

        while ((size = read (fd[0], buf, sizeof (buf) - 1)) > 0) {

            buf[size] = '\0';
            printf ("got from parnt: %s", buf);

            write (fd[1], buf, size);

            sleep (0.1);
        }
    }
}