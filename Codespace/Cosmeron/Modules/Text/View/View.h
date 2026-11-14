#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../Encoding/UTF8.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TEXT_VIEW_STRUCT(CHAR_TYPE, SUFFIX)                                    \
  typedef struct MODULE_TYPE(Text, View, SUFFIX) {                                      \
    const CHAR_TYPE *data;                                                     \
    size_t size;                                                               \
  } MODULE_TYPE(Text, View, SUFFIX);

#define TEXT_VIEW_CREATE_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, View, SUFFIX, Create)(         \
      const CHAR_TYPE *data, size_t size)

#define TEXT_VIEW_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, View, SUFFIX, FromCStr)(       \
      const CHAR_TYPE *cstr)

#define TEXT_VIEW_EMPTY_PROTOTYPE(SUFFIX)                                      \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, View, SUFFIX, Empty)(void)

#define TEXT_VIEW_LENGTH_PROTOTYPE(SUFFIX)                                     \
  static inline size_t MODULE_FUNC(Text, View, SUFFIX, Length)(                         \
      MODULE_TYPE(Text, View, SUFFIX) view)

#define TEXT_VIEW_IS_EMPTY_PROTOTYPE(SUFFIX)                                   \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, IsEmpty)(                          \
      MODULE_TYPE(Text, View, SUFFIX) view)

#define TEXT_VIEW_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  static inline const CHAR_TYPE *MODULE_FUNC(Text, View, SUFFIX, Data)(                 \
      MODULE_TYPE(Text, View, SUFFIX) view)

#define TEXT_VIEW_AT_PROTOTYPE(CHAR_TYPE, SUFFIX)                              \
  static inline OPSTATUS MODULE_FUNC(Text, View, SUFFIX, At)(                           \
      MODULE_TYPE(Text, View, SUFFIX) view, size_t index, const CHAR_TYPE **outChar)

#define TEXT_VIEW_SLICE_PROTOTYPE(SUFFIX)                                      \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, View, SUFFIX, Slice)(          \
      MODULE_TYPE(Text, View, SUFFIX) view, size_t pos, size_t count)

#define TEXT_VIEW_PREFIX_PROTOTYPE(SUFFIX)                                     \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, View, SUFFIX, Prefix)(         \
      MODULE_TYPE(Text, View, SUFFIX) view, size_t count)

#define TEXT_VIEW_SUFFIX_PROTOTYPE(SUFFIX)                                     \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, View, SUFFIX, Suffix)(         \
      MODULE_TYPE(Text, View, SUFFIX) view, size_t count)

#define TEXT_VIEW_COMPARE_PROTOTYPE(SUFFIX)                                    \
  static inline CMPOUT MODULE_FUNC(Text, View, SUFFIX, Compare)(                        \
      MODULE_TYPE(Text, View, SUFFIX) left, MODULE_TYPE(Text, View, SUFFIX) right)

#define TEXT_VIEW_EQUAL_PROTOTYPE(SUFFIX)                                      \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, Equal)(                            \
      MODULE_TYPE(Text, View, SUFFIX) left, MODULE_TYPE(Text, View, SUFFIX) right)

#define TEXT_VIEW_EQUAL_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                      \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, EqualCStr)(                        \
      MODULE_TYPE(Text, View, SUFFIX) view, const CHAR_TYPE *cstr)

#define TEXT_VIEW_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  static inline OPSTATUS MODULE_FUNC(Text, View, SUFFIX, Find)(                         \
      MODULE_TYPE(Text, View, SUFFIX) view, CHAR_TYPE ch, size_t start,                 \
      size_t *outIndex)

#define TEXT_VIEW_FIND_VIEW_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Text, View, SUFFIX, FindView)(                     \
      MODULE_TYPE(Text, View, SUFFIX) view, MODULE_TYPE(Text, View, SUFFIX) needle,              \
      size_t start, size_t *outIndex)

#define TEXT_VIEW_FIND_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Text, View, SUFFIX, FindCStr)(                     \
      MODULE_TYPE(Text, View, SUFFIX) view, const CHAR_TYPE *needle, size_t start,      \
      size_t *outIndex)

#define TEXT_VIEW_STARTS_WITH_PROTOTYPE(SUFFIX)                                \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, StartsWith)(                       \
      MODULE_TYPE(Text, View, SUFFIX) view, MODULE_TYPE(Text, View, SUFFIX) prefix)

#define TEXT_VIEW_STARTS_WITH_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, StartsWithCStr)(                   \
      MODULE_TYPE(Text, View, SUFFIX) view, const CHAR_TYPE *prefix)

#define TEXT_VIEW_ENDS_WITH_PROTOTYPE(SUFFIX)                                  \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, EndsWith)(                         \
      MODULE_TYPE(Text, View, SUFFIX) view, MODULE_TYPE(Text, View, SUFFIX) suffix)

#define TEXT_VIEW_ENDS_WITH_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                  \
  static inline bool MODULE_FUNC(Text, View, SUFFIX, EndsWithCStr)(                     \
      MODULE_TYPE(Text, View, SUFFIX) view, const CHAR_TYPE *suffix)

#define TEXT_VIEW_DECLARE_ALL(CHAR_TYPE, SUFFIX)                               \
  TEXT_VIEW_STRUCT(CHAR_TYPE, SUFFIX)                                          \
  TEXT_VIEW_CREATE_PROTOTYPE(CHAR_TYPE, SUFFIX);                               \
  TEXT_VIEW_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                            \
  TEXT_VIEW_EMPTY_PROTOTYPE(SUFFIX);                                           \
  TEXT_VIEW_LENGTH_PROTOTYPE(SUFFIX);                                          \
  TEXT_VIEW_IS_EMPTY_PROTOTYPE(SUFFIX);                                        \
  TEXT_VIEW_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX);                                 \
  TEXT_VIEW_AT_PROTOTYPE(CHAR_TYPE, SUFFIX);                                   \
  TEXT_VIEW_SLICE_PROTOTYPE(SUFFIX);                                           \
  TEXT_VIEW_PREFIX_PROTOTYPE(SUFFIX);                                          \
  TEXT_VIEW_SUFFIX_PROTOTYPE(SUFFIX);                                          \
  TEXT_VIEW_COMPARE_PROTOTYPE(SUFFIX);                                         \
  TEXT_VIEW_EQUAL_PROTOTYPE(SUFFIX);                                           \
  TEXT_VIEW_EQUAL_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                           \
  TEXT_VIEW_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX);                                \
  TEXT_VIEW_FIND_VIEW_PROTOTYPE(CHAR_TYPE, SUFFIX);                            \
  TEXT_VIEW_FIND_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                            \
  TEXT_VIEW_STARTS_WITH_PROTOTYPE(SUFFIX);                                     \
  TEXT_VIEW_STARTS_WITH_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                     \
  TEXT_VIEW_ENDS_WITH_PROTOTYPE(SUFFIX);                                       \
  TEXT_VIEW_ENDS_WITH_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);

TEXT_VIEW_DECLARE_ALL(char, 8)
TEXT_VIEW_DECLARE_ALL(MODULE_TYPE(Text, Char16), 16)
TEXT_VIEW_DECLARE_ALL(MODULE_TYPE(Text, Char32), 32)

/* UTF-8 specialized operations for Text_TView8 */
#define TEXT_VIEW8_VALIDATE_PROTOTYPE                                          \
  static inline bool MODULE_FUNC(Text, View, 8, Validate)(MODULE_TYPE(Text, View, 8) view)

#define TEXT_VIEW8_COUNT_CODEPOINTS_PROTOTYPE                                  \
  static inline OPSTATUS MODULE_FUNC(Text, View, 8, CountCodepoints)(                   \
      MODULE_TYPE(Text, View, 8) view, size_t *outCount)

#define TEXT_VIEW8_NEXT_CODEPOINT_PROTOTYPE                                    \
  static inline OPSTATUS MODULE_FUNC(Text, View, 8, NextCodepoint)(                     \
      MODULE_TYPE(Text, View, 8) *view, MODULE_TYPE(Text, Char32) *outCodepoint)

#define TEXT_VIEW8_TRIM_LEFT_PROTOTYPE                                         \
  static inline MODULE_TYPE(Text, View, 8) MODULE_FUNC(Text, View, 8, TrimLeft)(                 \
      MODULE_TYPE(Text, View, 8) view)

#define TEXT_VIEW8_TRIM_RIGHT_PROTOTYPE                                        \
  static inline MODULE_TYPE(Text, View, 8) MODULE_FUNC(Text, View, 8, TrimRight)(                \
      MODULE_TYPE(Text, View, 8) view)

#define TEXT_VIEW8_TRIM_PROTOTYPE                                              \
  static inline MODULE_TYPE(Text, View, 8) MODULE_FUNC(Text, View, 8, Trim)(                      \
      MODULE_TYPE(Text, View, 8) view)

#define TEXT_VIEW8_SLICE_CODEPOINTS_PROTOTYPE                                  \
  static inline OPSTATUS MODULE_FUNC(Text, View, 8, SliceCodepoints)(                   \
      MODULE_TYPE(Text, View, 8) view, size_t codepointStart, size_t codepointCount,    \
      MODULE_TYPE(Text, View, 8) *outView)

TEXT_VIEW8_VALIDATE_PROTOTYPE;
TEXT_VIEW8_COUNT_CODEPOINTS_PROTOTYPE;
TEXT_VIEW8_NEXT_CODEPOINT_PROTOTYPE;
TEXT_VIEW8_TRIM_LEFT_PROTOTYPE;
TEXT_VIEW8_TRIM_RIGHT_PROTOTYPE;
TEXT_VIEW8_TRIM_PROTOTYPE;
TEXT_VIEW8_SLICE_CODEPOINTS_PROTOTYPE;

#include "Impl/View.impl"
/* EOF */
