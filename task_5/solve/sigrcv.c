#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

void rcv_file(FILE* output);

int END = 0;
char VAL[4]   = {};
int WRITE     = 0;
pid_t SND_PID = 0;
void configure_signals();
void sigusr1_handler(int sig, siginfo_t* info, void* context);
void sigusr2_handler(int sig, siginfo_t* info, void* context);
void sigint_handler(int sig, siginfo_t* info, void* context);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf ("Usage: ./sig_rcv outputFileName\n");
        return 0;
    }
    printf ("my pid %d\n", getpid());
    configure_signals();

    FILE* outputFile = fopen(argv[1], "wb");
    assert (outputFile != NULL);
    
    rcv_file(outputFile);

    fclose(outputFile);
}

void configure_signals() {
    union sigval val;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigusr1_handler;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_sigaction = sigusr2_handler;
    sigaction(SIGUSR2, &sa, NULL);

    sa.sa_sigaction = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
}

void rcv_file(FILE* output) {
    while (1) {
        pause();
        if (WRITE) {
            fwrite(VAL, (int) VAL[3], sizeof(char), output);
            WRITE = 0;
        }

        if (END)  {
            kill(SND_PID, SIGINT);
            return;
        }

        kill (SND_PID, SIGINT);
    }
}

void sigusr1_handler(int sig, siginfo_t* info, void* context) {
    *((int*) VAL) = info->si_value.sival_int;
    WRITE = 1;
    return;
}

void sigusr2_handler(int sig, siginfo_t* info, void* context) {
    END = 1;
    return;
}

void sigint_handler(int sig, siginfo_t* info, void* context) {
    SND_PID = info->si_value.sival_int;
    return;
}