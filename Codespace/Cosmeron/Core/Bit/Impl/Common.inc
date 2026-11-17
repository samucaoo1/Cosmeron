#pragma once

#if defined(BIT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Core_Bit)
#define COSMERON_DISABLE_FT_Core_Bit 1
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
/* Internal Constants & SWAR Helpers (Zero FunctionTable Overhead)            */
/* ========================================================================== */

#define BIT_INTERNAL_CONST_TABLE(X)                                            \
  X((BITS_PER_BYTE, 8U))                                                       \
  X((U64_BITS, 64U))                                                           \
  X((POPCOUNT_SHIFT, 56U))                                                     \
  X((SHIFT_1, 1U))                                                             \
  X((SHIFT_2, 2U))                                                             \
  X((SHIFT_4, 4U))                                                             \
  X((SHIFT_8, 8U))                                                             \
  X((SHIFT_16, 16U))                                                           \
  X((SHIFT_24, 24U))                                                           \
  X((SHIFT_32, 32U))                                                           \
  X((SHIFT_40, 40U))                                                           \
  X((SHIFT_56, 56U))                                                           \
  X((BYTES_1, 1U))                                                             \
  X((BYTES_2, 2U))                                                             \
  X((BYTES_4, 4U))                                                             \
  X((BYTE_ALL_ONES, 0xFFU))

MODULE(DOMAIN(Core), Bit,
  INTERNAL(
    ENUM(Constants, BIT_INTERNAL_CONST_TABLE),

    FUNCTION(size_t, Popcount64, (uint64_t value), {
      const uint64_t evenOddMask = UINT64_C(0x5555555555555555);
      const uint64_t pairsMask = UINT64_C(0x3333333333333333);
      const uint64_t nibblesMask = UINT64_C(0x0F0F0F0F0F0F0F0F);
      const uint64_t mulFactor = UINT64_C(0x0101010101010101);
      uint64_t temp = value;
      temp -= (temp >> CNS(Bit, Internal, SHIFT_1)) & evenOddMask;
      temp = (temp & pairsMask) +
             ((temp >> CNS(Bit, Internal, SHIFT_2)) & pairsMask);
      temp = (temp + (temp >> CNS(Bit, Internal, SHIFT_4))) & nibblesMask;
      return (size_t)((temp * mulFactor) >> CNS(Bit, Internal, POPCOUNT_SHIFT));
    }),

    FUNCTION(size_t, CountLeadingZeros64, (uint64_t value, size_t totalBits), {
      if (value == 0U)
        return totalBits;
      const uint64_t mask32 = UINT64_C(0xFFFFFFFF00000000);
      const uint64_t mask16 = UINT64_C(0xFFFF000000000000);
      const uint64_t mask8 = UINT64_C(0xFF00000000000000);
      const uint64_t mask4 = UINT64_C(0xF000000000000000);
      const uint64_t mask2 = UINT64_C(0xC000000000000000);
      const uint64_t mask1 = UINT64_C(0x8000000000000000);
      uint64_t v = value << (CNS(Bit, Internal, U64_BITS) - totalBits);
      size_t count = 0U;
      if ((v & mask32) == 0U) {
        count += CNS(Bit, Internal, SHIFT_32);
        v <<= CNS(Bit, Internal, SHIFT_32);
      }
      if ((v & mask16) == 0U) {
        count += CNS(Bit, Internal, SHIFT_16);
        v <<= CNS(Bit, Internal, SHIFT_16);
      }
      if ((v & mask8) == 0U) {
        count += CNS(Bit, Internal, SHIFT_8);
        v <<= CNS(Bit, Internal, SHIFT_8);
      }
      if ((v & mask4) == 0U) {
        count += CNS(Bit, Internal, SHIFT_4);
        v <<= CNS(Bit, Internal, SHIFT_4);
      }
      if ((v & mask2) == 0U) {
        count += CNS(Bit, Internal, SHIFT_2);
        v <<= CNS(Bit, Internal, SHIFT_2);
      }
      if ((v & mask1) == 0U)
        count += CNS(Bit, Internal, SHIFT_1);
      return count;
    }),

    FUNCTION(size_t, CountTrailingZeros64, (uint64_t value, size_t totalBits), {
      if (value == 0U)
        return totalBits;
      const uint64_t mask32 = UINT64_C(0xFFFFFFFF);
      const uint64_t mask16 = UINT64_C(0xFFFF);
      const uint64_t mask8 = UINT64_C(0xFF);
      const uint64_t mask4 = UINT64_C(0xF);
      const uint64_t mask2 = UINT64_C(0x3);
      const uint64_t mask1 = UINT64_C(0x1);
      uint64_t v = value;
      size_t count = 0U;
      if (totalBits > CNS(Bit, Internal, SHIFT_32) && (v & mask32) == 0U) {
        count += CNS(Bit, Internal, SHIFT_32);
        v >>= CNS(Bit, Internal, SHIFT_32);
      }
      if (totalBits > CNS(Bit, Internal, SHIFT_16) && (v & mask16) == 0U) {
        count += CNS(Bit, Internal, SHIFT_16);
        v >>= CNS(Bit, Internal, SHIFT_16);
      }
      if (totalBits > CNS(Bit, Internal, SHIFT_8) && (v & mask8) == 0U) {
        count += CNS(Bit, Internal, SHIFT_8);
        v >>= CNS(Bit, Internal, SHIFT_8);
      }
      if ((v & mask4) == 0U) {
        count += CNS(Bit, Internal, SHIFT_4);
        v >>= CNS(Bit, Internal, SHIFT_4);
      }
      if ((v & mask2) == 0U) {
        count += CNS(Bit, Internal, SHIFT_2);
        v >>= CNS(Bit, Internal, SHIFT_2);
      }
      if ((v & mask1) == 0U)
        count += CNS(Bit, Internal, SHIFT_1);
      return count;
    }),

    FUNCTION(uint64_t, ReverseBits64, (uint64_t value, size_t byteWidth), {
      const uint64_t evenOddMask = UINT64_C(0x5555555555555555);
      const uint64_t pairsMask = UINT64_C(0x3333333333333333);
      const uint64_t nibblesMask = UINT64_C(0x0F0F0F0F0F0F0F0F);
      const uint64_t bytesMask = UINT64_C(0x00FF00FF00FF00FF);
      const uint64_t wordsMask = UINT64_C(0x0000FFFF0000FFFF);
      uint64_t v = value;
      v = ((v >> CNS(Bit, Internal, SHIFT_1)) & evenOddMask) |
          ((v & evenOddMask) << CNS(Bit, Internal, SHIFT_1));
      v = ((v >> CNS(Bit, Internal, SHIFT_2)) & pairsMask) |
          ((v & pairsMask) << CNS(Bit, Internal, SHIFT_2));
      v = ((v >> CNS(Bit, Internal, SHIFT_4)) & nibblesMask) |
          ((v & nibblesMask) << CNS(Bit, Internal, SHIFT_4));
      if (byteWidth > CNS(Bit, Internal, BYTES_1)) {
        v = ((v >> CNS(Bit, Internal, SHIFT_8)) & bytesMask) |
            ((v & bytesMask) << CNS(Bit, Internal, SHIFT_8));
      }
      if (byteWidth > CNS(Bit, Internal, BYTES_2)) {
        v = ((v >> CNS(Bit, Internal, SHIFT_16)) & wordsMask) |
            ((v & wordsMask) << CNS(Bit, Internal, SHIFT_16));
      }
      if (byteWidth > CNS(Bit, Internal, BYTES_4)) {
        v = (v >> CNS(Bit, Internal, SHIFT_32)) |
            (v << CNS(Bit, Internal, SHIFT_32));
      }
      return v;
    }),

    FUNCTION(uint64_t, ByteSwap64, (uint64_t value, size_t byteWidth), {
      if (byteWidth == CNS(Bit, Internal, BYTES_1))
        return value;
      if (byteWidth == CNS(Bit, Internal, BYTES_2)) {
        const uint64_t lowByteMask = UINT64_C(0x00FF);
        const uint64_t highByteMask = UINT64_C(0xFF00);
        return ((value & lowByteMask) << CNS(Bit, Internal, SHIFT_8)) |
               ((value & highByteMask) >> CNS(Bit, Internal, SHIFT_8));
      }
      if (byteWidth == CNS(Bit, Internal, BYTES_4)) {
        const uint64_t b0Mask = UINT64_C(0x000000FF);
        const uint64_t b1Mask = UINT64_C(0x0000FF00);
        const uint64_t b2Mask = UINT64_C(0x00FF0000);
        const uint64_t b3Mask = UINT64_C(0xFF000000);
        return ((value & b0Mask) << CNS(Bit, Internal, SHIFT_24)) |
               ((value & b1Mask) << CNS(Bit, Internal, SHIFT_8)) |
               ((value & b2Mask) >> CNS(Bit, Internal, SHIFT_8)) |
               ((value & b3Mask) >> CNS(Bit, Internal, SHIFT_24));
      }
      const uint64_t b0Mask = UINT64_C(0x00000000000000FF);
      const uint64_t b1Mask = UINT64_C(0x000000000000FF00);
      const uint64_t b2Mask = UINT64_C(0x0000000000FF0000);
      const uint64_t b3Mask = UINT64_C(0x00000000FF000000);
      const uint64_t b4Mask = UINT64_C(0x000000FF00000000);
      const uint64_t b5Mask = UINT64_C(0x0000FF0000000000);
      const uint64_t b6Mask = UINT64_C(0x00FF000000000000);
      const uint64_t b7Mask = UINT64_C(0xFF00000000000000);
      return ((value & b0Mask) << CNS(Bit, Internal, SHIFT_56)) |
             ((value & b1Mask) << CNS(Bit, Internal, SHIFT_40)) |
             ((value & b2Mask) << CNS(Bit, Internal, SHIFT_24)) |
             ((value & b3Mask) << CNS(Bit, Internal, SHIFT_8)) |
             ((value & b4Mask) >> CNS(Bit, Internal, SHIFT_8)) |
             ((value & b5Mask) >> CNS(Bit, Internal, SHIFT_24)) |
             ((value & b6Mask) >> CNS(Bit, Internal, SHIFT_40)) |
             ((value & b7Mask) >> CNS(Bit, Internal, SHIFT_56));
    })
  )
);

/* EOF */

