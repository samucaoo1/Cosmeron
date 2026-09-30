#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Error/Status.h"

#include "../Bit/Bit.h"
#include "Type.space"

#define TBIGINT_TYPE(SUFFIX) TYPE_TYPE(TBigint, SUFFIX)
#define TBIGINT_FUNC(SUFFIX, FUNCTION) TYPE_FUNC(TBigint, SUFFIX, FUNCTION)
#define TBIGINT_INS(SUFFIX, NAME)                                             \
  GNS2(TYPE_INS(PP_OP_CAT2(TBigint, SUFFIX)), NAME)
#define TBIGINT_FUNCTION_TABLE_TYPE(SUFFIX) \
  GNS2(TBIGINT_TYPE(SUFFIX), FunctionTable)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TBIGINT_FUNCTION_TABLE_FORWARD(SUFFIX) \
  struct TBIGINT_FUNCTION_TABLE_TYPE(SUFFIX);
#define TBIGINT_API_MEMBER(SUFFIX) \
  const struct TBIGINT_FUNCTION_TABLE_TYPE(SUFFIX) *api;
#else
#define TBIGINT_FUNCTION_TABLE_FORWARD(SUFFIX)
#define TBIGINT_API_MEMBER(SUFFIX)
#endif

#define TBIGINT_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                \
  TBIGINT_FUNCTION_TABLE_FORWARD(SUFFIX)                                                  \
  typedef struct {                                                             \
    TBIGINT_API_MEMBER(SUFFIX)                                               \
    union {                                                                    \
      TYPE limb[WORD_COUNT];                                                   \
      uint8_t byte[SIZE_IN_BYTES];                                             \
    };                                                                         \
  } TBIGINT_TYPE(SUFFIX);

#define TBIGINT_CLEAR_PROTOTYPE(SUFFIX)                               \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Clear)(TBIGINT_TYPE(SUFFIX) *target)

#define TBIGINT_INIT_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Init)(TBIGINT_TYPE(SUFFIX) *target)

#define TBIGINT_ADD_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Add)(                                \
      TBIGINT_TYPE(SUFFIX) * destination,                               \
      const TBIGINT_TYPE(SUFFIX) * source)

#define TBIGINT_SUB_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Sub)(                                \
      TBIGINT_TYPE(SUFFIX) * destination,                               \
      const TBIGINT_TYPE(SUFFIX) * source)

#define TBIGINT_MUL_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Mul)(                                \
      TBIGINT_TYPE(SUFFIX) * destination,                               \
      const TBIGINT_TYPE(SUFFIX) * source)

#define TBIGINT_DIV_MOD_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, DivMod)(                        \
      const TBIGINT_TYPE(SUFFIX) *dividend,                                   \
      const TBIGINT_TYPE(SUFFIX) *divisor,                                    \
      TBIGINT_TYPE(SUFFIX) *outQuotient,                                      \
      TBIGINT_TYPE(SUFFIX) *outRemainder)

#define TBIGINT_DIV_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Div)(                                \
      TBIGINT_TYPE(SUFFIX) * dividendBigint,                                  \
      const TBIGINT_TYPE(SUFFIX) * divisorBigint)

#define TBIGINT_MOD_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Mod)(                                \
      TBIGINT_TYPE(SUFFIX) * dividendBigint,                                  \
      const TBIGINT_TYPE(SUFFIX) * divisorBigint)

#define TBIGINT_INCREMENT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Increment)(TBIGINT_TYPE(SUFFIX) *    \
                                                     target)

#define TBIGINT_DECREMENT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Decrement)(TBIGINT_TYPE(SUFFIX) *    \
                                                     target)

#define TBIGINT_AND_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, And)(                                \
      TBIGINT_TYPE(SUFFIX) * destination,                               \
      const TBIGINT_TYPE(SUFFIX) * source)

#define TBIGINT_OR_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Or)(                                 \
      TBIGINT_TYPE(SUFFIX) * destination,                               \
      const TBIGINT_TYPE(SUFFIX) * source)

#define TBIGINT_XOR_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Xor)(                                \
      TBIGINT_TYPE(SUFFIX) * destination,                               \
      const TBIGINT_TYPE(SUFFIX) * source)

#define TBIGINT_NOT_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX,                                      \
                                  Not)(TBIGINT_TYPE(SUFFIX) * target)

#define TBIGINT_SHIFT_LEFT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ShiftLeft)(                          \
      TBIGINT_TYPE(SUFFIX) * target, unsigned bitCount)

#define TBIGINT_SHIFT_RIGHT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ShiftRight)(                         \
      TBIGINT_TYPE(SUFFIX) * target, unsigned bitCount)

#define TBIGINT_ROTATE_LEFT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, RotateLeft)(                         \
      TBIGINT_TYPE(SUFFIX) *target, unsigned shift)

#define TBIGINT_ROTATE_RIGHT_PROTOTYPE(SUFFIX)                         \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, RotateRight)(                        \
      TBIGINT_TYPE(SUFFIX) *target, unsigned shift)

#define TBIGINT_BIT_SET_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, BitSet)(                             \
      TBIGINT_TYPE(SUFFIX) *target, uint32_t bit)

#define TBIGINT_BIT_CLEAR_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, BitClear)(                           \
      TBIGINT_TYPE(SUFFIX) *target, uint32_t bit)

#define TBIGINT_BIT_CHECK_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, BitCheck)(                           \
      const TBIGINT_TYPE(SUFFIX) *target, uint32_t bit, bool *outResult)

#define TBIGINT_COMPARE_PROTOTYPE(SUFFIX)                             \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Compare)(                              \
      const TBIGINT_TYPE(SUFFIX) * left,                                            \
      const TBIGINT_TYPE(SUFFIX) * right, CMPOUT *result)

#define TBIGINT_EQUAL_PROTOTYPE(SUFFIX)                               \
  static inline bool TBIGINT_FUNC(SUFFIX, Equal)(                              \
      const TBIGINT_TYPE(SUFFIX) * left,                                \
      const TBIGINT_TYPE(SUFFIX) * right)

#define TBIGINT_NOT_EQUAL_PROTOTYPE(SUFFIX)                            \
  static inline bool TBIGINT_FUNC(SUFFIX, NotEqual)(                           \
      const TBIGINT_TYPE(SUFFIX) * left,                                \
      const TBIGINT_TYPE(SUFFIX) * right)

#define TBIGINT_LESS_THAN_PROTOTYPE(SUFFIX)                            \
  static inline bool TBIGINT_FUNC(SUFFIX, LessThan)(                           \
      const TBIGINT_TYPE(SUFFIX) * left,                                \
      const TBIGINT_TYPE(SUFFIX) * right)

#define TBIGINT_GREATER_THAN_PROTOTYPE(SUFFIX)                         \
  static inline bool TBIGINT_FUNC(SUFFIX, GreaterThan)(                        \
      const TBIGINT_TYPE(SUFFIX) * left,                                \
      const TBIGINT_TYPE(SUFFIX) * right)

#define TBIGINT_LESS_OR_EQUAL_PROTOTYPE(SUFFIX)                         \
  static inline bool TBIGINT_FUNC(SUFFIX, LessOrEqual)(                        \
      const TBIGINT_TYPE(SUFFIX) * left,                                \
      const TBIGINT_TYPE(SUFFIX) * right)

#define TBIGINT_GREATER_OR_EQUAL_PROTOTYPE(SUFFIX)                      \
  static inline bool TBIGINT_FUNC(SUFFIX, GreaterOrEqual)(                     \
      const TBIGINT_TYPE(SUFFIX) * left,                                \
      const TBIGINT_TYPE(SUFFIX) * right)

#define TBIGINT_TO_CSTRING_BASE_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ToCStringBase)(                     \
      const TBIGINT_TYPE(SUFFIX) *value, char *buffer, size_t bufferSize,         \
      unsigned base)

#define TBIGINT_TO_CSTRING_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ToCString)(                         \
      const TBIGINT_TYPE(SUFFIX) *value, char *buffer, size_t bufferSize)

#define TBIGINT_IS_ZERO_PROTOTYPE(SUFFIX)                              \
  static inline bool TBIGINT_FUNC(SUFFIX, IsZero)(const TBIGINT_TYPE(SUFFIX) * \
                                                  target)

#include "Impl/Arithmetic.impl"
#include "Impl/Bit.impl"
#include "Impl/Bitwise.impl"
#include "Impl/Common.impl"
#include "Impl/Comparison.impl"
#include "Impl/Format.impl"
#include "Impl/Shift.impl"
#include "Impl/TBigint.impl"
#ifndef TYPE_DISABLE_FUNCTION_TABLE
#include "FunctionTable/TBigint.h"
#endif

#define TBIGINT_DECLARE_PROTOTYPES(SUFFIX)                                     \
  TBIGINT_CLEAR_PROTOTYPE(SUFFIX);                                    \
  TBIGINT_INIT_PROTOTYPE(SUFFIX);                                     \
  TBIGINT_ADD_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_SUB_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_MUL_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_DIV_MOD_PROTOTYPE(SUFFIX);                                     \
  TBIGINT_DIV_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_MOD_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_INCREMENT_PROTOTYPE(SUFFIX);                                \
  TBIGINT_DECREMENT_PROTOTYPE(SUFFIX);                                \
  TBIGINT_AND_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_OR_PROTOTYPE(SUFFIX);                                       \
  TBIGINT_XOR_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_NOT_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_SHIFT_LEFT_PROTOTYPE(SUFFIX);                                \
  TBIGINT_SHIFT_RIGHT_PROTOTYPE(SUFFIX);                               \
  TBIGINT_ROTATE_LEFT_PROTOTYPE(SUFFIX);                               \
  TBIGINT_ROTATE_RIGHT_PROTOTYPE(SUFFIX);                              \
  TBIGINT_BIT_SET_PROTOTYPE(SUFFIX);                                   \
  TBIGINT_BIT_CLEAR_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_BIT_CHECK_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_COMPARE_PROTOTYPE(SUFFIX);                                  \
  TBIGINT_EQUAL_PROTOTYPE(SUFFIX);                                    \
  TBIGINT_NOT_EQUAL_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_LESS_THAN_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_GREATER_THAN_PROTOTYPE(SUFFIX);                              \
  TBIGINT_LESS_OR_EQUAL_PROTOTYPE(SUFFIX);                              \
  TBIGINT_GREATER_OR_EQUAL_PROTOTYPE(SUFFIX);                           \
  TBIGINT_IS_ZERO_PROTOTYPE(SUFFIX);                                              \
  TBIGINT_TO_CSTRING_BASE_PROTOTYPE(SUFFIX);                                   \
  TBIGINT_TO_CSTRING_PROTOTYPE(SUFFIX);

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TYPE_FUNCTION_TABLE_DECLARE_TBIGINT(SUFFIX) \
  TBIGINT_FUNCTION_TABLE_STRUCT(SUFFIX) \
  TBIGINT_FUNCTION_TABLE_INSTANCE(SUFFIX)
#else
#define TYPE_FUNCTION_TABLE_DECLARE_TBIGINT(SUFFIX)
#endif

#define TBIGINT_DECLARE(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)               \
  TBIGINT_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                      \
  TBIGINT_DECLARE_PROTOTYPES(SUFFIX)                                           \
  TBIGINT_IMPLEMENT(SUFFIX, TYPE)                                              \
  TYPE_FUNCTION_TABLE_DECLARE_TBIGINT(SUFFIX)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TBigint(SUFFIX, NAME)                                                   \
  TBIGINT_TYPE(SUFFIX) NAME = {0};                                           \
  NAME.api = &TBIGINT_FUNC(SUFFIX, functions);
#else
#define TBigint(SUFFIX, NAME) TBIGINT_TYPE(SUFFIX) NAME = {0};
#endif

TBIGINT_DECLARE(128, uint64_t, 2, 16)
TBIGINT_DECLARE(256, uint64_t, 4, 32)
TBIGINT_DECLARE(512, uint64_t, 8, 64)
TBIGINT_DECLARE(1024, uint64_t, 16, 128)
/* EOF */
