#include "duplex.hpp"

int main () {

    FILE* result = fopen (fileOutputName, "w");
    FILE* input = fopen (rngIFname, "r");
    FILE* output = fopen (rngOFname, "w");

    assert (result != NULL);
    assert (input != NULL);
    assert (output != NULL);

    setvbuf (result, NULL, _IONBF, 0);

    for (int cap = 128; cap <= 1024; cap*=2) {

        Duplex lol (cap);

        pid_t pid = fork ();

        if (pid < 0) {

            perror ("Fork gone wrong\n");

            exit (-1);
        }
        if (pid == 0) {

            while (lol.read2bufFromFile (input) > 0) {

                lol.send (1);
                // printf ("Child sent\n");

                lol.recieve (0);
                // printf ("Child recieved %d\n", lol.size);

                fwrite (lol.buf, sizeof (char), lol.size, output);
            }

            exit (42);
        }

        while (waitpid (-1, NULL, WNOHANG) == 0) {

            if (lol.recieve (1) > 0) {

                // printf ("Parent recieved\n");
                lol.send (0);
                // printf ("Parent sent\n");
            }
        }

        printf ("Left iteration with cap %d", cap);

        testTransmissionIntegrity (&input, &output, &result, &lol);

        lol.DTOR ();
    }

    fclose (input);
    fclose (output);
    fclose (result);
}