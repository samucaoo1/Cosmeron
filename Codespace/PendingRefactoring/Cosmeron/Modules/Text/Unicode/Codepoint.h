#pragma once

#include "../Text.space"

#define TEXT_CODEPOINT_IS_VALID_PROTOTYPE                                     \
  static inline bool TEXT_CODEPOINT_FUNC(IsValid)(TEXT_TYPE(TChar32) codepoint)
#define TEXT_CODEPOINT_IS_SCALAR_PROTOTYPE                                    \
  static inline bool TEXT_CODEPOINT_FUNC(IsScalar)(TEXT_TYPE(TChar32) codepoint)
#define TEXT_CODEPOINT_IS_ASCII_PROTOTYPE                                     \
  static inline bool TEXT_CODEPOINT_FUNC(IsASCII)(TEXT_TYPE(TChar32) codepoint)
#define TEXT_CODEPOINT_IS_CONTROL_PROTOTYPE                                   \
  static inline bool TEXT_CODEPOINT_FUNC(IsControl)(TEXT_TYPE(TChar32) codepoint)
#define TEXT_CODEPOINT_IS_WHITESPACE_PROTOTYPE                                \
  static inline bool TEXT_CODEPOINT_FUNC(IsWhitespace)(TEXT_TYPE(TChar32) codepoint)

TEXT_CODEPOINT_IS_VALID_PROTOTYPE;
TEXT_CODEPOINT_IS_SCALAR_PROTOTYPE;
TEXT_CODEPOINT_IS_ASCII_PROTOTYPE;
TEXT_CODEPOINT_IS_CONTROL_PROTOTYPE;
TEXT_CODEPOINT_IS_WHITESPACE_PROTOTYPE;

#include "Impl/Codepoint.impl"
/* EOF */
