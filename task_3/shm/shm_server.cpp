#include "shm.hpp"

int main () {

    flog ("This is a server");

    for (int iter = 0; iter < 3; iter++) {

        size_t cap = SHM_SIZE[iter];
        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        char* flag = buf + cap * sizeof (char);
        int* size = (int*) flag + sizeof (char);

        *flag |= FL_NO_WAIT_LIMIT | FL_SERVER_READY;

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        assert (output != NULL);

        wait4Flag (flag, FL_CLIENT_READY); // waits for client process to finish setup

        flog ("Server started");

        while (*flag != FL_EOF) {

            wait4Flag (flag, (FlagValues)(FL_READ | FL_EOF));

            if (*flag == FL_EOF) break;

            fwrite (buf, sizeof (char), *size, output);

            *flag = FL_WROTE;
        }

        fclose (output);

        *flag = FL_EOF_CONFIRMED;
    }
    shm_unlink (SHM_NAME);
}