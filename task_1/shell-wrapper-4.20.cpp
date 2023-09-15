#include "shell-wrapper-4.20.hpp"

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