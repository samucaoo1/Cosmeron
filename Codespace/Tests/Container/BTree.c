#include "Test.h"
#include "../../Cosmeron/Modules/Container/Tree/BTree.h"

static inline CMPOUT Test_Compare_Long(long left, long right) {
  if (left < right)
    return COMPARISON_LOWER_CONST;
  if (left > right)
    return COMPARISON_HIGHER_CONST;
  return COMPARISON_EQUAL_CONST;
}

BTREE_SET_IMPLEMENT_ALL_CMP(custom_long, long, Test_Compare_Long)
BTREE_MAP_IMPLEMENT_ALL_CMP(custom_long, int, long, int, Test_Compare_Long)

/* Universal template instantiation with custom type */
static inline CMPOUT Algorithm_Comparison_short(short left, short right) {
  return left < right ? COMPARISON_LOWER_CONST
                      : (left > right ? COMPARISON_HIGHER_CONST
                                      : COMPARISON_EQUAL_CONST);
}
TEMPLATE_IMPLEMENT(TBTREE_SET, short, short);

/* Custom degree B-Trees (Degree 2 = 2-3-4 tree) */
BTREE_SET_IMPLEMENT_ALL_DEGREE_CMP(deg2_int, int, 2, COMPARISON_FUNC(int))

static int Test_BTree_Set_Basic(void) {
  TBTree_Set_int set;
  int *found = NULL;
  int *minimum = NULL;
  int *maximum = NULL;

  TEST_ASSERT(TBTree_Set_int_Init(&set) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Create) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Empty));
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 0U);
  TEST_ASSERT(OBJECT_CALL_0(&set, Height) == 0U);

  /* Insert 3 elements: 20, 10, 30 */
  TEST_ASSERT(OBJECT_CALL(&set, Insert, 20) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL(&set, Insert, 10) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL(&set, Insert, 30) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 3U);
  TEST_ASSERT(!OBJECT_CALL_0(&set, Empty));
  TEST_ASSERT(OBJECT_CALL_0(&set, Height) == 1U);

  /* Duplicate key */
  TEST_ASSERT(OBJECT_CALL(&set, Insert, 20) == STATUS_CONST(ALREADY_EXISTS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 3U);

  /* Contains and Find */
  TEST_ASSERT(OBJECT_CALL(&set, Contains, 10));
  TEST_ASSERT(OBJECT_CALL(&set, Contains, 20));
  TEST_ASSERT(OBJECT_CALL(&set, Contains, 30));
  TEST_ASSERT(!OBJECT_CALL(&set, Contains, 99));

  TEST_ASSERT(OBJECT_CALL(&set, Find, 20, &found) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(found != NULL && *found == 20);
  TEST_ASSERT(OBJECT_CALL(&set, Find, 99, &found) == STATUS_CONST(NOT_FOUND));

  /* Min and Max */
  TEST_ASSERT(OBJECT_CALL(&set, Min, &minimum) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(minimum != NULL && *minimum == 10);
  TEST_ASSERT(OBJECT_CALL(&set, Max, &maximum) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(maximum != NULL && *maximum == 30);

  /* Remove 20 */
  TEST_ASSERT(OBJECT_CALL(&set, Remove, 20) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 2U);
  TEST_ASSERT(!OBJECT_CALL(&set, Contains, 20));
  TEST_ASSERT(OBJECT_CALL(&set, Remove, 20) == STATUS_CONST(NOT_FOUND));

  /* Clear */
  TEST_ASSERT(OBJECT_CALL_0(&set, Clear) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Empty));
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 0U);

  OBJECT_CALL_0(&set, Destroy);
  return 0;
}

static int Test_BTree_Set_MultiLevel(void) {
  /* Test with Degree 2 (2-3-4 tree: max 3 keys per node, 4 children) */
  TBTree_Set_deg2_int tree;
  TEST_ASSERT(TBTree_Set_deg2_int_Init(&tree) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&tree, Create) == STATUS_CONST(SUCCESS));

  /* Insert 1..20 */
  for (int i = 1; i <= 20; i++) {
    TEST_ASSERT(OBJECT_CALL(&tree, Insert, i) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(OBJECT_CALL_0(&tree, Size) == 20U);
  TEST_ASSERT(OBJECT_CALL_0(&tree, Height) >= 3U);

  for (int i = 1; i <= 20; i++) {
    TEST_ASSERT(OBJECT_CALL(&tree, Contains, i));
  }

  /* Forward iteration using Cursor */
  {
    BTree_Set_Cursor_deg2_int cur = OBJECT_CALL_0(&tree, First);
    int expected = 1;
    while (OBJECT_CALL(&tree, CursorValid, &cur)) {
      int *k = OBJECT_CALL(&tree, CursorKey, &cur);
      TEST_ASSERT(k != NULL && *k == expected);
      expected++;
      OBJECT_CALL(&tree, CursorNext, &cur);
    }
    TEST_ASSERT(expected == 21);
  }

  /* Backward iteration using Cursor */
  {
    BTree_Set_Cursor_deg2_int cur = OBJECT_CALL_0(&tree, Last);
    int expected = 20;
    while (OBJECT_CALL(&tree, CursorValid, &cur)) {
      int *k = OBJECT_CALL(&tree, CursorKey, &cur);
      TEST_ASSERT(k != NULL && *k == expected);
      expected--;
      OBJECT_CALL(&tree, CursorPrev, &cur);
    }
    TEST_ASSERT(expected == 0);
  }

  /* Remove odd numbers */
  for (int i = 1; i <= 20; i += 2) {
    TEST_ASSERT(OBJECT_CALL(&tree, Remove, i) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(!OBJECT_CALL(&tree, Contains, i));
  }
  TEST_ASSERT(OBJECT_CALL_0(&tree, Size) == 10U);

  /* Verify even numbers remaining */
  for (int i = 2; i <= 20; i += 2) {
    TEST_ASSERT(OBJECT_CALL(&tree, Contains, i));
  }

  /* Remove remaining even numbers */
  for (int i = 2; i <= 20; i += 2) {
    TEST_ASSERT(OBJECT_CALL(&tree, Remove, i) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(OBJECT_CALL_0(&tree, Empty));
  TEST_ASSERT(OBJECT_CALL_0(&tree, Size) == 0U);

  OBJECT_CALL_0(&tree, Destroy);
  return 0;
}

static int Test_BTree_Map_Basic(void) {
  TBTree_Map_int_int map;
  int *found = NULL;

  TEST_ASSERT(TBTree_Map_int_int_Init(&map) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&map, Create) == STATUS_CONST(SUCCESS));

  for (int i = 1; i <= 15; i++) {
    TEST_ASSERT(OBJECT_CALL(&map, Insert, i, i * 10) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(OBJECT_CALL_0(&map, Size) == 15U);

  for (int i = 1; i <= 15; i++) {
    TEST_ASSERT(OBJECT_CALL(&map, Find, i, &found) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(found != NULL && *found == i * 10);
  }

  /* Duplicate key updates value and returns ALREADY_EXISTS */
  TEST_ASSERT(OBJECT_CALL(&map, Insert, 5, 999) == STATUS_CONST(ALREADY_EXISTS));
  TEST_ASSERT(OBJECT_CALL(&map, Find, 5, &found) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(found != NULL && *found == 999);

  /* Restore */
  TEST_ASSERT(OBJECT_CALL(&map, Insert, 5, 50) == STATUS_CONST(ALREADY_EXISTS));

  /* Map Cursor */
  {
    BTree_Map_Cursor_int_int cur = OBJECT_CALL_0(&map, First);
    int expected = 1;
    while (OBJECT_CALL(&map, CursorValid, &cur)) {
      int *k = OBJECT_CALL(&map, CursorKey, &cur);
      int *v = OBJECT_CALL(&map, CursorValue, &cur);
      TEST_ASSERT(k != NULL && *k == expected);
      TEST_ASSERT(v != NULL && *v == expected * 10);
      expected++;
      OBJECT_CALL(&map, CursorNext, &cur);
    }
    TEST_ASSERT(expected == 16);
  }

  for (int i = 1; i <= 15; i++) {
    TEST_ASSERT(OBJECT_CALL(&map, Remove, i) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(OBJECT_CALL_0(&map, Empty));

  OBJECT_CALL_0(&map, Destroy);
  return 0;
}

static int Test_BTree_FunctionTable(void) {
  TBTree_Set_int set;
  int *found = NULL;
  int *minVal = NULL;
  int *maxVal = NULL;

  TEST_ASSERT(TBTree_Set_int_Init(&set) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(set.api != NULL);
  TEST_ASSERT(set.api->Create(&set) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(set.api->Empty(&set));

  for (int i = 10; i <= 50; i += 10) {
    TEST_ASSERT(set.api->Insert(&set, i) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(set.api->Size(&set) == 5U);
  TEST_ASSERT(set.api->Height(&set) >= 1U);

  TEST_ASSERT(set.api->Contains(&set, 30));
  TEST_ASSERT(!set.api->Contains(&set, 99));

  TEST_ASSERT(set.api->Find(&set, 30, &found) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(found != NULL && *found == 30);

  TEST_ASSERT(set.api->Min(&set, &minVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(minVal != NULL && *minVal == 10);

  TEST_ASSERT(set.api->Max(&set, &maxVal) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(maxVal != NULL && *maxVal == 50);

  /* FunctionTable Cursor */
  {
    BTree_Set_Cursor_int cur = set.api->First(&set);
    int expected = 10;
    while (set.api->CursorValid(&set, &cur)) {
      int *k = set.api->CursorKey(&set, &cur);
      TEST_ASSERT(k != NULL && *k == expected);
      expected += 10;
      set.api->CursorNext(&set, &cur);
    }
    TEST_ASSERT(expected == 60);
  }

  TEST_ASSERT(set.api->Remove(&set, 30) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(set.api->Size(&set) == 4U);

  set.api->Destroy(&set);
  TEST_ASSERT(set.api->Size(&set) == 0U);
  return 0;
}

static int Test_BTree_CustomComparator(void) {
  TBTree_Set_custom_long set;
  long *found = NULL;

  TEST_ASSERT(TBTree_Set_custom_long_Init(&set) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Create) == STATUS_CONST(SUCCESS));

  TEST_ASSERT(OBJECT_CALL(&set, Insert, 7000000000L) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL(&set, Insert, 3000000000L) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL(&set, Insert, 9000000000L) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 3U);

  TEST_ASSERT(OBJECT_CALL(&set, Find, 7000000000L, &found) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(found != NULL && *found == 7000000000L);

  OBJECT_CALL_0(&set, Destroy);

  /* Verify TEMPLATE_IMPLEMENT instantiated TBTree_Set_short */
  {
    TBTree_Set_short sset;
    TEST_ASSERT(TBTree_Set_short_Init(&sset) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(OBJECT_CALL_0(&sset, Create) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(OBJECT_CALL(&sset, Insert, (short)42) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(OBJECT_CALL(&sset, Contains, (short)42));
    TEST_ASSERT(OBJECT_CALL_0(&sset, Size) == 1U);
    OBJECT_CALL_0(&sset, Destroy);
  }

  /* Verify custom comparator Map */
  {
    TBTree_Map_custom_long_int map;
    int *v = NULL;
    TEST_ASSERT(TBTree_Map_custom_long_int_Init(&map) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(OBJECT_CALL_0(&map, Create) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(OBJECT_CALL(&map, Insert, 7000000000L, 77) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(OBJECT_CALL(&map, Find, 7000000000L, &v) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(v != NULL && *v == 77);
    OBJECT_CALL_0(&map, Destroy);
  }
  return 0;
}

static int Test_BTree_Stress(void) {
  TBTree_Set_int set;
  int n = 500;

  TEST_ASSERT(TBTree_Set_int_Init(&set) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(OBJECT_CALL_0(&set, Create) == STATUS_CONST(SUCCESS));

  unsigned int val = 137U;
  for (int i = 0; i < n; i++) {
    val = (val * 1103515245U + 12345U) & 0x7FFFFFFFU;
    int k = (int)(val % 5000U);
    OPSTATUS s = OBJECT_CALL(&set, Insert, k);
    TEST_ASSERT(s == STATUS_CONST(SUCCESS) || s == STATUS_CONST(ALREADY_EXISTS));
  }
  TEST_ASSERT(!OBJECT_CALL_0(&set, Empty));
  size_t count = OBJECT_CALL_0(&set, Size);

  /* Verify in-order traversal strictly increasing */
  {
    BTree_Set_Cursor_int cur = OBJECT_CALL_0(&set, First);
    int prev = -1;
    size_t traversed = 0;
    while (OBJECT_CALL(&set, CursorValid, &cur)) {
      int *k = OBJECT_CALL(&set, CursorKey, &cur);
      TEST_ASSERT(k != NULL);
      if (prev != -1) {
        TEST_ASSERT(*k > prev);
      }
      prev = *k;
      traversed++;
      OBJECT_CALL(&set, CursorNext, &cur);
    }
    TEST_ASSERT(traversed == count);
  }

  /* Remove all elements one by one using First */
  while (!OBJECT_CALL_0(&set, Empty)) {
    BTree_Set_Cursor_int cur = OBJECT_CALL_0(&set, First);
    TEST_ASSERT(OBJECT_CALL(&set, CursorValid, &cur));
    int *k = OBJECT_CALL(&set, CursorKey, &cur);
    int keyToRemove = *k;
    TEST_ASSERT(OBJECT_CALL(&set, Remove, keyToRemove) == STATUS_CONST(SUCCESS));
  }
  TEST_ASSERT(OBJECT_CALL_0(&set, Size) == 0U);
  TEST_ASSERT(OBJECT_CALL_0(&set, Empty));

  OBJECT_CALL_0(&set, Destroy);
  return 0;
}

int main(void) {
  if (Test_BTree_Set_Basic() != 0) return 1;
  if (Test_BTree_Set_MultiLevel() != 0) return 1;
  if (Test_BTree_Map_Basic() != 0) return 1;
  if (Test_BTree_FunctionTable() != 0) return 1;
  if (Test_BTree_CustomComparator() != 0) return 1;
  if (Test_BTree_Stress() != 0) return 1;
  return 0;
}
