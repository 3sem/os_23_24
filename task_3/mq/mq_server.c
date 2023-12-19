#include "mq.h"

int main () {

    char* buf = (char*) calloc (MSG_SIZE[CAP - 1] + 1, sizeof (char));
    asrt ((buf != NULL), "Server: failed to allocate buffer", NULL, NULL);

    char* serverQname = (char*) calloc (NAME_LEN_CAP + 1, sizeof (char));
    asrt ((serverQname != NULL), "Server: failed to allocate serverQname", NULL, NULL);

    char* clientQname = (char*) calloc (NAME_LEN_CAP + 1, sizeof (char));
    asrt ((clientQname != NULL), "Server: failed to allocate clientQname", NULL, NULL);

    for (int iter = 0; iter < CAP; iter++) {

        sprintf (serverQname, "%s%d", SERVER_Q_NAME_TEMPLATE, MSG_SIZE[iter]);

        struct mq_attr attr;
        attr.mq_maxmsg = MAX_MSG_CNT;
        attr.mq_msgsize = MSG_SIZE[iter];

        mqd_t serverQD = mq_open (serverQname, O_CREAT | O_RDONLY, Q_PERM, &attr);
        asrt ((serverQD != -1), "Server: failed to open serverQD", NULL, NULL);

        printf ("%d\n", serverQD);

        struct mq_attr attr1;
        mq_getattr (serverQD, &attr1);
        printf ("%d, %d, %d\n", attr1.mq_curmsgs, attr1.mq_maxmsg, attr1.mq_msgsize);

        asrt ((mq_receive (serverQD, clientQname, MSG_SIZE[iter], NULL) != -1),
        "Server: failed to receive clientQname", serverQname, NULL);

        perror (clientQname);

        mqd_t clientQD = mq_open (clientQname, O_WRONLY);
        asrt (clientQD != -1, "Server: failed to open clientQD", serverQname, NULL);

        FILE* output = fopen (OUTPUT_FILE_NAME, "w");
        asrt ((output != NULL), "Server: failed to open output file", serverQname, clientQname);

        size_t size = 0;
        unsigned int priority = 0;

        while (size = mq_receive (serverQD, buf, MSG_SIZE[iter], &priority)) {

            asrt ((size != (size_t) -1), "Server: error during mq_receive in cycle", serverQname, clientQname);
            if (priority == 0 && size == 0) break;

            asrt ((fwrite (buf, sizeof (char), size, output) > 0), "Server: failed to write to output file", serverQname, clientQname);
        }

        asrt ((mq_send (clientQD, "", 0, 1) != -1), "Server: failed to confirm end of file", serverQname, clientQname);

        fclose (output);
        mq_close (serverQD);
        mq_close (clientQD);
        mq_unlink (serverQname);
        mq_unlink (clientQname);
    }

    free (buf);
    free (clientQname);
    free (serverQname);
}