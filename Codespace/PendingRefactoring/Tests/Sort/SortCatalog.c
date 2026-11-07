#include "../../Cosmeron/Modules/Sort/Sort.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static CMPOUT cmpInt(const void *a, const void *b, void *ctx) {
  (void)ctx;
  int x = *(const int *)a;
  int y = *(const int *)b;
  return x < y ? COMPARISON_CONST(LOWER)
               : (x > y ? COMPARISON_CONST(HIGHER) : COMPARISON_CONST(EQUAL));
}

typedef OPSTATUS (*SortFunc)(void *, size_t, size_t, TCALLBACK(Comparison));

static void checkAlgorithm(SortFunc fn) {
  TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(cmpInt, NULL);
  const int original[] = {9, 2, -3, 9, 0, 4, 1, -3, 7};
  int values[9];
  memcpy(values, original, sizeof values);
  assert(fn(values, 9, sizeof(int), cb) == STATUS_CONST(SUCCESS));
  for (size_t i = 1; i < 9; ++i)
    assert(values[i - 1] <= values[i]);
  assert(values[0] == -3 && values[8] == 9);
  assert(fn(NULL, 0, sizeof(int), cb) == STATUS_CONST(SUCCESS));
  assert(fn(NULL, 2, sizeof(int), cb) == STATUS_CONST(INVALID_ARGUMENT));
  assert(fn(values, 9, 0, cb) == STATUS_CONST(INVALID_ARGUMENT));
}

static int stdlibCompareInt(const void *a, const void *b) {
  int x = *(const int *)a;
  int y = *(const int *)b;
  return x < y ? -1 : (x > y ? 1 : 0);
}

static void checkProperties(void) {
  TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(cmpInt, NULL);
  SortFunc algorithms[] = {
      SORT_FUNC(Array),           SORT_FUNC(Stable),
      SORT_FUNC(Insertion),       SORT_FUNC(Heap),
      SORT_FUNC(Intro),           SORT_FUNC(Merge),
      SORT_FUNC(Bubble),          SORT_FUNC(Cocktail),
      SORT_FUNC(Comb),            SORT_FUNC(Gnome),
      SORT_FUNC(OddEven),         SORT_FUNC(Selection),
      SORT_FUNC(DoubleSelection), SORT_FUNC(Shell),
      SORT_FUNC(Pancake),         SORT_FUNC(Quick),
      SORT_FUNC(QuickThreeWay)};
  uint32_t state = UINT32_C(0x12345678);
  for (size_t count = 0; count <= 128; ++count) {
    for (unsigned round = 0; round < 4u; ++round) {
      int original[128], expected[128], actual[128];
      for (size_t i = 0; i < count; ++i) {
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        original[i] = (int)((state >> 8) % 101u) - 50;
      }
      memcpy(expected, original, count * sizeof(int));
      qsort(expected, count, sizeof(int), stdlibCompareInt);
      for (size_t algorithm = 0;
           algorithm < sizeof algorithms / sizeof algorithms[0]; ++algorithm) {
        memcpy(actual, original, count * sizeof(int));
        assert(algorithms[algorithm](actual, count, sizeof(int), cb) ==
               STATUS_CONST(SUCCESS));
        assert(memcmp(actual, expected, count * sizeof(int)) == 0);
      }
    }
  }
}

int main(void) {
  TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(cmpInt, NULL);

  checkProperties();
  checkAlgorithm(SORT_FUNC(Bubble));
  checkAlgorithm(SORT_FUNC(Cocktail));
  checkAlgorithm(SORT_FUNC(Comb));
  checkAlgorithm(SORT_FUNC(Gnome));
  checkAlgorithm(SORT_FUNC(OddEven));
  checkAlgorithm(SORT_FUNC(Selection));
  checkAlgorithm(SORT_FUNC(DoubleSelection));
  checkAlgorithm(SORT_FUNC(Shell));
  checkAlgorithm(SORT_FUNC(Pancake));
  checkAlgorithm(SORT_FUNC(Quick));
  checkAlgorithm(SORT_FUNC(QuickThreeWay));

  {
    int values[] = {1, 4, 2, 8, 5, 7, 9, 3};
    size_t count = 0;
    assert(SORT_FUNC(Stalin)(values, 8, sizeof(int), cb, &count) ==
           STATUS_CONST(SUCCESS));
    assert(count == 4);
    assert(values[0] == 1 && values[1] == 4 && values[2] == 8 &&
           values[3] == 9);
  }
  {
    int values[] = {2, 1};
    assert(SORT_FUNC(Bogo)(values, 2, sizeof(int), cb, 42, 0) ==
           STATUS_CONST(TIMEOUT));
    assert(SORT_FUNC(Bogo)(values, 2, sizeof(int), cb, 42, 1000) ==
           STATUS_CONST(SUCCESS));
    int more[] = {2, 1};
    assert(SORT_FUNC(Bozo)(more, 2, sizeof(int), cb, 42, 1000) ==
           STATUS_CONST(SUCCESS));
  }
  {
    int values[] = {6, 4, 2, 5, 3, 1};
    assert(SORT_FUNC(Stooge)(values, 6, sizeof(int), cb) ==
           STATUS_CONST(SUCCESS));
    for (size_t i = 1; i < 6; ++i)
      assert(values[i - 1] <= values[i]);
  }
  {
    int values[] = {3, 2, 1};
    assert(SORT_FUNC(Slow)(values, 3, sizeof(int), cb) ==
           STATUS_CONST(SUCCESS));
    assert(values[0] == 1 && values[1] == 2 && values[2] == 3);
  }
  {
    int values[] = {2, 1};
    assert(SORT_FUNC(Miracle)(values, 2, sizeof(int), cb, 5) ==
           STATUS_CONST(TIMEOUT));
    assert(values[0] == 2 && values[1] == 1);
  }
  return 0;
}

