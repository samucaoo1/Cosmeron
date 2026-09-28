#include "../../Cosmeron/Modules/Random/Random.h"

static int sorted_sum(const int *v, size_t n) {
  int s = 0;
  for (size_t i = 0; i < n; ++i)
    s += v[i];
  return s;
}

int main(void) {
  RANDOM_SOURCE_TYPE(Value) source;
  RANDOM_ENGINE_TYPE(Xoshiro) storage;
  uint64_t u = 0;
  int64_t s = 0;
  double d = 0.0;
  bool b = false;

  if (Random_Source_InitWithState(
          &source, &Random_Engine_Xoshiro_Descriptor, &storage,
          sizeof(storage), 42U, Random_Mixer_Splitmix64) !=
      STATUS_CONST(SUCCESS))
    return 1;

  for (int i = 0; i < 64; ++i) {
    if (Random_Distribution_U64(&source, 10U, 20U, &u) !=
            STATUS_CONST(SUCCESS) ||
        Random_Distribution_I64(&source, -10, 10, &s) !=
            STATUS_CONST(SUCCESS) ||
        Random_Distribution_F64(&source, &d) != STATUS_CONST(SUCCESS) ||
        Random_Distribution_Bool(&source, &b) != STATUS_CONST(SUCCESS))
      return 2;
    if (u < 10U || u > 20U || s < -10 || s > 10 || d < 0.0 || d >= 1.0)
      return 3;
  }

  if (Random_Distribution_U64(NULL, 0U, 1U, &u) !=
          STATUS_CONST(INVALID_ARGUMENT) ||
      Random_Distribution_U64(&source, 0U, 1U, NULL) !=
          STATUS_CONST(INVALID_ARGUMENT) ||
      Random_Distribution_F64(&source, NULL) !=
          STATUS_CONST(INVALID_ARGUMENT) ||
      Random_Distribution_Bool(&source, NULL) !=
          STATUS_CONST(INVALID_ARGUMENT))
    return 4;

  if (Random_Source_Reseed(&source, 42U) != STATUS_CONST(SUCCESS))
    return 5;
  {
    uint64_t first = 0;
    uint64_t second = 0;
    if (Random_Source_NextU64(&source, &first) != STATUS_CONST(SUCCESS))
      return 6;
    if (Random_Source_Reseed(&source, 42U) != STATUS_CONST(SUCCESS))
      return 7;
    if (Random_Source_NextU64(&source, &second) != STATUS_CONST(SUCCESS) ||
        first != second)
      return 8;
    if (Random_Source_NextU64(NULL, &second) !=
            STATUS_CONST(INVALID_ARGUMENT) ||
        Random_Source_NextU64(&source, NULL) !=
            STATUS_CONST(INVALID_ARGUMENT))
      return 9;
  }

  {
    int v[] = {1, 2, 3, 4, 5, 6};
    int before = sorted_sum(v, 6);
    if (Random_Shuffle_VectorFromSource(v, 6, sizeof(v[0]), &source) !=
        STATUS_CONST(SUCCESS))
      return 10;
    if (sorted_sum(v, 6) != before)
      return 11;
  }

  Random_Source_Destroy(&source);
  if (source.state != NULL || source.engine != NULL)
    return 12;
  return 0;
}
