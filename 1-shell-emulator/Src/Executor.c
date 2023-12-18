#include "Executor.h"

ShellStatus ExecutorContextInit(ExecutorContext* ctx, char** env, int inputFd,
                                int outputFd) {
  if (!ctx || !env) return SH_BAD_ARG_PTR;

  ctx->Env = env;
  ctx->Active = 1;

  ctx->InputFd = inputFd;
  ctx->OutputFd = outputFd;

  ExecutorCleanFlags(ctx);

  return SH_SUCCESS;
}

ShellStatus Execute(ExecutorContext* ctx, char** tokens) {
  if (!ctx || !tokens) return SH_BAD_ARG_PTR;

  ExecutorCleanFlags(ctx);

  size_t endToken;
  ShellStatus status;

  for (size_t startToken = 0; tokens[startToken] != NULL;
       startToken = endToken + 1) {
    endToken = startToken;
    TokenType tokenType = ParseToken(tokens[startToken]);

    while (!TokenIsSeparator(tokenType)) {
      if (TokenIsOperator(tokenType)) {
        status = ProcessOperator(ctx, tokens, &endToken, tokenType);
        if (status != SH_SUCCESS) return status;
      }
      endToken++;
      tokenType = ParseToken(tokens[endToken]);
    }

    if (endToken == startToken)  // Empty command
      return SH_SYNTAX_ERROR;

    status = ProcessSeparator(ctx, tokens, &endToken, tokenType);
    if (status != SH_SUCCESS) return status;

    if (!ctx->ReadFdSet && startToken == 0)  // First command
      SetReadFd(ctx, ctx->InputFd);

    assert(ctx->ReadFdSet);
    assert(ctx->WriteFdSet);

    status = ExecuteSingleCommand(ctx, tokens + startToken);

    ExecutorContinue(ctx);

    if (status != SH_SUCCESS) return status;
  }

  return SH_SUCCESS;
}

ShellStatus ExecutorDestroy(ExecutorContext* ctx) {
  if (!ctx) return SH_BAD_ARG_PTR;
  return SH_SUCCESS;
}

static void ExecutorCleanFlags(ExecutorContext* ctx) {
  assert(ctx);

  ctx->ReadFdSet = 0;
  ctx->WriteFdSet = 0;

  ctx->HasPipeline = 0;
}

static ShellStatus SetReadFd(ExecutorContext* ctx, int newReadFd) {
  assert(ctx);

  if (ctx->ReadFdSet) return SH_INPUT_OVERRIDE;

  ctx->ReadFdSet = 1;
  ctx->ReadFd = newReadFd;

  return SH_SUCCESS;
}

static ShellStatus SetWriteFd(ExecutorContext* ctx, int newWriteFd) {
  assert(ctx);

  if (ctx->WriteFdSet) return SH_OUTPUT_OVERRIDE;

  ctx->WriteFdSet = 1;
  ctx->WriteFd = newWriteFd;

  return SH_SUCCESS;
}

static int CheckInternalCmds(ExecutorContext* ctx, char** tokens) {
  assert(ctx);
  assert(tokens);

  if (strcmp(tokens[0], EXIT_CMD) == 0) {
    ctx->Active = 0;
    return 1;
  }

  return 0;
}

static ShellStatus ExecuteSingleCommand(ExecutorContext* ctx, char** argv) {
  assert(ctx);
  assert(argv);

  if (CheckInternalCmds(ctx, argv)) {
    return SH_SUCCESS;
  }

  pid_t pid = fork();

  if (pid < 0) return SH_ERRNO_ERROR;

  if (pid > 0) {
    int status;
    wait(&status);

    assert(WIFEXITED(status));
    ctx->ExitCode = WEXITSTATUS(status);

    if (ctx->ExitCode != 0) return SH_PROGRAM_FAILURE;
    return SH_SUCCESS;
  }

  // Child code
  dup2(ctx->ReadFd, STDIN_FILENO);
  dup2(ctx->WriteFd, STDOUT_FILENO);

  execvpe(argv[0], argv, ctx->Env);  // Using p-version to search PATH

  perror("Failed to run program");  // Didn't find a better solution to
                                    // propagate errno
  exit(1);
}

static TokenType ParseToken(char* token) {
  if (token == NULL) return TOKEN_END;

  switch (*token) {
    case PIPE_SEPARATOR:
      return TOKEN_PIPELINE;
    case IFILE_SEPARATOR:
      return TOKEN_FILE_INPUT;
    case OFILE_SEPARATOR:
      return TOKEN_FILE_OUTPUT;
    default:
      return TOKEN_IDENTIFIER;
  }
}

static int TokenIsSeparator(TokenType type) {
  switch (type) {
    case TOKEN_END:
    case TOKEN_PIPELINE:
      return 1;
    default:
      return 0;
  }
}

static int TokenIsOperator(TokenType type) {
  switch (type) {
    case TOKEN_FILE_INPUT:
    case TOKEN_FILE_OUTPUT:
      return 1;
    default:
      return 0;
  }
}

static ShellStatus ProcessOperator(ExecutorContext* ctx, char** tokens,
                                   size_t* endTokenPtr, TokenType tokenType) {
  assert(ctx);
  assert(tokens);
  assert(endTokenPtr);

  tokens[*endTokenPtr] = NULL;

  switch (tokenType) {
    case TOKEN_FILE_INPUT: {
      int newFd = open(tokens[*endTokenPtr + 1], O_RDONLY);
      if (newFd < 0) return SH_ERRNO_ERROR;

      ShellStatus status = SetReadFd(ctx, newFd);
      if (status != SH_SUCCESS) return status;

      (*endTokenPtr)++;
      break;
    }

    case TOKEN_FILE_OUTPUT: {
      int newFd = open(tokens[*endTokenPtr + 1], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
      if (newFd < 0) return SH_ERRNO_ERROR;

      ShellStatus status = SetWriteFd(ctx, newFd);
      if (status != SH_SUCCESS) return status;

      (*endTokenPtr)++;
      break;
    }

    default:
      assert(0 && "Invalid token type");
  }

  return SH_SUCCESS;
}

static ShellStatus ProcessSeparator(ExecutorContext* ctx, char** tokens,
                                    size_t* endTokenPtr, TokenType tokenType) {
  assert(ctx);
  assert(tokens);
  assert(endTokenPtr);

  tokens[*endTokenPtr] = NULL;

  switch (tokenType) {
    case TOKEN_END: {
      if (!ctx->WriteFdSet) SetWriteFd(ctx, ctx->OutputFd);
      break;
    }

    case TOKEN_PIPELINE: {
      if (ctx->WriteFdSet) return SH_OUTPUT_OVERRIDE;

      int pipeFd[2];
      if (pipe(pipeFd) < 0) return SH_ERRNO_ERROR;

      SetWriteFd(ctx, pipeFd[1]);

      ctx->NextReadFd = pipeFd[0];
      ctx->HasPipeline = 1;

      break;
    }

    default:
      assert(0 && "Invalid token type");
  }

  return SH_SUCCESS;
}

static void ExecutorContinue(ExecutorContext* ctx) {
  assert(ctx);

  if (ctx->ReadFdSet && ctx->ReadFd != ctx->InputFd) close(ctx->ReadFd);
  if (ctx->WriteFdSet && ctx->WriteFd != ctx->OutputFd) close(ctx->WriteFd);

  if (ctx->HasPipeline) {
    ExecutorCleanFlags(ctx);
    SetReadFd(ctx, ctx->NextReadFd);
    return;
  }

  ExecutorCleanFlags(ctx);
}