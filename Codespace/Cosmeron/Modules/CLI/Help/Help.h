#pragma once
#include "../Command/Command.h"

#define CLI_HELP_WRITE_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Help_Write)( \
      const CLI_TYPE(TCommand) *command, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

CLI_HELP_WRITE_PROTOTYPE;

#include "Impl/Help.impl"
