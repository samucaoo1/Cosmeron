#include "../../Cosmeron/Modules/Tuple/Tuple.h"

#include <stdio.h>

int main(void) {
  MODULE_TYPED(Tuple, Pair, Int32) pair = {.first = 10, .second = 20};
  MODULE_TYPED(Tuple, Triple, UInt32) triple = {
      .first = 1U, .second = 2U, .third = 3U};
  MODULE_TYPED(Tuple, Quad, Int64) quad = {
      .first = -1, .second = 2, .third = -3, .fourth = 4};

  printf("Pair: (%d, %d)\n", (int)pair.first, (int)pair.second);
  printf("Triple: (%u, %u, %u)\n", triple.first, triple.second, triple.third);
  printf("Quad: (%lld, %lld, %lld, %lld)\n", (long long)quad.first,
         (long long)quad.second, (long long)quad.third,
         (long long)quad.fourth);

  return 0;
}
