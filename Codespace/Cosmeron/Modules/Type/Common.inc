#pragma once

#if defined(TYPE_DISABLE_FUNCTION_TABLE) &&                                    \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../Core/Algorithm/Comparison.h"
#include "../../Core/Bit/Bit.h"
#include "../../Core/Module/Module.h"
#include "../../Core/Outcome/Status.h"
#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Operations.h"
#include "../../Core/Primitive/Types.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* ==========================================================================
 * Tag Width Table (Row-Tuple Pattern)
 * ========================================================================== */

#define TYPE_TAG_WIDTH_COL_SUFFIX(SUFFIX, TYPE, BYTES) SUFFIX
#define TYPE_TAG_WIDTH_COL_TYPE(SUFFIX, TYPE, BYTES)   TYPE
#define TYPE_TAG_WIDTH_COL_BYTES(SUFFIX, TYPE, BYTES)  BYTES
#define TYPE_TAG_WIDTH_GET(FIELD, ROW)                                         \
  PP_OP_CAT2(TYPE_TAG_WIDTH_COL_, FIELD) ROW

#define TYPE_TAG_WIDTH_TABLE(X)                                                \
  X((16, uint16_t, 2))                                                         \
  X((32, uint32_t, 4))                                                         \
  X((64, uint64_t, 8))

/* ==========================================================================
 * Type Width Table (Row-Tuple Pattern: Block, Bigint, Decimal, Bitset)
 * ========================================================================== */

#define TYPE_WIDTH_COL_SUFFIX(SUFFIX, TYPE, WORDS, BYTES) SUFFIX
#define TYPE_WIDTH_COL_TYPE(SUFFIX, TYPE, WORDS, BYTES)   TYPE
#define TYPE_WIDTH_COL_WORDS(SUFFIX, TYPE, WORDS, BYTES)  WORDS
#define TYPE_WIDTH_COL_BYTES(SUFFIX, TYPE, WORDS, BYTES)  BYTES
#define TYPE_WIDTH_GET(FIELD, ROW) PP_OP_CAT2(TYPE_WIDTH_COL_, FIELD) ROW

#define TYPE_WIDTH_TABLE(X)                                                    \
  X((128, uint64_t, 2, 16))                                                    \
  X((256, uint64_t, 4, 32))                                                    \
  X((512, uint64_t, 8, 64))                                                    \
  X((1024, uint64_t, 16, 128))

/* ==========================================================================
 * Module Template Forwarding Helper
 * ========================================================================== */

#ifndef TEMPLATE_Type
#define TEMPLATE_Type(TARGET, ...) NS(TEMPLATE_Type, TARGET)(__VA_ARGS__)
#endif

#define TEMPLATE_Type_Tag_HAS_IMPL     1
#define TEMPLATE_Type_Block_HAS_IMPL   1
#define TEMPLATE_Type_Bigint_HAS_IMPL  1
#define TEMPLATE_Type_Decimal_HAS_IMPL 1
#define TEMPLATE_Type_Bitset_HAS_IMPL  1

/* ==========================================================================
 * Shared Internal Bit/Limb Helpers (Zero FunctionTable Overhead)
 * ========================================================================== */

MODULE(Type,
  INTERNAL(
    FUNCTION(size_t, Popcount64, (uint64_t value), {
      return MODULE_FUNC(Bit, 64, Popcount)(value);
    }),

    FUNCTION(uint32_t, FindFirstSet64, (uint64_t value), {
      return (uint32_t)MODULE_FUNC(Bit, 64, CountTrailingZeros)(value);
    }),

    FUNCTION(uint64_t, LimbsTailMask, (size_t bitCount), {
      if (bitCount == 0U)
        return 0U;
      const size_t rem = bitCount % 64U;
      if (rem == 0U)
        return UINT64_MAX;
      uint64_t mask = 0U;
      MODULE_FUNC(Bit, 64, MaskRange)((uint8_t)(rem - 1U), 0U, &mask);
      return mask;
    }),

    PROCEDURE(LimbsMaskTail,
              (uint64_t *limbs, size_t limbCount, size_t bitCount), {
      if (!limbs || limbCount == 0U || bitCount == 0U)
        return;
      limbs[limbCount - 1U] &= MODULE_IFUNC(Type, LimbsTailMask)(bitCount);
    }),

    FUNCTION(size_t, LimbsPopcount, (const uint64_t *limbs, size_t limbCount), {
      if (!limbs)
        return 0U;
      size_t total = 0U;
      for (size_t i = 0U; i < limbCount; ++i)
        total += MODULE_IFUNC(Type, Popcount64)(limbs[i]);
      return total;
    }),

    PREDICATE(LimbsAny, (const uint64_t *limbs, size_t limbCount), {
      if (!limbs)
        return false;
      for (size_t i = 0U; i < limbCount; ++i) {
        if (limbs[i] != 0U)
          return true;
      }
      return false;
    }),

    PREDICATE(LimbsAll,
              (const uint64_t *limbs, size_t limbCount, size_t bitCount), {
      if (!limbs || limbCount == 0U || bitCount == 0U)
        return false;
      for (size_t i = 0U; i + 1U < limbCount; ++i) {
        if (limbs[i] != UINT64_MAX)
          return false;
      }
      const uint64_t mask = MODULE_IFUNC(Type, LimbsTailMask)(bitCount);
      return (limbs[limbCount - 1U] & mask) == mask;
    }),

    PREDICATE(LimbsEqual,
              (const uint64_t *left, const uint64_t *right, size_t limbCount), {
      if (!left || !right)
        return left == right;
      for (size_t i = 0U; i < limbCount; ++i) {
        if (left[i] != right[i])
          return false;
      }
      return true;
    }),

    PREDICATE(LimbsIsSubset,
              (const uint64_t *subset, const uint64_t *superset,
               size_t limbCount), {
      if (!subset || !superset)
        return false;
      for (size_t i = 0U; i < limbCount; ++i) {
        if ((subset[i] & ~superset[i]) != 0U)
          return false;
      }
      return true;
    }),

    PREDICATE(LimbsIntersects,
              (const uint64_t *left, const uint64_t *right, size_t limbCount), {
      if (!left || !right)
        return false;
      for (size_t i = 0U; i < limbCount; ++i) {
        if ((left[i] & right[i]) != 0U)
          return true;
      }
      return false;
    }),

    PROCEDURE(LimbsSetRange,
              (uint64_t *limbs, size_t startBit, size_t endBit), {
      if (!limbs || startBit >= endBit)
        return;
      const size_t firstLimb = startBit / 64U;
      const size_t lastLimb = (endBit - 1U) / 64U;
      const uint64_t startMask = UINT64_MAX << (startBit % 64U);
      const uint64_t endMask = MODULE_IFUNC(Type, LimbsTailMask)(endBit);
      if (firstLimb == lastLimb) {
        MODULE_FUNC(Bit, 64, MaskSet)(&limbs[firstLimb], startMask & endMask);
        return;
      }
      MODULE_FUNC(Bit, 64, MaskSet)(&limbs[firstLimb], startMask);
      for (size_t i = firstLimb + 1U; i < lastLimb; ++i)
        limbs[i] = UINT64_MAX;
      MODULE_FUNC(Bit, 64, MaskSet)(&limbs[lastLimb], endMask);
    }),

    PROCEDURE(LimbsClearRange,
              (uint64_t *limbs, size_t startBit, size_t endBit), {
      if (!limbs || startBit >= endBit)
        return;
      const size_t firstLimb = startBit / 64U;
      const size_t lastLimb = (endBit - 1U) / 64U;
      const uint64_t startMask = UINT64_MAX << (startBit % 64U);
      const uint64_t endMask = MODULE_IFUNC(Type, LimbsTailMask)(endBit);
      if (firstLimb == lastLimb) {
        MODULE_FUNC(Bit, 64, MaskClear)(&limbs[firstLimb], startMask & endMask);
        return;
      }
      MODULE_FUNC(Bit, 64, MaskClear)(&limbs[firstLimb], startMask);
      for (size_t i = firstLimb + 1U; i < lastLimb; ++i)
        limbs[i] = 0U;
      MODULE_FUNC(Bit, 64, MaskClear)(&limbs[lastLimb], endMask);
    }),

    PROCEDURE(LimbsFlipRange,
              (uint64_t *limbs, size_t startBit, size_t endBit), {
      if (!limbs || startBit >= endBit)
        return;
      const size_t firstLimb = startBit / 64U;
      const size_t lastLimb = (endBit - 1U) / 64U;
      const uint64_t startMask = UINT64_MAX << (startBit % 64U);
      const uint64_t endMask = MODULE_IFUNC(Type, LimbsTailMask)(endBit);
      if (firstLimb == lastLimb) {
        MODULE_FUNC(Bit, 64, MaskFlip)(&limbs[firstLimb], startMask & endMask);
        return;
      }
      MODULE_FUNC(Bit, 64, MaskFlip)(&limbs[firstLimb], startMask);
      for (size_t i = firstLimb + 1U; i < lastLimb; ++i)
        limbs[i] ^= UINT64_MAX;
      MODULE_FUNC(Bit, 64, MaskFlip)(&limbs[lastLimb], endMask);
    }),

    PREDICATE(LimbsFindFirstSet,
              (const uint64_t *limbs, size_t limbCount, size_t bitCount,
               size_t *outBit), {
      if (!limbs || !outBit || limbCount == 0U || bitCount == 0U)
        return false;
      for (size_t i = 0U; i < limbCount; ++i) {
        uint64_t word = limbs[i];
        if (i + 1U == limbCount)
          word &= MODULE_IFUNC(Type, LimbsTailMask)(bitCount);
        if (word != 0U) {
          *outBit = (i * 64U) + MODULE_IFUNC(Type, FindFirstSet64)(word);
          return true;
        }
      }
      return false;
    }),

    PREDICATE(LimbsFindFirstZero,
              (const uint64_t *limbs, size_t limbCount, size_t bitCount,
               size_t *outBit), {
      if (!limbs || !outBit || limbCount == 0U || bitCount == 0U)
        return false;
      for (size_t i = 0U; i < limbCount; ++i) {
        uint64_t inverted = ~limbs[i];
        if (i + 1U == limbCount)
          inverted &= MODULE_IFUNC(Type, LimbsTailMask)(bitCount);
        if (inverted != 0U) {
          *outBit = (i * 64U) + MODULE_IFUNC(Type, FindFirstSet64)(inverted);
          return true;
        }
      }
      return false;
    }),

    PREDICATE(LimbsFindNextSet,
              (const uint64_t *limbs, size_t limbCount, size_t bitCount,
               size_t startBit, size_t *outBit), {
      if (!limbs || !outBit || limbCount == 0U || startBit >= bitCount)
        return false;
      const size_t startLimb = startBit / 64U;
      const size_t startRem = startBit % 64U;
      for (size_t i = startLimb; i < limbCount; ++i) {
        uint64_t word = limbs[i];
        if (i == startLimb)
          word &= (UINT64_MAX << startRem);
        if (i + 1U == limbCount)
          word &= MODULE_IFUNC(Type, LimbsTailMask)(bitCount);
        if (word != 0U) {
          *outBit = (i * 64U) + MODULE_IFUNC(Type, FindFirstSet64)(word);
          return true;
        }
      }
      return false;
    }),

    PROCEDURE(LimbsShiftLeft,
              (uint64_t *limbs, size_t limbCount, size_t bitCount,
               size_t shift), {
      if (!limbs || limbCount == 0U || bitCount == 0U || shift == 0U)
        return;
      if (shift >= bitCount) {
        memset(limbs, 0, limbCount * sizeof(uint64_t));
        return;
      }
      const size_t wholeLimbShift = shift / 64U;
      const size_t intraLimbShift = shift % 64U;
      if (wholeLimbShift > 0U) {
        for (size_t i = limbCount; i-- > wholeLimbShift;)
          limbs[i] = limbs[i - wholeLimbShift];
        memset(limbs, 0, wholeLimbShift * sizeof(uint64_t));
      }
      if (intraLimbShift > 0U) {
        for (size_t i = limbCount; i-- > 1U;)
          limbs[i] = (limbs[i] << intraLimbShift) |
                     (limbs[i - 1U] >> (64U - intraLimbShift));
        limbs[0] <<= intraLimbShift;
      }
      MODULE_IFUNC(Type, LimbsMaskTail)(limbs, limbCount, bitCount);
    }),

    PROCEDURE(LimbsShiftRight,
              (uint64_t *limbs, size_t limbCount, size_t shift), {
      if (!limbs || limbCount == 0U || shift == 0U)
        return;
      const size_t totalBits = limbCount * 64U;
      if (shift >= totalBits) {
        memset(limbs, 0, limbCount * sizeof(uint64_t));
        return;
      }
      const size_t wholeLimbShift = shift / 64U;
      const size_t intraLimbShift = shift % 64U;
      if (wholeLimbShift > 0U) {
        for (size_t i = 0U; i < limbCount - wholeLimbShift; ++i)
          limbs[i] = limbs[i + wholeLimbShift];
        memset(&limbs[limbCount - wholeLimbShift], 0,
               wholeLimbShift * sizeof(uint64_t));
      }
      if (intraLimbShift > 0U) {
        for (size_t i = 0U; i + 1U < limbCount; ++i)
          limbs[i] = (limbs[i] >> intraLimbShift) |
                     (limbs[i + 1U] << (64U - intraLimbShift));
        limbs[limbCount - 1U] >>= intraLimbShift;
      }
    })
  )
);

/* EOF */
