#include "../../Cosmeron/Modules/Random/Random.h"

#define TEST_ENGINE(NAME) do {                                                  \
  RANDOM_ENGINE_TYPE(NAME) a, b;                                               \
  RANDOM_ENGINE_FUNC(NAME, Seed)(&a, UINT64_C(1234567));                       \
  RANDOM_ENGINE_FUNC(NAME, Seed)(&b, UINT64_C(1234567));                       \
  for (int i=0;i<8;++i) {                                                      \
    uint64_t x=RANDOM_ENGINE_FUNC(NAME, Next)(&a);                              \
    uint64_t y=RANDOM_ENGINE_FUNC(NAME, Next)(&b);                              \
    if (x!=y) return __LINE__;                                                  \
  }                                                                            \
  if (RANDOM_ENGINE_FUNC(NAME, Descriptor).stateSize != sizeof(a)) return __LINE__;\
} while (0)

int main(void) {
  TEST_ENGINE(Lcg);
  TEST_ENGINE(Pcg);
  TEST_ENGINE(Romu);
  TEST_ENGINE(Sfc);
  TEST_ENGINE(Splitmix);
  TEST_ENGINE(WyRand);
  TEST_ENGINE(Xoroshiro);
  TEST_ENGINE(Xoshiro);
  return 0;
}
