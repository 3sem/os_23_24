#include <stdio.h>
#include <unistd.h>

char **parse_cmd(const char *arg);

char **prep_cmds(const char *arg);

void seq_pipe(char ***cmds);

void run_cmds(char *arg);
