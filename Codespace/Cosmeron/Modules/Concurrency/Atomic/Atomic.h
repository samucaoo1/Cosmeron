#pragma once

#include "../Concurrency.space"


#define ATOMIC_LOAD_PROTOTYPE(TYPE, SUFFIX)                                   \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, Load))(               \
      const ATOMIC_TYPE(SUFFIX) *atomic, CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_STORE_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline void ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, Store))(              \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_EXCHANGE_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, Exchange))(           \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_COMPARE_EXCHANGE_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline bool ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, CompareExchange))(    \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE *expected, TYPE desired,             \
      CONCURRENCY_TYPE(MemoryOrder) success,                                  \
      CONCURRENCY_TYPE(MemoryOrder) failure)
#define ATOMIC_FETCH_ADD_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, FetchAdd))(           \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_FETCH_SUB_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, FetchSub))(           \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_FETCH_AND_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, FetchAnd))(           \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_FETCH_OR_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, FetchOr))(            \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_FETCH_XOR_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, FetchXor))(           \
      ATOMIC_TYPE(SUFFIX) *atomic, TYPE value,                               \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_BOOL_LOAD_PROTOTYPE                                            \
  static inline bool ATOMIC_FUNC(Bool_Load)(                                 \
      const CONCURRENCY_TYPE(TAtomicBool) *atomic,                            \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_BOOL_STORE_PROTOTYPE                                           \
  static inline void ATOMIC_FUNC(Bool_Store)(                                \
      CONCURRENCY_TYPE(TAtomicBool) *atomic, bool value,                      \
      CONCURRENCY_TYPE(MemoryOrder) order)
#define ATOMIC_BOOL_EXCHANGE_PROTOTYPE                                        \
  static inline bool ATOMIC_FUNC(Bool_Exchange)(                             \
      CONCURRENCY_TYPE(TAtomicBool) *atomic, bool value,                      \
      CONCURRENCY_TYPE(MemoryOrder) order)

#if COMPILER_GCC || COMPILER_CLANG

/*
 * Keep capability macros preprocessor-constant. __atomic_always_lock_free()
 * is a compiler builtin, but it is not a C integer constant expression and
 * cannot be used portably in #if or pedantic _Static_assert.
 */
#if defined(__GCC_ATOMIC_INT_LOCK_FREE) && __GCC_ATOMIC_INT_LOCK_FREE == 2
#define CONCURRENCY_HAS_ATOMIC_U32 1
#else
#define CONCURRENCY_HAS_ATOMIC_U32 0
#endif

#if defined(__GCC_ATOMIC_LLONG_LOCK_FREE) && __GCC_ATOMIC_LLONG_LOCK_FREE == 2
#define CONCURRENCY_HAS_ATOMIC_U64 1
#else
#define CONCURRENCY_HAS_ATOMIC_U64 0
#endif

#if defined(__GCC_ATOMIC_POINTER_LOCK_FREE) && __GCC_ATOMIC_POINTER_LOCK_FREE == 2
#define CONCURRENCY_HAS_ATOMIC_PTR 1
#else
#define CONCURRENCY_HAS_ATOMIC_PTR 0
#endif

#if !CONCURRENCY_HAS_ATOMIC_U32
#error "Concurrency requires lock-free 32-bit atomics"
#endif
#if !CONCURRENCY_HAS_ATOMIC_PTR
#error "Concurrency requires lock-free pointer atomics"
#endif

typedef struct { uint32_t value; } CONCURRENCY_TYPE(TAtomicU32);
#if CONCURRENCY_HAS_ATOMIC_U64
typedef struct { uint64_t value; } CONCURRENCY_TYPE(TAtomicU64);
#endif
typedef struct { uintptr_t value; } CONCURRENCY_TYPE(TAtomicPtr);
typedef struct { uint32_t value; } CONCURRENCY_TYPE(TAtomicBool);

typedef enum CONCURRENCY_TYPE(MemoryOrder) {
  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_RELAXED) = __ATOMIC_RELAXED,
  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_ACQUIRE) = __ATOMIC_ACQUIRE,
  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_RELEASE) = __ATOMIC_RELEASE,
  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_ACQ_REL) = __ATOMIC_ACQ_REL,
  CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST) = __ATOMIC_SEQ_CST
} CONCURRENCY_TYPE(MemoryOrder);

#define ATOMIC_TYPE(SUFFIX) CONCURRENCY_TYPE(PP_OP_CAT2(TAtomic, SUFFIX))

#define ATOMIC_U32_INIT(VALUE) { (uint32_t)(VALUE) }
#if CONCURRENCY_HAS_ATOMIC_U64
#define ATOMIC_U64_INIT(VALUE) { (uint64_t)(VALUE) }
#endif
#define ATOMIC_PTR_INIT(VALUE) { (uintptr_t)(VALUE) }
#define ATOMIC_BOOL_INIT(VALUE) { (uint32_t)((VALUE) != false) }

ATOMIC_LOAD_PROTOTYPE(uint32_t, U32);
ATOMIC_STORE_PROTOTYPE(uint32_t, U32);
ATOMIC_EXCHANGE_PROTOTYPE(uint32_t, U32);
ATOMIC_COMPARE_EXCHANGE_PROTOTYPE(uint32_t, U32);
ATOMIC_FETCH_ADD_PROTOTYPE(uint32_t, U32);
ATOMIC_FETCH_SUB_PROTOTYPE(uint32_t, U32);
ATOMIC_FETCH_AND_PROTOTYPE(uint32_t, U32);
ATOMIC_FETCH_OR_PROTOTYPE(uint32_t, U32);
ATOMIC_FETCH_XOR_PROTOTYPE(uint32_t, U32);

#if CONCURRENCY_HAS_ATOMIC_U64
ATOMIC_LOAD_PROTOTYPE(uint64_t, U64);
ATOMIC_STORE_PROTOTYPE(uint64_t, U64);
ATOMIC_EXCHANGE_PROTOTYPE(uint64_t, U64);
ATOMIC_COMPARE_EXCHANGE_PROTOTYPE(uint64_t, U64);
ATOMIC_FETCH_ADD_PROTOTYPE(uint64_t, U64);
ATOMIC_FETCH_SUB_PROTOTYPE(uint64_t, U64);
ATOMIC_FETCH_AND_PROTOTYPE(uint64_t, U64);
ATOMIC_FETCH_OR_PROTOTYPE(uint64_t, U64);
ATOMIC_FETCH_XOR_PROTOTYPE(uint64_t, U64);
#endif

ATOMIC_LOAD_PROTOTYPE(uintptr_t, Ptr);
ATOMIC_STORE_PROTOTYPE(uintptr_t, Ptr);
ATOMIC_EXCHANGE_PROTOTYPE(uintptr_t, Ptr);
ATOMIC_COMPARE_EXCHANGE_PROTOTYPE(uintptr_t, Ptr);

ATOMIC_BOOL_LOAD_PROTOTYPE;
ATOMIC_BOOL_STORE_PROTOTYPE;
ATOMIC_BOOL_EXCHANGE_PROTOTYPE;

#include "Impl/Atomic.impl"

#elif COMPILER_MSVC
#include "Impl/MSVC.impl"
#else
#error "Concurrency/Atomic requires a supported compiler backend"
#endif
