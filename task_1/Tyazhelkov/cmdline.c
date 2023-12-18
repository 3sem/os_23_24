#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const int MAX_NUM_OF_TOKENS = 2097152;
static const int MAX_STR_LEN = 2097152;
static const int MAX_NUM_OF_CMD = 2097152;

char** parse_cmd (char* cmd) {
	char** a = malloc (sizeof(char*) * MAX_NUM_OF_TOKENS);
	const char delim[] = " ";
	
	int i = 0;
	for (char* arg = strtok(cmd, delim); arg != NULL; arg = strtok(NULL, delim)) {
		size_t str_size = strlen(arg) + 1;
		a[i] = (char*) malloc (str_size);
		strncpy(a[i], arg, str_size);

		i++;
	}

	return a;
}

char*** parse_cmdline (char* cmdline) 
{
    char*** cmd_array = malloc(sizeof(char**) * MAX_NUM_OF_CMD);

    char** cmds = malloc(sizeof(char*) * MAX_NUM_OF_TOKENS);

    int cmd_len = 0;
    int num_of_cmd = 0;
    int prev_cmd_offset = 0;
    for (int i = 0; ; i++)
    {
        if (cmdline[i] == '|' || cmdline[i] == '\0')
        {
            cmd_len = i - prev_cmd_offset;

            cmds[num_of_cmd] = malloc(sizeof(char) * MAX_STR_LEN);          // forms cmd with arguments
            strncpy(cmds[num_of_cmd], cmdline + prev_cmd_offset, cmd_len);
            cmds[num_of_cmd][cmd_len] = '\0';


            cmd_array[num_of_cmd] = parse_cmd(cmds[num_of_cmd]); // parsing argument and cmd in tokens

            if (cmdline[i] == '|') // skips '|' for next cmd
                prev_cmd_offset += 2;

            prev_cmd_offset += cmd_len;
            num_of_cmd++;
            if (cmdline[i] == '\0')
                break;
        }
    }
    

    for (int i = 0; i < num_of_cmd; i++)
    {
        free(cmds[i]);
    }
    free(cmds);

    return cmd_array;
}

void seq_pipe(char ***cmd)
{
    int   p[2];
    pid_t pid;
    int   fd_in = 0;
    int   i = 0;

    while (cmd[i] != NULL) {
            pipe(p);
            if ((pid = fork()) == -1) {
                exit(1);
            } else if (pid == 0) {
                if (i > 0)
                    dup2(fd_in, 0); //stdin <- read from fd_in 
                if (cmd[i+1] != NULL)
                   dup2(p[1], 1); //stdout -> write to pipe
                close(p[0]);
                execvp((cmd)[i][0], cmd[i]);
                exit(2);
            } else {
                waitpid(NULL);
                close(p[1]);
                if (i>0)
                    close(fd_in); // old pipe from previous step is not used further, and can be destroyed
                fd_in = p[0]; //fd_in <--read from pipe
                i++;
            }
        }
    return;
}


int main() {
	while (1) {
		char str[MAX_STR_LEN];
		fgets (str, MAX_NUM_OF_TOKENS, stdin);
		*strchr(str, '\n') = '\0';
        seq_pipe(parse_cmdline(str));
	}
}
