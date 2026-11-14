#pragma once

#include "../../../Core/Condition/Status.h"
#include "../Unicode/Codepoint.h"

#define TEXT_UTF16_ENCODED_LENGTH_PROTOTYPE                                   \
  static inline OPSTATUS MODULE_FUNC(Text, UTF16, EncodedLength)(                      \
      MODULE_TYPE(Text, Char32) codepoint, size_t *outUnits)

#define TEXT_UTF16_ENCODE_PROTOTYPE                                           \
  static inline OPSTATUS MODULE_FUNC(Text, UTF16, Encode)(                             \
      MODULE_TYPE(Text, Char32) codepoint, MODULE_TYPE(Text, Char16) *output,               \
      size_t capacity, size_t *outUnits)

#define TEXT_UTF16_DECODE_PROTOTYPE                                           \
  static inline OPSTATUS MODULE_FUNC(Text, UTF16, Decode)(                             \
      const MODULE_TYPE(Text, Char16) *input, size_t size,                           \
      MODULE_TYPE(Text, Char32) *outCodepoint, size_t *outUnits)

#define TEXT_UTF16_VALIDATE_PROTOTYPE                                         \
  static inline bool MODULE_FUNC(Text, UTF16, Validate)(                               \
      const MODULE_TYPE(Text, Char16) *input, size_t size)

TEXT_UTF16_ENCODED_LENGTH_PROTOTYPE;
TEXT_UTF16_ENCODE_PROTOTYPE;
TEXT_UTF16_DECODE_PROTOTYPE;
TEXT_UTF16_VALIDATE_PROTOTYPE;

#include "Impl/UTF16.impl"
/* EOF */
