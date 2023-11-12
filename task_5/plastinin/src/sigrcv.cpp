#include <csignal>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

char VAL[5];
bool WRITE = false;
bool END = false;
int SND_PID;

void sigusr1_handler(int sig, siginfo_t* info, void* context) {
    *((int*)VAL) = info->si_value.sival_int;
    WRITE = true;
}

void sigusr2_handler(int sig, siginfo_t* info, void* context) {
    END = true;
}

void sigint_handler(int sig, siginfo_t* info, void* context) {
    SND_PID = info->si_value.sival_int;
    return;
}

int main(int argc, char** argv) {
    VAL[4] = '\0';
    if (argc != 2) {
        printf("Please, specify ouput file\n");
        return 0;
    }

    char* output_name = argv[1];
    FILE* output = fopen(output_name, "wb+");

    FILE* pid_file = fopen("pid", "w");
    int pid = getpid();
    fprintf(pid_file, "%d", pid);
    fclose(pid_file);

    union sigval val;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigusr1_handler;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_sigaction = sigusr2_handler;
    sigaction(SIGUSR2, &sa, NULL);
    sa.sa_sigaction = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    while(true) {
        pause();
        if (WRITE) {
            fprintf(output, "%s", VAL);
            WRITE = false;
        }
        else if (END) {
            fclose(output);
            kill(SND_PID, SIGINT);
            return 0;
        }
        kill(SND_PID, SIGINT);
    }

    return 0;
}
