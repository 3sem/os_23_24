#include "duplex.hpp"

FILE* generateRngFileOf5Gb () {

    pid_t pid = fork ();

    if (pid < 0) {

        perror ("Fork gone wrong\n");
        exit (-1);
    }
    if (pid == 0) {

        char* const com[] = {"dd", "if=/dev/urandom", "of=" rngIFname, "bs=1G", "count=5"};

        execvp (com[0], com);
    }

    waitpid (pid, NULL, 0);

    FILE* retVal = fopen (rngIFname, "r");
    assert (retVal != NULL);

    return retVal;
}

size_t getFileSize (char* fileName) {

    struct stat fileStat;

    if (stat (fileName, &fileStat) < 0) {

        perror ("Error during file measurement\n");
        return -1;
    }

    return fileStat.st_size;
}

void transferFile (char* IFname, char* OFname) {

    assert (IFname != NULL);
    assert (OFname != NULL);

    FILE* IF = fopen (IFname, "r");
    FILE* OF = fopen (OFname, "w");

    if (IF == NULL or OF == NULL) {

        perror ("Error opening files\n");
        exit (-1);
    }
}