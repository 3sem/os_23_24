#include "shm.hpp"

int main () {

    flog ("This is a server");

    for (int iter = 0; iter < CAP; iter++) {

        size_t cap = SHM_SIZE[iter];
        flog (cap);

        // Basically copy of code in client (better create separate function for this)
        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        assert (buf != NULL);

        char* flag = buf + cap;
        int* size = (int*) (flag + 1);

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        assert (output != NULL);

        // Adds to flag that server is ready + no wait limit
        *flag |= (FL_NO_WAIT_LIMIT | FL_SERVER_READY);

        wait4Flag (flag, FL_CLIENT_READY); // waits for client process to finish setup

        flog ("Server started");

        // Iterate if not eof and not error
        while ((*flag & (FL_EOF | FL_ERROR)) == 0) {

            // Wait for client to read
            wait4Flag (flag, (FlagValues)(FL_READ | FL_EOF));

            if (*flag & FL_ERROR) exit (42); // If error - exit

            flog (*size);
            flog (buf);

            // Writes buffer contents to file
            fwrite (buf, sizeof (char), *size, output);

            if (*flag & FL_EOF) break; // if EOF was reached - break

            *flag = FL_WROTE;
        }

        fclose (output);

        *flag = FL_EOF_CONFIRMED; // Informs that eof was recieved
    }
    shm_unlink (SHM_NAME); // Unlinks shm segment
}