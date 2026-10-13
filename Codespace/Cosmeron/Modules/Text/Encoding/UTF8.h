#pragma once

#include "../../../Core/Error/Status.h"
#include "../Text.space"
#include "../Unicode/Codepoint.h"

#define TEXT_UTF8_ENCODED_LENGTH_PROTOTYPE                                    \
  static inline OPSTATUS TEXT_UTF8_FUNC(EncodedLength)(                       \
      TEXT_TYPE(TChar32) codepoint, size_t *outUnits)

#define TEXT_UTF8_ENCODE_PROTOTYPE                                            \
  static inline OPSTATUS TEXT_UTF8_FUNC(Encode)(                              \
      TEXT_TYPE(TChar32) codepoint, unsigned char *output,                    \
      size_t capacity, size_t *outUnits)

#define TEXT_UTF8_DECODE_PROTOTYPE                                            \
  static inline OPSTATUS TEXT_UTF8_FUNC(Decode)(                              \
      const unsigned char *input, size_t size,                                \
      TEXT_TYPE(TChar32) *outCodepoint, size_t *outUnits)

#define TEXT_UTF8_VALIDATE_PROTOTYPE                                          \
  static inline bool TEXT_UTF8_FUNC(Validate)(                                \
      const unsigned char *input, size_t size)

#define TEXT_UTF8_COUNT_PROTOTYPE                                             \
  static inline OPSTATUS TEXT_UTF8_FUNC(Count)(                               \
      const unsigned char *input, size_t size,                                \
      size_t *outCodepoints, size_t *outUnits)

TEXT_UTF8_ENCODED_LENGTH_PROTOTYPE;
TEXT_UTF8_ENCODE_PROTOTYPE;
TEXT_UTF8_DECODE_PROTOTYPE;
TEXT_UTF8_VALIDATE_PROTOTYPE;
TEXT_UTF8_COUNT_PROTOTYPE;

#include "Impl/UTF8.impl"
/* EOF */
