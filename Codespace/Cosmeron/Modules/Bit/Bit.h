#pragma once

#include "Bit.space"

/* ============================================================
 * Names and Prototypes — Common
 * ============================================================ */

#define BIT_SET_PROTOTYPE(TYPE, SUFFIX)                               \
  inline static void BIT_FUNC(SUFFIX, Set)(TYPE *value, uint8_t bit)

#define BIT_CLEAR_PROTOTYPE(TYPE, SUFFIX)                             \
  inline static void BIT_FUNC(SUFFIX, Clear)(TYPE *value, uint8_t bit)

#define BIT_FLIP_PROTOTYPE(TYPE, SUFFIX)                              \
  inline static void BIT_FUNC(SUFFIX, Flip)(TYPE *value, uint8_t bit)

#define BIT_CHECK_PROTOTYPE(TYPE, SUFFIX)                             \
  inline static bool BIT_FUNC(SUFFIX, Check)(const TYPE *value, uint8_t bit)

#define BIT_MASK_SET_PROTOTYPE(TYPE, SUFFIX)                          \
  inline static void BIT_FUNC(SUFFIX, MaskSet)(TYPE *value, TYPE mask)

#define BIT_MASK_CLEAR_PROTOTYPE(TYPE, SUFFIX)                        \
  inline static void BIT_FUNC(SUFFIX, MaskClear)(TYPE *value, TYPE mask)

#define BIT_MASK_FLIP_PROTOTYPE(TYPE, SUFFIX)                         \
  inline static void BIT_FUNC(SUFFIX, MaskFlip)(TYPE *value, TYPE mask)

#define BIT_MASK_CHECK_ALL_PROTOTYPE(TYPE, SUFFIX)                    \
  inline static bool BIT_FUNC(SUFFIX, MaskCheckAll)(const TYPE *value, TYPE mask)

#define BIT_MASK_CHECK_ANY_PROTOTYPE(TYPE, SUFFIX)                    \
  inline static bool BIT_FUNC(SUFFIX, MaskCheckAny)(const TYPE *value, TYPE mask)

/* ============================================================
 * Names and Prototypes — Extra
 * ============================================================ */

#define BIT_POPCOUNT_PROTOTYPE(TYPE, SUFFIX)                          \
  inline static uint64_t BIT_FUNC(SUFFIX, Popcount)(TYPE value)

#define BIT_IS_POWER_OF_TWO_PROTOTYPE(TYPE, SUFFIX)                   \
  inline static bool BIT_FUNC(SUFFIX, IsPowerOfTwo)(TYPE value)

#define BIT_INDEX_LSB_PROTOTYPE(TYPE, SUFFIX)                         \
  inline static TYPE BIT_FUNC(SUFFIX, IndexLSB)(TYPE value)

#define BIT_INDEX_MSB_PROTOTYPE(TYPE, SUFFIX)                         \
  inline static TYPE BIT_FUNC(SUFFIX, IndexMSB)(TYPE value)

#define BIT_ROTATE_LEFT_PROTOTYPE(TYPE, SUFFIX)                       \
  inline static TYPE BIT_FUNC(SUFFIX, RotateLeft)(TYPE value, TYPE shift)

#define BIT_ROTATE_RIGHT_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE BIT_FUNC(SUFFIX, RotateRight)(TYPE value, TYPE shift)

/* ============================================================
 * Names and Prototypes — Field
 * ============================================================ */

#define BIT_EXTRACT_PROTOTYPE(TYPE, SUFFIX)                           \
  inline static TYPE BIT_FUNC(SUFFIX, Extract)(const TYPE *value, uint8_t high,  \
                                             uint8_t low)

#define BIT_INSERT_PROTOTYPE(TYPE, SUFFIX)                            \
  inline static void BIT_FUNC(SUFFIX, Insert)(TYPE *value, uint8_t high,         \
                                            uint8_t low, TYPE field_value)

/* ============================================================
 * Implementation snippets
 * ============================================================ */

#include "Impl/Common.impl"
#include "Impl/Extra.impl"
#include "Impl/Field.impl"

/* ============================================================
 * Declaration & Instantiation
 * ============================================================ */

#define BIT_DECLARE(TYPE, SUFFIX, LITERAL)                                     \
  BIT_SET_PROTOTYPE(TYPE, SUFFIX);                                    \
  BIT_CLEAR_PROTOTYPE(TYPE, SUFFIX);                                  \
  BIT_FLIP_PROTOTYPE(TYPE, SUFFIX);                                   \
  BIT_CHECK_PROTOTYPE(TYPE, SUFFIX);                                  \
  BIT_MASK_SET_PROTOTYPE(TYPE, SUFFIX);                               \
  BIT_MASK_CLEAR_PROTOTYPE(TYPE, SUFFIX);                             \
  BIT_MASK_FLIP_PROTOTYPE(TYPE, SUFFIX);                              \
  BIT_MASK_CHECK_ALL_PROTOTYPE(TYPE, SUFFIX);                         \
  BIT_MASK_CHECK_ANY_PROTOTYPE(TYPE, SUFFIX);                         \
  BIT_POPCOUNT_PROTOTYPE(TYPE, SUFFIX);                               \
  BIT_IS_POWER_OF_TWO_PROTOTYPE(TYPE, SUFFIX);                        \
  BIT_INDEX_LSB_PROTOTYPE(TYPE, SUFFIX);                              \
  BIT_INDEX_MSB_PROTOTYPE(TYPE, SUFFIX);                              \
  BIT_ROTATE_LEFT_PROTOTYPE(TYPE, SUFFIX);                            \
  BIT_ROTATE_RIGHT_PROTOTYPE(TYPE, SUFFIX);                           \
  BIT_EXTRACT_PROTOTYPE(TYPE, SUFFIX);                                \
  BIT_INSERT_PROTOTYPE(TYPE, SUFFIX);                                 \
  BIT_IMPLEMENT(TYPE, SUFFIX, LITERAL)

#define BIT_IMPLEMENT(TYPE, SUFFIX, LITERAL)                                   \
  BIT_IMPLEMENT_SET(TYPE, SUFFIX, LITERAL)                                     \
  BIT_IMPLEMENT_CLEAR(TYPE, SUFFIX, LITERAL)                                   \
  BIT_IMPLEMENT_FLIP(TYPE, SUFFIX, LITERAL)                                    \
  BIT_IMPLEMENT_CHECK(TYPE, SUFFIX, LITERAL)                                   \
  BIT_IMPLEMENT_MASK_SET(TYPE, SUFFIX)                                         \
  BIT_IMPLEMENT_MASK_CLEAR(TYPE, SUFFIX)                                       \
  BIT_IMPLEMENT_MASK_FLIP(TYPE, SUFFIX)                                        \
  BIT_IMPLEMENT_MASK_CHECK_ALL(TYPE, SUFFIX)                                   \
  BIT_IMPLEMENT_MASK_CHECK_ANY(TYPE, SUFFIX)                                   \
  BIT_IMPLEMENT_POPCOUNT(TYPE, SUFFIX)                                         \
  BIT_IMPLEMENT_IS_POWER_OF_TWO(TYPE, SUFFIX)                                  \
  BIT_IMPLEMENT_INDEX_LSB(TYPE, SUFFIX)                                        \
  BIT_IMPLEMENT_INDEX_MSB(TYPE, SUFFIX)                                        \
  BIT_IMPLEMENT_ROTATE_LEFT(TYPE, SUFFIX)                                      \
  BIT_IMPLEMENT_ROTATE_RIGHT(TYPE, SUFFIX)                                     \
  BIT_IMPLEMENT_EXTRACT(TYPE, SUFFIX, LITERAL)                                 \
  BIT_IMPLEMENT_INSERT(TYPE, SUFFIX, LITERAL)

BIT_DECLARE(uint8_t, 8, 1U)
BIT_DECLARE(uint16_t, 16, 1U)
BIT_DECLARE(uint32_t, 32, 1UL)
BIT_DECLARE(uint64_t, 64, 1ULL)
/* EOF */
