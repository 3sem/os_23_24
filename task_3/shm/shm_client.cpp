#include "shm.hpp"

int main (int argc, char* argv[]) {

    generateFile (argc, argv); // Generate file dependent on comline args

    flog ("This is a client");

    for (int iter = 0; iter < CAP; iter++) {

        clock_t startTime = clock (); // starts timer

        size_t cap = SHM_SIZE[iter];
        flog (cap);

        char* buf = createShm (cap + sizeof (char) + sizeof (int));
        assert (buf != NULL);

        char* flag = buf + cap * sizeof (char); // Variable for cross process communication
        int* size = (int*) (flag + 1); // Var for current size of data in buf

        FILE* input = fopen (TEST_FILE_NAME, "r");
        assert (input != NULL);

        // Adds 2 fl values to flag
        *flag |= (FL_NO_WAIT_LIMIT | FL_CLIENT_READY);

        wait4Flag (flag, FL_SERVER_READY); // Wait for error or server ready

        *flag = FL_CLIENT_READY; // This line seems to be the only one to fix the code

        flog ("Client started");

        while ((*flag & FL_ERROR) == 0) {

            // Read to buffer
            *size = fread (buf, sizeof (char), cap, input);

            if (*size == 0) break; // if read zero break

            *flag = FL_READ; // indicate that read
            flog (*size);
            flog (buf);

            wait4Flag (flag, FL_WROTE); // Wait for server to write

            if (*flag == FL_ERROR) exit (42); // if error occured during writing - exit
        }

        // If not eof <=> error => leave
        if (feof (input) == 0) {

            perror ("Some error in fread");
            flog ("Error occured, leaving");
            flogFlag (flag);
            *flag = FL_ERROR;
            exit (42);
        }

        *flag = FL_EOF;
        // Sets flag that eof reached, and waits for comfirmation
        wait4Flag (flag, FL_EOF_CONFIRMED);

        // Print results
        printf ("Test with cap %d finished\n", cap);

        printf ("Elapsed time : %13.3lf ms\n", (double)(clock () - startTime) * 1000 / CLOCKS_PER_SEC);

        fclose (input);
    }

    shm_unlink (SHM_NAME); //Unlinks shm segment
}