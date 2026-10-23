#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Module/Package.h"
#include "../Text.space"

#include <stddef.h>
#include <stdint.h>

enum {
  TEXT_FORMAT_CONST(BYTE_UNIT_IEC) = 0U,
  TEXT_FORMAT_CONST(BYTE_UNIT_SI) = 1U,
  TEXT_FORMAT_CONST(BYTES_DEFAULT_PRECISION) = 2U,
  TEXT_FORMAT_CONST(BYTES_MAX_PRECISION) = 6U,
  TEXT_FORMAT_CONST(BYTES_IEC_BASE) = 1024U,
  TEXT_FORMAT_CONST(BYTES_SI_BASE) = 1000U
};

#if defined(TEXT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(TEXT_BYTES_HUMAN_DISABLE_FUNCTION_TABLE)
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE
#endif
#endif

PACKAGE(Text_BytesHuman,
    FUNCTION(OPSTATUS, Format,
             (char *buffer, size_t capacity, size_t *outWritten,
              uint64_t bytes)),
    FUNCTION(OPSTATUS, FormatEx,
             (char *buffer, size_t capacity, size_t *outWritten, uint64_t bytes,
              unsigned unitStandard, unsigned precision)),
    FUNCTION(OPSTATUS, Parse, (const char *str, uint64_t *outBytes))
);

/* Convenience forwards */
static inline OPSTATUS Text_Format_BytesHuman(char *buffer, size_t capacity,
                                              size_t *outWritten,
                                              uint64_t bytes) {
  return Text_BytesHuman_Format(buffer, capacity, outWritten, bytes);
}

static inline OPSTATUS Text_Format_BytesHumanEx(char *buffer, size_t capacity,
                                                size_t *outWritten,
                                                uint64_t bytes,
                                                unsigned unitStandard,
                                                unsigned precision) {
  return Text_BytesHuman_FormatEx(buffer, capacity, outWritten, bytes,
                                  unitStandard, precision);
}

static inline OPSTATUS Text_Parse_BytesHuman(const char *str,
                                             uint64_t *outBytes) {
  return Text_BytesHuman_Parse(str, outBytes);
}

static inline OPSTATUS Text_Format_ParseBytesHuman(const char *str,
                                                  uint64_t *outBytes) {
  return Text_BytesHuman_Parse(str, outBytes);
}

#include "Impl/BytesHuman.impl"
