#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define FORK_FAIL -1
#define PIPE_FAIL -2

// struct Nod {

//     enum Type {

//         VOID = 0,
//         PLUS = 1,
//         MIN = 2,
//         MUL = 3,
//         DIV = 4,
//         CONST = -1
//     } type = VOID;
//     Nod* left = NULL;
//     Nod* right = NULL;
//     Nod* prev = NULL;
//     double val = 0.0;

//     Nod () {

//         type = VOID;
//         left = NULL;
//         right = NULL;
//         prev = NULL;
//     }

// };


// Nod* allocNod () {

//     Nod* retVal = (Nod*) calloc (sizeof (Nod), 1);
//     assert (retVal != NULL);

//     *retVal = Nod ();
//     return retVal;
// }


int main () {

    pid_t pid = fork ();

    if (pid) {

        int a = 0, b = 0;
        char c = 0;

        scanf ("%d%c%d ", &a, &c, &b);


    }
    else {


    }

}