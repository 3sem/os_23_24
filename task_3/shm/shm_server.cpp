#include "shm.hpp"

int main () {

    for (int iter = 0; iter < 3; iter++) {

        size_t cap = SHM_SIZE[iter];
        char* buf = createShm (cap + sizeof (char) + sizeof (int));// This may impact performance as hell but it looks funny
        char* flag = buf + cap * sizeof (char);
        int* size = (int*) flag + sizeof (char);

        *flag = NUL;

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        assert (output != NULL);

        while (*flag != EOF_MET) {

            wait4Flag (flag, READ);

            *size = fwrite (buf, sizeof (char), *size, output);

            *flag = WROTE;
        }

        fclose (output);

        *flag = EOF_WRITTEN;

        shm_unlink (SHM_NAME);
    }
}