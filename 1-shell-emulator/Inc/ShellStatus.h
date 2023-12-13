#pragma once

#include <errno.h>
#include <string.h>

// Error codes
typedef enum {
  SH_SUCCESS,
  SH_BAD_ARG_PTR,
  SH_BAD_ALLOC,
  SH_ERRNO_ERROR,
  SH_PROGRAM_FAILURE,
  SH_INPUT_OVERRIDE,
  SH_OUTPUT_OVERRIDE,
  SH_SYNTAX_ERROR,
} ShellStatus;

const char* GetErrorDescription(ShellStatus status);