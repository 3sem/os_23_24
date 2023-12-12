#include "mq.h"

int main (int argc, char* argv[]) {

    generateFile (argc, argv);

    char* clientQname = (char*) calloc (NAME_LEN_CAP + 1, sizeof (char));
    asrt ((clientQname != NULL), "Client: failed to allocate clientQname", NULL, NULL);

    char* serverQname = (char*) calloc (NAME_LEN_CAP + 1, sizeof (char));
    asrt ((serverQname != NULL), "Client: failed to allocate serverQname", NULL, NULL);

    char* buf = (char*) calloc (MSG_SIZE[CAP - 1] + 1, sizeof (char));
    asrt ((buf != NULL), "Client: failed to allocate buffer", NULL, NULL);

    for (int iter = 0; iter < CAP; iter++) {

        sprintf (serverQname, "%s%d", SERVER_Q_NAME_TEMPLATE, MSG_SIZE[iter]);
        sprintf (clientQname, "%s%d_%d", CLIENT_Q_NAME_TEMPLATE, getpid (), MSG_SIZE[iter]);

        perror (serverQname);
        perror (clientQname);

        struct mq_attr attr;
        attr.mq_maxmsg = MAX_MSG_CNT;
        attr.mq_msgsize = MSG_SIZE[iter];

        mqd_t clientQD = mq_open (clientQname, O_CREAT | O_RDONLY, Q_PERM, &attr);
        asrt ((clientQD != -1), "Client: failed to open clientQD", NULL, NULL);

        mqd_t serverQD = mq_open (serverQname, O_EXCL | O_WRONLY);
        while (serverQD == -1 && errno == ENOENT) serverQD = mq_open (serverQname, O_WRONLY | O_EXCL);
        asrt ((serverQD != -1), "Client: failed to open serverQD", NULL, clientQname);

        asrt ((mq_send (serverQD, clientQname, NAME_LEN_CAP, 1) == 0), "Client: failed to send clientQname to serverQD", serverQname, clientQname);

        FILE* input = fopen (TEST_FILE_NAME, "r");
        asrt ((input != NULL), "Client: failed to open input file", serverQname, clientQname);

        time_t timer = clock ();

        size_t size;
        while (size = fread (buf, sizeof (char), MSG_SIZE[iter], input)) {
            mq_send (serverQD, buf, MSG_SIZE[iter], 1);
        }

        asrt ((feof (input) != 0), "Client: some error during fread", serverQname, clientQname);

        mq_send (serverQD, "", 0, 0);

        mq_receive (clientQD, buf, MSG_SIZE[CAP - 1], (unsigned int*) &size);

        asrt ((size == 1), "Client: got incorrect response from server", serverQname, clientQname);

        printf ("Test with cap %d finished\nElapsed time : %13.3lf ms\n", MSG_SIZE[iter], (double)(clock () - timer) * 1000 / CLOCKS_PER_SEC);

        fclose (input);
        mq_close (serverQD);
        mq_close (clientQD);
        mq_unlink (serverQname);
        mq_unlink (clientQname);
    }

    free (buf);
    free (clientQname);
    free (serverQname);
}