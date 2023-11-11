#include "shm.hpp"

int main (int argc, char* argv[]) {

    generateFile (argc, argv);

    for (int iter = 0; iter < 3; iter++) {

        clock_t startTime = clock ();

        size_t cap = SHM_SIZE[iter];
        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        char* flag = buf + cap * sizeof (char);
        int* size = (int*) flag + sizeof (char);
        for (int i = 0; i < cap; i++) buf[i] = '\0';

        *flag = FL_NULL;

        FILE* input = fopen (TEST_FILE_NAME, "r");
        assert (input != NULL);

        while (true) {

            *size = fread (buf, sizeof (char), cap, input);

            if (*size == 0) break;

            *flag = FL_READ;

            wait4Flag (flag, FL_WROTE);
        }

        if (feof (input) == 0) {

            perror ("Some error in fread");
            exit (42);
        }

        *flag = FL_EOF;

        wait4Flag (flag, FL_EOF_CONFIRMED);

        printf ("Test with cap %d finished\n", cap);

        printf ("Elapsed time : %13.3lf ms\n", (double)(clock () - startTime) * 1000 / CLOCKS_PER_SEC);

        fclose (input);
    }

    shm_unlink (SHM_NAME);
}