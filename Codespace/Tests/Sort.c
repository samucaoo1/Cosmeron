/* Compile with: cc -std=c11 -Wall -Wextra -pedantic -I Codespace
 * Codespace/Tests/Sort.c -o sort-test && ./sort-test */
#include "Cosmeron/Modules/Sort/Sort.h"
#include <assert.h>
#include <stdint.h>
#include <limits.h>

typedef struct Person { int age; int index; } Person;
static CMPOUT comparePeople(const void *a, const void *b) {
  int x = ((const Person *)a)->age, y = ((const Person *)b)->age;
  return x < y ? COMPARISON_LOWER_CONST :
         x > y ? COMPARISON_HIGHER_CONST : COMPARISON_EQUAL_CONST;
}
static CMPOUT compareInt(const void *a, const void *b) {
  int x = *(const int *)a, y = *(const int *)b;
  return x < y ? COMPARISON_LOWER_CONST :
         x > y ? COMPARISON_HIGHER_CONST : COMPARISON_EQUAL_CONST;
}
static CMPOUT typedInt(const int *a, const int *b) {
  return compareInt(a, b);
}
static CMPOUT compareContext(const void *a, const void *b, void *ctx) {
  CMPOUT r = compareInt(a, b);
  return *(int *)ctx ? (CMPOUT)(-r) : r;
}
SORT_IMPLEMENT_ALL(int, IntTest, typedInt)

int main(void) {
  int values[] = { 7, 1, 4, 1, 9, 0, -3, 2, 2 };
  int direction = 1;
  assert(SORT_FUNC(Array)(values, 9, sizeof(int), compareInt) ==
         STATUS_CONST(SUCCESS));
  for (size_t i = 1; i < 9; ++i) assert(values[i - 1] <= values[i]);
  assert(SORT_FUNC(ArrayWithContext)(values, 9, sizeof(int), compareContext,
                                     &direction) == STATUS_CONST(SUCCESS));
  for (size_t i = 1; i < 9; ++i) assert(values[i - 1] >= values[i]);
  assert(SORT_FUNC(Heap)(values, 9, sizeof(int), compareInt) ==
         STATUS_CONST(SUCCESS));
  assert(SORT_FUNC(Insertion)(values, 9, sizeof(int), compareInt) ==
         STATUS_CONST(SUCCESS));

  Person people[] = {{23,0}, {19,1}, {23,2}, {19,3}, {23,4}};
  unsigned char scratch[sizeof people];
  assert(SORT_FUNC(StableWithBuffer)(people, 5, sizeof(Person), comparePeople,
               scratch, sizeof scratch - 1) == STATUS_CONST(INSUFFICIENT_SPACE));
  assert(people[0].index == 0);
  assert(SORT_FUNC(StableWithBuffer)(people, 5, sizeof(Person), comparePeople,
               scratch, sizeof scratch) == STATUS_CONST(SUCCESS));
  assert(people[0].index == 1 && people[1].index == 3);
  assert(people[2].index == 0 && people[3].index == 2 &&
         people[4].index == 4);
  bool isSorted = false;
  assert(SORT_FUNC(IsSorted)(people, 5, sizeof(Person), comparePeople,
                             &isSorted) == STATUS_CONST(SUCCESS) && isSorted);

  int typed[] = {5, -10, 5, 0, 11};
  assert(SORT_TYPED_FUNC(Stable, IntTest)(typed, 5) == STATUS_CONST(SUCCESS));
  assert(typed[0] == -10 && typed[4] == 11);
  /* All six typed wrappers must compile and sort correctly, independently. */
  typedef OPSTATUS (*TypedSort)(int *, size_t);
  TypedSort typedSorts[] = {
    SORT_TYPED_FUNC(Array, IntTest),
    SORT_TYPED_FUNC(Insertion, IntTest),
    SORT_TYPED_FUNC(Heap, IntTest),
    SORT_TYPED_FUNC(Stable, IntTest),
    SORT_TYPED_FUNC(Intro, IntTest),
    SORT_TYPED_FUNC(Merge, IntTest)
  };
  for (size_t variant = 0; variant < sizeof typedSorts / sizeof typedSorts[0];
       ++variant) {
    int sample[] = {5, -10, 5, 0, 11};
    assert(typedSorts[variant](sample, 5) == STATUS_CONST(SUCCESS));
    for (size_t i = 1; i < 5; ++i) assert(sample[i - 1] <= sample[i]);
  }
  assert(SORT_FUNC(Reverse)(typed, 5, sizeof(int)) == STATUS_CONST(SUCCESS));
  assert(typed[0] == 11 && typed[4] == -10);

  uint32_t u32[] = { UINT32_MAX, 0u, 7u, 7u, 1u };
  assert(SORT_TYPED_FUNC(Radix, U32)(u32, 5, SORT_CONST(ASCENDING)) ==
         STATUS_CONST(SUCCESS));
  assert(u32[0] == 0 && u32[4] == UINT32_MAX);
  int32_t i32[] = { INT32_MAX, INT32_MIN, -1, 0, 1 };
  int32_t i32buffer[5];
  assert(SORT_TYPED_FUNC(RadixWithBuffer, I32)(
      i32, 5, SORT_CONST(DESCENDING), i32buffer, 4) ==
      STATUS_CONST(INSUFFICIENT_SPACE));
  assert(SORT_TYPED_FUNC(RadixWithBuffer, I32)(
      i32, 5, SORT_CONST(DESCENDING), i32buffer, 5) ==
      STATUS_CONST(SUCCESS));
  assert(i32[0] == INT32_MAX && i32[4] == INT32_MIN);
  int64_t i64[] = { INT64_MIN, 3, INT64_MAX, -9 };
  uint64_t u64[] = { UINT64_MAX, 1, 0, 42 };
  assert(SORT_TYPED_FUNC(Radix, I64)(i64, 4, SORT_CONST(ASCENDING)) ==
         STATUS_CONST(SUCCESS));
  assert(SORT_TYPED_FUNC(Radix, U64)(u64, 4, SORT_CONST(ASCENDING)) ==
         STATUS_CONST(SUCCESS));
  assert(i64[0] == INT64_MIN && i64[3] == INT64_MAX);
  assert(u64[0] == 0 && u64[3] == UINT64_MAX);
  assert(SORT_FUNC(Array)(NULL, 0, sizeof(int), compareInt) ==
         STATUS_CONST(SUCCESS));
  assert(SORT_FUNC(Array)(NULL, SIZE_MAX, sizeof(int), compareInt) ==
         STATUS_CONST(ARITHMETIC_OVERFLOW));
  return 0;
}
