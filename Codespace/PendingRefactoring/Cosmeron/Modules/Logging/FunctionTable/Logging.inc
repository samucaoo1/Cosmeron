#pragma once

#ifndef LOGGING_DISABLE_FUNCTION_TABLE

#define LOGGING_FUNCTION_TABLE_STRUCT()                                        \
  typedef struct LOGGING_TYPE(FunctionTable) {                                \
    OPSTATUS (*Logger_Init)(                                                   \
        LOGGING_TYPE(TLogger) *logger, const char *name,                       \
        LOGGING_TYPE(LogLevel) level);                                         \
    void (*Logger_Destroy)(LOGGING_TYPE(TLogger) *logger);                     \
    OPSTATUS (*Logger_AddSink)(                                                \
        LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(TSink) *sink);             \
    OPSTATUS (*Logger_Log)(                                                    \
        LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) level,            \
        const char *message, const char *file, int line,                       \
        const char *function);                                                 \
    const char *(*Level_Name)(LOGGING_TYPE(LogLevel) level);                   \
    OPSTATUS (*Level_FromName)(                                                \
        const char *name, LOGGING_TYPE(LogLevel) *outLevel);                   \
    OPSTATUS (*Format_Text)(                                                   \
        const LOGGING_TYPE(TRecord) *record, bool useColors, char *buffer,      \
        size_t capacity, size_t *outSize);                                     \
    OPSTATUS (*Format_JSON)(                                                   \
        const LOGGING_TYPE(TRecord) *record, char *buffer, size_t capacity,    \
        size_t *outSize);                                                      \
    OPSTATUS (*Sink_ConsoleInit)(                                              \
        LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel,            \
        bool useColors, bool jsonFormat);                                      \
    OPSTATUS (*Sink_BufferInit)(                                               \
        LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel,            \
        char *buffer, size_t capacity, bool jsonFormat);                       \
    OPSTATUS (*Sink_Write)(                                                    \
        LOGGING_TYPE(TSink) *sink, const LOGGING_TYPE(TRecord) *record);       \
  } LOGGING_TYPE(FunctionTable);

#define LOGGING_FUNCTION_TABLE_INSTANCE()                                      \
  static const LOGGING_TYPE(FunctionTable) LOGGING_NS(Functions) = {           \
    LOGGING_FUNC(Logger_Init),                                                 \
    LOGGING_FUNC(Logger_Destroy),                                              \
    LOGGING_FUNC(Logger_AddSink),                                              \
    LOGGING_FUNC(Logger_Log),                                                  \
    LOGGING_FUNC(Level_Name),                                                  \
    LOGGING_FUNC(Level_FromName),                                              \
    LOGGING_FUNC(Format_Text),                                                 \
    LOGGING_FUNC(Format_JSON),                                                 \
    LOGGING_FUNC(Sink_ConsoleInit),                                            \
    LOGGING_FUNC(Sink_BufferInit),                                             \
    LOGGING_FUNC(Sink_Write)                                                   \
  };

LOGGING_FUNCTION_TABLE_STRUCT()
LOGGING_FUNCTION_TABLE_INSTANCE()

#endif

/* EOF */
