#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE_CMD 256
#define MAX_SIZE_ARG 16

#ifndef READ_CMD_H
#include "read_cmd.h"
#endif
#ifndef PURSE_H
#include "purse.h"
#endif

char cmd[MAX_SIZE_CMD];	  // string holder for the command
char *argv[MAX_SIZE_ARG]; // an array for command and arguments
pid_t pid;				  // global variable for the child process ID
char i;					  // global for loop counter

void read_cmd(); // get command string from the user
void purse();	 // convert the command string to the required format by execvp()
void run_cmd();	 // to start the shell

int main()
{
	// start the shell
	run_cmd();

	return 0;
}

void run_cmd()
{
	while (1)
	{
		// get the command from user
		read_cmd();

		// bypass empty commands
		if (!strcmp("", cmd))
			continue;

		// check for "exit" command
		if (!strcmp("exit", cmd))
			break;

		// fit the command into *argv[]
		purse();

		// fork and execute the command
		pid = fork();
		if (-1 == pid)
		{
			printf("failed to create a child\n");
		}
		else if (0 == pid)
		{
			// printf("hello from child\n");
			// execute a command
			int res = execvp(argv[0], argv);
			if (res == -1)
			{
				printf("no such command\n");
				return;
			}
		}
		else
		{
			// printf("hello from parent\n");
			// wait for the command to finish if "&" is not present
			if (NULL == argv[i])
				waitpid(pid, NULL, 0);
		}
	}
}
