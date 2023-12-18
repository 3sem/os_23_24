#include "Shell.h"

ShellStatus ShellInit(Shell* sh, FILE* input, FILE* output, char** env) {
  assert(input);
  assert(output);

  if (!sh) return SH_BAD_ARG_PTR;

  sh->Input = input;
  sh->Output = output;

  sh->Buffer = NULL;
  sh->Length = 0;

  ShellStatus status;

  status = ExecutorContextInit(&sh->Executor, env, fileno(sh->Input), fileno(sh->Output));

  if (status != SH_SUCCESS) return status;

  return TokenParserInit(&sh->Parser);
}

static ShellStatus ShellTick(Shell* sh) {
  assert(sh);
  ShellStatus status;

  int readLen = getline(&sh->Buffer, &sh->Length, sh->Input);
  if (readLen < 0) return SH_ERRNO_ERROR;

  assert(sh->Buffer[readLen - 1] == '\n');
  sh->Buffer[readLen - 1] = '\0';  // Remove endline character

  status = ParseTokens(&sh->Parser, sh->Buffer, DELIMETERS);

  if (status != SH_SUCCESS) return status;

  ExecutorContext* executor = &sh->Executor;
  char** tokens = sh->Parser.Tokens;

  status = Execute(executor, tokens);

  if (status != SH_SUCCESS) return status;
}

ShellStatus ShellRun(Shell* sh) {
  if (!sh) return SH_BAD_ARG_PTR;

  while (sh->Executor.Active) {
    fprintf(sh->Output, PROMPT_STR);
    ShellStatus status = ShellTick(sh);

    if (status != SH_SUCCESS)
      fprintf(stderr, "Error occured: %s\n", GetErrorDescription(status));

    if (status == SH_PROGRAM_FAILURE)
      fprintf(stderr, "Exit Code: %d\n", sh->Executor.ExitCode);
  }

  return SH_SUCCESS;
}

ShellStatus ShellDestroy(Shell* sh) {
  if (!sh) return SH_BAD_ARG_PTR;

  free(sh->Buffer);

  ShellStatus s1 = ExecutorDestroy(&sh->Executor);
  ShellStatus s2 = TokenParserDestroy(&sh->Parser);

  if (s1 != SH_SUCCESS) return s1;
  if (s2 != SH_SUCCESS) return s2;

  return SH_SUCCESS;
}
