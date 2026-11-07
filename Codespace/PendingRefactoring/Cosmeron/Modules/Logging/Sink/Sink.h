#pragma once

#include "../Logging.space"

#define LOGGING_SINK_CONSOLE_INIT_PROTOTYPE                                    \
  static inline OPSTATUS LOGGING_FUNC(Sink_ConsoleInit)(                       \
      LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel,              \
      bool useColors, bool jsonFormat)

#define LOGGING_SINK_FILE_INIT_PROTOTYPE                                       \
  static inline OPSTATUS LOGGING_FUNC(Sink_FileInit)(                          \
      LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel,              \
      FILE *file, bool jsonFormat)

#define LOGGING_SINK_BUFFER_INIT_PROTOTYPE                                     \
  static inline OPSTATUS LOGGING_FUNC(Sink_BufferInit)(                        \
      LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel,              \
      char *buffer, size_t capacity, bool jsonFormat)

#define LOGGING_SINK_CUSTOM_INIT_PROTOTYPE                                     \
  static inline OPSTATUS LOGGING_FUNC(Sink_CustomInit)(                        \
      LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel,              \
      LOGGING_TYPE(SinkWriteFn) writeFn, void *target)

#define LOGGING_SINK_WRITE_PROTOTYPE                                           \
  static inline OPSTATUS LOGGING_FUNC(Sink_Write)(                             \
      LOGGING_TYPE(TSink) *sink, const LOGGING_TYPE(TRecord) *record)

LOGGING_SINK_CONSOLE_INIT_PROTOTYPE;
LOGGING_SINK_FILE_INIT_PROTOTYPE;
LOGGING_SINK_BUFFER_INIT_PROTOTYPE;
LOGGING_SINK_CUSTOM_INIT_PROTOTYPE;
LOGGING_SINK_WRITE_PROTOTYPE;

#include "Impl/Sink.impl"

/* EOF */
