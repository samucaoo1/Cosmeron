/* Compile: cc -std=c11 -Wall -Wextra -Wpedantic -Werror -I Codespace
 * Codespace/Tests/SortCatalog.c -o /tmp/sort-catalog && /tmp/sort-catalog */
#include "Cosmeron/Modules/Sort/Sort.h"
#include <assert.h>
#include <string.h>

static CMPOUT cmpInt(const void *a, const void *b) {
  int x = *(const int *)a, y = *(const int *)b;
  return x < y ? COMPARISON_LOWER_CONST :
         x > y ? COMPARISON_HIGHER_CONST : COMPARISON_EQUAL_CONST;
}
static CMPOUT cmpCtx(const void *a, const void *b, void *ctx) {
  (void)ctx;
  return cmpInt(a, b);
}
typedef OPSTATUS (*SortFunc)(void *, size_t, size_t, COMPARISON_TYPE(Comparator));
static void checkAlgorithm(SortFunc fn) {
  const int original[] = {9, 2, -3, 9, 0, 4, 1, -3, 7};
  int values[9];
  memcpy(values, original, sizeof values);
  assert(fn(values, 9, sizeof(int), cmpInt) == STATUS_CONST(SUCCESS));
  for (size_t i = 1; i < 9; ++i) assert(values[i - 1] <= values[i]);
  assert(values[0] == -3 && values[8] == 9);
  assert(fn(NULL, 0, sizeof(int), cmpInt) == STATUS_CONST(SUCCESS));
  assert(fn(NULL, 2, sizeof(int), cmpInt) == STATUS_CONST(INVALID_ARGUMENT));
  assert(fn(values, 9, 0, cmpInt) == STATUS_CONST(INVALID_ARGUMENT));
}
int main(void) {
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
    assert(SORT_FUNC(Stalin)(values, 8, sizeof(int), cmpInt, &count) ==
           STATUS_CONST(SUCCESS));
    assert(count == 4);
    assert(values[0] == 1 && values[1] == 4 &&
           values[2] == 8 && values[3] == 9);
  }
  {
    int values[] = {2, 1};
    assert(SORT_FUNC(Bogo)(values, 2, sizeof(int), cmpInt, 42, 0) ==
           STATUS_CONST(TIMEOUT));
    assert(SORT_FUNC(BogoWithContext)(values, 2, sizeof(int),
              cmpCtx, NULL, 42, 1000) == STATUS_CONST(SUCCESS));
    int more[] = {2, 1};
    assert(SORT_FUNC(Bozo)(more, 2, sizeof(int), cmpInt, 42, 1000) ==
           STATUS_CONST(SUCCESS));
  }
  {
    int values[] = {6, 4, 2, 5, 3, 1};
    assert(SORT_FUNC(Stooge)(values, 6, sizeof(int), cmpInt) ==
           STATUS_CONST(SUCCESS));
    for (size_t i = 1; i < 6; ++i) assert(values[i-1] <= values[i]);
  }
  {
    int values[] = {3, 2, 1};
    assert(SORT_FUNC(Slow)(values, 3, sizeof(int), cmpInt) ==
           STATUS_CONST(SUCCESS));
    assert(values[0] == 1 && values[1] == 2 && values[2] == 3);
  }
  {
    int values[] = {2, 1};
    assert(SORT_FUNC(Miracle)(values, 2, sizeof(int), cmpInt, 5) ==
           STATUS_CONST(TIMEOUT));
    assert(values[0] == 2 && values[1] == 1);
  }
  return 0;
}
