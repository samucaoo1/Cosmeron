#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../Encoding/UTF8.h"
#include "../Format/Format.h"
#include "../View/View.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
  CNS(Text, String, INTERNAL, INITIAL_CAPACITY) = 16U,
  CNS(Text, String, INTERNAL, GROWTH_FACTOR) = 2U
};

#define TEXT_STRING_STRUCT(CHAR_TYPE, SUFFIX)                                  \
  typedef struct MODULE_TYPE(Text, String, SUFFIX) {                                    \
    CHAR_TYPE *data;                                                           \
    size_t size;                                                               \
    size_t capacity;                                                           \
  } MODULE_TYPE(Text, String, SUFFIX);

#define TEXT_STRING_INIT_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Init)(                       \
      MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_DESTROY_PROTOTYPE(SUFFIX)                                  \
  static inline void MODULE_FUNC(Text, String, SUFFIX, Destroy)(                        \
      MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_CLEAR_PROTOTYPE(SUFFIX)                                    \
  static inline void MODULE_FUNC(Text, String, SUFFIX, Clear)(                          \
      MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_RESERVE_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Reserve)(                     \
      MODULE_TYPE(Text, String, SUFFIX) *str, size_t newCapacity)

#define TEXT_STRING_SHRINK_TO_FIT_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, ShrinkToFit)(                \
      MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_LENGTH_PROTOTYPE(SUFFIX)                                   \
  static inline size_t MODULE_FUNC(Text, String, SUFFIX, Length)(                       \
      const MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_CAPACITY_PROTOTYPE(SUFFIX)                                 \
  static inline size_t MODULE_FUNC(Text, String, SUFFIX, Capacity)(                     \
      const MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_IS_EMPTY_PROTOTYPE(SUFFIX)                                 \
  static inline bool MODULE_FUNC(Text, String, SUFFIX, IsEmpty)(                        \
      const MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, CStr)(                       \
      const MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE **outCStr)

#define TEXT_STRING_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Data)(                       \
      MODULE_TYPE(Text, String, SUFFIX) *str, CHAR_TYPE **outData)

#define TEXT_STRING_AT_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, At)(                         \
      MODULE_TYPE(Text, String, SUFFIX) *str, size_t index, CHAR_TYPE **outChar)

#define TEXT_STRING_FROM_BUFFER_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, FromBuffer)(                  \
      MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *buffer, size_t count)

#define TEXT_STRING_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, FromCStr)(                   \
      MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *cstr)

#define TEXT_STRING_COPY_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Copy)(                       \
      MODULE_TYPE(Text, String, SUFFIX) *destination,                                   \
      const MODULE_TYPE(Text, String, SUFFIX) *source)

#define TEXT_STRING_PUSH_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, PushBack)(                   \
      MODULE_TYPE(Text, String, SUFFIX) *str, CHAR_TYPE ch)

#define TEXT_STRING_POP_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                      \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, PopBack)(                    \
      MODULE_TYPE(Text, String, SUFFIX) *str, CHAR_TYPE *outChar)

#define TEXT_STRING_APPEND_BUFFER_PROTOTYPE(CHAR_TYPE, SUFFIX)                 \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, AppendBuffer)(               \
      MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *buffer, size_t count)

#define TEXT_STRING_APPEND_PROTOTYPE(CHAR_TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Append)(                     \
      MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *cstr)

#define TEXT_STRING_APPEND_STRING_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, AppendString)(               \
      MODULE_TYPE(Text, String, SUFFIX) *str,                                           \
      const MODULE_TYPE(Text, String, SUFFIX) *other)

#define TEXT_STRING_INSERT_PROTOTYPE(CHAR_TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Insert)(                      \
      MODULE_TYPE(Text, String, SUFFIX) *str, size_t pos, const CHAR_TYPE *cstr)

#define TEXT_STRING_INSERT_CHAR_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, InsertChar)(                 \
      MODULE_TYPE(Text, String, SUFFIX) *str, size_t pos, CHAR_TYPE ch)

#define TEXT_STRING_ERASE_PROTOTYPE(SUFFIX)                                    \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Erase)(                       \
      MODULE_TYPE(Text, String, SUFFIX) *str, size_t pos, size_t count)

#define TEXT_STRING_COMPARE_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Compare)(                     \
      const MODULE_TYPE(Text, String, SUFFIX) *left,                                    \
      const MODULE_TYPE(Text, String, SUFFIX) *right, CMPOUT *outResult)

#define TEXT_STRING_EQUAL_PROTOTYPE(SUFFIX)                                    \
  static inline bool MODULE_FUNC(Text, String, SUFFIX, Equal)(                          \
      const MODULE_TYPE(Text, String, SUFFIX) *left,                                    \
      const MODULE_TYPE(Text, String, SUFFIX) *right)

#define TEXT_STRING_EQUAL_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline bool MODULE_FUNC(Text, String, SUFFIX, EqualCStr)(                      \
      const MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *cstr)

#define TEXT_STRING_SUBSTR_PROTOTYPE(SUFFIX)                                   \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Substr)(                      \
      const MODULE_TYPE(Text, String, SUFFIX) *str, size_t pos, size_t count,           \
      MODULE_TYPE(Text, String, SUFFIX) *outStr)

#define TEXT_STRING_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, Find)(                       \
      const MODULE_TYPE(Text, String, SUFFIX) *str, CHAR_TYPE ch, size_t start,         \
      size_t *outIndex)

#define TEXT_STRING_FIND_STR_PROTOTYPE(CHAR_TYPE, SUFFIX)                      \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, FindStr)(                    \
      const MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *needle,            \
      size_t start, size_t *outIndex)

#define TEXT_STRING_STARTS_WITH_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline bool MODULE_FUNC(Text, String, SUFFIX, StartsWith)(                     \
      const MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *prefix)

#define TEXT_STRING_ENDS_WITH_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  static inline bool MODULE_FUNC(Text, String, SUFFIX, EndsWith)(                       \
      const MODULE_TYPE(Text, String, SUFFIX) *str, const CHAR_TYPE *suffix)

#define TEXT_STRING_VIEW_PROTOTYPE(SUFFIX)                                     \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, String, SUFFIX, View)(        \
      const MODULE_TYPE(Text, String, SUFFIX) *str)

#define TEXT_STRING_SLICE_PROTOTYPE(SUFFIX)                                    \
  static inline MODULE_TYPE(Text, View, SUFFIX) MODULE_FUNC(Text, String, SUFFIX, Slice)(       \
      const MODULE_TYPE(Text, String, SUFFIX) *str, size_t pos, size_t count)

#define TEXT_STRING_FROM_VIEW_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, FromView)(                   \
      MODULE_TYPE(Text, String, SUFFIX) *str, MODULE_TYPE(Text, View, SUFFIX) view)

#define TEXT_STRING_APPEND_VIEW_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS MODULE_FUNC(Text, String, SUFFIX, AppendView)(                 \
      MODULE_TYPE(Text, String, SUFFIX) *str, MODULE_TYPE(Text, View, SUFFIX) view)

#define TEXT_STRING_EQUAL_VIEW_PROTOTYPE(SUFFIX)                               \
  static inline bool MODULE_FUNC(Text, String, SUFFIX, EqualView)(                      \
      const MODULE_TYPE(Text, String, SUFFIX) *str, MODULE_TYPE(Text, View, SUFFIX) view)

#define TEXT_STRING_DECLARE_ALL(CHAR_TYPE, SUFFIX)                             \
  TEXT_STRING_STRUCT(CHAR_TYPE, SUFFIX)                                        \
  TEXT_STRING_INIT_PROTOTYPE(SUFFIX);                                          \
  TEXT_STRING_DESTROY_PROTOTYPE(SUFFIX);                                       \
  TEXT_STRING_CLEAR_PROTOTYPE(SUFFIX);                                         \
  TEXT_STRING_RESERVE_PROTOTYPE(SUFFIX);                                       \
  TEXT_STRING_SHRINK_TO_FIT_PROTOTYPE(SUFFIX);                                 \
  TEXT_STRING_LENGTH_PROTOTYPE(SUFFIX);                                        \
  TEXT_STRING_CAPACITY_PROTOTYPE(SUFFIX);                                      \
  TEXT_STRING_IS_EMPTY_PROTOTYPE(SUFFIX);                                      \
  TEXT_STRING_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                              \
  TEXT_STRING_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX);                              \
  TEXT_STRING_AT_PROTOTYPE(CHAR_TYPE, SUFFIX);                                \
  TEXT_STRING_FROM_BUFFER_PROTOTYPE(CHAR_TYPE, SUFFIX);                        \
  TEXT_STRING_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                          \
  TEXT_STRING_COPY_PROTOTYPE(SUFFIX);                                          \
  TEXT_STRING_PUSH_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX);                          \
  TEXT_STRING_POP_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX);                           \
  TEXT_STRING_APPEND_BUFFER_PROTOTYPE(CHAR_TYPE, SUFFIX);                      \
  TEXT_STRING_APPEND_PROTOTYPE(CHAR_TYPE, SUFFIX);                             \
  TEXT_STRING_APPEND_STRING_PROTOTYPE(SUFFIX);                                 \
  TEXT_STRING_INSERT_PROTOTYPE(CHAR_TYPE, SUFFIX);                             \
  TEXT_STRING_INSERT_CHAR_PROTOTYPE(CHAR_TYPE, SUFFIX);                        \
  TEXT_STRING_ERASE_PROTOTYPE(SUFFIX);                                         \
  TEXT_STRING_COMPARE_PROTOTYPE(SUFFIX);                                       \
  TEXT_STRING_EQUAL_PROTOTYPE(SUFFIX);                                         \
  TEXT_STRING_EQUAL_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                        \
  TEXT_STRING_SUBSTR_PROTOTYPE(SUFFIX);                                        \
  TEXT_STRING_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX);                               \
  TEXT_STRING_FIND_STR_PROTOTYPE(CHAR_TYPE, SUFFIX);                           \
  TEXT_STRING_STARTS_WITH_PROTOTYPE(CHAR_TYPE, SUFFIX);                        \
  TEXT_STRING_ENDS_WITH_PROTOTYPE(CHAR_TYPE, SUFFIX);                         \
  TEXT_STRING_VIEW_PROTOTYPE(SUFFIX);                                          \
  TEXT_STRING_SLICE_PROTOTYPE(SUFFIX);                                         \
  TEXT_STRING_FROM_VIEW_PROTOTYPE(SUFFIX);                                     \
  TEXT_STRING_APPEND_VIEW_PROTOTYPE(SUFFIX);                                   \
  TEXT_STRING_EQUAL_VIEW_PROTOTYPE(SUFFIX);

TEXT_STRING_DECLARE_ALL(char, 8)
TEXT_STRING_DECLARE_ALL(MODULE_TYPE(Text, Char16), 16)
TEXT_STRING_DECLARE_ALL(MODULE_TYPE(Text, Char32), 32)

/* UTF-8 specialized operations for Text_TString8 */
#define TEXT_STRING8_APPEND_CODEPOINT_PROTOTYPE                                \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, AppendCodepoint)(                 \
      MODULE_TYPE(Text, String, 8) *str, MODULE_TYPE(Text, Char32) codepoint)

#define TEXT_STRING8_COUNT_CODEPOINTS_PROTOTYPE                                \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, CountCodepoints)(                 \
      const MODULE_TYPE(Text, String, 8) *str, size_t *outCount)

#define TEXT_STRING8_VALIDATE_PROTOTYPE                                        \
  static inline bool MODULE_FUNC(Text, String, 8, Validate)(                             \
      const MODULE_TYPE(Text, String, 8) *str)

#define TEXT_STRING8_TRIM_PROTOTYPE                                            \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, Trim)(MODULE_TYPE(Text, String, 8) *str)

#define TEXT_STRING8_TRIM_LEFT_PROTOTYPE                                       \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, TrimLeft)(MODULE_TYPE(Text, String, 8) *str)

#define TEXT_STRING8_TRIM_RIGHT_PROTOTYPE                                      \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, TrimRight)(MODULE_TYPE(Text, String, 8) *str)

#define TEXT_STRING8_FORMAT_VARIADIC_PROTOTYPE                                 \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, Format_Variadic)(                 \
      MODULE_TYPE(Text, String, 8) *str, const char *format, va_list arguments)

#define TEXT_STRING8_FORMAT_PROTOTYPE                                          \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, Format)(                          \
      MODULE_TYPE(Text, String, 8) *str, const char *format, ...)

#define TEXT_STRING8_APPEND_FORMAT_VARIADIC_PROTOTYPE                          \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, AppendFormat_Variadic)(           \
      MODULE_TYPE(Text, String, 8) *str, const char *format, va_list arguments)

#define TEXT_STRING8_APPEND_FORMAT_PROTOTYPE                                   \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, AppendFormat)(                    \
      MODULE_TYPE(Text, String, 8) *str, const char *format, ...)

#define TEXT_STRING8_SLICE_CODEPOINTS_PROTOTYPE                                \
  static inline OPSTATUS MODULE_FUNC(Text, String, 8, SliceCodepoints)(                 \
      const MODULE_TYPE(Text, String, 8) *str, size_t codepointStart,                   \
      size_t codepointCount, MODULE_TYPE(Text, View, 8) *outView)

TEXT_STRING8_APPEND_CODEPOINT_PROTOTYPE;
TEXT_STRING8_COUNT_CODEPOINTS_PROTOTYPE;
TEXT_STRING8_VALIDATE_PROTOTYPE;
TEXT_STRING8_TRIM_PROTOTYPE;
TEXT_STRING8_TRIM_LEFT_PROTOTYPE;
TEXT_STRING8_TRIM_RIGHT_PROTOTYPE;
TEXT_STRING8_FORMAT_VARIADIC_PROTOTYPE;
TEXT_STRING8_FORMAT_PROTOTYPE;
TEXT_STRING8_APPEND_FORMAT_VARIADIC_PROTOTYPE;
TEXT_STRING8_APPEND_FORMAT_PROTOTYPE;
TEXT_STRING8_SLICE_CODEPOINTS_PROTOTYPE;

#include "Impl/String.impl"
/* EOF */
