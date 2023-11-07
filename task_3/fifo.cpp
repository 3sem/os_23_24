#include "fifo.hpp"

int main (int argc, char* argv[]) {

    createFile (argc, argv);

    unsigned int _cap[3] = {128, 4096, 1<<15};
    double elapsed_times[3];
    unsigned int resulting_hash[3];


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

            FILE* output = fopen (OUTPUT_FILE_NAME, "w");
            assert (input != NULL);

            setvbuf (output, NULL, _IONBF, 0);

            while (duplex.write2bufFromFile (input) > 0) {

                size_t CHild_send_code = duplex.send (Dpipe::C2P);

                flog (CHild_send_code);

                duplex.recieve (Dpipe::P2C);
                flog ("Child recieved");

                duplex.writeBuf2File (output);
                flog ("Child wrote to file");
            }

            exit (42);
        }

        duplex.toggleBlock (Dpipe::C2P);

        while (waitpid (pid, NULL, WNOHANG) == 0) {

            flog ("Parent wait iteration");
            if (duplex.recieve (Dpipe::C2P) > 0) {

                duplex.send (Dpipe::P2C);
            }
        }

        printf ("Test with cap %d finished\n", duplex.cap);

        printf ("Elapsed time : %13.3lf ms\n", (double)(clock () - elapsed_time) * 1000 / CLOCKS_PER_SEC);

        duplex.DTOR ();
    }

//310 309
}