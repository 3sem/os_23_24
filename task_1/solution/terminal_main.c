#include "terminal.h"

int main()
{   
    const size_t MAX_CMD_SIZE = sysconf(_SC_ARG_MAX);

    while (1)
    {
        printf("Please enter command: ");

        char *str = NULL;

        size_t n = MAX_CMD_SIZE;
        int chars_read = getline(&str, &n, stdin); // cant pass &MAX_CMD_SIZE to getline since getline accepts int*

        str[chars_read - 1] = '\0';

        run_cmd(str);
    }

    return 0;
}
