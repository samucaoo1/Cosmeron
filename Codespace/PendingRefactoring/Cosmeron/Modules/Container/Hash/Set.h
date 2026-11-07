#pragma once

#include "Hash.space"

/*
 * Open-addressed hash set with linear probing.
 *
 * HASH_SET_IMPLEMENT_ALL is the canonical typed generator for keys-only sets
 * without value field overhead.
 */

#define HASH_SET_BUCKET_STRUCT(KEY_TYPE, KEY_SUFFIX)                          \
  typedef struct THASH_SET_BUCKET_TYPE(KEY_SUFFIX) {                          \
    KEY_TYPE key;                                                              \
    HASH_NS(BucketState) state;                                                \
  } THASH_SET_BUCKET_TYPE(KEY_SUFFIX);

#define HASH_SET_STRUCT(KEY_SUFFIX)                                           \
  CONTAINER_API_FORWARD(HASH_SET_FUNCTION_TABLE_TYPE(KEY_SUFFIX))             \
  typedef struct THASH_SET_TYPE(KEY_SUFFIX) {                                  \
    CONTAINER_API_FIELD(HASH_SET_FUNCTION_TABLE_TYPE(KEY_SUFFIX))              \
    THASH_SET_BUCKET_TYPE(KEY_SUFFIX) * buckets;                               \
    size_t capacity;                                                           \
    size_t size;                                                               \
    size_t deleted;                                                            \
  } THASH_SET_TYPE(KEY_SUFFIX);

/* ============================================================
 * Prototypes — Lifecycle
 * ============================================================ */

#define HASH_SET_INIT_PROTOTYPE(KEY_SUFFIX)                                   \
  static inline OPSTATUS HASH_SET_OPERATION(KEY_SUFFIX, Init)(                 \
      THASH_SET_TYPE(KEY_SUFFIX) * set)

#define HASH_SET_DESTROY_PROTOTYPE(KEY_SUFFIX)                                \
  static inline void HASH_SET_OPERATION(KEY_SUFFIX, Destroy)(                  \
      THASH_SET_TYPE(KEY_SUFFIX) * set)

/* ============================================================
 * Prototypes — Capacity
 * ============================================================ */

#define HASH_SET_REHASH_PROTOTYPE(KEY_SUFFIX)                                 \
  static inline OPSTATUS HASH_SET_OPERATION(KEY_SUFFIX, Rehash)(               \
      THASH_SET_TYPE(KEY_SUFFIX) * set, size_t newCapacity)

/* ============================================================
 * Prototypes — Observers & Access
 * ============================================================ */

#define HASH_SET_CONTAINS_PROTOTYPE(KEY_TYPE, KEY_SUFFIX)                     \
  static inline bool HASH_SET_OPERATION(KEY_SUFFIX, Contains)(                 \
      const THASH_SET_TYPE(KEY_SUFFIX) * set, KEY_TYPE key)

#define HASH_SET_IS_EMPTY_PROTOTYPE(KEY_SUFFIX)                               \
  static inline bool HASH_SET_OPERATION(KEY_SUFFIX, IsEmpty)(                  \
      const THASH_SET_TYPE(KEY_SUFFIX) * set)

#define HASH_SET_SIZE_PROTOTYPE(KEY_SUFFIX)                                   \
  static inline size_t HASH_SET_OPERATION(KEY_SUFFIX, Size)(                   \
      const THASH_SET_TYPE(KEY_SUFFIX) * set)

#define HASH_SET_CAPACITY_PROTOTYPE(KEY_SUFFIX)                               \
  static inline size_t HASH_SET_OPERATION(KEY_SUFFIX, Capacity)(               \
      const THASH_SET_TYPE(KEY_SUFFIX) * set)

/* ============================================================
 * Prototypes — Modifiers
 * ============================================================ */

#define HASH_SET_INSERT_PROTOTYPE(KEY_TYPE, KEY_SUFFIX)                       \
  static inline OPSTATUS HASH_SET_OPERATION(KEY_SUFFIX, Insert)(               \
      THASH_SET_TYPE(KEY_SUFFIX) * set, KEY_TYPE key)

#define HASH_SET_REMOVE_PROTOTYPE(KEY_TYPE, KEY_SUFFIX)                       \
  static inline OPSTATUS HASH_SET_OPERATION(KEY_SUFFIX, Remove)(               \
      THASH_SET_TYPE(KEY_SUFFIX) * set, KEY_TYPE key)

#define HASH_SET_CLEAR_PROTOTYPE(KEY_SUFFIX)                                  \
  static inline void HASH_SET_OPERATION(KEY_SUFFIX, Clear)(                    \
      THASH_SET_TYPE(KEY_SUFFIX) * set)

#include "Impl/Set.impl"
#include "FunctionTable/Hash.h"

/* ============================================================
 * Complete declaration & implementation
 * ============================================================ */

#define HASH_SET_IMPLEMENT_ALL(KEY_TYPE, KEY_SUFFIX, HASH_FUNCTION,           \
                               EQUAL_FUNCTION)                                 \
  HASH_SET_BUCKET_STRUCT(KEY_TYPE, KEY_SUFFIX)                                 \
  HASH_SET_STRUCT(KEY_SUFFIX)                                                  \
  HASH_SET_INIT_PROTOTYPE(KEY_SUFFIX);                                         \
  HASH_SET_DESTROY_PROTOTYPE(KEY_SUFFIX);                                      \
  HASH_SET_REHASH_PROTOTYPE(KEY_SUFFIX);                                       \
  HASH_SET_CONTAINS_PROTOTYPE(KEY_TYPE, KEY_SUFFIX);                            \
  HASH_SET_INSERT_PROTOTYPE(KEY_TYPE, KEY_SUFFIX);                              \
  HASH_SET_REMOVE_PROTOTYPE(KEY_TYPE, KEY_SUFFIX);                              \
  HASH_SET_CLEAR_PROTOTYPE(KEY_SUFFIX);                                        \
  HASH_SET_IS_EMPTY_PROTOTYPE(KEY_SUFFIX);                                     \
  HASH_SET_SIZE_PROTOTYPE(KEY_SUFFIX);                                         \
  HASH_SET_CAPACITY_PROTOTYPE(KEY_SUFFIX);                                     \
  COSMERON_MACRO_INTERNAL_HASH_SET_IMPLEMENT(KEY_TYPE, KEY_SUFFIX,             \
                                             HASH_FUNCTION, EQUAL_FUNCTION)     \
  HASH_SET_FUNCTION_TABLE_STRUCT(KEY_TYPE, KEY_SUFFIX)                         \
  HASH_SET_FUNCTION_TABLE_INSTANCE(KEY_TYPE, KEY_SUFFIX)

#define HASH_SET_DECLARE(KEY_SUFFIX, NAME)                                     \
  THASH_SET_TYPE(KEY_SUFFIX) NAME = {                                          \
      CONTAINER_API_INITIALIZER(                                               \
          HASH_SET_OPERATION(KEY_SUFFIX, functions))                           \
      .buckets = NULL,                                                         \
      .capacity = 0,                                                           \
      .size = 0,                                                               \
      .deleted = 0,                                                            \
  };                                                                           \
  (void)HASH_SET_OPERATION(KEY_SUFFIX, Init)(&(NAME))

/* ============================================================
 * Common instantiations
 * ============================================================ */

HASH_SET_IMPLEMENT_ALL(int, int, HASH_INT, HASH_EQUAL_INT)
HASH_SET_IMPLEMENT_ALL(const char *, cstring, HASH_CSTRING, HASH_EQUAL_CSTRING)

typedef THASH_SET_TYPE(int) THashSet_int;
typedef THASH_SET_TYPE(cstring) THashSet_cstring;

/* EOF */
