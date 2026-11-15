#pragma once

#if defined(NUMERIC_DISABLE_FUNCTION_TABLE) &&                                 \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#if defined(NUMERIC_DISABLE_FUNCTION_TABLE) &&                                 \
    !defined(PACKAGE_DISABLE_FUNCTION_TABLE)
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Preprocessor/Operations.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ==========================================================================
 * Numeric Instantiation Tables (Row-Tuple Pattern)
 * ========================================================================== */

#define NUMERIC_WIDTH_TABLE(X)                                                 \
  X((128, uint64_t))                                                           \
  X((256, uint64_t))                                                           \
  X((512, uint64_t))                                                           \
  X((1024, uint64_t))

#define NUMERIC_COMPLEX_TYPE_TABLE(X)                                          \
  X((Float32, float))                                                          \
  X((Float64, double))                                                         \
  X((Float128, long double))

#define NUMERIC_FIXED_TYPE_TABLE(X)                                            \
  X((Q8_8, int16_t, int32_t, 8))                                               \
  X((Q16_16, int32_t, int64_t, 16))

#define NUMERIC_RATIONAL_TYPE_TABLE(X)                                         \
  X((Int32, int32_t))                                                          \
  X((Int64, int64_t))

/* ==========================================================================
 * Shared Multi-Limb Internal Infrastructure (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE_IMPLEMENT(DOMAIN(Modules), Numeric,
  INTERNAL(
    PROCEDURE(LimbClear, (uint64_t *limbs, size_t limbCount), {
      memset(limbs, 0, limbCount * sizeof(uint64_t));
    }),

    PREDICATE(LimbIsZero, (const uint64_t *limbs, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        if (limbs[i] != 0U) {
          return false;
        }
      }
      return true;
    }),

    FUNCTION(CMPOUT, LimbCompare,
             (const uint64_t *left, const uint64_t *right, size_t limbCount), {
      for (size_t i = limbCount; i-- > 0;) {
        if (left[i] < right[i]) {
          return COMPARISON_CONST(LOWER);
        }
        if (left[i] > right[i]) {
          return COMPARISON_CONST(HIGHER);
        }
      }
      return COMPARISON_CONST(EQUAL);
    }),

    PREDICATE(LimbEqual,
              (const uint64_t *left, const uint64_t *right, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        if (left[i] != right[i]) {
          return false;
        }
      }
      return true;
    }),

    OPERATION(LimbAdd,
              (uint64_t *limbs, const uint64_t *source, size_t limbCount), {
      unsigned char carry = 0U;
      for (size_t i = 0; i < limbCount; ++i) {
        const uint64_t left = limbs[i];
        const uint64_t right = source[i];
        const uint64_t sum = left + right;
        const unsigned char c1 = (unsigned char)(sum < left);
        const uint64_t result = sum + (uint64_t)carry;
        const unsigned char c2 = (unsigned char)(carry && result == 0U);
        limbs[i] = result;
        carry = (unsigned char)(c1 | c2);
      }
      if (carry != 0U) {
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
      }
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbSub,
              (uint64_t *limbs, const uint64_t *source, size_t limbCount), {
      unsigned char borrow = 0U;
      for (size_t i = 0; i < limbCount; ++i) {
        const uint64_t left = limbs[i];
        const uint64_t right = source[i];
        const uint64_t difference = left - right;
        const unsigned char b1 = (unsigned char)(left < right);
        const unsigned char b2 = (unsigned char)(borrow && difference == 0U);
        limbs[i] = difference - (uint64_t)borrow;
        borrow = (unsigned char)(b1 | b2);
      }
      if (borrow != 0U) {
        return STATUS_CONST(ARITHMETIC_OVERFLOW);
      }
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbIncrement, (uint64_t *limbs, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        if (++limbs[i] != 0U) {
          return STATUS_CONST(SUCCESS);
        }
      }
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
    }),

    OPERATION(LimbDecrement, (uint64_t *limbs, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        if (limbs[i]-- != 0U) {
          return STATUS_CONST(SUCCESS);
        }
      }
      return STATUS_CONST(ARITHMETIC_OVERFLOW);
    }),

    PROCEDURE(LimbAnd,
              (uint64_t *limbs, const uint64_t *source, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        limbs[i] &= source[i];
      }
    }),

    PROCEDURE(LimbOr,
              (uint64_t *limbs, const uint64_t *source, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        limbs[i] |= source[i];
      }
    }),

    PROCEDURE(LimbXor,
              (uint64_t *limbs, const uint64_t *source, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        limbs[i] ^= source[i];
      }
    }),

    PROCEDURE(LimbNot, (uint64_t *limbs, size_t limbCount), {
      for (size_t i = 0; i < limbCount; ++i) {
        limbs[i] = ~limbs[i];
      }
    }),

    PROCEDURE(LimbShiftLeft,
              (uint64_t *limbs, size_t limbCount, unsigned bitCount), {
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      const size_t totalBitCapacity = limbCount * bitsPerLimb;
      if ((size_t)bitCount >= totalBitCapacity) {
        MODULE_IFUNC(Numeric, LimbClear)(limbs, limbCount);
        return;
      }
      const size_t wholeLimbShift = (size_t)bitCount / bitsPerLimb;
      const size_t intraLimbShift = (size_t)bitCount % bitsPerLimb;
      if (wholeLimbShift > 0U) {
        for (size_t i = limbCount; i-- > wholeLimbShift;) {
          limbs[i] = limbs[i - wholeLimbShift];
        }
        memset(limbs, 0, wholeLimbShift * sizeof(uint64_t));
      }
      if (intraLimbShift > 0U) {
        for (size_t i = limbCount; i-- > 1U;) {
          limbs[i] = (limbs[i] << intraLimbShift) |
                     (limbs[i - 1U] >> (bitsPerLimb - intraLimbShift));
        }
        limbs[0] <<= intraLimbShift;
      }
    }),

    PROCEDURE(LimbShiftRight,
              (uint64_t *limbs, size_t limbCount, unsigned bitCount), {
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      const size_t totalBitCapacity = limbCount * bitsPerLimb;
      if ((size_t)bitCount >= totalBitCapacity) {
        MODULE_IFUNC(Numeric, LimbClear)(limbs, limbCount);
        return;
      }
      const size_t wholeLimbShift = (size_t)bitCount / bitsPerLimb;
      const size_t intraLimbShift = (size_t)bitCount % bitsPerLimb;
      if (wholeLimbShift > 0U) {
        for (size_t i = 0; i < limbCount - wholeLimbShift; ++i) {
          limbs[i] = limbs[i + wholeLimbShift];
        }
        memset(&limbs[limbCount - wholeLimbShift], 0,
               wholeLimbShift * sizeof(uint64_t));
      }
      if (intraLimbShift > 0U) {
        for (size_t i = 0; i < limbCount - 1U; ++i) {
          limbs[i] = (limbs[i] >> intraLimbShift) |
                     (limbs[i + 1U] << (bitsPerLimb - intraLimbShift));
        }
        limbs[limbCount - 1U] >>= intraLimbShift;
      }
    }),

    OPERATION(LimbRotateLeft,
              (uint64_t *limbs, uint64_t *scratch, size_t limbCount,
               unsigned shift), {
      const size_t totalBits =
          limbCount * sizeof(uint64_t) * (size_t)CHAR_BIT;
      if (totalBits > (size_t)UINT_MAX) {
        return STATUS_CONST(OUT_OF_RANGE);
      }
      const unsigned totalBitsValue = (unsigned)totalBits;
      if (shift == 0U || shift % totalBitsValue == 0U) {
        return STATUS_CONST(SUCCESS);
      }
      shift %= totalBitsValue;
      memcpy(scratch, limbs, limbCount * sizeof(uint64_t));
      MODULE_IFUNC(Numeric, LimbShiftLeft)(limbs, limbCount, shift);
      MODULE_IFUNC(Numeric, LimbShiftRight)(scratch, limbCount,
                                            totalBitsValue - shift);
      for (size_t i = 0; i < limbCount; ++i) {
        limbs[i] |= scratch[i];
      }
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbRotateRight,
              (uint64_t *limbs, uint64_t *scratch, size_t limbCount,
               unsigned shift), {
      const size_t totalBits =
          limbCount * sizeof(uint64_t) * (size_t)CHAR_BIT;
      if (totalBits > (size_t)UINT_MAX) {
        return STATUS_CONST(OUT_OF_RANGE);
      }
      const unsigned totalBitsValue = (unsigned)totalBits;
      if (shift == 0U || shift % totalBitsValue == 0U) {
        return STATUS_CONST(SUCCESS);
      }
      shift %= totalBitsValue;
      memcpy(scratch, limbs, limbCount * sizeof(uint64_t));
      MODULE_IFUNC(Numeric, LimbShiftRight)(limbs, limbCount, shift);
      MODULE_IFUNC(Numeric, LimbShiftLeft)(scratch, limbCount,
                                           totalBitsValue - shift);
      for (size_t i = 0; i < limbCount; ++i) {
        limbs[i] |= scratch[i];
      }
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbBitSet, (uint64_t *limbs, size_t limbCount, uint32_t bit), {
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      const size_t limbIndex = (size_t)bit / bitsPerLimb;
      const uint8_t intraBit = (uint8_t)((size_t)bit % bitsPerLimb);
      if (limbIndex >= limbCount) {
        return STATUS_CONST(OUT_OF_RANGE);
      }
      limbs[limbIndex] |= (uint64_t)1U << intraBit;
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbBitClear,
              (uint64_t *limbs, size_t limbCount, uint32_t bit), {
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      const size_t limbIndex = (size_t)bit / bitsPerLimb;
      const uint8_t intraBit = (uint8_t)((size_t)bit % bitsPerLimb);
      if (limbIndex >= limbCount) {
        return STATUS_CONST(OUT_OF_RANGE);
      }
      limbs[limbIndex] &= ~((uint64_t)1U << intraBit);
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbBitCheck,
              (const uint64_t *limbs, size_t limbCount, uint32_t bit,
               bool *outResult), {
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      const size_t limbIndex = (size_t)bit / bitsPerLimb;
      const uint8_t intraBit = (uint8_t)((size_t)bit % bitsPerLimb);
      if (limbIndex >= limbCount) {
        return STATUS_CONST(OUT_OF_RANGE);
      }
      *outResult = ((limbs[limbIndex] >> intraBit) & (uint64_t)1U) != 0U;
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbMul,
              (uint64_t *limbs, const uint64_t *source, uint64_t *result,
               uint64_t *shifted, size_t limbCount), {
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      MODULE_IFUNC(Numeric, LimbClear)(result, limbCount);
      for (size_t bit = 0; bit < limbCount * bitsPerLimb; ++bit) {
        const size_t limbIndex = bit / bitsPerLimb;
        const size_t bitIndex = bit % bitsPerLimb;
        if (((source[limbIndex] >> bitIndex) & (uint64_t)1U) != 0U) {
          if (bit != 0U) {
            const size_t wholeLimbShift = bit / bitsPerLimb;
            const size_t intraLimbShift = bit % bitsPerLimb;
            for (size_t i = limbCount; i-- > 0;) {
              if (limbs[i] == 0U) {
                continue;
              }
              if (i + wholeLimbShift >= limbCount) {
                return STATUS_CONST(ARITHMETIC_OVERFLOW);
              }
              if (intraLimbShift != 0U &&
                  i + wholeLimbShift + 1U >= limbCount &&
                  (limbs[i] >> (bitsPerLimb - intraLimbShift)) != 0U) {
                return STATUS_CONST(ARITHMETIC_OVERFLOW);
              }
            }
          }
          memcpy(shifted, limbs, limbCount * sizeof(uint64_t));
          MODULE_IFUNC(Numeric, LimbShiftLeft)(shifted, limbCount,
                                               (unsigned)bit);
          const OPSTATUS status =
              MODULE_IFUNC(Numeric, LimbAdd)(result, shifted, limbCount);
          if (status != STATUS_CONST(SUCCESS)) {
            return status;
          }
        }
      }
      memcpy(limbs, result, limbCount * sizeof(uint64_t));
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbDivMod,
              (const uint64_t *dividend, const uint64_t *divisor,
               uint64_t *outQuotient, uint64_t *outRemainder,
               size_t limbCount), {
      if (MODULE_IFUNC(Numeric, LimbIsZero)(divisor, limbCount)) {
        return STATUS_CONST(DIVISION_BY_ZERO);
      }
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      MODULE_IFUNC(Numeric, LimbClear)(outQuotient, limbCount);
      MODULE_IFUNC(Numeric, LimbClear)(outRemainder, limbCount);
      for (size_t bit = limbCount * bitsPerLimb; bit-- > 0;) {
        const bool carriedHighBit =
            (outRemainder[limbCount - 1U] >> (bitsPerLimb - 1U)) != 0U;
        MODULE_IFUNC(Numeric, LimbShiftLeft)(outRemainder, limbCount, 1U);
        const size_t limbIndex = bit / bitsPerLimb;
        const size_t bitIndex = bit % bitsPerLimb;
        outRemainder[0] |=
            (dividend[limbIndex] >> bitIndex) & (uint64_t)1U;
        CMPOUT comparison = COMPARISON_CONST(LOWER);
        if (!carriedHighBit) {
          comparison = MODULE_IFUNC(Numeric, LimbCompare)(
              outRemainder, divisor, limbCount);
        }
        if (carriedHighBit || comparison != COMPARISON_CONST(LOWER)) {
          (void)MODULE_IFUNC(Numeric, LimbSub)(outRemainder, divisor,
                                               limbCount);
          outQuotient[limbIndex] |= (uint64_t)1U << bitIndex;
        }
      }
      return STATUS_CONST(SUCCESS);
    }),

    OPERATION(LimbToCStringBase,
              (uint64_t *scratch, size_t limbCount, char *buffer,
               size_t bufferSize, unsigned base), {
      if (bufferSize == 0U || base < 2U || base > 36U) {
        return STATUS_CONST(INVALID_ARGUMENT);
      }
      if (MODULE_IFUNC(Numeric, LimbIsZero)(scratch, limbCount)) {
        if (bufferSize < 2U) {
          return STATUS_CONST(OUT_OF_RANGE);
        }
        buffer[0] = '0';
        buffer[1] = '\0';
        return STATUS_CONST(SUCCESS);
      }
      const size_t bitsPerLimb = sizeof(uint64_t) * (size_t)CHAR_BIT;
      char reverse[1025];
      size_t length = 0U;
      while (!MODULE_IFUNC(Numeric, LimbIsZero)(scratch, limbCount)) {
        unsigned remainder = 0U;
        for (size_t i = limbCount; i-- > 0;) {
          const uint64_t next = scratch[i];
          uint64_t quotient = 0U;
          for (size_t bit = bitsPerLimb; bit-- > 0;) {
            remainder =
                (remainder << 1U) | (unsigned)((next >> bit) & (uint64_t)1U);
            if (remainder >= base) {
              remainder -= base;
              quotient |= (uint64_t)1U << bit;
            }
          }
          scratch[i] = quotient;
        }
        reverse[length++] = (char)(remainder < 10U ? '0' + remainder
                                                   : 'A' + remainder - 10U);
      }
      if (bufferSize <= length) {
        return STATUS_CONST(OUT_OF_RANGE);
      }
      for (size_t i = 0; i < length; ++i) {
        buffer[i] = reverse[length - i - 1U];
      }
      buffer[length] = '\0';
      return STATUS_CONST(SUCCESS);
    })
  )
);

/* EOF */
