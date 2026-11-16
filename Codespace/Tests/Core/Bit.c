#include "../../../Cosmeron/Core/Bit/Bit.h"

int main(void) {
  /* 1. Unit operations: Set, Clear, Flip, Check */
  uint32_t v32 = 0U;
  if (Bit_32_Set(&v32, 5U) != STATUS_CONST(SUCCESS) || !Bit_32_Check(v32, 5U))
    return 1;
  if (Bit_32_Set(&v32, 32U) != STATUS_CONST(OUT_OF_RANGE))
    return 2;
  if (Bit_32_Flip(&v32, 5U) != STATUS_CONST(SUCCESS) || Bit_32_Check(v32, 5U))
    return 3;
  if (Bit_32_Set(&v32, 3U) != STATUS_CONST(SUCCESS) ||
      Bit_32_Clear(&v32, 3U) != STATUS_CONST(SUCCESS) || v32 != 0U)
    return 4;

  /* 2. Masks & MaskRange */
  uint16_t mask16 = 0U;
  if (Bit_16_MaskRange(7U, 4U, &mask16) != STATUS_CONST(SUCCESS) ||
      mask16 != 0x00F0U)
    return 5;
  uint16_t v16 = 0x000FU;
  Bit_16_MaskSet(&v16, mask16);
  if (!Bit_16_MaskCheckAll(v16, 0x00FFU))
    return 6;
  Bit_16_MaskFlip(&v16, 0x000FU);
  if (v16 != 0x00F0U || !Bit_16_MaskCheckAny(v16, 0x0020U))
    return 7;
  Bit_16_MaskClear(&v16, 0x00A0U);
  if (v16 != 0x0050U)
    return 8;

  /* 3. Popcount, CLZ, CTZ, Parity, IndexLSB, IndexMSB */
  uint64_t v64 = UINT64_C(0x00F0000000000008);
  if (Bit_64_Popcount(v64) != 5U || !Bit_64_Parity(v64))
    return 9;
  if (Bit_64_CountTrailingZeros(v64) != 3U ||
      Bit_64_CountLeadingZeros(v64) != 8U)
    return 10;
  if (Bit_64_CountLeadingZeros(0U) != 64U ||
      Bit_64_CountTrailingZeros(0U) != 64U)
    return 11;
  uint64_t idx = 0U;
  if (Bit_64_IndexLSB(v64, &idx) != STATUS_CONST(SUCCESS) || idx != 3U)
    return 12;
  if (Bit_64_IndexMSB(v64, &idx) != STATUS_CONST(SUCCESS) || idx != 55U)
    return 13;
  if (Bit_64_IndexLSB(0U, &idx) != STATUS_CONST(NOT_FOUND))
    return 14;

  /* 4. Power of two, Log2, IsolateLowestSet, ClearLowestSet */
  if (!Bit_32_IsPowerOfTwo(1024U) || Bit_32_IsPowerOfTwo(0U) ||
      Bit_32_IsPowerOfTwo(6U))
    return 15;
  if (Bit_32_FloorPowerOfTwo(19U) != 16U || Bit_32_FloorPowerOfTwo(0U) != 0U)
    return 16;
  uint32_t nextPow = 0U;
  if (Bit_32_NextPowerOfTwo(19U, &nextPow) != STATUS_CONST(SUCCESS) ||
      nextPow != 32U)
    return 17;
  uint8_t nextPow8 = 0U;
  if (Bit_8_NextPowerOfTwo(200U, &nextPow8) != STATUS_CONST(ARITHMETIC_OVERFLOW))
    return 18;
  uint8_t logVal = 0U;
  if (Bit_32_Log2Floor(19U, &logVal) != STATUS_CONST(SUCCESS) || logVal != 4U)
    return 19;
  if (Bit_32_Log2Ceil(19U, &logVal) != STATUS_CONST(SUCCESS) || logVal != 5U)
    return 20;
  if (Bit_32_IsolateLowestSet(0x2CU) != 0x04U ||
      Bit_32_ClearLowestSet(0x2CU) != 0x28U)
    return 21;

  /* 5. Rotate, ReverseBits, ByteSwap */
  if (Bit_8_RotateLeft(0x81U, 1U) != 0x03U ||
      Bit_8_RotateRight(0x03U, 1U) != 0x81U)
    return 22;
  if (Bit_8_ReverseBits(0xB2U) != 0x4DU)
    return 23;
  if (Bit_16_ByteSwap(0x1234U) != 0x3412U ||
      Bit_32_ByteSwap(0x12345678U) != 0x78563412U)
    return 24;

  /* 6. Extract & Insert */
  uint32_t field = 0U;
  if (Bit_32_Extract(0xABCD1234U, 15U, 8U, &field) != STATUS_CONST(SUCCESS) ||
      field != 0x12U)
    return 25;
  uint32_t target = 0xABCD1234U;
  if (Bit_32_Insert(&target, 15U, 8U, 0xFFU) != STATUS_CONST(SUCCESS) ||
      target != 0xABCDFF34U)
    return 26;

  /* 7. PACKAGE_CALL dispatch */
  if (PACKAGE_CALL(Bit_32, Popcount, 0x0F0F0F0FU) != 16U)
    return 27;

  return 0;
}

