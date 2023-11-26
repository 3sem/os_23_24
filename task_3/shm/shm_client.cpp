#include "shm.hpp"

int main (int argc, char* argv[]) {

    generateFile (argc, argv);

    flog ("This is a client");

    for (int iter = 0; iter < 3; iter++) {

        clock_t startTime = clock ();

        size_t cap = SHM_SIZE[iter];
        flog (cap);

        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        assert (buf != NULL);

        char* flag = buf + cap * sizeof (char);
        int* size = (int*) (flag + 1);

        for (int i = 0; i < cap; i++) buf[i] = '\0';
        flog (buf);

        FILE* input = fopen (TEST_FILE_NAME, "r");
        assert (input != NULL);

        *flag |= (FL_NO_WAIT_LIMIT | FL_CLIENT_READY);

        wait4Flag (flag, FL_SERVER_READY);

        flog ("Client started");

        *flag = FL_NULL;

        while ((*flag & FL_ERROR) == 0) {

            *size = fread (buf, sizeof (char), cap, input);

            if (*size == 0) break;

            *flag = FL_READ;

            flogFlag (flag);
            flog (*size);
            flog (buf);

            wait4Flag (flag, FL_WROTE);

            if (*flag == FL_ERROR) exit (42);
        }

        if (feof (input) == 0) {

            perror ("Some error in fread");
            flog ("Error occured, leaving");
            flogFlag (flag);
            *flag = FL_ERROR;
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