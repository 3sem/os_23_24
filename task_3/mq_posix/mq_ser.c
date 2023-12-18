#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define QNAME "/example"

#define SZ 16384

int main() {
    FILE* output = fopen ("output.txt", "w");

    mqd_t server;
    unsigned prio  = 1;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_msgsize = SZ;
    attr.mq_curmsgs = 0;
    attr.mq_maxmsg = 3;

    if ((server = mq_open(QNAME, O_RDONLY | O_CREAT, 0666, &attr)) == -1) {
            perror("Server: mq open failed");
            return 1;
    }
    char buf[SZ];

    printf ("Hello\n");
    while (1) {
        int buf_len = mq_receive(server, buf, SZ, &prio);
        if (strcmp(buf, "quit") == 0) {
            break;
        }

        fwrite(buf, sizeof(char), buf_len, output);
    }

    fclose (output);

    return 0;
}