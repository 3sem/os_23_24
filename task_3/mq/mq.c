#include "mq.h"

const char SERVER_Q_NAME_TEMPLATE[] = "/serverq_"; // add buffer size in bytes
const char CLIENT_Q_NAME_TEMPLATE[] = "/clientq_"; // add pid and buf size
const int MAX_MSG_CNT = 10; // Max amount of msg per q
const int CAP = 3; // Capacity of MSG_SIZE array
const int NAME_LEN_CAP = 128;
const size_t MSG_SIZE[] = {128, 4096, 1<<15}; // Shm sizes to test
const mode_t Q_PERM = (S_IRWXG | S_IRWXO | S_IRWXU) ^ (S_IXGRP | S_IXOTH | S_IXUSR);

/// @brief Generates file of predetermined size with random contents
/// @param argc argc from main
/// @param argv argv from main
/// @return size of generated file (-1 if not generated or some error occured with lstat)
/// @note i guess that error of reading a file's size is a sufficient evidence to something going wrong with generation of file (i.e. not generated) or file itself (i.e. bad metadata i guess)
size_t generateFile (int argc, char* argv[]) {

    assert (argc > 0);
    assert (argv != NULL);

    char cmd[100]; //String for command

    char fl = (argc > 1 && strcmp (argv[1], "--small") == 0); //flag that shows if small file should be generated

    // Command parsing
    sprintf (cmd, "dd if=/dev/urandom of=%s bs=%s count=%s", TEST_FILE_NAME,
            (fl ? SMALL_FILE_BS : BIG_FILE_BS),
                (fl ? SMALL_FILE_COUNT : BIG_FILE_COUNT));

    system (cmd); //Execution of command

    struct stat test_file_info;

    // Checking for file existence and not being broken somehow
    if (lstat (TEST_FILE_NAME, &test_file_info) == 0) return test_file_info.st_size;
    else return -1;
}

/// @brief Just like assert but optimised for current task
/// @param clause expression to check
/// @param msg message to display if true
/// @param q1 name of first q  to unlink
/// @param q2 name of second q to unlink
void asrt (bool clause, const char* msg, const char* q1, const char* q2) {
    if (!clause) {

        perror (msg);
        if (q1 != NULL) mq_unlink (q1);
        if (q2 != NULL) mq_unlink (q2);
        exit (42);
    }
}

