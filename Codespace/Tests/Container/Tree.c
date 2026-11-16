#include "../../../Cosmeron/Modules/Container/Tree/Tree.h"
#include <assert.h>
#include <stdio.h>

/* Custom BTreeSet degree instantiation */
TEMPLATE_INSTANTIATE(Tree, BTreeSetDegree, Int32_Deg4, int32_t, 4U);

/* Internal test helpers (strictly namespaced) */
INTERNAL_DEF(
    TreeTest,
    COMPARISON(DescInt32, (int32_t left, int32_t right, void *context), {
      (void)context;
      return (left > right)
                 ? COMPARISON_CONST(LOWER)
                 : ((left < right) ? COMPARISON_CONST(HIGHER)
                                   : COMPARISON_CONST(EQUAL));
    }),
    PROCEDURE(SumSetKey, (const int32_t *key, void *context), {
      int64_t *sum = (int64_t *)context;
      if (key && sum)
        *sum += *key;
    }),
    PROCEDURE(CountOrder, (const int32_t *key, void *context), {
      int32_t *buf = (int32_t *)context;
      if (key && buf) {
        int32_t idx = buf[0];
        buf[idx + 1] = *key;
        buf[0] = idx + 1;
      }
    }),
    PROCEDURE(DoubleMapVal, (int32_t key, int32_t *value, void *context), {
      (void)key;
      (void)context;
      if (value)
        *value *= 2;
    }))

static void NS(TreeTest, BSTSetAndIter)(void) {
  MODULE_TYPED(Tree, BSTSet, Int32) set;
  assert(MODULE_FUNC(Tree, BSTSet, Int32, Create)(&set) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&set, IsEmpty));

  int32_t vals[] = {50, 20, 70, 10, 30, 60, 80};
  for (size_t i = 0U; i < 7U; i++) {
    assert(OBJECT_CALL(&set, Insert, vals[i]) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL(&set, Insert, 30) == STATUS_CONST(ALREADY_EXISTS));
  assert(OBJECT_CALL_0(&set, Size) == 7U);

  const int32_t *minKey = NULL;
  const int32_t *maxKey = NULL;
  assert(OBJECT_CALL(&set, Min, &minKey) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&set, Max, &maxKey) == STATUS_CONST(SUCCESS));
  assert(*minKey == 10 && *maxKey == 80);

  /* Optional Iterator: Begin -> End */
  MODULE_TYPED(Tree, BSTSetIter, Int32) it;
  MODULE_TYPED(Tree, BSTSetIter, Int32) end;
  assert(OBJECT_CALL(&set, Begin, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&set, End, &end) == STATUS_CONST(SUCCESS));

  int32_t expected[] = {10, 20, 30, 50, 60, 70, 80};
  size_t idx = 0U;
  while (!OBJECT_CALL(&it, Equals, &end)) {
    const int32_t *k = NULL;
    assert(OBJECT_CALL(&it, Next, &k) == STATUS_CONST(SUCCESS));
    assert(*k == expected[idx++]);
  }
  assert(idx == 7U);

  /* Reverse step from End() */
  const int32_t *lastKey = NULL;
  assert(OBJECT_CALL(&end, Prev, &lastKey) == STATUS_CONST(SUCCESS));
  assert(*lastKey == 80);

  /* Range query: LowerBound / UpperBound */
  MODULE_TYPED(Tree, BSTSetIter, Int32) lb;
  MODULE_TYPED(Tree, BSTSetIter, Int32) ub;
  assert(OBJECT_CALL(&set, LowerBound, 25, &lb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&set, UpperBound, 60, &ub) == STATUS_CONST(SUCCESS));
  int64_t rangeSum = 0;
  while (!OBJECT_CALL(&lb, Equals, &ub)) {
    const int32_t *k = NULL;
    assert(OBJECT_CALL(&lb, Next, &k) == STATUS_CONST(SUCCESS));
    rangeSum += *k;
  }
  assert(rangeSum == (30 + 50 + 60));

  /* EraseIter */
  assert(OBJECT_CALL(&set, FindIter, 30, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&set, EraseIter, &it) == STATUS_CONST(SUCCESS));
  const int32_t *afterErase = NULL;
  assert(OBJECT_CALL(&it, Get, &afterErase) == STATUS_CONST(SUCCESS));
  assert(*afterErase == 50);
  assert(!OBJECT_CALL(&set, Contains, 30));

  /* ForEach PreOrder & InOrder */
  int64_t total = 0;
  MODULE_CLOSURE(Tree, BSTSet, Int32, Visitor) visitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Tree, BSTSet, Int32, Visitor), Bind)(
          MODULE_IFUNC(TreeTest, SumSetKey), &total);
  assert(OBJECT_CALL(&set, ForEach, Tree_Traversal_IN_ORDER, visitor) ==
         STATUS_CONST(SUCCESS));
  assert(total == (10 + 20 + 50 + 60 + 70 + 80));

  OBJECT_CALL_0(&set, Destroy);
}

static void NS(TreeTest, AVLBalanceAndMap)(void) {
  MODULE_TYPED(Tree, AVLSet, Int32) avl;
  assert(MODULE_FUNC(Tree, AVLSet, Int32, Create)(&avl) ==
         STATUS_CONST(SUCCESS));

  for (int32_t i = 1; i <= 31; i++) {
    assert(OBJECT_CALL(&avl, Insert, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&avl, Size) == 31U);
  assert(OBJECT_CALL_0(&avl, Height) <= 6U);

  for (int32_t i = 1; i <= 15; i++) {
    assert(OBJECT_CALL(&avl, Remove, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&avl, Size) == 16U);
  assert(OBJECT_CALL_0(&avl, Height) <= 5U);
  OBJECT_CALL_0(&avl, Destroy);

  /* AVLMap with custom descending comparator */
  MODULE_TYPED(Tree, AVLMap, Int32_Int32) map;
  assert(MODULE_FUNC(Tree, AVLMap, Int32_Int32, CreateWithComparator)(
             &map, MODULE_IFUNC(TreeTest, DescInt32), NULL) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Insert, 1, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Insert, 2, 200) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Upsert, 2, 250) == STATUS_CONST(SUCCESS));

  int32_t *v = NULL;
  assert(OBJECT_CALL(&map, Find, 2, &v) == STATUS_CONST(SUCCESS));
  assert(*v == 250);

  MODULE_TYPED(Tree, AVLMapIter, Int32_Int32) mit;
  assert(OBJECT_CALL(&map, Begin, &mit) == STATUS_CONST(SUCCESS));
  const int32_t *firstKey = NULL;
  assert(OBJECT_CALL(&mit, Key, &firstKey) == STATUS_CONST(SUCCESS));
  assert(*firstKey == 2); /* Descending order */

  MODULE_CLOSURE(Tree, AVLMap, Int32_Int32, Visitor) mv =
      NS_JOIN(MODULE_CLOSURE_FUNC(Tree, AVLMap, Int32_Int32, Visitor), Bind)(
          MODULE_IFUNC(TreeTest, DoubleMapVal), NULL);
  assert(OBJECT_CALL(&map, ForEach, Tree_Traversal_IN_ORDER, mv) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Find, 1, &v) == STATUS_CONST(SUCCESS));
  assert(*v == 200);
  OBJECT_CALL_0(&map, Destroy);
}

static void NS(TreeTest, RedBlackSetAndMap)(void) {
  MODULE_TYPED(Tree, RBSet, Int32) rb;
  assert(MODULE_FUNC(Tree, RBSet, Int32, Create)(&rb) ==
         STATUS_CONST(SUCCESS));

  for (int32_t i = 1; i <= 31; i++) {
    assert(OBJECT_CALL(&rb, Insert, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&rb, Size) == 31U);
  assert(OBJECT_CALL_0(&rb, Height) <= 10U);

  MODULE_TYPED(Tree, RBSetIter, Int32) rit;
  assert(OBJECT_CALL(&rb, RBegin, &rit) == STATUS_CONST(SUCCESS));
  const int32_t *k = NULL;
  assert(OBJECT_CALL(&rit, Next, &k) == STATUS_CONST(SUCCESS));
  assert(*k == 31);
  assert(OBJECT_CALL(&rit, Next, &k) == STATUS_CONST(SUCCESS));
  assert(*k == 30);

  for (int32_t i = 1; i <= 31; i += 2) {
    assert(OBJECT_CALL(&rb, Remove, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&rb, Size) == 15U);
  OBJECT_CALL_0(&rb, Destroy);

  MODULE_TYPED(Tree, RBMap, Int32_Float64) rbm;
  assert(MODULE_FUNC(Tree, RBMap, Int32_Float64, Create)(&rbm) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&rbm, Insert, 10, 1.5) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&rbm, Upsert, 10, 2.5) == STATUS_CONST(SUCCESS));
  double *dv = NULL;
  assert(OBJECT_CALL(&rbm, Find, 10, &dv) == STATUS_CONST(SUCCESS));
  assert(*dv == 2.5);
  OBJECT_CALL_0(&rbm, Destroy);
}

static void NS(TreeTest, BTreeSetAndMap)(void) {
  MODULE_TYPED(Tree, BTreeSet, Int32_Deg4) bt;
  assert(MODULE_FUNC(Tree, BTreeSet, Int32_Deg4, Create)(&bt) ==
         STATUS_CONST(SUCCESS));

  for (int32_t i = 1; i <= 40; i++) {
    assert(OBJECT_CALL(&bt, Insert, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&bt, Size) == 40U);

  MODULE_TYPED(Tree, BTreeSetIter, Int32_Deg4) it;
  MODULE_TYPED(Tree, BTreeSetIter, Int32_Deg4) end;
  assert(OBJECT_CALL(&bt, Begin, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bt, End, &end) == STATUS_CONST(SUCCESS));
  int32_t expected = 1;
  while (!OBJECT_CALL(&it, Equals, &end)) {
    const int32_t *k = NULL;
    assert(OBJECT_CALL(&it, Next, &k) == STATUS_CONST(SUCCESS));
    assert(*k == expected++);
  }
  assert(expected == 41);

  /* EraseIter across BTree merges/splits */
  assert(OBJECT_CALL(&bt, LowerBound, 15, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bt, EraseIter, &it) == STATUS_CONST(SUCCESS));
  const int32_t *nextAfter = NULL;
  assert(OBJECT_CALL(&it, Get, &nextAfter) == STATUS_CONST(SUCCESS));
  assert(*nextAfter == 16);
  assert(!OBJECT_CALL(&bt, Contains, 15));

  for (int32_t i = 1; i <= 40; i++) {
    if (i != 15)
      assert(OBJECT_CALL(&bt, Remove, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL_0(&bt, IsEmpty));
  OBJECT_CALL_0(&bt, Destroy);

  /* BTreeMap */
  MODULE_TYPED(Tree, BTreeMap, Int32_Int32) btm;
  assert(MODULE_FUNC(Tree, BTreeMap, Int32_Int32, Create)(&btm) ==
         STATUS_CONST(SUCCESS));
  for (int32_t i = 1; i <= 20; i++) {
    assert(OBJECT_CALL(&btm, Insert, i, i * 10) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL(&btm, Upsert, 10, 999) == STATUS_CONST(SUCCESS));
  int32_t *found = NULL;
  assert(OBJECT_CALL(&btm, Find, 10, &found) == STATUS_CONST(SUCCESS));
  assert(*found == 999);
  OBJECT_CALL_0(&btm, Destroy);
}

int main(void) {
  NS(TreeTest, BSTSetAndIter)();
  NS(TreeTest, AVLBalanceAndMap)();
  NS(TreeTest, RedBlackSetAndMap)();
  NS(TreeTest, BTreeSetAndMap)();
  printf("Tree tests passed!\n");
  return 0;
}
