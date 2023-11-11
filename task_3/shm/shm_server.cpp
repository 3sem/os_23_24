#include "shm.hpp"

int main () {

    for (int iter = 0; iter < 3; iter++) {

        size_t cap = SHM_SIZE[iter];
        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        char* flag = buf + cap * sizeof (char);
        int* size = (int*) flag + sizeof (char);

        *flag = FL_NULL;

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        assert (output != NULL);

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