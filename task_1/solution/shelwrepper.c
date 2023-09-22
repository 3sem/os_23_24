#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>






int main()
{
    while(1){
    char str[80];
    int status;
    char *argv[80];

        for(int i = 0; i < 80; i++)
        { argv[i] = NULL; }
        //scanf("%s", str);
        fgets(str, 80, stdin);
        //printf("str = %s\n", str);
        int step = 0;
        for(int i = 0; i < 80; i++)
        {
          if(str[i] == '|')
          {
            i+=2;
            step+=2;
          }
          else if(str[i] == '\n'){ 
              str[i - step] = NULL;
              continue;
          }
          str[i - step] = str[i];
        }


        char *token = strtok(str, " ");
        int j = 0;

        while(token != NULL)
        {
            argv[j++] = token;
      //      printf("argv[%d] %s\n", &j, argv[j-1]);
            token = strtok(NULL, " ");
        }


    

        char* command = "ls";
        char* argument_list[] = {"ls", "-l", NULL};
  int   p[2];
  pid_t pid;
  int   fd_in = 0;
  int   i = 0;

  while (argv[i] != NULL) {
    pipe(p);
    //printf("New used pipe descriptors: %d %d\n",p[0],p[1]);
    //printf("Input descriptor for current child process: %d\n", fd_in);
    if ((pid = fork()) == -1) {
          exit(1);
    } else if (pid == 0) {
        if (i > 0)
          dup2(fd_in, 0); //stdin <- read from fd_in (dup / dup2(int oldfd, int newfd) newfd <--copy( oldfd ) )
        if (argv[i+1] != NULL)
          dup2(p[1], 1); //stdout -> write to pipe
        close(p[0]);
        execvp(*argv, argv);
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






      /* pid_t pid = fork();

        if(pid==0)
        {
            if(execvp(*argv, argv)<0)
            {
                perror("EXEC: ");
                printf("\n");
                return 0;
            }
        }else if(pid > 0)
        {
            waitpid(pid, &status, WUNTRACED);
            if(WIFEXITED(status)) printf("exited, status=%d\n", WEXITSTATUS(status));
            printf("\n");
        }*/
    
    }
	return 0;
}
