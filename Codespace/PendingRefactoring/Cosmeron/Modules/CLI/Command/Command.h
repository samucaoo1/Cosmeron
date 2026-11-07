#pragma once
#include "../Arguments/Arguments.h"

typedef struct CLI_INS(Scanner) {
  CLI_TYPE(TArguments) arguments; const CLI_TYPE(TCommand) *command;
  size_t index, shortOffset, positionalIndex, depth;
  bool stopped;
} CLI_INS(Scanner);

/* Rescan rather than allocating an occurrence table; small command schemas are the target. */

#define CLI_COMMAND_PARSE_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Command_Parse)( \
      const CLI_TYPE(TCommand) *command, \
      int argc, \
      char **argv, \
      CLI_TYPE(TMatch) *matches, \
      size_t matchCapacity, \
      size_t *outMatchCount, \
      CLI_TYPE(TDiagnostic) *diagnostic)

CLI_COMMAND_PARSE_PROTOTYPE;

#include "Impl/Command.impl"
