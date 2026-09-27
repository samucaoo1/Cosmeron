#include "../../Cosmeron/Modules/Bit/Bit.h"

#include <stdint.h>

int main(void) {
  uint32_t value = 0;

  Bit_32_Set(&value, 5);
  if (value != UINT32_C(32) || !Bit_32_Check(&value, 5))
    return 1;

  Bit_32_Flip(&value, 5);
  if (value != 0)
    return 2;

  value = UINT32_C(0x0F);
  Bit_32_MaskSet(&value, UINT32_C(0xF0));
  if (value != UINT32_C(0xFF) || !Bit_32_MaskCheckAll(&value, UINT32_C(0x0F)))
    return 3;

  if (Bit_32_Popcount(UINT32_C(0xF0F0)) != 8)
    return 4;
  if (!Bit_32_IsPowerOfTwo(UINT32_C(1024)) || Bit_32_IsPowerOfTwo(0))
    return 5;
  if (Bit_32_IndexLSB(UINT32_C(0x10)) != 4 ||
      Bit_32_IndexMSB(UINT32_C(0x10)) != 4)
    return 6;

  if (Bit_32_RotateLeft(UINT32_C(1), 4) != UINT32_C(16))
    return 7;
  if (Bit_32_RotateRight(UINT32_C(16), 4) != UINT32_C(1))
    return 8;

  value = UINT32_C(0xABCD);
  if (Bit_32_Extract(&value, 11, 8) != UINT32_C(0xB))
    return 9;

  Bit_32_Insert(&value, 7, 4, UINT32_C(0x2));
  if (Bit_32_Extract(&value, 7, 4) != UINT32_C(0x2))
    return 10;

  value = UINT32_C(0x12345678);
  Bit_32_Set(&value, 32);
  if (value != UINT32_C(0x12345678) || Bit_32_Check(&value, 32))
    return 11;

  if (Bit_32_Extract(&value, 32, 32) != 0 ||
      Bit_32_Extract(&value, 31, 32) != 0)
    return 12;

  Bit_32_Insert(&value, 32, 32, UINT32_MAX);
  if (value != UINT32_C(0x12345678))
    return 13;

  if (Bit_32_RotateLeft(value, 32) != value ||
      Bit_32_RotateRight(value, 64) != value)
    return 14;

  if (Bit_8_IndexLSB(0) != UINT8_MAX || Bit_8_IndexMSB(0) != UINT8_MAX ||
      Bit_32_IndexLSB(0) != UINT32_MAX || Bit_32_IndexMSB(0) != UINT32_MAX)
    return 15;

  return 0;
}
