#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  // For execvpe()
#endif

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ShellStatus.h"

#define PIPE_SEPARATOR '|'
#define IFILE_SEPARATOR '<'
#define OFILE_SEPARATOR '>'
#define EXIT_CMD "exit"

typedef enum {
  TOKEN_END,
  TOKEN_PIPELINE,
  TOKEN_FILE_INPUT,
  TOKEN_FILE_OUTPUT,
  TOKEN_IDENTIFIER
} TokenType;

typedef struct {
  char** Env;
  int ExitCode;

  int InputFd;
  int OutputFd;

  int ReadFd;
  int ReadFdSet;

  int WriteFd;
  int WriteFdSet;

  int HasPipeline;
  int NextReadFd;

  int Active;
} ExecutorContext;

#ifdef __cplusplus
extern "C" {
#endif

ShellStatus ExecutorContextInit(ExecutorContext* ctx, char** env, int inputFd,
                                int outputFd);
ShellStatus Execute(ExecutorContext* ctx, char** tokens);
ShellStatus ExecutorDestroy(ExecutorContext* ctx);

static void ExecutorCleanFlags(ExecutorContext* ctx);
static ShellStatus SetReadFd(ExecutorContext* ctx, int newReadFd);
static ShellStatus SetWriteFd(ExecutorContext* ctx, int newWriteFd);
static int CheckInternalCmds(ExecutorContext* ctx, char** tokens);
static ShellStatus ExecuteSingleCommand(ExecutorContext* ctx, char** argv);
static TokenType ParseToken(char* token);
static int TokenIsSeparator(TokenType type);
static int TokenIsOperator(TokenType type);
static ShellStatus ProcessOperator(ExecutorContext* ctx, char** tokens,
                                   size_t* endTokenPtr, TokenType tokenType);
static ShellStatus ProcessSeparator(ExecutorContext* ctx, char** tokens,
                                    size_t* endTokenPtr, TokenType tokenType);
static void ExecutorContinue(ExecutorContext* ctx);

#ifdef __cplusplus
}
#endif