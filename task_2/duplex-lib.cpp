#include "duplex.hpp"

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