#define SORT_DISABLE_FUNCTION_TABLE

#include "../../Cosmeron/Modules/Sort/Sort.h"
#include <assert.h>

static CMPOUT compareInt(const void *a, const void *b, void *ctx) {
  (void)ctx;
  int x = *(const int *)a;
  int y = *(const int *)b;
  return x < y ? COMPARISON_CONST(LOWER)
               : (x > y ? COMPARISON_CONST(HIGHER) : COMPARISON_CONST(EQUAL));
}

int main(void) {
  int values[] = {5, 2, 8, 1, 9};
  TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(compareInt, NULL);
  assert(SORT_FUNC(Array)(values, 5, sizeof(int), cb) == STATUS_CONST(SUCCESS));
  for (size_t i = 1; i < 5; ++i) {
    assert(values[i - 1] <= values[i]);
  }
  return 0;
}

