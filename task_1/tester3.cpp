#include <stdio.h>
#include <stdlib.h>

int main () {

    FILE* lol = fopen ("huynya.lol", "w");
    char lol1[1000] = {};

    scanf ("%s", lol1);
    fprintf (lol, "<%s>", lol1);
    printf ("wrote to file\n\n");

    fclose (lol);
}