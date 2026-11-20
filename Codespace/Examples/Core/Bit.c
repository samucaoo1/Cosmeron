#include "../../Cosmeron/Core/Bit/Bit.h"
#include "../../Cosmeron/Core/Bit/Bitset.h"

#include <stdio.h>

int main(void) {
  /* 1. Scalar Bit operations (32-bit) */
  uint32_t flags = 0U;
  (void)Bit_32_Set(&flags, 3U);
  (void)Bit_32_Set(&flags, 7U);

  uint32_t pop = Bit_32_Popcount(flags);
  uint32_t ctz = Bit_32_CountTrailingZeros(flags);
  printf("flags=0x%02X popcount=%u ctz=%u\n", flags, pop, ctz);

  /* 2. Fixed-width Bitset (128-bit) */
  MODULE_TYPE(Bitset128) bits = {0};
  Bitset128_Init(&bits);
  (void)OBJECT_CALL(&bits, SetRange, 10U, 20U);

  size_t firstSet = 0U;
  (void)OBJECT_CALL(&bits, FindFirstSet, &firstSet);
  printf("Bitset128 count=%zu firstSet=%zu\n", OBJECT_CALL_0(&bits, Count),
         firstSet);

  return 0;
}
