#include "duplex.hpp"

//Described in hpp
size_t getFileSize (char* fileName) {

    struct stat fileStat;

    if (stat (fileName, &fileStat) < 0) {

        perror ("Error during file measurement\n");
        return -1;
    }

    return fileStat.st_size;
}

//Described in hpp
void testTransmissionIntegrity (FILE** input, FILE** output, FILE** result, Duplex* lol, clock_t startTime) {

    fclose (*output);
    *output = fopen (rngOFname, "r");
    assert (*output != NULL);
    fseek (*input, SEEK_SET, 0);

    int sourceSize = getFileSize (rngIFname);
    int outputSize = getFileSize (rngOFname);

    size_t hashSource = 0;
    size_t hashOutput = 0;

    //Counts hash of input file with hash multiplier from flog
    while (lol->read2bufFromFile (*input) > 0) {

        for (int i = 0; i < lol->size; i++) {

            hashSource *= HashMult;
            hashSource += lol->buf[i];
        }
    }

    // -//- of output file -//-
    while (lol->read2bufFromFile (*output) > 0) {

        for (int i = 0; i < lol->size; i++) {

            hashOutput *= HashMult;
            hashOutput += lol->buf[i];
        }
    }

    fprintf (*result, "For buffer size of %lu bytes:\nRead %d bytes\nWritten %d bytes\nIntegrity percent of transaction: %5.2lf\nTime consumed on transaction: %.6lf\nHash value for input: %lu\nHash value for output: %lu\n------------------------\n",
        lol->cap, sourceSize, outputSize, ((double) outputSize) / ((double) sourceSize) * 100.0, (double)(clock() - startTime) / CLOCKS_PER_SEC, hashSource, hashOutput);

    fclose (*output);

    // Reopens output as write file destroying it's contents
    *output = fopen (rngOFname, "w");

    // resets ptr in input file
    fseek (*input, SEEK_SET, 0);
}