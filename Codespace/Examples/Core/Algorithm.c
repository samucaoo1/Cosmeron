#include "../../Cosmeron/Core/Algorithm/Comparison.h"
#include "../../Cosmeron/Core/Algorithm/Repeat.h"
#include "../../Cosmeron/Core/Algorithm/Swap.h"

#include <stdio.h>

TEMPLATE_INSTANTIATE(Memory, Swap, Int32, int32_t);
TEMPLATE_INSTANTIATE(Algorithm_Repeat, Int32, int32_t);

int main(void) {
  /* 1. Three-way comparison */
  CMPOUT cmp = Algorithm_Comparison_Int(10, 20);
  printf("Compare(10, 20) == LOWER: %s\n",
         (cmp == COMPARISON_CONST(LOWER)) ? "true" : "false");

  /* 2. Typed swap */
  int32_t a = 42;
  int32_t b = 99;
  MODULE_FUNC(Memory, Swap, Int32, Values)(&a, &b);
  printf("After Swap: a=%d, b=%d\n", (int)a, (int)b);

  /* 3. Repeat fill */
  int32_t buffer[4] = {0};
  if (MODULE_FUNC(Algorithm, Repeat, Int32, Fill)(buffer, 7, 4U) ==
      STATUS_CONST(SUCCESS)) {
    printf("Repeat Fill: [%d, %d, %d, %d]\n", (int)buffer[0], (int)buffer[1],
           (int)buffer[2], (int)buffer[3]);
  }

  return 0;
}
