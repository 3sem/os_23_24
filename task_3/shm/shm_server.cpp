#include "shm.hpp"

int main () {

    flog ("This is a server");

    for (int iter = 0; iter < CAP; iter++) {

        size_t cap = SHM_SIZE[iter];
        flog (cap);

        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        assert (buf != NULL);

        char* flag = buf + cap;
        int* size = (int*) (flag + 1);

        *flag = FL_NULL;

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        assert (output != NULL);

        *flag |= (FL_NO_WAIT_LIMIT | FL_SERVER_READY);

        wait4Flag (flag, FL_CLIENT_READY); // waits for client process to finish setup

        flog ("Server started");

        while ((*flag & (FL_EOF | FL_ERROR)) == 0) {

            wait4Flag (flag, (FlagValues)(FL_READ | FL_EOF));

            if (*flag & FL_ERROR) exit (42);

            flog (*size);
            flog (buf);

            fwrite (buf, sizeof (char), *size, output);

            if (*flag & FL_EOF) break;

            *flag = FL_WROTE;
        }

        fclose (output);

        *flag = FL_EOF_CONFIRMED;
    }
    shm_unlink (SHM_NAME);
}