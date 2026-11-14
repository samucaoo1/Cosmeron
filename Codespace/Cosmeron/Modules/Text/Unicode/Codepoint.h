#pragma once

#include <stdbool.h>

#include "../Impl/Common.inc"


#define TEXT_CODEPOINT_IS_VALID_PROTOTYPE                                     \
  static inline bool MODULE_FUNC(Text, Codepoint, IsValid)(MODULE_TYPE(Text, Char32) codepoint)
#define TEXT_CODEPOINT_IS_SCALAR_PROTOTYPE                                    \
  static inline bool MODULE_FUNC(Text, Codepoint, IsScalar)(MODULE_TYPE(Text, Char32) codepoint)
#define TEXT_CODEPOINT_IS_ASCII_PROTOTYPE                                     \
  static inline bool MODULE_FUNC(Text, Codepoint, IsASCII)(MODULE_TYPE(Text, Char32) codepoint)
#define TEXT_CODEPOINT_IS_CONTROL_PROTOTYPE                                   \
  static inline bool MODULE_FUNC(Text, Codepoint, IsControl)(MODULE_TYPE(Text, Char32) codepoint)
#define TEXT_CODEPOINT_IS_WHITESPACE_PROTOTYPE                                \
  static inline bool MODULE_FUNC(Text, Codepoint, IsWhitespace)(MODULE_TYPE(Text, Char32) codepoint)

TEXT_CODEPOINT_IS_VALID_PROTOTYPE;
TEXT_CODEPOINT_IS_SCALAR_PROTOTYPE;
TEXT_CODEPOINT_IS_ASCII_PROTOTYPE;
TEXT_CODEPOINT_IS_CONTROL_PROTOTYPE;
TEXT_CODEPOINT_IS_WHITESPACE_PROTOTYPE;

#include "Impl/Codepoint.impl"
/* EOF */
