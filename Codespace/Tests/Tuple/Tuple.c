#include "../../Cosmeron/Modules/Tuple/Tuple.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  MODULE_TYPED(Tuple, Pair, Int32) pair = { .first = 10, .second = 20 };
  assert(pair.first == 10);
  assert(pair.second == 20);

  MODULE_TYPED(Tuple, Triple, UInt32) triple = { .first = 1U, .second = 2U, .third = 3U };
  assert(triple.first == 1U);
  assert(triple.second == 2U);
  assert(triple.third == 3U);

  MODULE_TYPED(Tuple, Quad, Int64) quad = { .first = -1, .second = 2, .third = -3, .fourth = 4 };
  assert(quad.first == -1);
  assert(quad.fourth == 4);

  printf("Tuple tests passed!\n");
  return 0;
}
