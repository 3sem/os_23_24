#include "terminal.h"

// try with /bin/ls -ltr .|grep rw|grep 1|grep 89

int main()
{   
    const size_t MAX_CMD_SIZE = sysconf(_SC_ARG_MAX);

    while (1)
    {
        printf("Please enter command: ");

        char *str = NULL;

        size_t n = MAX_CMD_SIZE;
        int chars_read = getline(&str, &n, stdin); // cant pass &MAX_CMD_SIZE to getline since getline accepts int*

        str[chars_read - 1] = '|';
        str[chars_read] = '\0';

        run_cmds(str);
    }

    return 0;
}
