#include "../../../Cosmeron/Modules/Container/Hash/Hash.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Custom string-keyed HashMap instantiation */
TEMPLATE_INSTANTIATE(Hash, HashMap, Str_Int32, const char *, int32_t);

INTERNAL_DEF(
    HashTest,
    FUNCTION(size_t, StrHash, (const char *key, void *context), {
      (void)context;
      if (!key)
        return 0U;
      return MODULE_IFUNC(Hash, HashBytes)(key, strlen(key));
    }),
    PREDICATE(StrEquals, (const char *left, const char *right, void *context), {
      (void)context;
      if (left == right)
        return true;
      if (!left || !right)
        return false;
      return strcmp(left, right) == 0;
    }),
    PROCEDURE(SumSet, (const int32_t *key, void *context), {
      int64_t *sum = (int64_t *)context;
      if (key && sum)
        *sum += *key;
    }),
    PROCEDURE(AddOneMap, (int32_t key, int32_t *value, void *context), {
      (void)key;
      (void)context;
      if (value)
        (*value)++;
    }))

static void NS(HashTest, SetAndIter)(void) {
  MODULE_TYPED(Hash, HashSet, Int32) set;
  assert(MODULE_FUNC(Hash, HashSet, Int32, Create)(&set) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&set, IsEmpty));

  for (int32_t i = 1; i <= 50; i++) {
    assert(OBJECT_CALL(&set, Insert, i) == STATUS_CONST(SUCCESS));
  }
  assert(OBJECT_CALL(&set, Insert, 25) == STATUS_CONST(ALREADY_EXISTS));
  assert(OBJECT_CALL_0(&set, Size) == 50U);
  assert(OBJECT_CALL_0(&set, Capacity) >= 64U);

  /* Remove without iterator */
  assert(OBJECT_CALL(&set, Remove, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&set, Remove, 10) == STATUS_CONST(NOT_FOUND));
  assert(!OBJECT_CALL(&set, Contains, 10));
  assert(OBJECT_CALL_0(&set, Size) == 49U);

  /* Optional Iterator: Begin -> End & EraseIter */
  MODULE_TYPED(Hash, HashSetIter, Int32) it;
  MODULE_TYPED(Hash, HashSetIter, Int32) end;
  assert(OBJECT_CALL(&set, FindIter, 20, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&it, IsValid));
  assert(OBJECT_CALL(&set, EraseIter, &it) == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL(&set, Contains, 20));
  assert(OBJECT_CALL_0(&set, Size) == 48U);

  assert(OBJECT_CALL(&set, Begin, &it) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&set, End, &end) == STATUS_CONST(SUCCESS));
  int64_t iterSum = 0;
  size_t count = 0U;
  while (!OBJECT_CALL(&it, Equals, &end)) {
    const int32_t *k = NULL;
    assert(OBJECT_CALL(&it, Next, &k) == STATUS_CONST(SUCCESS));
    iterSum += *k;
    count++;
  }
  assert(count == 48U);

  int64_t forEachSum = 0;
  MODULE_CLOSURE(Hash, HashSet, Int32, Visitor) visitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Hash, HashSet, Int32, Visitor), Bind)(
          MODULE_IFUNC(HashTest, SumSet), &forEachSum);
  assert(OBJECT_CALL(&set, ForEach, visitor) == STATUS_CONST(SUCCESS));
  assert(forEachSum == iterSum);

  OBJECT_CALL_0(&set, Destroy);
}

static void NS(HashTest, MapAndCustomHash)(void) {
  MODULE_TYPED(Hash, HashMap, Int32_Int32) map;
  assert(MODULE_FUNC(Hash, HashMap, Int32_Int32, Create)(&map) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Reserve, 32U) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&map, Capacity) >= 32U);

  assert(OBJECT_CALL(&map, Insert, 1, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Insert, 1, 99) == STATUS_CONST(ALREADY_EXISTS));
  assert(OBJECT_CALL(&map, Upsert, 1, 20) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&map, Upsert, 2, 30) == STATUS_CONST(SUCCESS));

  int32_t *val = NULL;
  assert(OBJECT_CALL(&map, Find, 1, &val) == STATUS_CONST(SUCCESS));
  assert(*val == 20);

  MODULE_CLOSURE(Hash, HashMap, Int32_Int32, Visitor) mv =
      NS_JOIN(MODULE_CLOSURE_FUNC(Hash, HashMap, Int32_Int32, Visitor), Bind)(
          MODULE_IFUNC(HashTest, AddOneMap), NULL);
  assert(OBJECT_CALL(&map, ForEach, mv) == STATUS_CONST(SUCCESS));
  assert(*val == 21);

  /* Mutate value via iterator */
  MODULE_TYPED(Hash, HashMapIter, Int32_Int32) mit;
  assert(OBJECT_CALL(&map, FindIter, 2, &mit) == STATUS_CONST(SUCCESS));
  const int32_t *k = NULL;
  int32_t *mvPtr = NULL;
  assert(OBJECT_CALL(&mit, Key, &k) == STATUS_CONST(SUCCESS));
  assert(*k == 2);
  assert(OBJECT_CALL(&mit, Get, &mvPtr) == STATUS_CONST(SUCCESS));
  assert(*mvPtr == 31);
  *mvPtr = 300;
  assert(OBJECT_CALL(&map, Find, 2, &val) == STATUS_CONST(SUCCESS));
  assert(*val == 300);

  OBJECT_CALL_0(&map, Destroy);

  /* String-keyed HashMap using CreateWithHash */
  MODULE_TYPED(Hash, HashMap, Str_Int32) smap;
  assert(MODULE_FUNC(Hash, HashMap, Str_Int32, CreateWithHash)(
             &smap, MODULE_IFUNC(HashTest, StrHash),
             MODULE_IFUNC(HashTest, StrEquals), NULL) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&smap, Insert, "alpha", 1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&smap, Insert, "beta", 2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&smap, Upsert, "beta", 22) == STATUS_CONST(SUCCESS));

  int32_t *sval = NULL;
  assert(OBJECT_CALL(&smap, Find, "beta", &sval) == STATUS_CONST(SUCCESS));
  assert(*sval == 22);
  assert(OBJECT_CALL(&smap, Remove, "alpha") == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL(&smap, Contains, "alpha"));

  OBJECT_CALL_0(&smap, Destroy);
}

int main(void) {
  NS(HashTest, SetAndIter)();
  NS(HashTest, MapAndCustomHash)();
  printf("Hash tests passed!\n");
  return 0;
}

