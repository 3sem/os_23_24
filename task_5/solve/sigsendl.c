#include <bits/types/siginfo_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>

const size_t MAX_BUFF_SIZE = 4096;

void snd_pid (pid_t rcv_pid);
void snd_file (FILE* input_file, pid_t rcv_pid);
void sigint_handler(int, siginfo_t*, void*);
void configure_signals();

void sigint_handler(int sig, siginfo_t* info, void* context) {};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf ("Usage: ./sig_snd filename rcv_pid\n");
        return 0;
    }
    configure_signals();

    pid_t rcv_pid = atoi(argv[2]);

    FILE* input_file = fopen(argv[1], "rb");
    assert (input_file != NULL);

    snd_pid(rcv_pid);

    snd_file(input_file, rcv_pid);

    fclose (input_file);
}

void snd_file (FILE* input_file, pid_t rcv_pid) {
    union sigval val = {};
    char buffer[MAX_BUFF_SIZE];
    int num_readed = 0;

    // Здесь я передаю по три байта, где в последнем байте храниться длина данного сообщения 
    while (num_readed = fread(buffer, sizeof(char), MAX_BUFF_SIZE, input_file)) {
        int i = 0;
        char package[4] = {};
        for (; num_readed - i > 3; i+=3) {
            memcpy(package, buffer + i, 3);
            package[3] = 0x03;
            val.sival_int = *(int*) package;
            sigqueue (rcv_pid, SIGUSR1, val);
            pause();
        }
        if (i < num_readed) {
            memcpy(package, buffer + i, num_readed - i);
            package[3] = num_readed - i;
            val.sival_int = *(int*) package;
            sigqueue (rcv_pid, SIGUSR1, val);
            pause();
        }

    }

    kill(rcv_pid, SIGUSR2);

    return ;
}

void snd_pid (pid_t rcv_pid) {
    union sigval val;
    val.sival_int = getpid();
    sigqueue(rcv_pid, SIGINT, val);
}

void configure_signals() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigint_handler;
    sigaction(SIGINT, &sa, NULL); 
}