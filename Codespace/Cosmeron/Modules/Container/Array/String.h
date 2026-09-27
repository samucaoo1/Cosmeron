#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "Flat.space"

/* ============================================================
 * TString - string sequencial com armazenamento direto
 * ============================================================ */

#define TSTRING_TYPE(SUFFIX) GNS2(FLAT_NS(TString), SUFFIX)
#define TSTRING_FUNC(SUFFIX, FUNC) GNS2(GNS2(FLAT_NS(String), SUFFIX), FUNC)

#define TSTRING_STRUCT(CHAR_TYPE, SUFFIX)                                      \
  CONTAINER_API_FORWARD(TSTRING_FUNC(SUFFIX, FunctionTable))                                \
  typedef struct TSTRING_FUNC(SUFFIX, str) {                                    \
    CONTAINER_API_FIELD(TSTRING_FUNC(SUFFIX, FunctionTable))                  \
    CHAR_TYPE *data;                                                           \
    size_t size;                                                               \
    size_t capacity;                                                           \
  } TSTRING_TYPE(SUFFIX);

#define TSTRING_INIT_PROTOTYPE(SUFFIX)                                \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Init)(TSTRING_TYPE(SUFFIX) * str)
#define TSTRING_DESTROY_PROTOTYPE(SUFFIX)                             \
  inline static void TSTRING_FUNC(SUFFIX, Destroy)(TSTRING_TYPE(SUFFIX) * str)
#define TSTRING_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, FromCStr)(                       \
      TSTRING_TYPE(SUFFIX) * str, const CHAR_TYPE *cstr)
#define TSTRING_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  inline static const CHAR_TYPE *TSTRING_FUNC(SUFFIX, CStr)(                   \
      TSTRING_TYPE(SUFFIX) * str)
#define TSTRING_LENGTH_PROTOTYPE(SUFFIX)                              \
  inline static size_t TSTRING_FUNC(SUFFIX, Length)(const TSTRING_TYPE(SUFFIX) *str)
#define TSTRING_EMPTY_PROTOTYPE(SUFFIX)                               \
  inline static bool TSTRING_FUNC(SUFFIX, Empty)(const TSTRING_TYPE(SUFFIX) *str)
#define TSTRING_CLEAR_PROTOTYPE(SUFFIX)                               \
  inline static void TSTRING_FUNC(SUFFIX, Clear)(TSTRING_TYPE(SUFFIX) * str)
#define TSTRING_RESERVE_PROTOTYPE(SUFFIX)                             \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Reserve)(                        \
      TSTRING_TYPE(SUFFIX) * str, size_t newCapacity)
#define TSTRING_PUSH_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, PushBack)(                       \
      TSTRING_TYPE(SUFFIX) * str, CHAR_TYPE ch)
#define TSTRING_POP_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                            \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX,                                  \
                                      PopBack)(TSTRING_TYPE(SUFFIX) * str,      \
                                               CHAR_TYPE *outValue)
#define TSTRING_CAPACITY_PROTOTYPE(SUFFIX)                           \
  inline static size_t TSTRING_FUNC(SUFFIX,                                    \
                                    Capacity)(const TSTRING_TYPE(SUFFIX) *str)
#define TSTRING_INSERT_CHAR_PROTOTYPE(CHAR_TYPE, SUFFIX)             \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, InsertChar)(                    \
      TSTRING_TYPE(SUFFIX) * str, size_t pos, CHAR_TYPE ch)
#define TSTRING_AT_PROTOTYPE(CHAR_TYPE, SUFFIX)                       \
  inline static CHAR_TYPE *TSTRING_FUNC(SUFFIX, At)(                           \
      TSTRING_TYPE(SUFFIX) * str, size_t index)
#define TSTRING_FRONT_PROTOTYPE(CHAR_TYPE, SUFFIX)                    \
  inline static CHAR_TYPE *TSTRING_FUNC(SUFFIX,                                \
                                        Front)(TSTRING_TYPE(SUFFIX) * str)
#define TSTRING_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  inline static CHAR_TYPE *TSTRING_FUNC(SUFFIX,                                \
                                        Back)(TSTRING_TYPE(SUFFIX) * str)
#define TSTRING_APPEND_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Append)(                         \
      TSTRING_TYPE(SUFFIX) * str, const CHAR_TYPE *cstr)
#define TSTRING_APPEND_STR_PROTOTYPE(SUFFIX)                          \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, AppendStr)(                      \
      TSTRING_TYPE(SUFFIX) * str, TSTRING_TYPE(SUFFIX) * other)
#define TSTRING_INSERT_PROTOTYPE(CHAR_TYPE, SUFFIX)                   \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Insert)(                         \
      TSTRING_TYPE(SUFFIX) * str, size_t pos, const CHAR_TYPE *cstr)
#define TSTRING_ERASE_PROTOTYPE(SUFFIX)                               \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Erase)(                          \
      TSTRING_TYPE(SUFFIX) * str, size_t pos, size_t count)
#define TSTRING_SUBSTR_PROTOTYPE(SUFFIX)                              \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Substr)(                         \
      TSTRING_TYPE(SUFFIX) * str, size_t pos, size_t count,                    \
      TSTRING_TYPE(SUFFIX) * out)
#define TSTRING_COMPARE_PROTOTYPE(SUFFIX)                                     \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Compare)(                       \
      const TSTRING_TYPE(SUFFIX) *str, const TSTRING_TYPE(SUFFIX) *other,     \
      TComparisonResult *result)
#define TSTRING_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX)                              \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, Find)(                          \
      const TSTRING_TYPE(SUFFIX) *str, CHAR_TYPE ch, size_t start,            \
      size_t *index)
#define TSTRING_FIND_STR_PROTOTYPE(CHAR_TYPE, SUFFIX)                          \
  inline static OPSTATUS TSTRING_FUNC(SUFFIX, FindStr)(                       \
      const TSTRING_TYPE(SUFFIX) *str, const CHAR_TYPE *needle, size_t start, \
      size_t *index)
#define TSTRING_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX)                     \
  inline static CHAR_TYPE *TSTRING_FUNC(SUFFIX,                                \
                                        Data)(TSTRING_TYPE(SUFFIX) * str)

#include "Impl/String.impl"
#include "VTable/String.h"

#define TSTRING_DECLARE(CHAR_TYPE, SUFFIX)                                     \
  TSTRING_STRUCT(CHAR_TYPE, SUFFIX)                                            \
  TSTRING_INIT_PROTOTYPE(SUFFIX);                                     \
  TSTRING_DESTROY_PROTOTYPE(SUFFIX);                                  \
  TSTRING_FROM_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                     \
  TSTRING_CSTR_PROTOTYPE(CHAR_TYPE, SUFFIX);                          \
  TSTRING_LENGTH_PROTOTYPE(SUFFIX);                                   \
  TSTRING_EMPTY_PROTOTYPE(SUFFIX);                                    \
  TSTRING_CLEAR_PROTOTYPE(SUFFIX);                                    \
  TSTRING_RESERVE_PROTOTYPE(SUFFIX);                                  \
  TSTRING_PUSH_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX);                     \
  TSTRING_POP_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX);                                 \
  TSTRING_CAPACITY_PROTOTYPE(SUFFIX);                                \
  TSTRING_INSERT_CHAR_PROTOTYPE(CHAR_TYPE, SUFFIX);                  \
  TSTRING_AT_PROTOTYPE(CHAR_TYPE, SUFFIX);                            \
  TSTRING_FRONT_PROTOTYPE(CHAR_TYPE, SUFFIX);                         \
  TSTRING_BACK_PROTOTYPE(CHAR_TYPE, SUFFIX);                          \
  TSTRING_APPEND_PROTOTYPE(CHAR_TYPE, SUFFIX);                        \
  TSTRING_APPEND_STR_PROTOTYPE(SUFFIX);                               \
  TSTRING_INSERT_PROTOTYPE(CHAR_TYPE, SUFFIX);                        \
  TSTRING_ERASE_PROTOTYPE(SUFFIX);                                    \
  TSTRING_SUBSTR_PROTOTYPE(SUFFIX);                                   \
  TSTRING_COMPARE_PROTOTYPE(SUFFIX);                                  \
  TSTRING_FIND_PROTOTYPE(CHAR_TYPE, SUFFIX);                          \
  TSTRING_FIND_STR_PROTOTYPE(CHAR_TYPE, SUFFIX);                      \
  TSTRING_DATA_PROTOTYPE(CHAR_TYPE, SUFFIX);                          \
  TSTRING_IMPLEMENT(CHAR_TYPE, SUFFIX)                                         \
  TSTRING_FUNCTION_TABLE(CHAR_TYPE, SUFFIX)                                  \
  TSTRING_FUNCTION_TABLE_INSTANCE(CHAR_TYPE, SUFFIX)

#define TSTRING_INSTANCE_DECLARE(SUFFIX, NAME)                                                  \
  TSTRING_TYPE(SUFFIX) NAME;                                                   \
  CONTAINER_API_BIND(NAME, TSTRING_FUNC(SUFFIX, functions));                                 \
  TSTRING_FUNC(SUFFIX, Init)(&NAME);
/* Compatibility alias: prefer TSTRING_INSTANCE_DECLARE. */
#define TString(SUFFIX, NAME) TSTRING_INSTANCE_DECLARE(SUFFIX, NAME)

typedef uint8_t utf8;
typedef uint16_t utf16;
typedef uint32_t utf32;

TSTRING_DECLARE(utf8, 8)
TSTRING_DECLARE(utf16, 16)
TSTRING_DECLARE(utf32, 32)


