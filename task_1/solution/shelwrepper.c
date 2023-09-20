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
	char* path;
	size_t n;
	n = confstr(_CS_PATH, NULL, (size_t) 0);
	path = malloc(n);
	if(path == NULL) return 0;
	confstr(_CS_PATH, path, n);
	printf("%s\n", getenv("PATH"));
    printf("%s\n", path);
    while(1)
    {
        char str[50];
        char *argv[2];
        int status;
        //scanf("%s", str);
        fgets(str, 50, stdin);
        printf("str = %s\n", str);
	
        if(strcmp(str, "q") == 0)
        {
            return 0;
        }
        char *token = strtok(str, " ");
        int j = 0;

        while(token != NULL)
        {
            argv[j++] = token;
            printf("tokens %s\n", token);
            token = strtok(NULL, " ");
        }
        printf("argv[0] %s\n", argv[0]);
        printf("argv[1] %s\n", argv[1]);
        pid_t pid = fork();

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
        }
    }
	
	return 0;
}
