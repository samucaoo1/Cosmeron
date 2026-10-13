#include "../../Cosmeron/Modules/Bit/Bit.h"

#include <stdint.h>

int main(void) {
  uint32_t value = 0;
  uint32_t index = 0;
  uint32_t field = 0;
  uint8_t index8 = 0;

  if (Bit32_Set(&value, 5) != STATUS_CONST(SUCCESS))
    return 1;
  if (value != UINT32_C(32) || !Bit32_Check(value, 5))
    return 2;

  if (Bit32_Flip(&value, 5) != STATUS_CONST(SUCCESS) || value != 0)
    return 3;

  value = UINT32_C(0x0F);
  Bit32_MaskSet(&value, UINT32_C(0xF0));
  if (value != UINT32_C(0xFF) ||
      !Bit32_MaskCheckAll(value, UINT32_C(0x0F)))
    return 4;

  if (Bit32_Popcount(UINT32_C(0xF0F0)) != 8)
    return 5;
  if (!Bit32_IsPowerOfTwo(UINT32_C(1024)) || Bit32_IsPowerOfTwo(0))
    return 6;

  if (Bit32_IndexLSB(UINT32_C(0x10), &index) != STATUS_CONST(SUCCESS) ||
      index != 4U)
    return 7;
  if (Bit32_IndexMSB(UINT32_C(0x10), &index) != STATUS_CONST(SUCCESS) ||
      index != 4U)
    return 8;

  if (Bit32_RotateLeft(UINT32_C(1), 4) != UINT32_C(16))
    return 9;
  if (Bit32_RotateRight(UINT32_C(16), 4) != UINT32_C(1))
    return 10;

  value = UINT32_C(0xABCD);
  if (Bit32_Extract(value, 11, 8, &field) != STATUS_CONST(SUCCESS) ||
      field != UINT32_C(0xB))
    return 11;

  if (Bit32_Insert(&value, 7, 4, UINT32_C(0x2)) != STATUS_CONST(SUCCESS))
    return 12;
  if (Bit32_Extract(value, 7, 4, &field) != STATUS_CONST(SUCCESS) ||
      field != UINT32_C(0x2))
    return 13;

  value = UINT32_C(0x12345678);
  if (Bit32_Set(&value, 32) != STATUS_CONST(OUT_OF_RANGE) ||
      value != UINT32_C(0x12345678) || Bit32_Check(value, 32))
    return 14;

  if (Bit32_Extract(value, 32, 32, &field) != STATUS_CONST(OUT_OF_RANGE) ||
      Bit32_Extract(value, 31, 32, &field) != STATUS_CONST(OUT_OF_RANGE))
    return 15;

  if (Bit32_Insert(&value, 32, 32, UINT32_MAX) !=
          STATUS_CONST(OUT_OF_RANGE) ||
      value != UINT32_C(0x12345678))
    return 16;

  if (Bit32_RotateLeft(value, 32) != value ||
      Bit32_RotateRight(value, 64) != value)
    return 17;

  if (Bit8_IndexLSB(0, &index8) != STATUS_CONST(NOT_FOUND) ||
      Bit8_IndexMSB(0, &index8) != STATUS_CONST(NOT_FOUND) ||
      Bit32_IndexLSB(0, &index) != STATUS_CONST(NOT_FOUND) ||
      Bit32_IndexMSB(0, &index) != STATUS_CONST(NOT_FOUND))
    return 18;

  return 0;
}
