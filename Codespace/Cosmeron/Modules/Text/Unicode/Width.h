#pragma once

#include "../../../Core/Condition/Status.h"
#include "../Encoding/UTF8.h"

#define TEXT_WIDTH_CODEPOINT_PROTOTYPE                                        \
  static inline OPSTATUS MODULE_FUNC(Text, Width, Codepoint)(                          \
      MODULE_TYPE(Text, Char32) codepoint, int *outWidth)

#define TEXT_WIDTH_UTF8_PROTOTYPE                                             \
  static inline OPSTATUS MODULE_FUNC(Text, Width, UTF8)(                               \
      const unsigned char *input, size_t size,                                \
      size_t *outColumns, size_t *outUnits)

TEXT_WIDTH_CODEPOINT_PROTOTYPE;
TEXT_WIDTH_UTF8_PROTOTYPE;

#include "Impl/Width.impl"
/* EOF */
