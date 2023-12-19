#include "fifo.hpp"

int main (int argc, char* argv[]) {

    createFile (argc, argv);

    unsigned int _cap[3] = {128, 4096, 1<<15};
    double elapsed_times[3];

    for (int iter = 0; iter < 3; iter++) {

        Dpipe duplex (_cap[iter]);

        clock_t elapsed_time = clock ();

        pid_t pid = fork ();

        if (pid < 0) {

            perror ("Fork gone wrong");
            exit (-1);
        }
        if (pid == 0) {

            FILE* input = fopen (TEST_FILE_NAME, "r");
            assert (input != NULL);

            while (duplex.write2bufFromFile (input) > 0) {
                flog ("Child read");

                size_t outSize = duplex.send (Dpipe::C2P);
                flog ("Child sent");
                flog (outSize);

                // duplex.receive (Dpipe::P2C);
                // flog ("Child received confirmation");
            }

            exit (42);
        }

        duplex.toggleBlock (Dpipe::C2P);

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        assert (output != NULL);
        setvbuf (output, NULL, _IONBF, 0);

        while (waitpid (pid, NULL, WNOHANG) == 0) {

            flog ("Parent wait iteration");
            if (duplex.receive (Dpipe::C2P) > 0) {
                flog ("Parent received")

                duplex.writeBuf2File (output);
                flog ("Parent wrote to file");

                duplex.size = 1;
                duplex.buf[0] = '\0';

                duplex.send (Dpipe::P2C);
                flog ("Parent sent confirmation");
            }
        }

        elapsed_times[iter] = (double)(clock () - elapsed_time) * 1000 / CLOCKS_PER_SEC;

        duplex.DTOR ();
    }

    for (int i = 0; i < 3; i++) {

        printf ("Test with cap %d finished\nElapsed time : %13.3lf ms\n", _cap[i], elapsed_times[i]);
    }
}