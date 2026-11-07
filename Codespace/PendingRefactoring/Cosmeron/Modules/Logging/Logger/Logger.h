#pragma once

#include "../Logging.space"
#include <stdarg.h>

#define LOGGING_LOGGER_INIT_PROTOTYPE                                          \
  static inline OPSTATUS LOGGING_FUNC(Logger_Init)(                            \
      LOGGING_TYPE(TLogger) *logger, const char *name,                         \
      LOGGING_TYPE(LogLevel) level)

#define LOGGING_LOGGER_DESTROY_PROTOTYPE                                       \
  static inline void LOGGING_FUNC(Logger_Destroy)(                             \
      LOGGING_TYPE(TLogger) *logger)

#define LOGGING_LOGGER_ADD_SINK_PROTOTYPE                                      \
  static inline OPSTATUS LOGGING_FUNC(Logger_AddSink)(                         \
      LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(TSink) *sink)

#define LOGGING_LOGGER_CLEAR_SINKS_PROTOTYPE                                   \
  static inline void LOGGING_FUNC(Logger_ClearSinks)(                          \
      LOGGING_TYPE(TLogger) *logger)

#define LOGGING_LOGGER_SET_LEVEL_PROTOTYPE                                     \
  static inline OPSTATUS LOGGING_FUNC(Logger_SetLevel)(                        \
      LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) level)

#define LOGGING_LOGGER_GET_LEVEL_PROTOTYPE                                     \
  static inline OPSTATUS LOGGING_FUNC(Logger_GetLevel)(                        \
      const LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) *outLevel)

#define LOGGING_LOGGER_LOG_RECORD_PROTOTYPE                                    \
  static inline OPSTATUS LOGGING_FUNC(Logger_LogRecord)(                       \
      LOGGING_TYPE(TLogger) *logger, const LOGGING_TYPE(TRecord) *record)

#define LOGGING_LOGGER_LOG_PROTOTYPE                                           \
  static inline OPSTATUS LOGGING_FUNC(Logger_Log)(                             \
      LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) level,              \
      const char *message, const char *file, int line, const char *function)

#define LOGGING_LOGGER_LOG_FORMAT_PROTOTYPE                                    \
  static inline OPSTATUS LOGGING_FUNC(Logger_LogFormat)(                       \
      LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) level,              \
      const char *file, int line, const char *function, const char *format, ...)

#define LOGGING_GET_DEFAULT_PROTOTYPE                                          \
  static inline LOGGING_TYPE(TLogger) *LOGGING_FUNC(GetDefault)(void)

#define LOGGING_SET_DEFAULT_PROTOTYPE                                          \
  static inline void LOGGING_FUNC(SetDefault)(LOGGING_TYPE(TLogger) *logger)

LOGGING_LOGGER_INIT_PROTOTYPE;
LOGGING_LOGGER_DESTROY_PROTOTYPE;
LOGGING_LOGGER_ADD_SINK_PROTOTYPE;
LOGGING_LOGGER_CLEAR_SINKS_PROTOTYPE;
LOGGING_LOGGER_SET_LEVEL_PROTOTYPE;
LOGGING_LOGGER_GET_LEVEL_PROTOTYPE;
LOGGING_LOGGER_LOG_RECORD_PROTOTYPE;
LOGGING_LOGGER_LOG_PROTOTYPE;
LOGGING_LOGGER_LOG_FORMAT_PROTOTYPE;
LOGGING_GET_DEFAULT_PROTOTYPE;
LOGGING_SET_DEFAULT_PROTOTYPE;

#include "Impl/Logger.impl"

/* ============================================================
 * Ergonomic Logging Macros (captures __FILE__, __LINE__, __func__)
 * ============================================================ */

#define LOG_TRACE(LOGGER, MSG) \
  LOGGING_FUNC(Logger_Log)((LOGGER), LOGGING_CONST(LEVEL_TRACE), (MSG), __FILE__, __LINE__, __func__)

#define LOG_DEBUG(LOGGER, MSG) \
  LOGGING_FUNC(Logger_Log)((LOGGER), LOGGING_CONST(LEVEL_DEBUG), (MSG), __FILE__, __LINE__, __func__)

#define LOG_INFO(LOGGER, MSG) \
  LOGGING_FUNC(Logger_Log)((LOGGER), LOGGING_CONST(LEVEL_INFO), (MSG), __FILE__, __LINE__, __func__)

#define LOG_WARN(LOGGER, MSG) \
  LOGGING_FUNC(Logger_Log)((LOGGER), LOGGING_CONST(LEVEL_WARN), (MSG), __FILE__, __LINE__, __func__)

#define LOG_ERROR(LOGGER, MSG) \
  LOGGING_FUNC(Logger_Log)((LOGGER), LOGGING_CONST(LEVEL_ERROR), (MSG), __FILE__, __LINE__, __func__)

#define LOG_FATAL(LOGGER, MSG) \
  LOGGING_FUNC(Logger_Log)((LOGGER), LOGGING_CONST(LEVEL_FATAL), (MSG), __FILE__, __LINE__, __func__)

/* EOF */
