#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Error/Status.h"

#include "../Bit/Bit.h"
#include "Type.space"

#define TDECIMAL_TYPE(SUFFIX) TYPE_TYPE(TDecimal, SUFFIX)
#define TDECIMAL_FUNC(SUFFIX, FUNCTION) TYPE_FUNC(TDecimal, SUFFIX, FUNCTION)
#define TDECIMAL_INS(SUFFIX, NAME)                                            \
  GNS2(TYPE_INS(PP_OP_CAT2(TDecimal, SUFFIX)), NAME)
#define TDECIMAL_FUNCTION_TABLE_TYPE(SUFFIX) \
  GNS2(TDECIMAL_TYPE(SUFFIX), FunctionTable)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TDECIMAL_FUNCTION_TABLE_FORWARD(SUFFIX) \
  struct TDECIMAL_FUNCTION_TABLE_TYPE(SUFFIX);
#define TDECIMAL_API_MEMBER(SUFFIX) \
  const struct TDECIMAL_FUNCTION_TABLE_TYPE(SUFFIX) *api;
#else
#define TDECIMAL_FUNCTION_TABLE_FORWARD(SUFFIX)
#define TDECIMAL_API_MEMBER(SUFFIX)
#endif

#define TDECIMAL_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)               \
  TDECIMAL_FUNCTION_TABLE_FORWARD(SUFFIX)                                                  \
  typedef struct {                                                             \
    TDECIMAL_API_MEMBER(SUFFIX)                                               \
    union {                                                                    \
      TYPE limb[WORD_COUNT];                                                   \
      uint8_t byte[SIZE_IN_BYTES];                                             \
    };                                                                         \
  } TDECIMAL_TYPE(SUFFIX);

#define TDECIMAL_CLEAR_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX,                                     \
                                   Clear)(TDECIMAL_TYPE(SUFFIX) * decimal)

#define TDECIMAL_INIT_PROTOTYPE(SUFFIX)                               \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX,                                     \
                                   Init)(TDECIMAL_TYPE(SUFFIX) * decimal)

#define TDECIMAL_ADD_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Add)(                               \
      TDECIMAL_TYPE(SUFFIX) * destinationDecimal,                             \
      const TDECIMAL_TYPE(SUFFIX) * sourceDecimal)

#define TDECIMAL_SUB_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Sub)(                               \
      TDECIMAL_TYPE(SUFFIX) * destinationDecimal,                             \
      const TDECIMAL_TYPE(SUFFIX) * sourceDecimal)

#define TDECIMAL_MUL_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Mul)(                               \
      TDECIMAL_TYPE(SUFFIX) * destinationDecimal,                             \
      const TDECIMAL_TYPE(SUFFIX) * sourceDecimal)

#define TDECIMAL_DIV_MOD_PROTOTYPE(SUFFIX)                                    \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, DivMod)(                       \
      const TDECIMAL_TYPE(SUFFIX) *dividend,                                  \
      const TDECIMAL_TYPE(SUFFIX) *divisor,                                   \
      TDECIMAL_TYPE(SUFFIX) *outQuotient,                                     \
      TDECIMAL_TYPE(SUFFIX) *outRemainder)

#define TDECIMAL_DIV_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Div)(                               \
      TDECIMAL_TYPE(SUFFIX) * dividendDecimal,                                \
      const TDECIMAL_TYPE(SUFFIX) * divisorDecimal)

#define TDECIMAL_MOD_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Mod)(                               \
      TDECIMAL_TYPE(SUFFIX) * dividendDecimal,                                \
      const TDECIMAL_TYPE(SUFFIX) * divisorDecimal)

#define TDECIMAL_INCREMENT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Increment)(TDECIMAL_TYPE(SUFFIX) *  \
                                                      targetDecimal)

#define TDECIMAL_DECREMENT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Decrement)(TDECIMAL_TYPE(SUFFIX) *  \
                                                      targetDecimal)

#define TDECIMAL_AND_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, And)(                               \
      TDECIMAL_TYPE(SUFFIX) * destinationDecimal,                             \
      const TDECIMAL_TYPE(SUFFIX) * sourceDecimal)

#define TDECIMAL_OR_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Or)(                                \
      TDECIMAL_TYPE(SUFFIX) * destinationDecimal,                             \
      const TDECIMAL_TYPE(SUFFIX) * sourceDecimal)

#define TDECIMAL_XOR_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Xor)(                               \
      TDECIMAL_TYPE(SUFFIX) * destinationDecimal,                             \
      const TDECIMAL_TYPE(SUFFIX) * sourceDecimal)

#define TDECIMAL_NOT_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Not)(TDECIMAL_TYPE(SUFFIX) *        \
                                                targetDecimal)

#define TDECIMAL_SHIFT_LEFT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, ShiftLeft)(                         \
      TDECIMAL_TYPE(SUFFIX) * targetDecimal, unsigned bitCount)

#define TDECIMAL_SHIFT_RIGHT_PROTOTYPE(SUFFIX)                         \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, ShiftRight)(                        \
      TDECIMAL_TYPE(SUFFIX) * targetDecimal, unsigned bitCount)

#define TDECIMAL_ROTATE_LEFT_PROTOTYPE(SUFFIX)                         \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, RotateLeft)(                        \
      TDECIMAL_TYPE(SUFFIX) * targetDecimal, unsigned bitCount)

#define TDECIMAL_ROTATE_RIGHT_PROTOTYPE(SUFFIX)                        \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, RotateRight)(                       \
      TDECIMAL_TYPE(SUFFIX) * targetDecimal, unsigned bitCount)

#define TDECIMAL_BIT_SET_PROTOTYPE(SUFFIX)                             \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, BitSet)(                            \
      TDECIMAL_TYPE(SUFFIX) * decimal, uint32_t bit)

#define TDECIMAL_BIT_CLEAR_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, BitClear)(                          \
      TDECIMAL_TYPE(SUFFIX) * decimal, uint32_t bit)

#define TDECIMAL_BIT_CHECK_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, BitCheck)(                          \
      const TDECIMAL_TYPE(SUFFIX) *decimal, uint32_t bit, bool *outResult)

#define TDECIMAL_COMPARE_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, Compare)(                              \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                                            \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal, CMPOUT *result)

#define TDECIMAL_EQUAL_PROTOTYPE(SUFFIX)                              \
  static inline bool TDECIMAL_FUNC(SUFFIX, Equal)(                             \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                              \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal)

#define TDECIMAL_NOT_EQUAL_PROTOTYPE(SUFFIX)                           \
  static inline bool TDECIMAL_FUNC(SUFFIX, NotEqual)(                          \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                              \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal)

#define TDECIMAL_LESS_THAN_PROTOTYPE(SUFFIX)                           \
  static inline bool TDECIMAL_FUNC(SUFFIX, LessThan)(                          \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                              \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal)

#define TDECIMAL_GREATER_THAN_PROTOTYPE(SUFFIX)                        \
  static inline bool TDECIMAL_FUNC(SUFFIX, GreaterThan)(                       \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                              \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal)

#define TDECIMAL_LESS_OR_EQUAL_PROTOTYPE(SUFFIX)                        \
  static inline bool TDECIMAL_FUNC(SUFFIX, LessOrEqual)(                       \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                              \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal)

#define TDECIMAL_GREATER_OR_EQUAL_PROTOTYPE(SUFFIX)                     \
  static inline bool TDECIMAL_FUNC(SUFFIX, GreaterOrEqual)(                    \
      const TDECIMAL_TYPE(SUFFIX) * leftDecimal,                              \
      const TDECIMAL_TYPE(SUFFIX) * rightDecimal)

#define TDECIMAL_TO_CSTRING_BASE_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, ToCStringBase)(                     \
      const TDECIMAL_TYPE(SUFFIX) *value, char *buffer, size_t bufferSize,         \
      unsigned base)

#define TDECIMAL_TO_CSTRING_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS TDECIMAL_FUNC(SUFFIX, ToCString)(                         \
      const TDECIMAL_TYPE(SUFFIX) *value, char *buffer, size_t bufferSize)

#define TDECIMAL_IS_ZERO_PROTOTYPE(SUFFIX)                             \
  static inline bool TDECIMAL_FUNC(SUFFIX, IsZero)(                            \
      const TDECIMAL_TYPE(SUFFIX) * targetDecimal)

#include "Impl/Arithmetic.impl"
#include "Impl/Bit.impl"
#include "Impl/Bitwise.impl"
#include "Impl/Common.impl"
#include "Impl/Comparison.impl"
#include "Impl/Format.impl"
#include "Impl/Shift.impl"
#include "Impl/TDecimal.impl"
#ifndef TYPE_DISABLE_FUNCTION_TABLE
#include "FunctionTable/TDecimal.h"
#endif

#define TDECIMAL_DECLARE_PROTOTYPES(SUFFIX)                                    \
  TDECIMAL_CLEAR_PROTOTYPE(SUFFIX);                                   \
  TDECIMAL_INIT_PROTOTYPE(SUFFIX);                                    \
  TDECIMAL_ADD_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_SUB_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_MUL_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_DIV_MOD_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_DIV_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_MOD_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_INCREMENT_PROTOTYPE(SUFFIX);                               \
  TDECIMAL_DECREMENT_PROTOTYPE(SUFFIX);                               \
  TDECIMAL_AND_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_OR_PROTOTYPE(SUFFIX);                                      \
  TDECIMAL_XOR_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_NOT_PROTOTYPE(SUFFIX);                                     \
  TDECIMAL_SHIFT_LEFT_PROTOTYPE(SUFFIX);                               \
  TDECIMAL_SHIFT_RIGHT_PROTOTYPE(SUFFIX);                              \
  TDECIMAL_ROTATE_LEFT_PROTOTYPE(SUFFIX);                              \
  TDECIMAL_ROTATE_RIGHT_PROTOTYPE(SUFFIX);                             \
  TDECIMAL_BIT_SET_PROTOTYPE(SUFFIX);                                  \
  TDECIMAL_BIT_CLEAR_PROTOTYPE(SUFFIX);                                \
  TDECIMAL_BIT_CHECK_PROTOTYPE(SUFFIX);                                \
  TDECIMAL_COMPARE_PROTOTYPE(SUFFIX);                                 \
  TDECIMAL_EQUAL_PROTOTYPE(SUFFIX);                                   \
  TDECIMAL_NOT_EQUAL_PROTOTYPE(SUFFIX);                                \
  TDECIMAL_LESS_THAN_PROTOTYPE(SUFFIX);                                \
  TDECIMAL_GREATER_THAN_PROTOTYPE(SUFFIX);                             \
  TDECIMAL_LESS_OR_EQUAL_PROTOTYPE(SUFFIX);                             \
  TDECIMAL_GREATER_OR_EQUAL_PROTOTYPE(SUFFIX);                          \
  TDECIMAL_IS_ZERO_PROTOTYPE(SUFFIX);                                              \
  TDECIMAL_TO_CSTRING_BASE_PROTOTYPE(SUFFIX);                                   \
  TDECIMAL_TO_CSTRING_PROTOTYPE(SUFFIX);

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TYPE_FUNCTION_TABLE_DECLARE_TDECIMAL(SUFFIX) \
  TDECIMAL_FUNCTION_TABLE_STRUCT(SUFFIX) \
  TDECIMAL_FUNCTION_TABLE_INSTANCE(SUFFIX)
#else
#define TYPE_FUNCTION_TABLE_DECLARE_TDECIMAL(SUFFIX)
#endif

#define TDECIMAL_DECLARE(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)              \
  TDECIMAL_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                     \
  TDECIMAL_DECLARE_PROTOTYPES(SUFFIX)                                          \
  TDECIMAL_IMPLEMENT(SUFFIX, TYPE)                                              \
  TYPE_FUNCTION_TABLE_DECLARE_TDECIMAL(SUFFIX)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TDecimal(SUFFIX, NAME)                                                   \
  TDECIMAL_TYPE(SUFFIX) NAME = {0};                                           \
  NAME.api = &TDECIMAL_FUNC(SUFFIX, functions);
#else
#define TDecimal(SUFFIX, NAME) TDECIMAL_TYPE(SUFFIX) NAME = {0};
#endif

TDECIMAL_DECLARE(128, uint64_t, 2, 16)
TDECIMAL_DECLARE(256, uint64_t, 4, 32)
TDECIMAL_DECLARE(512, uint64_t, 8, 64)
TDECIMAL_DECLARE(1024, uint64_t, 16, 128)
/* EOF */
