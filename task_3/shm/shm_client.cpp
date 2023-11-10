#include "shm.hpp"

int main (int argc, char* argv[]) {

    generateFile (argc, argv);

    for (int iter = 0; iter < 3; iter++) {

        clock_t startTime = clock ();

        size_t cap = SHM_SIZE[iter];
        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        char* flag = buf + cap * sizeof (char);
        int* size = (int*) flag + sizeof (char);

        *flag = NUL;

        FILE* input = fopen (TEST_FILE_NAME, "r");
        assert (input != NULL);

        while (*size = fread (buf, sizeof (char), cap, input) > 0 and *flag != EOF_MET) {

            *flag = READ;

            wait4Flag (flag, WROTE);
        }

        if (*flag == EOF_MET) {

            perror ("Some error occured, stopping all processes");
            *flag = EOF_MET;
            exit (42);
        }

        if (feof (input) == 0) {

            perror ("Some error during reading from file");
            *flag = EOF_MET;
            exit (42);
        }

        *flag = EOF_MET;

        wait4Flag (flag, EOF_WRITTEN);

        printf ("Test with cap %d finished\n", cap);

        printf ("Elapsed time : %13.3lf ms\n", (double)(clock () - startTime) * 1000 / CLOCKS_PER_SEC);

        fclose (input);

        shm_unlink (SHM_NAME);
    }
}