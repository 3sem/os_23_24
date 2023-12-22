#include <stdio.h>
#include <unistd.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void run(char *commands);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{   
    while (1)
    {
        printf("\033[0;32m" "~> " "\33[0m");

        char *commands = NULL;
        size_t max_command_length = sysconf(_SC_ARG_MAX); // Максимальная длина аргументов, которые можно подать exec'у
        int commands_length = getline(&commands, &max_command_length, stdin);

        commands[commands_length - 1] = '|'; // Нужно в токенайзере, чтобы разбить на токены последнюю команду
        commands[commands_length] = '\0';

        run(commands);
    }

    return 0;
}
