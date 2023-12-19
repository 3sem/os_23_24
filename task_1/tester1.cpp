#include <stdio.h>
#include <stdlib.h>

/// @brief This program reads up to 100 chars from stdin and prints to stdout as <%s>
/// @return 
int main () {

    char s[100] = "";
    scanf ("%s", s);
    printf ("<%s>\n", s);
}