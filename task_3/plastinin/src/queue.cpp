#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include "../include/queue.h"

#define KEY 1024

int queue_receiver() {
    timeval start_time;
    gettimeofday(&start_time, NULL);

    FILE* output = fopen("output_queue", "wb+");
    FILE* data = fopen("data_queue.csv", "a");

    int msgid;
    key_t key = KEY;

    if ((msgid = msgget(key, 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    msg_buf buf;

    while(true) {
        if (msgrcv(msgid, &buf, MSG_SZ + sizeof(long), 1, 0) < 0) {
            continue;
        }
        buf.mtext[MSG_SZ - 1] = '\0';
        if (strcmp(buf.mtext, "quit") == 0) {
            break;
        }
        int len = strlen(buf.mtext);
        fwrite(buf.mtext, sizeof(char), len, output);
    }

    timeval end_time;
    gettimeofday(&end_time, NULL);

    fprintf(data, "%lf,\n", (end_time.tv_sec - end_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6);

    fclose(output);
    fclose(data);

    return 0;
}

int queue_sender(char* input_name) {
    FILE* input = fopen(input_name, "r");

    int msgid;
    key_t key = KEY;
    int prio;

    if ((msgid = msgget(key, IPC_CREAT | 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    msg_buf buf;
    buf.mtype = 1;
    while(int num = fread(buf.mtext, sizeof(char), MSG_SZ - 1, input)) {
        buf.mtext[num] = '\0';
        int buf_len = num + 1;
        msgsnd(msgid, &buf, buf_len, 0);
    }

    strcpy(buf.mtext, "quit");
    int buf_len = strlen(buf.mtext) + 1 + sizeof(long);
    msgsnd(msgid, &buf, buf_len, 0);

    return 0;
}


