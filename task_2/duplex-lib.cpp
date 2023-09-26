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

void testTransmissionIntegrity (FILE** input, FILE** output, FILE** result, Duplex* lol) {

    fclose (*output);
    *output = fopen (rngOFname, "r");
    assert (*output != NULL);
    fseek (*input, SEEK_SET, 0);

    int sourceSize = getFileSize (rngIFname);
    int outputSize = getFileSize (rngOFname);

    size_t hashSource = 0;
    size_t hashOutput = 0;

    while (lol->read2bufFromFile (*input) > 0) {

        for (int i = 0; i < lol->size; i++) {

            hashSource *= HashMult;
            hashSource += lol->buf[i];
        }
    }

    while (lol->read2bufFromFile (*output) > 0) {

        for (int i = 0; i < lol->size; i++) {

            hashOutput *= HashMult;
            hashOutput += lol->buf[i];
        }
    }

    fprintf (*result, "For buffer size of %lu bytes:\nRead %d bytes\nWritten %d bytes\nIntegrity percent of transaction: %5.2lf\nHash value for input: %lu\nHash value for output: %lu\n------------------------\n",
        lol->cap, sourceSize, outputSize, ((double) outputSize) / ((double) sourceSize) * 100.0, hashSource, hashOutput);

    fclose (*output);

    *output = fopen (rngOFname, "w");

    fseek (*input, SEEK_SET, 0);
}