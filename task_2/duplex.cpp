#include "duplex.hpp"

int main () {

    FILE* result = fopen (fileOutputName, "w");
    FILE* input = fopen (rngIFname, "r");
    FILE* output = fopen (rngOFname, "w");

    assert (result != NULL);
    assert (input != NULL);
    assert (output != NULL);

    for (int cap = 128; cap <= 1024; cap*=2) {

        Duplex lol (cap);

        pid_t pid = fork ();

        if (pid < 0) {

            perror ("Fork gone wrong\n");

            exit (-1);
        }
        if (pid == 0) {

            while (lol.read2bufFromFile (input, 0) > 0) {

                lol.send (1);
                printf ("Child sent\n");

                int charCnt = lol.recieve (0);
                printf ("Child recieved %d\n", lol.size);

                fwrite (lol.buf, sizeof (char), charCnt, output);
            }

            lol.buf[0] = EOF;
            lol.size = 1;

            lol.send (1);

            exit (42);
        }

        while (true) {

            size_t bytesRecieved = lol.recieve (1);

            printf ("Parent recieved %lu\n", bytesRecieved);

            if (lol.buf[0] == EOF) break;

            lol.send (0);
            printf ("Parent sent\n");
        }

        testTransmissionIntegrity (&input, &output, &result, &lol);
    }

    fclose (input);
    fclose (output);
    fclose (result);
}