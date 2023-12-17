#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SZ 16384

#define QNAME "/example"

int main() {
    mqd_t queue;
    struct mq_attr attr;
    unsigned prio = 1;
    attr.mq_flags = 0;
    attr.mq_msgsize = SZ;
    attr.mq_curmsgs = 0;

    if ((queue = mq_open(QNAME, O_WRONLY, 0666, &attr)) == -1) {
            perror("Client: mq open error");
            return 1;
    }

    FILE* input = fopen("input.txt", "r");
    while (1) {
        char buf[SZ-1];
        int buf_len = fread(buf, sizeof(char), SZ-1, input);

        
        if (buf_len == 0) { 
            strcpy(buf, "quit");
            mq_send(queue, buf, 5, prio);
            break;
        }

        mq_send(queue, buf, buf_len + 1, prio);
    }
    
    fclose (input);

    return 0;
}