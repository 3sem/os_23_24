#include "TokenParser.h"

ShellStatus ParseTokens(TokenParser* tp, char* string, const char* delim) {
  assert(tp);
  assert(string);
  assert(delim);

  size_t size = 0;
  char* savePtr;  // For strtok_r

  char* stringTmp = string;

  while (1) {
    char* token = strtok_r(stringTmp, delim, &savePtr);
    if (!token) break;

    if (size + 2 == tp->Capacity) {  // We should leave space for null
      size_t newCapacity = tp->Capacity * 2;

      char** newTokens =
          (char**)realloc(tp->Tokens, newCapacity * sizeof(char*));

      if (!newTokens) return SH_BAD_ALLOC;

      tp->Tokens = newTokens;
      tp->Capacity = newCapacity;
    }

    tp->Tokens[size++] = token;
    stringTmp = NULL;
  }

  tp->Tokens[size] = NULL;
  tp->Tokens[size + 1] = NULL;
  return SH_SUCCESS;
}

ShellStatus TokenParserInit(TokenParser* tp) {
  if (!tp) return SH_BAD_ARG_PTR;

  size_t capacity = INITIAL_TOKEN_ARRAY_LENGTH;
  char** tokens = (char**)malloc(capacity * sizeof(char*));

  if (!tokens) return SH_BAD_ALLOC;

  tp->Capacity = capacity;
  tp->Tokens = tokens;

  return SH_SUCCESS;
}

ShellStatus TokenParserDestroy(TokenParser* tp) {
  if (!tp) return SH_BAD_ARG_PTR;
  free(tp->Tokens);

  return SH_SUCCESS;
}

ShellStatus TokenParserDump(const TokenParser* tp) {
  if (!tp) return SH_BAD_ARG_PTR;

  char** token = tp->Tokens;
  assert(token);

  for (; *token != NULL; ++token) printf("%s ", *token);

  printf("NULL\n");

  return SH_SUCCESS;
}