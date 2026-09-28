#pragma once

#include "../Concurrency.space"

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
  CONCURRENCY_ATOMIC_MEMORY_ORDER_RELAXED = __ATOMIC_RELAXED,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_ACQUIRE = __ATOMIC_ACQUIRE,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_RELEASE = __ATOMIC_RELEASE,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_ACQ_REL = __ATOMIC_ACQ_REL,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST = __ATOMIC_SEQ_CST
} CONCURRENCY_TYPE(MemoryOrder);

#define ATOMIC_U32_INIT(VALUE) { (uint32_t)(VALUE) }
#if CONCURRENCY_HAS_ATOMIC_U64
#define ATOMIC_U64_INIT(VALUE) { (uint64_t)(VALUE) }
#endif
#define ATOMIC_PTR_INIT(VALUE) { (uintptr_t)(VALUE) }
#define ATOMIC_BOOL_INIT(VALUE) { (uint32_t)((VALUE) != false) }

#define ATOMIC_DECLARE(TYPE, SUFFIX)                                  \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, Load))(const PP_OP_CAT2(CONCURRENCY_TYPE(TAtomic), SUFFIX) *atomic,  \
                                               CONCURRENCY_TYPE(MemoryOrder) order);            \
  static inline void ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, Store))(PP_OP_CAT2(CONCURRENCY_TYPE(TAtomic), SUFFIX) *atomic,       \
                                                TYPE value,                     \
                                                CONCURRENCY_TYPE(MemoryOrder) order);           \
  static inline TYPE ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, Exchange))(PP_OP_CAT2(CONCURRENCY_TYPE(TAtomic), SUFFIX) *atomic,    \
                                                   TYPE value,                  \
                                                   CONCURRENCY_TYPE(MemoryOrder) order);        \
  static inline bool ATOMIC_FUNC(PP_OP_CAT3(SUFFIX, _, CompareExchange))(                      \
      PP_OP_CAT2(CONCURRENCY_TYPE(TAtomic), SUFFIX) *atomic, TYPE *expected, TYPE desired,                  \
      CONCURRENCY_TYPE(MemoryOrder) success, CONCURRENCY_TYPE(MemoryOrder) failure)

ATOMIC_DECLARE(uint32_t, U32);
#if CONCURRENCY_HAS_ATOMIC_U64
ATOMIC_DECLARE(uint64_t, U64);
#endif
ATOMIC_DECLARE(uintptr_t, Ptr);

static inline uint32_t ATOMIC_FUNC(U32_FetchAdd)(CONCURRENCY_TYPE(TAtomicU32) *atomic,
                                                uint32_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint32_t ATOMIC_FUNC(U32_FetchSub)(CONCURRENCY_TYPE(TAtomicU32) *atomic,
                                                uint32_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint32_t ATOMIC_FUNC(U32_FetchAnd)(CONCURRENCY_TYPE(TAtomicU32) *atomic,
                                                uint32_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint32_t ATOMIC_FUNC(U32_FetchOr)(CONCURRENCY_TYPE(TAtomicU32) *atomic,
                                               uint32_t value,
                                               CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint32_t ATOMIC_FUNC(U32_FetchXor)(CONCURRENCY_TYPE(TAtomicU32) *atomic,
                                                uint32_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);

#if CONCURRENCY_HAS_ATOMIC_U64
static inline uint64_t ATOMIC_FUNC(U64_FetchAdd)(CONCURRENCY_TYPE(TAtomicU64) *atomic,
                                                uint64_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint64_t ATOMIC_FUNC(U64_FetchSub)(CONCURRENCY_TYPE(TAtomicU64) *atomic,
                                                uint64_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint64_t ATOMIC_FUNC(U64_FetchAnd)(CONCURRENCY_TYPE(TAtomicU64) *atomic,
                                                uint64_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint64_t ATOMIC_FUNC(U64_FetchOr)(CONCURRENCY_TYPE(TAtomicU64) *atomic,
                                               uint64_t value,
                                               CONCURRENCY_TYPE(MemoryOrder) order);
static inline uint64_t ATOMIC_FUNC(U64_FetchXor)(CONCURRENCY_TYPE(TAtomicU64) *atomic,
                                                uint64_t value,
                                                CONCURRENCY_TYPE(MemoryOrder) order);

#endif

static inline bool ATOMIC_FUNC(Bool_Load)(const CONCURRENCY_TYPE(TAtomicBool) *atomic,
                                        CONCURRENCY_TYPE(MemoryOrder) order);
static inline void ATOMIC_FUNC(Bool_Store)(CONCURRENCY_TYPE(TAtomicBool) *atomic, bool value,
                                         CONCURRENCY_TYPE(MemoryOrder) order);
static inline bool ATOMIC_FUNC(Bool_Exchange)(CONCURRENCY_TYPE(TAtomicBool) *atomic, bool value,
                                             CONCURRENCY_TYPE(MemoryOrder) order);

#include "Impl/Atomic.impl"

#elif COMPILER_MSVC
#include "Impl/MSVC.impl"
#else
#error "Concurrency/Atomic requires a supported compiler backend"
#endif
