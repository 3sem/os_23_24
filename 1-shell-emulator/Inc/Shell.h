#pragma once

#include <stdio.h>

#include "Executor.h"
#include "TokenParser.h"

#define DELIMETERS " "
#define PROMPT_STR "\e[34m->\e[0m$ "

typedef struct {
  FILE* Input;
  FILE* Output;

  ExecutorContext Executor;
  TokenParser Parser;

  char* Buffer;
  size_t Length;
} Shell;

ShellStatus ShellInit(Shell* sh, FILE* input, FILE* output, char** env);
ShellStatus ShellDestroy(Shell* sh);
static ShellStatus ShellTick(Shell* sh);
ShellStatus ShellRun(Shell* sh);