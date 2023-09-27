#include "duplex.hpp"

int main () {

    // Open all files required for later operation (input, output, analisys)
    FILE* result = fopen (fileOutputName, "w");
    FILE* input = fopen (rngIFname, "r");
    FILE* output = fopen (rngOFname, "w");

    assert (result != NULL);
    assert (input != NULL);
    assert (output != NULL);

    setvbuf (result, NULL, _IONBF, 0);
    // sets analisys file buffer to zero so that if program is terminated prematurely 
    // already calculated data will be saved

    for (int cap = 128; cap <= (1 << 12); cap*=2) {

        clock_t start_T = clock ();

        Duplex dPipe (cap); // Duplex pipe c++ style structure (keyword is style)

        pid_t pid = fork ();

        if (pid < 0) {// Check is forking didn't work

            perror ("Fork gone wrong\n");

            exit (-1);
        }
        if (pid == 0) { //In child

            while (dPipe.read2bufFromFile (input) > 0) { // reads from input file while there is what to read

                dPipe.send (1); //sends to parent

                dPipe.recieve (0); // recieves back

                fwrite (dPipe.buf, sizeof (char), dPipe.size, output); //writes response to output
            }

            exit (42); // exits with answer to all questions
        }

        // In parent
        while (waitpid (-1, NULL, WNOHANG) == 0) { //Iterates while child is running

            if (dPipe.recieve (1) > 0) { // If pipe is not empty recieves...

                dPipe.send (0); // ... and sends back
            }
        }

        printf ("Left iteration with cap %d\n", cap);

        // basically compares sizes and calculates hash values of input and output
        testTransmissionIntegrity (&input, &output, &result, &dPipe, start_T);

        dPipe.DTOR (); //destructs duplex pipe
    }

    fclose (input);
    fclose (output);
    fclose (result);
}