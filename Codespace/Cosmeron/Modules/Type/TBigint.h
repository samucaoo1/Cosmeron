#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Error/Status.h"

#include "../Bit/Bit.h"
#include "Type.space"

#define TBIGINT_TYPE(SUFFIX) TYPE_TYPE(TBigint, SUFFIX)
#define TBIGINT_FUNC(SUFFIX, FUNCTION) TYPE_FUNC(TBigint, SUFFIX, FUNCTION)

#if TYPE_ENABLE_VTABLE
#define TBIGINT_VTABLE_FORWARD(SUFFIX) \
  struct TBIGINT_FUNC(SUFFIX, FunctionTable);
#define TBIGINT_API_MEMBER(SUFFIX) \
  const struct TBIGINT_FUNC(SUFFIX, FunctionTable) *api;
#else
#define TBIGINT_VTABLE_FORWARD(SUFFIX)
#define TBIGINT_API_MEMBER(SUFFIX)
#endif

#define TBIGINT_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                \
  TBIGINT_VTABLE_FORWARD(SUFFIX)                                                  \
  typedef struct {                                                             \
    TBIGINT_API_MEMBER(SUFFIX)                                               \
    union {                                                                    \
      TYPE limb[WORD_COUNT];                                                   \
      uint8_t byte[SIZE_IN_BYTES];                                             \
    };                                                                         \
  } TBIGINT_TYPE(SUFFIX);

#define TBIGINT_CLEAR_PROTOTYPE(SUFFIX)                               \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Clear)(TBIGINT_TYPE(SUFFIX) * bigint)

#define TBIGINT_INIT_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Init)(TBIGINT_TYPE(SUFFIX) * bigint)

#define TBIGINT_ADD_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Add)(                                \
      TBIGINT_TYPE(SUFFIX) * destinationBigint,                               \
      const TBIGINT_TYPE(SUFFIX) * sourceBigint)

#define TBIGINT_SUB_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Sub)(                                \
      TBIGINT_TYPE(SUFFIX) * destinationBigint,                               \
      const TBIGINT_TYPE(SUFFIX) * sourceBigint)

#define TBIGINT_MUL_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Mul)(                                \
      TBIGINT_TYPE(SUFFIX) * destinationBigint,                               \
      const TBIGINT_TYPE(SUFFIX) * sourceBigint)

#define TBIGINT_DIVMOD_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, DivMod)(                            \
      const TBIGINT_TYPE(SUFFIX) *dividend, const TBIGINT_TYPE(SUFFIX) *divisor,     \
      TBIGINT_TYPE(SUFFIX) *quotient, TBIGINT_TYPE(SUFFIX) *remainder)

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
                                                     targetBigint)

#define TBIGINT_DECREMENT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Decrement)(TBIGINT_TYPE(SUFFIX) *    \
                                                     targetBigint)

#define TBIGINT_AND_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, And)(                                \
      TBIGINT_TYPE(SUFFIX) * destinationBigint,                               \
      const TBIGINT_TYPE(SUFFIX) * sourceBigint)

#define TBIGINT_OR_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Or)(                                 \
      TBIGINT_TYPE(SUFFIX) * destinationBigint,                               \
      const TBIGINT_TYPE(SUFFIX) * sourceBigint)

#define TBIGINT_XOR_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Xor)(                                \
      TBIGINT_TYPE(SUFFIX) * destinationBigint,                               \
      const TBIGINT_TYPE(SUFFIX) * sourceBigint)

#define TBIGINT_NOT_PROTOTYPE(SUFFIX)                                 \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX,                                      \
                                  Not)(TBIGINT_TYPE(SUFFIX) * targetBigint)

#define TBIGINT_SHIFTLEFT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ShiftLeft)(                          \
      TBIGINT_TYPE(SUFFIX) * targetBigint, unsigned bitCount)

#define TBIGINT_SHIFTRIGHT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ShiftRight)(                         \
      TBIGINT_TYPE(SUFFIX) * targetBigint, unsigned bitCount)

#define TBIGINT_ROTATELEFT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, RotateLeft)(                         \
      TBIGINT_TYPE(SUFFIX) * targetBigint, unsigned bitCount)

#define TBIGINT_ROTATERIGHT_PROTOTYPE(SUFFIX)                         \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, RotateRight)(                        \
      TBIGINT_TYPE(SUFFIX) * targetBigint, unsigned bitCount)

#define TBIGINT_BITSET_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, BitSet)(                             \
      TBIGINT_TYPE(SUFFIX) * bigint, uint32_t bit)

#define TBIGINT_BITCLEAR_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, BitClear)(                           \
      TBIGINT_TYPE(SUFFIX) * bigint, uint32_t bit)

#define TBIGINT_BITCHECK_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, BitCheck)(                           \
      const TBIGINT_TYPE(SUFFIX) *bigint, uint32_t bit, bool *result)

#define TBIGINT_COMPARE_PROTOTYPE(SUFFIX)                             \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, Compare)(                              \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                            \
      const TBIGINT_TYPE(SUFFIX) * rightBigint, TComparisonResult *result)

#define TBIGINT_EQUAL_PROTOTYPE(SUFFIX)                               \
  static inline bool TBIGINT_FUNC(SUFFIX, Equal)(                              \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                \
      const TBIGINT_TYPE(SUFFIX) * rightBigint)

#define TBIGINT_NOTEQUAL_PROTOTYPE(SUFFIX)                            \
  static inline bool TBIGINT_FUNC(SUFFIX, NotEqual)(                           \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                \
      const TBIGINT_TYPE(SUFFIX) * rightBigint)

#define TBIGINT_LESSTHAN_PROTOTYPE(SUFFIX)                            \
  static inline bool TBIGINT_FUNC(SUFFIX, LessThan)(                           \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                \
      const TBIGINT_TYPE(SUFFIX) * rightBigint)

#define TBIGINT_GREATERTHAN_PROTOTYPE(SUFFIX)                         \
  static inline bool TBIGINT_FUNC(SUFFIX, GreaterThan)(                        \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                \
      const TBIGINT_TYPE(SUFFIX) * rightBigint)

#define TBIGINT_LESSOREQUAL_PROTOTYPE(SUFFIX)                         \
  static inline bool TBIGINT_FUNC(SUFFIX, LessOrEqual)(                        \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                \
      const TBIGINT_TYPE(SUFFIX) * rightBigint)

#define TBIGINT_GREATEROREQUAL_PROTOTYPE(SUFFIX)                      \
  static inline bool TBIGINT_FUNC(SUFFIX, GreaterOrEqual)(                     \
      const TBIGINT_TYPE(SUFFIX) * leftBigint,                                \
      const TBIGINT_TYPE(SUFFIX) * rightBigint)

#define TBIGINT_TO_CSTRING_BASE_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ToCStringBase)(                     \
      const TBIGINT_TYPE(SUFFIX) *value, char *buffer, size_t bufferSize,         \
      unsigned base)

#define TBIGINT_TO_CSTRING_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS TBIGINT_FUNC(SUFFIX, ToCString)(                         \
      const TBIGINT_TYPE(SUFFIX) *value, char *buffer, size_t bufferSize)

#define TBIGINT_ISZERO_PROTOTYPE(SUFFIX)                              \
  static inline bool TBIGINT_FUNC(SUFFIX, IsZero)(const TBIGINT_TYPE(SUFFIX) * \
                                                  targetBigint)

#include "Impl/Arithmetic.impl"
#include "Impl/Bit.impl"
#include "Impl/Bitwise.impl"
#include "Impl/Common.impl"
#include "Impl/Comparison.impl"
#include "Impl/Format.impl"
#include "Impl/Shift.impl"
#include "Impl/TBigint.impl"
#if TYPE_ENABLE_VTABLE
#include "VTable/TBigint.h"
#endif

#define TBIGINT_DECLARE_PROTOTYPES(SUFFIX)                                     \
  TBIGINT_CLEAR_PROTOTYPE(SUFFIX);                                    \
  TBIGINT_INIT_PROTOTYPE(SUFFIX);                                     \
  TBIGINT_ADD_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_SUB_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_MUL_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_DIVMOD_PROTOTYPE(SUFFIX);                                     \
  TBIGINT_DIV_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_MOD_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_INCREMENT_PROTOTYPE(SUFFIX);                                \
  TBIGINT_DECREMENT_PROTOTYPE(SUFFIX);                                \
  TBIGINT_AND_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_OR_PROTOTYPE(SUFFIX);                                       \
  TBIGINT_XOR_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_NOT_PROTOTYPE(SUFFIX);                                      \
  TBIGINT_SHIFTLEFT_PROTOTYPE(SUFFIX);                                \
  TBIGINT_SHIFTRIGHT_PROTOTYPE(SUFFIX);                               \
  TBIGINT_ROTATELEFT_PROTOTYPE(SUFFIX);                               \
  TBIGINT_ROTATERIGHT_PROTOTYPE(SUFFIX);                              \
  TBIGINT_BITSET_PROTOTYPE(SUFFIX);                                   \
  TBIGINT_BITCLEAR_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_BITCHECK_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_COMPARE_PROTOTYPE(SUFFIX);                                  \
  TBIGINT_EQUAL_PROTOTYPE(SUFFIX);                                    \
  TBIGINT_NOTEQUAL_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_LESSTHAN_PROTOTYPE(SUFFIX);                                 \
  TBIGINT_GREATERTHAN_PROTOTYPE(SUFFIX);                              \
  TBIGINT_LESSOREQUAL_PROTOTYPE(SUFFIX);                              \
  TBIGINT_GREATEROREQUAL_PROTOTYPE(SUFFIX);                           \
  TBIGINT_ISZERO_PROTOTYPE(SUFFIX);                                              \
  TBIGINT_TO_CSTRING_BASE_PROTOTYPE(SUFFIX);                                   \
  TBIGINT_TO_CSTRING_PROTOTYPE(SUFFIX);

#if TYPE_ENABLE_VTABLE
#define TYPE_VTABLE_DECLARE_TBIGINT(SUFFIX) \
  TBIGINT_FUNCTION_TABLE(SUFFIX) \
  TBIGINT_FUNCTION_TABLE_INSTANCE(SUFFIX)
#else
#define TYPE_VTABLE_DECLARE_TBIGINT(SUFFIX)
#endif

#define TBIGINT_DECLARE(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)               \
  TBIGINT_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                      \
  TBIGINT_DECLARE_PROTOTYPES(SUFFIX)                                           \
  TBIGINT_IMPLEMENT(SUFFIX, TYPE)                                              \
  TYPE_VTABLE_DECLARE_TBIGINT(SUFFIX)

#if TYPE_ENABLE_VTABLE
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
