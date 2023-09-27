#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	MAX_SIZE_CMD	256

char cmd[MAX_SIZE_CMD];				// string holder for the command

void read_cmd(){
	// get command from user
	printf("Shell-wrapper>\t");
	if (fgets(cmd, MAX_SIZE_CMD, stdin) != NULL){
		if ((strlen(cmd) > 0) && (cmd[strlen (cmd) - 1] == '\n'))
    	    	cmd[strlen (cmd) - 1] = '\0';
	}
}