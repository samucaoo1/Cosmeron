#pragma once

#include "../../../Core/Condition/Status.h"
#include "../Unicode/Codepoint.h"

#define TEXT_UTF8_ENCODED_LENGTH_PROTOTYPE                                    \
  static inline OPSTATUS MODULE_FUNC(Text, UTF8, EncodedLength)(                       \
      MODULE_TYPE(Text, Char32) codepoint, size_t *outUnits)

#define TEXT_UTF8_ENCODE_PROTOTYPE                                            \
  static inline OPSTATUS MODULE_FUNC(Text, UTF8, Encode)(                              \
      MODULE_TYPE(Text, Char32) codepoint, unsigned char *output,                    \
      size_t capacity, size_t *outUnits)

#define TEXT_UTF8_DECODE_PROTOTYPE                                            \
  static inline OPSTATUS MODULE_FUNC(Text, UTF8, Decode)(                              \
      const unsigned char *input, size_t size,                                \
      MODULE_TYPE(Text, Char32) *outCodepoint, size_t *outUnits)

#define TEXT_UTF8_VALIDATE_PROTOTYPE                                          \
  static inline bool MODULE_FUNC(Text, UTF8, Validate)(                                \
      const unsigned char *input, size_t size)

#define TEXT_UTF8_COUNT_PROTOTYPE                                             \
  static inline OPSTATUS MODULE_FUNC(Text, UTF8, Count)(                               \
      const unsigned char *input, size_t size,                                \
      size_t *outCodepoints, size_t *outUnits)

TEXT_UTF8_ENCODED_LENGTH_PROTOTYPE;
TEXT_UTF8_ENCODE_PROTOTYPE;
TEXT_UTF8_DECODE_PROTOTYPE;
TEXT_UTF8_VALIDATE_PROTOTYPE;
TEXT_UTF8_COUNT_PROTOTYPE;

#include "Impl/UTF8.impl"
/* EOF */
