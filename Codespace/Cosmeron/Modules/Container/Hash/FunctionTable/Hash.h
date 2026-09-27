#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define HASH_FUNCTION_TABLE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX)   \
  typedef struct HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, FunctionTable) {    \
    OPSTATUS (*init)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *);                 \
    void (*destroy)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *);                  \
    OPSTATUS (*rehash)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *, size_t);       \
    VALUE_TYPE *(*find)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *, KEY_TYPE);    \
    VALUE_TYPE const *(*constFind)(                                           \
        const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *, KEY_TYPE);              \
    bool (*contains)(const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *, KEY_TYPE); \
    OPSTATUS (*insert)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *, KEY_TYPE,      \
                       VALUE_TYPE);                                           \
    OPSTATUS (*remove)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *, KEY_TYPE);     \
    void (*clear)(THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *);                    \
    bool (*isEmpty)(const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *);            \
    size_t (*size)(const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *);             \
    size_t (*capacity)(const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) *);         \
  } HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, FunctionTable);

#define HASH_FUNCTION_TABLE_INSTANCE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE,        \
                                     VALUE_SUFFIX)                            \
  static const HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, FunctionTable)        \
      HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, functions) = {                 \
          .init = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Init),             \
          .destroy = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Destroy),       \
          .rehash = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Rehash),         \
          .find = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Find),             \
          .constFind = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, ConstFind),   \
          .contains = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Contains),     \
          .insert = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Insert),         \
          .remove = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Remove),         \
          .clear = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Clear),           \
          .isEmpty = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, IsEmpty),       \
          .size = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Size),             \
          .capacity = HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Capacity),     \
      };

#else

#define HASH_FUNCTION_TABLE(...)
#define HASH_FUNCTION_TABLE_INSTANCE(...)

#endif
