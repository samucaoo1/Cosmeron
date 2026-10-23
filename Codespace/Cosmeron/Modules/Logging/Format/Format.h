#pragma once

#include "../Logging.space"

#define LOGGING_FORMAT_TEXT_PROTOTYPE                                          \
  static inline OPSTATUS LOGGING_FUNC(Format_Text)(                            \
      const LOGGING_TYPE(TRecord) *record, bool useColors, char *buffer,        \
      size_t capacity, size_t *outSize)

#define LOGGING_FORMAT_JSON_PROTOTYPE                                          \
  static inline OPSTATUS LOGGING_FUNC(Format_JSON)(                            \
      const LOGGING_TYPE(TRecord) *record, char *buffer, size_t capacity,      \
      size_t *outSize)

LOGGING_FORMAT_TEXT_PROTOTYPE;
LOGGING_FORMAT_JSON_PROTOTYPE;

#include "Impl/Format.impl"

/* EOF */
