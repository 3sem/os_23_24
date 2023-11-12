#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define BUFFERSIZE 4096

void signal_handler(int sig, siginfo_t* info, void* context) {}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Please, enter file name and receiver's pid file\n");
        return 0;
    }

    FILE* rcv_pid = fopen(argv[2], "r");
    int pid = 0;
    fscanf(rcv_pid, "%d", &pid);

    char buffer[BUFFERSIZE];
    int num;

    FILE* input = fopen(argv[1], "rb");
    if (input == NULL) {
        return 0;
    }

    union sigval val;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;

    sigaction(SIGINT, &sa, NULL);

    val.sival_int = getpid();
    sigqueue(pid, SIGINT, val);

    while (int num = fread(buffer, sizeof(char), BUFFERSIZE, input)) {
        buffer[num + 1] = '\0';
        int i = 0;
        for (; num - i >= 4 ; i += 4) {
            val.sival_int = *((int*)buffer + i / 4);
            sigqueue(pid, SIGUSR1, val);
            pause();
        }
        for (; i < num; i++) {
            val.sival_int = (int) buffer[i];
            sigqueue(pid, SIGUSR1, val);
            pause();
        }
    }

    kill(pid, SIGUSR2);

    fclose(input);

    return 0;
}
