#include "Shell.h"

int main(int argc, char* argv[], char* env[]) {
  Shell sh;
  ShellInit(&sh, stdin, stdout, env);
  ShellRun(&sh);
  ShellDestroy(&sh);

  return 0;
}