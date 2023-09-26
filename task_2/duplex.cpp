#include "duplex.hpp"

int main () {

    for (int cap = 128; cap <= 1024; cap*=2) {

        Duplex lol (cap);

        FILE* input = generateRngFileOf5Gb ();
        assert (input != NULL);

        pid_t pid = fork ();

        if (pid < 0) {

            perror ("Fork gone wrong\n");

            exit (-1);
        }
        if (pid == 0) {

            FILE* output = fopen (rngOFname, "w");
            assert (output != NULL);

            setvbuf (output, NULL, _IONBF, 0);

            while (lol.read2bufFromFile (input, 0) > 0) {

                lol.send (1);
                printf ("Child sent\n");

                int charCnt = lol.recieve (0);
                printf ("Child recieved\n");

                fwrite (lol.buf, sizeof (char), charCnt, output);
            }

            lol.send (1);

            int charCnt = lol.recieve (0);

            fwrite (lol.buf, sizeof (char), charCnt, output);

            exit (42);
        }

        while (true) {

            size_t bytesRecieved = lol.recieve (1);

            printf ("bytes recieved : %lu\n", bytesRecieved);

            if (bytesRecieved == 0) break;

            lol.send (0);
            printf ("Parent sent\n");
        }

        lol.send (0);

        printf ("Finished intercocksuck\n");

        exit (42*42);
    }
}