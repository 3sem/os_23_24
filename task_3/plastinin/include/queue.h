#ifndef QUEUE_H
#define QUEUE_H

#define MSG_SZ 4096

struct msg_buf {
    long mtype;
    char mtext[MSG_SZ];
};

int queue_sender(char* input_name);

int queue_receiver();

#endif //QUEUE_H
