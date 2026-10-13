#pragma once

#include "../../../Core/Error/Status.h"
#include "../Text.space"
#include "../Unicode/Codepoint.h"

#define TEXT_UTF16_ENCODED_LENGTH_PROTOTYPE                                   \
  static inline OPSTATUS TEXT_UTF16_FUNC(EncodedLength)(                      \
      TEXT_TYPE(TChar32) codepoint, size_t *outUnits)

#define TEXT_UTF16_ENCODE_PROTOTYPE                                           \
  static inline OPSTATUS TEXT_UTF16_FUNC(Encode)(                             \
      TEXT_TYPE(TChar32) codepoint, TEXT_TYPE(TChar16) *output,               \
      size_t capacity, size_t *outUnits)

#define TEXT_UTF16_DECODE_PROTOTYPE                                           \
  static inline OPSTATUS TEXT_UTF16_FUNC(Decode)(                             \
      const TEXT_TYPE(TChar16) *input, size_t size,                           \
      TEXT_TYPE(TChar32) *outCodepoint, size_t *outUnits)

#define TEXT_UTF16_VALIDATE_PROTOTYPE                                         \
  static inline bool TEXT_UTF16_FUNC(Validate)(                               \
      const TEXT_TYPE(TChar16) *input, size_t size)

TEXT_UTF16_ENCODED_LENGTH_PROTOTYPE;
TEXT_UTF16_ENCODE_PROTOTYPE;
TEXT_UTF16_DECODE_PROTOTYPE;
TEXT_UTF16_VALIDATE_PROTOTYPE;

#include "Impl/UTF16.impl"
/* EOF */
