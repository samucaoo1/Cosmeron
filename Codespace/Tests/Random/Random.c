#include "../../Cosmeron/Modules/Random/Random.h"

#include <stdint.h>

#define CHECK_MIXER(NAME)                                                      \
  do {                                                                         \
    uint64_t a = Random_Mixer_##NAME(UINT64_C(0x123456789ABCDEF0));            \
    uint64_t b = Random_Mixer_##NAME(UINT64_C(0x123456789ABCDEF0));            \
    if (a != b)                                                                \
      return __LINE__;                                                         \
  } while (0)

int main(void) {
  uint64_t first = Random_U64_FromSeed(UINT64_C(42));
  uint64_t second = Random_U64_FromSeed(UINT64_C(42));
  uint64_t entropyBuffer = 0U;
  RANDOM_SOURCE_TYPE(Value) source;
  RANDOM_SOURCE_TYPE(Value) systemSource;
  RANDOM_ENTROPY_TYPE(Pool) pool;
  int values[] = {1, 2, 3, 4, 5};

  if (first != second)
    return 1;
  if (Random_U64_FromSeed(UINT64_C(42)) != first)
    return 16;

  (void)Random_U64();
  {
    uint64_t value = Random_RangeU64(10U, 20U);
    if (value < 10U || value > 20U)
      return 2;
  }
  {
    int64_t value = Random_RangeI64(-10, 10);
    if (value < -10 || value > 10)
      return 3;
  }
  {
    double value = Random_F64();
    if (value < 0.0 || value >= 1.0)
      return 4;
  }
  (void)Random_Bool();

  if (Random_Source_Init(&source, &Random_Engine_Xoshiro_Descriptor,
                         UINT64_C(1234), NULL) != STATUS_CONST(SUCCESS))
    return 5;

  {
    uint64_t value = 0;
    if (Random_Distribution_U64(&source, 10U, 20U, &value) !=
            STATUS_CONST(SUCCESS) ||
        value < 10U || value > 20U)
      return 6;
  }

  if (Random_Shuffle_VectorFromSource(values, 5U, sizeof(values[0]), &source) !=
      STATUS_CONST(SUCCESS))
    return 7;
  if (Random_Shuffle_Vector(values, 5U, sizeof(values[0])) !=
      STATUS_CONST(SUCCESS))
    return 8;

  if (Random_Source_Init(NULL, &Random_Engine_Xoshiro_Descriptor,
                         UINT64_C(1), NULL) != STATUS_CONST(INVALID_ARGUMENT))
    return 9;

  if (source.state == NULL || !source.ownsState)
    return 10;
  Random_Source_Destroy(&source);
  if (source.state != NULL || source.engine != NULL)
    return 11;

  {
    OPSTATUS status = Random_Source_InitSystem(
        &systemSource, &Random_Engine_Xoshiro_Descriptor, Random_Mixer_WyHash);
    if (status == STATUS_CONST(SUCCESS)) {
      uint64_t value = 0;
      if (Random_Source_NextU64(&systemSource, &value) !=
          STATUS_CONST(SUCCESS))
        return 17;
      Random_Source_Destroy(&systemSource);
    } else if (status != STATUS_CONST(NOT_AVAILABLE)) {
      return 12;
    }
  }

  CHECK_MIXER(Jenkins);
  CHECK_MIXER(Knuth);
  CHECK_MIXER(Murmur3);
  CHECK_MIXER(Splitmix64);
  CHECK_MIXER(Stafford);
  CHECK_MIXER(Wang);
  CHECK_MIXER(WyHash);
  CHECK_MIXER(Xorshift);

  if (!Random_Entropy_System(&entropyBuffer, sizeof(entropyBuffer)))
    return 13;
  (void)Random_Entropy_Address();
  (void)Random_Entropy_Clock();
  (void)Random_Entropy_Jitter();
  (void)Random_Entropy_Thread();
  (void)Random_Entropy_Time();
  (void)Random_Entropy_Collect();

  Random_Entropy_Pool_Init(&pool);
  if (pool.count != 0U)
    return 14;
  Random_Entropy_Pool_Add(&pool, UINT64_C(123));
  if (pool.count != 1U)
    return 15;
  (void)Random_Entropy_Pool_Finalize(&pool);

  return 0;
}
