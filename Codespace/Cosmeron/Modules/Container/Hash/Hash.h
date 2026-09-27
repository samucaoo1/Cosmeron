#pragma once

#include "Hash.space"

/*
 * Open-addressed hash map with linear probing.
 *
 * HASH_MAP_DEFINE is the canonical typed generator. The suffix order follows
 * the Cosmeron map convention: key type/suffix first, then value type/suffix.
 */

#define HASH_BUCKET_STRUCT(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX)    \
  typedef struct THASH_BUCKET_TYPE(KEY_SUFFIX, VALUE_SUFFIX) {                \
    KEY_TYPE key;                                                              \
    VALUE_TYPE value;                                                          \
    uint8_t state;                                                             \
  } THASH_BUCKET_TYPE(KEY_SUFFIX, VALUE_SUFFIX);

#define HASH_STRUCT(KEY_SUFFIX, VALUE_SUFFIX)                                 \
  CONTAINER_API_FORWARD(                                                       \
      HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, FunctionTable))                \
  typedef struct THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) {                       \
    CONTAINER_API_FIELD(                                                       \
        HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, FunctionTable))              \
    THASH_BUCKET_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * buckets;                    \
    size_t capacity;                                                           \
    size_t size;                                                               \
    size_t deleted;                                                            \
  } THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX);

/* ============================================================
 * Prototypes — Lifecycle
 * ============================================================ */

#define HASH_INIT_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                         \
  static inline OPSTATUS HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Init)(      \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash)

#define HASH_DESTROY_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                      \
  static inline void HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Destroy)(       \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash)

/* ============================================================
 * Prototypes — Capacity
 * ============================================================ */

#define HASH_REHASH_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                       \
  static inline OPSTATUS HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Rehash)(    \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash, size_t newCapacity)

/* ============================================================
 * Prototypes — Access
 * ============================================================ */

#define HASH_FIND_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX)   \
  static inline VALUE_TYPE *                                                  \
  HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Find)(                             \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash, KEY_TYPE key)

#define HASH_CONST_FIND_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE,           \
                                  VALUE_SUFFIX)                               \
  static inline VALUE_TYPE const *                                            \
  HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, ConstFind)(                        \
      const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash, KEY_TYPE key)

#define HASH_CONTAINS_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_SUFFIX)           \
  static inline bool HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Contains)(      \
      const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash, KEY_TYPE key)

/* ============================================================
 * Prototypes — Modifiers
 * ============================================================ */

#define HASH_INSERT_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX) \
  static inline OPSTATUS HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Insert)(    \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash, KEY_TYPE key,              \
      VALUE_TYPE value)

#define HASH_REMOVE_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_SUFFIX)             \
  static inline OPSTATUS HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Remove)(    \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash, KEY_TYPE key)

#define HASH_CLEAR_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                        \
  static inline void HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Clear)(         \
      THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash)

/* ============================================================
 * Prototypes — Observers
 * ============================================================ */

#define HASH_IS_EMPTY_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                     \
  static inline bool HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, IsEmpty)(       \
      const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash)

#define HASH_SIZE_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                         \
  static inline size_t HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Size)(        \
      const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash)

#define HASH_CAPACITY_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX)                     \
  static inline size_t HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Capacity)(    \
      const THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) * hash)

#include "Impl/Hash.impl"
#include "FunctionTable/Hash.h"

/* ============================================================
 * Complete declaration
 * ============================================================ */

#define HASH_MAP_DEFINE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX,       \
                        HASH_FUNCTION, EQUAL_FUNCTION)                        \
  HASH_BUCKET_STRUCT(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX)          \
  HASH_STRUCT(KEY_SUFFIX, VALUE_SUFFIX)                                       \
  HASH_INIT_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                              \
  HASH_DESTROY_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                           \
  HASH_REHASH_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                            \
  HASH_FIND_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX);        \
  HASH_CONST_FIND_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX);  \
  HASH_CONTAINS_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_SUFFIX);                \
  HASH_INSERT_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX);      \
  HASH_REMOVE_PROTOTYPE(KEY_TYPE, KEY_SUFFIX, VALUE_SUFFIX);                  \
  HASH_CLEAR_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                             \
  HASH_IS_EMPTY_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                          \
  HASH_SIZE_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                              \
  HASH_CAPACITY_PROTOTYPE(KEY_SUFFIX, VALUE_SUFFIX);                          \
  HASH_IMPLEMENT(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX,              \
                 HASH_FUNCTION, EQUAL_FUNCTION)                              \
  HASH_FUNCTION_TABLE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX)         \
  HASH_FUNCTION_TABLE_INSTANCE(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX)

#define HASH_MAP_DECLARE(KEY_SUFFIX, VALUE_SUFFIX, NAME)                      \
  THASH_TYPE(KEY_SUFFIX, VALUE_SUFFIX) NAME = {                               \
      CONTAINER_API_INITIALIZER(                                              \
          HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, functions))                \
      .buckets = NULL,                                                        \
      .capacity = 0,                                                          \
      .size = 0,                                                              \
      .deleted = 0,                                                           \
  };                                                                          \
  (void)HASH_OPERATION(KEY_SUFFIX, VALUE_SUFFIX, Init)(&(NAME))

/* ============================================================
 * Common instantiations
 * ============================================================ */

HASH_MAP_DEFINE(int, int, int, int, HASH_INT, HASH_EQUAL_INT)
HASH_MAP_DEFINE(int, int, void *, ptr, HASH_INT, HASH_EQUAL_INT)
HASH_MAP_DEFINE(const char *, cstring, int, int, HASH_CSTRING,
                HASH_EQUAL_CSTRING)
HASH_MAP_DEFINE(const char *, cstring, void *, ptr, HASH_CSTRING,
                HASH_EQUAL_CSTRING)
