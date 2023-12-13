#include "ShellStatus.h"

const char* GetErrorDescription(ShellStatus status) {
  switch (status) {
    case SH_SUCCESS:
      return "Success";

    case SH_BAD_ARG_PTR:
      return "Bad pointer argument";

    case SH_BAD_ALLOC:
      return "Failed to allocate memory";

    case SH_ERRNO_ERROR:
      return strerror(errno);

    case SH_PROGRAM_FAILURE:
      return "Program return non-zero exit code";

    case SH_INPUT_OVERRIDE:
      return "Input override";

    case SH_OUTPUT_OVERRIDE:
      return "Output override";

    case SH_SYNTAX_ERROR:
      return "Syntax error";

    default:
      // assert(0 && "Unknown Error Code");
      return "Unknown Error Code";
  }
}