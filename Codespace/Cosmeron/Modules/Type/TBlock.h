#pragma once

#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Error/Status.h"

#include "../Bit/Bit.h"
#include "Type.space"

#define TBLOCK_TYPE(SUFFIX) TYPE_TYPE(TBlock, SUFFIX)
#define TBLOCK_FUNC(SUFFIX, FUNCTION) TYPE_FUNC(TBlock, SUFFIX, FUNCTION)
#define TBLOCK_FUNCTION_TABLE_TYPE(SUFFIX) \
  GNS2(TBLOCK_TYPE(SUFFIX), FunctionTable)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TBLOCK_FUNCTION_TABLE_FORWARD(SUFFIX) \
  struct TBLOCK_FUNCTION_TABLE_TYPE(SUFFIX);
#define TBLOCK_API_MEMBER(SUFFIX) \
  const struct TBLOCK_FUNCTION_TABLE_TYPE(SUFFIX) *api;
#else
#define TBLOCK_FUNCTION_TABLE_FORWARD(SUFFIX)
#define TBLOCK_API_MEMBER(SUFFIX)
#endif

#define TBLOCK_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                 \
  TBLOCK_FUNCTION_TABLE_FORWARD(SUFFIX)                                                  \
  typedef struct {                                                             \
    TBLOCK_API_MEMBER(SUFFIX)                                               \
    union {                                                                    \
      TYPE limb[WORD_COUNT];                                                   \
      uint8_t byte[SIZE_IN_BYTES];                                             \
    };                                                                         \
  } TBLOCK_TYPE(SUFFIX);

#define TBLOCK_CLEAR_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, Clear)(TBLOCK_TYPE(SUFFIX) *target)

#define TBLOCK_AND_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, And)(                                 \
      TBLOCK_TYPE(SUFFIX) * destination,                                 \
      const TBLOCK_TYPE(SUFFIX) * source)

#define TBLOCK_OR_PROTOTYPE(SUFFIX)                                   \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX,                                       \
                                 Or)(TBLOCK_TYPE(SUFFIX) * destination,  \
                                     const TBLOCK_TYPE(SUFFIX) * source)

#define TBLOCK_XOR_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, Xor)(                                 \
      TBLOCK_TYPE(SUFFIX) * destination,                                 \
      const TBLOCK_TYPE(SUFFIX) * source)

#define TBLOCK_NOT_PROTOTYPE(SUFFIX)                                  \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX,                                       \
                                 Not)(TBLOCK_TYPE(SUFFIX) * target)

#define TBLOCK_SHIFT_LEFT_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, ShiftLeft)(                           \
      TBLOCK_TYPE(SUFFIX) * target, unsigned bitCount)

#define TBLOCK_SHIFT_RIGHT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, ShiftRight)(                          \
      TBLOCK_TYPE(SUFFIX) * target, unsigned bitCount)

#define TBLOCK_ROTATE_LEFT_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, RotateLeft)(                          \
      TBLOCK_TYPE(SUFFIX) *target, unsigned shift)

#define TBLOCK_ROTATE_RIGHT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, RotateRight)(                         \
      TBLOCK_TYPE(SUFFIX) *target, unsigned shift)

#define TBLOCK_BIT_SET_PROTOTYPE(SUFFIX)                               \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, BitSet)(TBLOCK_TYPE(SUFFIX) *target,  \
                                                 uint32_t bit)

#define TBLOCK_BIT_CLEAR_PROTOTYPE(SUFFIX)                             \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, BitClear)(                            \
      TBLOCK_TYPE(SUFFIX) *target, uint32_t bit)

#define TBLOCK_BIT_CHECK_PROTOTYPE(SUFFIX)                             \
  static inline OPSTATUS TBLOCK_FUNC(SUFFIX, BitCheck)(                            \
      const TBLOCK_TYPE(SUFFIX) *target, uint32_t bit, bool *outResult)

#include "Impl/Bit.impl"
#include "Impl/Bitwise.impl"
#include "Impl/Common.impl"
#include "Impl/Shift.impl"
#include "Impl/TBlock.impl"
#ifndef TYPE_DISABLE_FUNCTION_TABLE
#include "FunctionTable/TBlock.h"
#endif

#define TBLOCK_DECLARE_PROTOTYPES(SUFFIX)                                      \
  TBLOCK_CLEAR_PROTOTYPE(SUFFIX);                                     \
  TBLOCK_AND_PROTOTYPE(SUFFIX);                                       \
  TBLOCK_OR_PROTOTYPE(SUFFIX);                                        \
  TBLOCK_XOR_PROTOTYPE(SUFFIX);                                       \
  TBLOCK_NOT_PROTOTYPE(SUFFIX);                                       \
  TBLOCK_SHIFT_LEFT_PROTOTYPE(SUFFIX);                                 \
  TBLOCK_SHIFT_RIGHT_PROTOTYPE(SUFFIX);                                \
  TBLOCK_ROTATE_LEFT_PROTOTYPE(SUFFIX);                                \
  TBLOCK_ROTATE_RIGHT_PROTOTYPE(SUFFIX);                               \
  TBLOCK_BIT_SET_PROTOTYPE(SUFFIX);                                    \
  TBLOCK_BIT_CLEAR_PROTOTYPE(SUFFIX);                                  \
  TBLOCK_BIT_CHECK_PROTOTYPE(SUFFIX);

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TYPE_FUNCTION_TABLE_DECLARE_TBLOCK(SUFFIX) \
  TBLOCK_FUNCTION_TABLE_STRUCT(SUFFIX) \
  TBLOCK_FUNCTION_TABLE_INSTANCE(SUFFIX)
#else
#define TYPE_FUNCTION_TABLE_DECLARE_TBLOCK(SUFFIX)
#endif

#define TBLOCK_DECLARE(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES, BIT_SUFFIX)    \
  TBLOCK_STRUCT(SUFFIX, TYPE, WORD_COUNT, SIZE_IN_BYTES)                       \
  TBLOCK_DECLARE_PROTOTYPES(SUFFIX)                                            \
  TBLOCK_IMPLEMENT(SUFFIX, TYPE, BIT_SUFFIX)                                              \
  TYPE_FUNCTION_TABLE_DECLARE_TBLOCK(SUFFIX)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TBlock(SUFFIX, NAME)                                                   \
  TBLOCK_TYPE(SUFFIX) NAME = {0};                                           \
  NAME.api = &TBLOCK_FUNC(SUFFIX, functions);
#else
#define TBlock(SUFFIX, NAME) TBLOCK_TYPE(SUFFIX) NAME = {0};
#endif

TBLOCK_DECLARE(128, uint64_t, 2, 16, 64)
TBLOCK_DECLARE(256, uint64_t, 4, 32, 64)
TBLOCK_DECLARE(512, uint64_t, 8, 64, 64)
TBLOCK_DECLARE(1024, uint64_t, 16, 128, 64)
/* EOF */
