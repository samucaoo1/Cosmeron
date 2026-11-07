#pragma once

#include "Sort.space"

/* ==========================================================================
 * Radix Sort Width Table (Row-Tuple Pattern)
 * ========================================================================== */

#define SORT_RADIX_COL_SUFFIX(SUFFIX, TYPE, UNSIGNED, BITS, SIGNED)   SUFFIX
#define SORT_RADIX_COL_TYPE(SUFFIX, TYPE, UNSIGNED, BITS, SIGNED)     TYPE
#define SORT_RADIX_COL_UNSIGNED(SUFFIX, TYPE, UNSIGNED, BITS, SIGNED) UNSIGNED
#define SORT_RADIX_COL_BITS(SUFFIX, TYPE, UNSIGNED, BITS, SIGNED)     BITS
#define SORT_RADIX_COL_SIGNED(SUFFIX, TYPE, UNSIGNED, BITS, SIGNED)   SIGNED
#define SORT_RADIX_GET(FIELD, ROW) PP_OP_CAT2(SORT_RADIX_COL_, FIELD) ROW

#define SORT_RADIX_TABLE(X)                                                    \
  X((U32, uint32_t, uint32_t, 32u, false))                                     \
  X((I32, int32_t, uint32_t, 32u, true))                                       \
  X((U64, uint64_t, uint64_t, 64u, false))                                     \
  X((I64, int64_t, uint64_t, 64u, true))

#define SORT_RADIX_OPERATIONS                                                  \
  OPERATION(Radix_U32, (uint32_t *data, size_t count, SORT_TYPE(Order) order)), \
  OPERATION(RadixWithBuffer_U32,                                               \
            (uint32_t *data, size_t count, SORT_TYPE(Order) order,             \
             uint32_t *buffer, size_t bufferCount)),                           \
  OPERATION(Radix_I32, (int32_t *data, size_t count, SORT_TYPE(Order) order)), \
  OPERATION(RadixWithBuffer_I32,                                               \
            (int32_t *data, size_t count, SORT_TYPE(Order) order,              \
             int32_t *buffer, size_t bufferCount)),                            \
  OPERATION(Radix_U64, (uint64_t *data, size_t count, SORT_TYPE(Order) order)), \
  OPERATION(RadixWithBuffer_U64,                                               \
            (uint64_t *data, size_t count, SORT_TYPE(Order) order,             \
             uint64_t *buffer, size_t bufferCount)),                           \
  OPERATION(Radix_I64, (int64_t *data, size_t count, SORT_TYPE(Order) order)), \
  OPERATION(RadixWithBuffer_I64,                                               \
            (int64_t *data, size_t count, SORT_TYPE(Order) order,              \
             int64_t *buffer, size_t bufferCount))

/* EOF */
