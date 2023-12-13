#include <stdio.h>
#include <stdlib.h>

int main () {

    FILE* outFile = fopen ("outFile.test", "w");

    char buffer[1000] = "";

    scanf ("%s", buffer);
    fprintf (outFile, "<%s>", buffer);
    printf ("Wrote stdin to file\n");

    fclose (outFile);
}