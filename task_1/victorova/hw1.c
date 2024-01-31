#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFSIZE 4056

void onecyc() {
  char buf[BUFSIZE];
  char* asks[BUFSIZE] = {NULL};
  
  fgets(buf, BUFSIZE, stdin);

  int i = 0;
  //printf("%s\n", buf);
  
  //printf("%s %d %d\n", buf, buf[0], buf[1]);


  char * token = strtok(buf, "|");
  int j = 0;
  
  while (token != NULL) 
  {
      asks[j++] = token;
      token = strtok(NULL, "|");
      //printf("%s %s\n", asks[j-1], asks [j]);
  }        

  int pipefd[2];
  int fin = 0;
  i = 0;

  while (asks[i] != NULL) {
  
      if (asks[i][0] == ' ')
          strcpy(asks[i], asks[i]+1);
      //printf("asks i: %s\n", asks[i]);
      
      j = 0;
      token = NULL;
      token = strtok(asks[i], " \n");
      
      char* ask[BUFSIZE] = {NULL};
      while (token != NULL) 
      {
          
          ask[j++] = token;
          token = strtok(NULL, " \n");
          //printf("%s %s\n", ask[j-1], ask[j]);
      }
  
      if (pipe(pipefd)<0)
          exit(3);
      
      pid_t pid = fork();
      
      if (pid == -1) {
          //printf("pid probs\n");
          exit(1);
          
      } else if (pid == 0) { // child to parent
      
          if (i > 0)
              dup2(fin, 0);
            
          if (asks[i+1] != NULL)
              dup2(pipefd[1], 1);
            
          close(pipefd[0]);
          execvp(asks[i], ask);
          perror("EXEC");
          exit(2);
          
      } else { //parent
      
          wait(NULL);
          close(pipefd[1]);
          if (i > 0)
              close(fin);
          fin = pipefd[0];
          i++;
      }
  }
  
  return;
}


int main() {
  while(1)
  {
      printf("\ninput a command, please:\n");
      onecyc();
       
  }
  return 0;
}
