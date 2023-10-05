#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <string.h>

#include <errno.h>

    int p[2];
    pid_t pid;
    int fd_in = 0;
char * argv[80][80];

int main() {
  
    char str[80];


    int status;

    fgets(str, 80, stdin);

    int step = 0;
    for (int i = 0; i < 80; i++) {
      if (str[i] == '\n') {
        str[i - step] = NULL;
        continue;
      }
      str[i - step] = str[i];
    }

    char * token = strtok(str, "|");
    int j = 0;
    int q = 0;

    while (token != NULL) {
      argv[j++][q] = token;
      token = strtok(NULL, "|");
    }
    for(int k = 0; k < j; k++)
    {
      int c = 0;
      char * tok = strtok(argv[k][0], " ");
      while(tok != NULL)
      {
        argv[k][c++] = tok;
        tok = strtok(NULL, " ");
      }
    }  

    int i = 0;
    while (argv[i][0] != NULL) {
      pipe(p);

    if ((pid = fork()) == -1) {
          exit(1);
    } else if (pid == 0) {
        if (i > 0)
          dup2(fd_in, 0); //stdin <- read from fd_in (dup / dup2(int oldfd, int newfd) newfd <--copy( oldfd ) )
        if (argv[i+1][0] != NULL)
          dup2(p[1], 1); //stdout -> write to pipe
        close(p[0]);
        execvp((argv)[i][0], argv[i]);
        perror("EXEC:");
        exit(2);
    } else {
      wait(NULL);
      close(p[1]);
      if (i>0)
        close(fd_in); // old pipe from previous step is not used further, and can be destroyed
      fd_in = p[0]; //fd_in <--read from pipe
        i++;
    }
}
  
  return 0;
}