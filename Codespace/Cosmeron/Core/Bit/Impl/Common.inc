#pragma once

#if defined(BIT_DISABLE_FUNCTION_TABLE) &&                                     \
    !defined(PACKAGE_DISABLE_FUNCTION_TABLE)
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../Module/Module.h"
#include "../../Condition/Status.h"
#include "../../Preprocessor/Compiling.inc"
#include "../../Preprocessor/Operations.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Standard Widths Table (Row-Tuple Pattern)                                  */
/* ========================================================================== */

#define BIT_WIDTH_COL_SUFFIX(SUFFIX, TYPE, BITS, MAX) SUFFIX
#define BIT_WIDTH_COL_TYPE(SUFFIX, TYPE, BITS, MAX)   TYPE
#define BIT_WIDTH_COL_BITS(SUFFIX, TYPE, BITS, MAX)   BITS
#define BIT_WIDTH_COL_MAX(SUFFIX, TYPE, BITS, MAX)    MAX
#define BIT_WIDTH_GET(FIELD, ROW) PP_OP_CAT2(BIT_WIDTH_COL_, FIELD) ROW

#define BIT_WIDTH_TABLE(X)                                                     \
  X((8, uint8_t, 8U, UINT8_MAX))                                               \
  X((16, uint16_t, 16U, UINT16_MAX))                                           \
  X((32, uint32_t, 32U, UINT32_MAX))                                           \
  X((64, uint64_t, 64U, UINT64_MAX))

#define BIT_WIDTH_TYPE_TABLE(X)                                                \
  X((8, uint8_t))                                                              \
  X((16, uint16_t))                                                            \
  X((32, uint32_t))                                                            \
  X((64, uint64_t))

/* ========================================================================== */
/* Internal Bitwise Masks                                                     */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_BIT_MASK_EVEN_ODD_U64                          \
  UINT64_C(0x5555555555555555)
#define COSMERON_MACRO_INTERNAL_BIT_MASK_PAIRS_U64                             \
  UINT64_C(0x3333333333333333)
#define COSMERON_MACRO_INTERNAL_BIT_MASK_NIBBLES_U64                           \
  UINT64_C(0x0F0F0F0F0F0F0F0F)

/* EOF */

