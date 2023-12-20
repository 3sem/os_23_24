#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "common.hpp"

#define SEQ_SIZE 100

int main (int argc, char* argv[]) {

    if (argc > 3) {

        perror ("Too many args");
        exit (42);
    }
    else if (argc < 2) {

        perror ("Not enough args");
        exit (42);
    }

    char* testSubject = argv[1];
    int seqCnt = atoi (argv[2]);
    if (seqCnt < 0) {

        perror ("Negative sequence count");
        exit (42);
    }

    seqCnt = (seqCnt >= SEQ_SIZE ? SEQ_SIZE - 1 : seqCnt);

    int seq[SEQ_SIZE] = {0};

    for (int i = 1; i <= 20; i++) seq[i - 1] = i;

    for (int i = 20; i < SEQ_SIZE; i++) seq[i] = (int)(1.13 * (double) seq[i - 1]);

    printf ("Starting test\nMin thread cnt : %d\nMax thread cnt : %d\n", seq[0], seq[seqCnt]);

    system ("mv " OUTPUT_FILE_NAME " " OUTPUT_FILE_NAME ".old");
    system ("mv *.png res.old.png");

    for (int i = 0; i < seqCnt;i++) {

        char command[100] = "";
        sprintf (command, "./%s %d", testSubject, seq[i]);
        system (command);
    }

    printf ("Finished test series consisting of %d tests\n", seqCnt);
}