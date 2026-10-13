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

  return 0;
}
