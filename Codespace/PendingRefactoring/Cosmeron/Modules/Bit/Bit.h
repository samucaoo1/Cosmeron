#pragma once

#include "../../Core/Outcome/Status.h"
#include "../../Core/Module/Package.h"
#include "../../Core/Preprocessor/Compiling.inc"
#include "Bit.space"

/* ==========================================================================
 * Standard Widths Table (Row-Tuple Pattern)
 * ========================================================================== */

#define BIT_WIDTH_COL_SUFFIX(SUFFIX, TYPE, BITS, MAX) SUFFIX
#define BIT_WIDTH_COL_TYPE(SUFFIX, TYPE, BITS, MAX) TYPE
#define BIT_WIDTH_COL_BITS(SUFFIX, TYPE, BITS, MAX) BITS
#define BIT_WIDTH_COL_MAX(SUFFIX, TYPE, BITS, MAX) MAX
#define BIT_WIDTH_GET(FIELD, ROW) PP_OP_CAT2(BIT_WIDTH_COL_, FIELD) ROW

#define BIT_WIDTH_TABLE(X)                                                     \
  X((8, uint8_t, 8U, UINT8_MAX))                                               \
  X((16, uint16_t, 16U, UINT16_MAX))                                           \
  X((32, uint32_t, 32U, UINT32_MAX))                                           \
  X((64, uint64_t, 64U, UINT64_MAX))

/* ==========================================================================
 * Universal Template Declaration: BIT
 * Parameterized package for bit manipulation of integer types.
 * ========================================================================== */

#define TEMPLATE_BIT(SUFFIX, TYPE)                                             \
  PACKAGE(BIT_PKG(SUFFIX),                                                     \
      OPERATION(Set, (TYPE *value, uint8_t bit)),                              \
      OPERATION(Clear, (TYPE *value, uint8_t bit)),                            \
      OPERATION(Flip, (TYPE *value, uint8_t bit)),                             \
      PREDICATE(Check, (TYPE value, uint8_t bit)),                             \
      PROCEDURE(MaskSet, (TYPE *value, TYPE mask)),                            \
      PROCEDURE(MaskClear, (TYPE *value, TYPE mask)),                          \
      PROCEDURE(MaskFlip, (TYPE *value, TYPE mask)),                           \
      PREDICATE(MaskCheckAll, (TYPE value, TYPE mask)),                        \
      PREDICATE(MaskCheckAny, (TYPE value, TYPE mask)),                        \
      FUNCTION(uint64_t, Popcount, (TYPE value)),                              \
      PREDICATE(IsPowerOfTwo, (TYPE value)),                                   \
      OPERATION(IndexLSB, (TYPE value, TYPE *outIndex)),                       \
      OPERATION(IndexMSB, (TYPE value, TYPE *outIndex)),                       \
      FUNCTION(TYPE, RotateLeft, (TYPE value, TYPE shift)),                    \
      FUNCTION(TYPE, RotateRight, (TYPE value, TYPE shift)),                   \
      OPERATION(Extract, (TYPE value, uint8_t high, uint8_t low, TYPE *outField)), \
      OPERATION(Insert, (TYPE *value, uint8_t high, uint8_t low, TYPE fieldValue)) \
  )

#include "Impl/Bit.impl"

/* ==========================================================================
 * Standard Width Instantiations
 * ========================================================================== */

#define BIT_INTERNAL_INSTANTIATE(ROW)                                          \
  TEMPLATE_IMPLEMENT(BIT, BIT_WIDTH_GET(SUFFIX, ROW),                          \
                     BIT_WIDTH_GET(TYPE, ROW));

BIT_WIDTH_TABLE(BIT_INTERNAL_INSTANTIATE)
#undef BIT_INTERNAL_INSTANTIATE

/* EOF */
