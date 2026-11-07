#pragma once

#include "../Logging.space"

#define LOGGING_RECORD_INIT_PROTOTYPE                                          \
  static inline OPSTATUS LOGGING_FUNC(Record_Init)(                            \
      LOGGING_TYPE(TRecord) *record, LOGGING_TYPE(LogLevel) level,            \
      const char *tag, const char *message, const char *file, int line,        \
      const char *function)

#define LOGGING_LEVEL_NAME_PROTOTYPE                                           \
  static inline const char *LOGGING_FUNC(Level_Name)(                          \
      LOGGING_TYPE(LogLevel) level)

#define LOGGING_LEVEL_FROM_NAME_PROTOTYPE                                      \
  static inline OPSTATUS LOGGING_FUNC(Level_FromName)(                         \
      const char *name, LOGGING_TYPE(LogLevel) *outLevel)

#define LOGGING_LEVEL_COLOR_PROTOTYPE                                          \
  static inline const char *LOGGING_FUNC(Level_Color)(                         \
      LOGGING_TYPE(LogLevel) level)

#define LOGGING_LEVEL_RESET_COLOR_PROTOTYPE                                    \
  static inline const char *LOGGING_FUNC(Level_ResetColor)(void)

LOGGING_RECORD_INIT_PROTOTYPE;
LOGGING_LEVEL_NAME_PROTOTYPE;
LOGGING_LEVEL_FROM_NAME_PROTOTYPE;
LOGGING_LEVEL_COLOR_PROTOTYPE;
LOGGING_LEVEL_RESET_COLOR_PROTOTYPE;

#include "Impl/Diagnostics.impl"

/* EOF */
