#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Error/Status.h"
#include "../Encoding/UTF8.h"
#include "../Text.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TEXT_VIEW_STRUCT(CHAR_TYPE, SUFFIX)                                    \
  typedef struct TEXT_VIEW_TYPE(SUFFIX) {                                      \
    const CHAR_TYPE *data;                                                     \
    size_t size;                                                               \
  } TEXT_VIEW_TYPE(SUFFIX);

#define TEXT_VIEW_CREATE_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_VIEW_FUNC(SUFFIX, Create)(         \
      const CHAR_TYPE *data, size_t size)

#define TEXT_VIEW_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_VIEW_FUNC(SUFFIX, FromCStr)(       \
      const CHAR_TYPE *cstr)

#define TEXT_VIEW_EMPTY_PROTOTYPE(SUFFIX)                                      \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_VIEW_FUNC(SUFFIX, Empty)(void)

#define TEXT_VIEW_LENGTH_PROTOTYPE(SUFFIX)                                     \
  static inline size_t TEXT_VIEW_FUNC(SUFFIX, Length)(                         \
      TEXT_VIEW_TYPE(SUFFIX) view)

#define TEXT_VIEW_IS_EMPTY_PROTOTYPE(SUFFIX)                                   \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, IsEmpty)(                          \
      TEXT_VIEW_TYPE(SUFFIX) view)

#define TEXT_VIEW_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  static inline const CHAR_TYPE *TEXT_VIEW_FUNC(SUFFIX, Data)(                 \
      TEXT_VIEW_TYPE(SUFFIX) view)

#define TEXT_VIEW_AT_PROTOTYPE(CHAR_TYPE, SUFFIX)                              \
  static inline OPSTATUS TEXT_VIEW_FUNC(SUFFIX, At)(                           \
      TEXT_VIEW_TYPE(SUFFIX) view, size_t index, const CHAR_TYPE **outChar)

#define TEXT_VIEW_SLICE_PROTOTYPE(SUFFIX)                                      \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_VIEW_FUNC(SUFFIX, Slice)(          \
      TEXT_VIEW_TYPE(SUFFIX) view, size_t pos, size_t count)

#define TEXT_VIEW_PREFIX_PROTOTYPE(SUFFIX)                                     \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_VIEW_FUNC(SUFFIX, Prefix)(         \
      TEXT_VIEW_TYPE(SUFFIX) view, size_t count)

#define TEXT_VIEW_SUFFIX_PROTOTYPE(SUFFIX)                                     \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_VIEW_FUNC(SUFFIX, Suffix)(         \
      TEXT_VIEW_TYPE(SUFFIX) view, size_t count)

#define TEXT_VIEW_COMPARE_PROTOTYPE(SUFFIX)                                    \
  static inline CMPOUT TEXT_VIEW_FUNC(SUFFIX, Compare)(                        \
      TEXT_VIEW_TYPE(SUFFIX) left, TEXT_VIEW_TYPE(SUFFIX) right)

#define TEXT_VIEW_EQUAL_PROTOTYPE(SUFFIX)                                      \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, Equal)(                            \
      TEXT_VIEW_TYPE(SUFFIX) left, TEXT_VIEW_TYPE(SUFFIX) right)

#define TEXT_VIEW_EQUAL_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                      \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, EqualCStr)(                        \
      TEXT_VIEW_TYPE(SUFFIX) view, const CHAR_TYPE *cstr)

#define TEXT_VIEW_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  static inline OPSTATUS TEXT_VIEW_FUNC(SUFFIX, Find)(                         \
      TEXT_VIEW_TYPE(SUFFIX) view, CHAR_TYPE ch, size_t start,                 \
      size_t *outIndex)

#define TEXT_VIEW_FIND_VIEW_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  static inline OPSTATUS TEXT_VIEW_FUNC(SUFFIX, FindView)(                     \
      TEXT_VIEW_TYPE(SUFFIX) view, TEXT_VIEW_TYPE(SUFFIX) needle,              \
      size_t start, size_t *outIndex)

#define TEXT_VIEW_FIND_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  static inline OPSTATUS TEXT_VIEW_FUNC(SUFFIX, FindCStr)(                     \
      TEXT_VIEW_TYPE(SUFFIX) view, const CHAR_TYPE *needle, size_t start,      \
      size_t *outIndex)

#define TEXT_VIEW_STARTS_WITH_PROTOTYPE(SUFFIX)                                \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, StartsWith)(                       \
      TEXT_VIEW_TYPE(SUFFIX) view, TEXT_VIEW_TYPE(SUFFIX) prefix)

#define TEXT_VIEW_STARTS_WITH_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, StartsWithCStr)(                   \
      TEXT_VIEW_TYPE(SUFFIX) view, const CHAR_TYPE *prefix)

#define TEXT_VIEW_ENDS_WITH_PROTOTYPE(SUFFIX)                                  \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, EndsWith)(                         \
      TEXT_VIEW_TYPE(SUFFIX) view, TEXT_VIEW_TYPE(SUFFIX) suffix)

#define TEXT_VIEW_ENDS_WITH_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                  \
  static inline bool TEXT_VIEW_FUNC(SUFFIX, EndsWithCStr)(                     \
      TEXT_VIEW_TYPE(SUFFIX) view, const CHAR_TYPE *suffix)

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
TEXT_VIEW_DECLARE_ALL(TEXT_TYPE(TChar16), 16)
TEXT_VIEW_DECLARE_ALL(TEXT_TYPE(TChar32), 32)

/* UTF-8 specialized operations for Text_TView8 */
#define TEXT_VIEW8_VALIDATE_PROTOTYPE                                          \
  static inline bool TEXT_VIEW_FUNC(8, Validate)(TEXT_VIEW_TYPE(8) view)

#define TEXT_VIEW8_COUNT_CODEPOINTS_PROTOTYPE                                  \
  static inline OPSTATUS TEXT_VIEW_FUNC(8, CountCodepoints)(                   \
      TEXT_VIEW_TYPE(8) view, size_t *outCount)

#define TEXT_VIEW8_NEXT_CODEPOINT_PROTOTYPE                                    \
  static inline OPSTATUS TEXT_VIEW_FUNC(8, NextCodepoint)(                     \
      TEXT_VIEW_TYPE(8) *view, TEXT_TYPE(TChar32) *outCodepoint)

#define TEXT_VIEW8_TRIM_LEFT_PROTOTYPE                                         \
  static inline TEXT_VIEW_TYPE(8) TEXT_VIEW_FUNC(8, TrimLeft)(                 \
      TEXT_VIEW_TYPE(8) view)

#define TEXT_VIEW8_TRIM_RIGHT_PROTOTYPE                                        \
  static inline TEXT_VIEW_TYPE(8) TEXT_VIEW_FUNC(8, TrimRight)(                \
      TEXT_VIEW_TYPE(8) view)

#define TEXT_VIEW8_TRIM_PROTOTYPE                                              \
  static inline TEXT_VIEW_TYPE(8) TEXT_VIEW_FUNC(8, Trim)(                      \
      TEXT_VIEW_TYPE(8) view)

#define TEXT_VIEW8_SLICE_CODEPOINTS_PROTOTYPE                                  \
  static inline OPSTATUS TEXT_VIEW_FUNC(8, SliceCodepoints)(                   \
      TEXT_VIEW_TYPE(8) view, size_t codepointStart, size_t codepointCount,    \
      TEXT_VIEW_TYPE(8) *outView)

TEXT_VIEW8_VALIDATE_PROTOTYPE;
TEXT_VIEW8_COUNT_CODEPOINTS_PROTOTYPE;
TEXT_VIEW8_NEXT_CODEPOINT_PROTOTYPE;
TEXT_VIEW8_TRIM_LEFT_PROTOTYPE;
TEXT_VIEW8_TRIM_RIGHT_PROTOTYPE;
TEXT_VIEW8_TRIM_PROTOTYPE;
TEXT_VIEW8_SLICE_CODEPOINTS_PROTOTYPE;

#include "Impl/View.impl"
/* EOF */
