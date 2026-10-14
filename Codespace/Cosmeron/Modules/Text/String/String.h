#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../Encoding/UTF8.h"
#include "../Format/Format.h"
#include "../Text.space"
#include "../View/View.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
  TEXT_STRING_CINS(INITIAL_CAPACITY) = 16U,
  TEXT_STRING_CINS(GROWTH_FACTOR) = 2U
};

#define TEXT_STRING_STRUCT(CHAR_TYPE, SUFFIX)                                  \
  typedef struct TEXT_STRING_TYPE(SUFFIX) {                                    \
    CHAR_TYPE *data;                                                           \
    size_t size;                                                               \
    size_t capacity;                                                           \
  } TEXT_STRING_TYPE(SUFFIX);

#define TEXT_STRING_INIT_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Init)(                       \
      TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_DESTROY_PROTOTYPE(SUFFIX)                                  \
  static inline void TEXT_STRING_FUNC(SUFFIX, Destroy)(                        \
      TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_CLEAR_PROTOTYPE(SUFFIX)                                    \
  static inline void TEXT_STRING_FUNC(SUFFIX, Clear)(                          \
      TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_RESERVE_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Reserve)(                     \
      TEXT_STRING_TYPE(SUFFIX) *str, size_t newCapacity)

#define TEXT_STRING_SHRINK_TO_FIT_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, ShrinkToFit)(                \
      TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_LENGTH_PROTOTYPE(SUFFIX)                                   \
  static inline size_t TEXT_STRING_FUNC(SUFFIX, Length)(                       \
      const TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_CAPACITY_PROTOTYPE(SUFFIX)                                 \
  static inline size_t TEXT_STRING_FUNC(SUFFIX, Capacity)(                     \
      const TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_IS_EMPTY_PROTOTYPE(SUFFIX)                                 \
  static inline bool TEXT_STRING_FUNC(SUFFIX, IsEmpty)(                        \
      const TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, CStr)(                       \
      const TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE **outCStr)

#define TEXT_STRING_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Data)(                       \
      TEXT_STRING_TYPE(SUFFIX) *str, CHAR_TYPE **outData)

#define TEXT_STRING_AT_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, At)(                         \
      TEXT_STRING_TYPE(SUFFIX) *str, size_t index, CHAR_TYPE **outChar)

#define TEXT_STRING_FROM_BUFFER_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, FromBuffer)(                  \
      TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *buffer, size_t count)

#define TEXT_STRING_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, FromCStr)(                   \
      TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *cstr)

#define TEXT_STRING_COPY_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Copy)(                       \
      TEXT_STRING_TYPE(SUFFIX) *destination,                                   \
      const TEXT_STRING_TYPE(SUFFIX) *source)

#define TEXT_STRING_PUSH_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, PushBack)(                   \
      TEXT_STRING_TYPE(SUFFIX) *str, CHAR_TYPE ch)

#define TEXT_STRING_POP_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                      \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, PopBack)(                    \
      TEXT_STRING_TYPE(SUFFIX) *str, CHAR_TYPE *outChar)

#define TEXT_STRING_APPEND_BUFFER_PROTOTYPE(CHAR_TYPE, SUFFIX)                 \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, AppendBuffer)(               \
      TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *buffer, size_t count)

#define TEXT_STRING_APPEND_PROTOTYPE(CHAR_TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Append)(                     \
      TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *cstr)

#define TEXT_STRING_APPEND_STRING_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, AppendString)(               \
      TEXT_STRING_TYPE(SUFFIX) *str,                                           \
      const TEXT_STRING_TYPE(SUFFIX) *other)

#define TEXT_STRING_INSERT_PROTOTYPE(CHAR_TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Insert)(                      \
      TEXT_STRING_TYPE(SUFFIX) *str, size_t pos, const CHAR_TYPE *cstr)

#define TEXT_STRING_INSERT_CHAR_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, InsertChar)(                 \
      TEXT_STRING_TYPE(SUFFIX) *str, size_t pos, CHAR_TYPE ch)

#define TEXT_STRING_ERASE_PROTOTYPE(SUFFIX)                                    \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Erase)(                       \
      TEXT_STRING_TYPE(SUFFIX) *str, size_t pos, size_t count)

#define TEXT_STRING_COMPARE_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Compare)(                     \
      const TEXT_STRING_TYPE(SUFFIX) *left,                                    \
      const TEXT_STRING_TYPE(SUFFIX) *right, CMPOUT *outResult)

#define TEXT_STRING_EQUAL_PROTOTYPE(SUFFIX)                                    \
  static inline bool TEXT_STRING_FUNC(SUFFIX, Equal)(                          \
      const TEXT_STRING_TYPE(SUFFIX) *left,                                    \
      const TEXT_STRING_TYPE(SUFFIX) *right)

#define TEXT_STRING_EQUAL_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline bool TEXT_STRING_FUNC(SUFFIX, EqualCStr)(                      \
      const TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *cstr)

#define TEXT_STRING_SUBSTR_PROTOTYPE(SUFFIX)                                   \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Substr)(                      \
      const TEXT_STRING_TYPE(SUFFIX) *str, size_t pos, size_t count,           \
      TEXT_STRING_TYPE(SUFFIX) *outStr)

#define TEXT_STRING_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, Find)(                       \
      const TEXT_STRING_TYPE(SUFFIX) *str, CHAR_TYPE ch, size_t start,         \
      size_t *outIndex)

#define TEXT_STRING_FIND_STR_PROTOTYPE(CHAR_TYPE, SUFFIX)                      \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, FindStr)(                    \
      const TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *needle,            \
      size_t start, size_t *outIndex)

#define TEXT_STRING_STARTS_WITH_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  static inline bool TEXT_STRING_FUNC(SUFFIX, StartsWith)(                     \
      const TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *prefix)

#define TEXT_STRING_ENDS_WITH_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  static inline bool TEXT_STRING_FUNC(SUFFIX, EndsWith)(                       \
      const TEXT_STRING_TYPE(SUFFIX) *str, const CHAR_TYPE *suffix)

#define TEXT_STRING_VIEW_PROTOTYPE(SUFFIX)                                     \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_STRING_FUNC(SUFFIX, View)(        \
      const TEXT_STRING_TYPE(SUFFIX) *str)

#define TEXT_STRING_SLICE_PROTOTYPE(SUFFIX)                                    \
  static inline TEXT_VIEW_TYPE(SUFFIX) TEXT_STRING_FUNC(SUFFIX, Slice)(       \
      const TEXT_STRING_TYPE(SUFFIX) *str, size_t pos, size_t count)

#define TEXT_STRING_FROM_VIEW_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, FromView)(                   \
      TEXT_STRING_TYPE(SUFFIX) *str, TEXT_VIEW_TYPE(SUFFIX) view)

#define TEXT_STRING_APPEND_VIEW_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS TEXT_STRING_FUNC(SUFFIX, AppendView)(                 \
      TEXT_STRING_TYPE(SUFFIX) *str, TEXT_VIEW_TYPE(SUFFIX) view)

#define TEXT_STRING_EQUAL_VIEW_PROTOTYPE(SUFFIX)                               \
  static inline bool TEXT_STRING_FUNC(SUFFIX, EqualView)(                      \
      const TEXT_STRING_TYPE(SUFFIX) *str, TEXT_VIEW_TYPE(SUFFIX) view)

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
TEXT_STRING_DECLARE_ALL(TEXT_TYPE(TChar16), 16)
TEXT_STRING_DECLARE_ALL(TEXT_TYPE(TChar32), 32)

/* UTF-8 specialized operations for Text_TString8 */
#define TEXT_STRING8_APPEND_CODEPOINT_PROTOTYPE                                \
  static inline OPSTATUS TEXT_STRING_FUNC(8, AppendCodepoint)(                 \
      TEXT_STRING_TYPE(8) *str, TEXT_TYPE(TChar32) codepoint)

#define TEXT_STRING8_COUNT_CODEPOINTS_PROTOTYPE                                \
  static inline OPSTATUS TEXT_STRING_FUNC(8, CountCodepoints)(                 \
      const TEXT_STRING_TYPE(8) *str, size_t *outCount)

#define TEXT_STRING8_VALIDATE_PROTOTYPE                                        \
  static inline bool TEXT_STRING_FUNC(8, Validate)(                             \
      const TEXT_STRING_TYPE(8) *str)

#define TEXT_STRING8_TRIM_PROTOTYPE                                            \
  static inline OPSTATUS TEXT_STRING_FUNC(8, Trim)(TEXT_STRING_TYPE(8) *str)

#define TEXT_STRING8_TRIM_LEFT_PROTOTYPE                                       \
  static inline OPSTATUS TEXT_STRING_FUNC(8, TrimLeft)(TEXT_STRING_TYPE(8) *str)

#define TEXT_STRING8_TRIM_RIGHT_PROTOTYPE                                      \
  static inline OPSTATUS TEXT_STRING_FUNC(8, TrimRight)(TEXT_STRING_TYPE(8) *str)

#define TEXT_STRING8_FORMAT_VARIADIC_PROTOTYPE                                 \
  static inline OPSTATUS TEXT_STRING_FUNC(8, Format_Variadic)(                 \
      TEXT_STRING_TYPE(8) *str, const char *format, va_list arguments)

#define TEXT_STRING8_FORMAT_PROTOTYPE                                          \
  static inline OPSTATUS TEXT_STRING_FUNC(8, Format)(                          \
      TEXT_STRING_TYPE(8) *str, const char *format, ...)

#define TEXT_STRING8_APPEND_FORMAT_VARIADIC_PROTOTYPE                          \
  static inline OPSTATUS TEXT_STRING_FUNC(8, AppendFormat_Variadic)(           \
      TEXT_STRING_TYPE(8) *str, const char *format, va_list arguments)

#define TEXT_STRING8_APPEND_FORMAT_PROTOTYPE                                   \
  static inline OPSTATUS TEXT_STRING_FUNC(8, AppendFormat)(                    \
      TEXT_STRING_TYPE(8) *str, const char *format, ...)

#define TEXT_STRING8_SLICE_CODEPOINTS_PROTOTYPE                                \
  static inline OPSTATUS TEXT_STRING_FUNC(8, SliceCodepoints)(                 \
      const TEXT_STRING_TYPE(8) *str, size_t codepointStart,                   \
      size_t codepointCount, TEXT_VIEW_TYPE(8) *outView)

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
