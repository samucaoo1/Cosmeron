#pragma once

#include "../../Core/Error/Status.h"
#include "Bit.space"

#define BIT_SET_PROTOTYPE(TYPE, SUFFIX)                                        \
  static inline OPSTATUS BIT_FUNC(SUFFIX, Set)(TYPE *value, uint8_t bit)

#define BIT_CLEAR_PROTOTYPE(TYPE, SUFFIX)                                      \
  static inline OPSTATUS BIT_FUNC(SUFFIX, Clear)(TYPE *value, uint8_t bit)

#define BIT_FLIP_PROTOTYPE(TYPE, SUFFIX)                                       \
  static inline OPSTATUS BIT_FUNC(SUFFIX, Flip)(TYPE *value, uint8_t bit)

#define BIT_CHECK_PROTOTYPE(TYPE, SUFFIX)                                      \
  static inline bool BIT_FUNC(SUFFIX, Check)(TYPE value, uint8_t bit)

#define BIT_MASK_SET_PROTOTYPE(TYPE, SUFFIX)                                   \
  static inline void BIT_FUNC(SUFFIX, MaskSet)(TYPE *value, TYPE mask)

#define BIT_MASK_CLEAR_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline void BIT_FUNC(SUFFIX, MaskClear)(TYPE *value, TYPE mask)

#define BIT_MASK_FLIP_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline void BIT_FUNC(SUFFIX, MaskFlip)(TYPE *value, TYPE mask)

#define BIT_MASK_CHECK_ALL_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline bool BIT_FUNC(SUFFIX, MaskCheckAll)(TYPE value, TYPE mask)

#define BIT_MASK_CHECK_ANY_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline bool BIT_FUNC(SUFFIX, MaskCheckAny)(TYPE value, TYPE mask)

#define BIT_POPCOUNT_PROTOTYPE(TYPE, SUFFIX)                                   \
  static inline uint64_t BIT_FUNC(SUFFIX, Popcount)(TYPE value)

#define BIT_IS_POWER_OF_TWO_PROTOTYPE(TYPE, SUFFIX)                            \
  static inline bool BIT_FUNC(SUFFIX, IsPowerOfTwo)(TYPE value)

#define BIT_INDEX_LSB_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline OPSTATUS BIT_FUNC(SUFFIX, IndexLSB)(TYPE value, TYPE *outIndex)

#define BIT_INDEX_MSB_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline OPSTATUS BIT_FUNC(SUFFIX, IndexMSB)(TYPE value, TYPE *outIndex)

#define BIT_ROTATE_LEFT_PROTOTYPE(TYPE, SUFFIX)                                \
  static inline TYPE BIT_FUNC(SUFFIX, RotateLeft)(TYPE value, TYPE shift)

#define BIT_ROTATE_RIGHT_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline TYPE BIT_FUNC(SUFFIX, RotateRight)(TYPE value, TYPE shift)

#define BIT_EXTRACT_PROTOTYPE(TYPE, SUFFIX)                                    \
  static inline OPSTATUS BIT_FUNC(SUFFIX, Extract)(                            \
      TYPE value, uint8_t high, uint8_t low, TYPE *outField)

#define BIT_INSERT_PROTOTYPE(TYPE, SUFFIX)                                     \
  static inline OPSTATUS BIT_FUNC(SUFFIX, Insert)(                             \
      TYPE *value, uint8_t high, uint8_t low, TYPE fieldValue)

#include "Impl/Common.impl"
#include "Impl/Extra.impl"
#include "Impl/Field.impl"

#define BIT_IMPLEMENT_ALL(TYPE, SUFFIX, LITERAL)                               \
  BIT_SET_IMPLEMENT(TYPE, SUFFIX, LITERAL)                                     \
  BIT_CLEAR_IMPLEMENT(TYPE, SUFFIX, LITERAL)                                   \
  BIT_FLIP_IMPLEMENT(TYPE, SUFFIX, LITERAL)                                    \
  BIT_CHECK_IMPLEMENT(TYPE, SUFFIX, LITERAL)                                   \
  BIT_MASK_SET_IMPLEMENT(TYPE, SUFFIX)                                         \
  BIT_MASK_CLEAR_IMPLEMENT(TYPE, SUFFIX)                                       \
  BIT_MASK_FLIP_IMPLEMENT(TYPE, SUFFIX)                                        \
  BIT_MASK_CHECK_ALL_IMPLEMENT(TYPE, SUFFIX)                                   \
  BIT_MASK_CHECK_ANY_IMPLEMENT(TYPE, SUFFIX)                                   \
  BIT_POPCOUNT_IMPLEMENT(TYPE, SUFFIX)                                         \
  BIT_IS_POWER_OF_TWO_IMPLEMENT(TYPE, SUFFIX)                                  \
  BIT_INDEX_LSB_IMPLEMENT(TYPE, SUFFIX)                                        \
  BIT_INDEX_MSB_IMPLEMENT(TYPE, SUFFIX)                                        \
  BIT_ROTATE_LEFT_IMPLEMENT(TYPE, SUFFIX)                                      \
  BIT_ROTATE_RIGHT_IMPLEMENT(TYPE, SUFFIX)                                     \
  BIT_EXTRACT_IMPLEMENT(TYPE, SUFFIX, LITERAL)                                 \
  BIT_INSERT_IMPLEMENT(TYPE, SUFFIX, LITERAL)

BIT_IMPLEMENT_ALL(uint8_t, 8, 1U)
BIT_IMPLEMENT_ALL(uint16_t, 16, 1U)
BIT_IMPLEMENT_ALL(uint32_t, 32, 1UL)
BIT_IMPLEMENT_ALL(uint64_t, 64, 1ULL)
/* EOF */
