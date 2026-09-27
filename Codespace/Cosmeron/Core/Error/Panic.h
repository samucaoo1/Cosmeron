#pragma once

#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Detect/OperationSystem.h"
#include "Error.space"
#include "Status.h"

#include <stdio.h>
#include <stdlib.h>

#define PANIC_INTERNAL_FUNC(NAME) GNS3(LIB_PREFIX(Panic), Internal, NAME)

#define PANIC_TRIGGER_PROTOTYPE                                                \
  static inline void PANIC_INTERNAL_FUNC(Trigger)(                             \
      const char *message, const char *file, int line, const char *function)

#define PANIC_STACKTRACE_PROTOTYPE                                             \
  static inline void PANIC_INTERNAL_FUNC(Stacktrace)(void)

PANIC_TRIGGER_PROTOTYPE;
PANIC_STACKTRACE_PROTOTYPE;

#define PANIC(message)                                                         \
  PANIC_INTERNAL_FUNC(Trigger)((message), __FILE__, __LINE__, __func__)

#define PANIC_ASSERT(condition)                                                \
  do {                                                                         \
    if (!(condition))                                                          \
      PANIC_INTERNAL_FUNC(Trigger)("Assertion failed: " #condition, __FILE__,  \
                                   __LINE__, __func__);                         \
  } while (0)

#include "Impl/Panic.impl"
/* EOF */
