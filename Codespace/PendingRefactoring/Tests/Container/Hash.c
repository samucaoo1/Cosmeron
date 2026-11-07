#include "Test.h"

#include "../../Cosmeron/Modules/Container/Hash/Hash.h"

typedef struct TRecord {
  int value;
} TRecord;

HASH_MAP_IMPLEMENT_ALL(int, recordKey, TRecord, record, HASH_INT, HASH_EQUAL_INT)

int main(void) {
  THASH_TYPE(int, int) hash = {0};
  THASH_TYPE(cstring, int) strings = {0};
  THASH_TYPE(recordKey, record) records = {0};
  size_t initialCapacity;
  int i;
  int *value;
  const int *constValue;

  CONTAINER_API_BIND(hash, HASH_OPERATION(int, int, functions));
  CONTAINER_API_BIND(strings, HASH_OPERATION(cstring, int, functions));
  CONTAINER_API_BIND(records, HASH_OPERATION(recordKey, record, functions));

  TEST_ASSERT(HASH_OPERATION(int, int, Init)(&hash) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(HASH_OPERATION(int, int, IsEmpty)(&hash));
  value = NULL;
  TEST_ASSERT(HASH_OPERATION(int, int, Find)(&hash, 1, &value) ==
              STATUS_CONST(NOT_FOUND));
  TEST_ASSERT(value == NULL);
  initialCapacity = HASH_OPERATION(int, int, Capacity)(&hash);
  TEST_ASSERT(initialCapacity > 0u);

  for (i = 0; i < 100; ++i) {
    TEST_ASSERT(HASH_OPERATION(int, int, Insert)(&hash, i, i * 3) ==
                STATUS_CONST(SUCCESS));
  }

  TEST_ASSERT(HASH_OPERATION(int, int, Size)(&hash) == 100u);
  TEST_ASSERT(HASH_OPERATION(int, int, Capacity)(&hash) > initialCapacity);
  TEST_ASSERT(HASH_OPERATION(int, int, Contains)(&hash, 42));

  value = NULL;
  TEST_ASSERT(HASH_OPERATION(int, int, Find)(&hash, 42, &value) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(value && *value == 126);

  TEST_ASSERT(HASH_OPERATION(int, int, Insert)(&hash, 42, 999) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(HASH_OPERATION(int, int, Size)(&hash) == 100u);
  value = NULL;
  TEST_ASSERT(HASH_OPERATION(int, int, Find)(&hash, 42, &value) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(value && *value == 999);

  constValue = NULL;
  TEST_ASSERT(HASH_OPERATION(int, int, ConstFind)(&hash, 10, &constValue) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(constValue && *constValue == 30);

  TEST_ASSERT(HASH_OPERATION(int, int, Remove)(&hash, 42) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(!HASH_OPERATION(int, int, Contains)(&hash, 42));
  TEST_ASSERT(HASH_OPERATION(int, int, Remove)(&hash, 42) ==
              STATUS_CONST(NOT_FOUND));

  TEST_ASSERT(HASH_OPERATION(int, int, Insert)(&hash, 142, 7) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(HASH_OPERATION(int, int, Contains)(&hash, 142));

  HASH_OPERATION(int, int, Clear)(&hash);
  TEST_ASSERT(HASH_OPERATION(int, int, IsEmpty)(&hash));
  TEST_ASSERT(HASH_OPERATION(int, int, Capacity)(&hash) > 0u);

#if CONTAINER_FUNCTION_TABLE_ENABLED
  TEST_ASSERT(hash.api != NULL);
  TEST_ASSERT(hash.api->insert(&hash, 1, 2) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(hash.api->contains(&hash, 1));
#endif

  TEST_ASSERT(HASH_OPERATION(cstring, int, Init)(&strings) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(HASH_OPERATION(cstring, int, Insert)(&strings, "alpha", 11) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(HASH_OPERATION(cstring, int, Insert)(&strings, "beta", 22) ==
              STATUS_CONST(SUCCESS));
  value = NULL;
  TEST_ASSERT(HASH_OPERATION(cstring, int, Find)(&strings, "alpha", &value) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(*value == 11);

  TEST_ASSERT(HASH_OPERATION(recordKey, record, Init)(&records) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(HASH_OPERATION(recordKey, record, Insert)(
                  &records, 7, (TRecord){.value = 70}) == STATUS_CONST(SUCCESS));
  {
    TRecord *recordValue = NULL;
    TEST_ASSERT(HASH_OPERATION(recordKey, record, Find)(
                    &records, 7, &recordValue) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(recordValue->value == 70);
  }

  HASH_OPERATION(recordKey, record, Destroy)(&records);
  HASH_OPERATION(cstring, int, Destroy)(&strings);
  HASH_OPERATION(int, int, Destroy)(&hash);

  value = NULL;
  TEST_ASSERT(HASH_OPERATION(int, int, Find)(NULL, 1, &value) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(HASH_OPERATION(int, int, Find)(&hash, 1, NULL) ==
              STATUS_CONST(INVALID_ARGUMENT));

  TEST_ASSERT(hash.buckets == NULL);
  TEST_ASSERT(hash.capacity == 0u);
  TEST_ASSERT(hash.size == 0u);

  /* ============================================================
   * Hash Set Tests
   * ============================================================ */
  {
    THashSet_int set = {0};
    THashSet_cstring strSet = {0};
    size_t setInitialCapacity;
    int j;

    /* Verify no value overhead: set bucket is smaller than map bucket */
    TEST_ASSERT(sizeof(THASH_SET_BUCKET_TYPE(int)) <= sizeof(THASH_BUCKET_TYPE(int, int)));

    CONTAINER_API_BIND(set, HASH_SET_OPERATION(int, functions));
    CONTAINER_API_BIND(strSet, HASH_SET_OPERATION(cstring, functions));

    /* Init & Observers */
    TEST_ASSERT(HASH_SET_OPERATION(int, Init)(&set) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(int, IsEmpty)(&set));
    TEST_ASSERT(HASH_SET_OPERATION(int, Size)(&set) == 0u);
    setInitialCapacity = HASH_SET_OPERATION(int, Capacity)(&set);
    TEST_ASSERT(setInitialCapacity > 0u);
    TEST_ASSERT(!HASH_SET_OPERATION(int, Contains)(&set, 42));

    /* Insert elements & capacity growth */
    for (j = 0; j < 150; ++j) {
      TEST_ASSERT(HASH_SET_OPERATION(int, Insert)(&set, j) == STATUS_CONST(SUCCESS));
    }
    TEST_ASSERT(HASH_SET_OPERATION(int, Size)(&set) == 150u);
    TEST_ASSERT(!HASH_SET_OPERATION(int, IsEmpty)(&set));
    TEST_ASSERT(HASH_SET_OPERATION(int, Capacity)(&set) > setInitialCapacity);

    /* Duplicate insert does not increase size */
    TEST_ASSERT(HASH_SET_OPERATION(int, Insert)(&set, 42) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(int, Size)(&set) == 150u);

    /* Contains checks */
    for (j = 0; j < 150; ++j) {
      TEST_ASSERT(HASH_SET_OPERATION(int, Contains)(&set, j));
    }
    TEST_ASSERT(!HASH_SET_OPERATION(int, Contains)(&set, 999));

    /* Remove & re-insert */
    TEST_ASSERT(HASH_SET_OPERATION(int, Remove)(&set, 42) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(!HASH_SET_OPERATION(int, Contains)(&set, 42));
    TEST_ASSERT(HASH_SET_OPERATION(int, Size)(&set) == 149u);
    TEST_ASSERT(HASH_SET_OPERATION(int, Remove)(&set, 42) == STATUS_CONST(NOT_FOUND));

    TEST_ASSERT(HASH_SET_OPERATION(int, Insert)(&set, 42) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(int, Contains)(&set, 42));
    TEST_ASSERT(HASH_SET_OPERATION(int, Size)(&set) == 150u);

#if CONTAINER_FUNCTION_TABLE_ENABLED
    TEST_ASSERT(set.api != NULL);
    TEST_ASSERT(set.api->contains(&set, 42));
    TEST_ASSERT(set.api->insert(&set, 777) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(set.api->contains(&set, 777));
    TEST_ASSERT(set.api->remove(&set, 777) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(!set.api->contains(&set, 777));
    TEST_ASSERT(set.api->size(&set) == 150u);
    TEST_ASSERT(!set.api->isEmpty(&set));
    TEST_ASSERT(set.api->capacity(&set) > 0u);
#endif

    /* Clear */
    HASH_SET_OPERATION(int, Clear)(&set);
    TEST_ASSERT(HASH_SET_OPERATION(int, IsEmpty)(&set));
    TEST_ASSERT(HASH_SET_OPERATION(int, Size)(&set) == 0u);
    TEST_ASSERT(!HASH_SET_OPERATION(int, Contains)(&set, 42));

    /* Destroy */
    HASH_SET_OPERATION(int, Destroy)(&set);
    TEST_ASSERT(set.buckets == NULL);
    TEST_ASSERT(set.capacity == 0u);
    TEST_ASSERT(set.size == 0u);

    /* String Set */
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Init)(&strSet) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Insert)(&strSet, "alpha") == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Insert)(&strSet, "beta") == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Insert)(&strSet, "gamma") == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Size)(&strSet) == 3u);
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Contains)(&strSet, "alpha"));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Contains)(&strSet, "beta"));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Contains)(&strSet, "gamma"));
    TEST_ASSERT(!HASH_SET_OPERATION(cstring, Contains)(&strSet, "delta"));

    /* Duplicate cstring insert */
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Insert)(&strSet, "alpha") == STATUS_CONST(SUCCESS));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Size)(&strSet) == 3u);

    TEST_ASSERT(HASH_SET_OPERATION(cstring, Remove)(&strSet, "beta") == STATUS_CONST(SUCCESS));
    TEST_ASSERT(!HASH_SET_OPERATION(cstring, Contains)(&strSet, "beta"));
    TEST_ASSERT(HASH_SET_OPERATION(cstring, Size)(&strSet) == 2u);

    HASH_SET_OPERATION(cstring, Destroy)(&strSet);

    /* Edge cases / Invalid arguments */
    TEST_ASSERT(HASH_SET_OPERATION(int, Insert)(NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
    TEST_ASSERT(!HASH_SET_OPERATION(int, Contains)(NULL, 1));
    TEST_ASSERT(HASH_SET_OPERATION(int, Remove)(NULL, 1) == STATUS_CONST(INVALID_ARGUMENT));
    TEST_ASSERT(HASH_SET_OPERATION(int, Rehash)(NULL, 16) == STATUS_CONST(INVALID_ARGUMENT));
  }

  return 0;
}
