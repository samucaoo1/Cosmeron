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

typedef struct { uint32_t value; } TAtomicU32;
#if CONCURRENCY_HAS_ATOMIC_U64
typedef struct { uint64_t value; } TAtomicU64;
#endif
typedef struct { uintptr_t value; } TAtomicPtr;
typedef struct { uint32_t value; } TAtomicBool;

typedef enum TMemoryOrder {
  CONCURRENCY_ATOMIC_MEMORY_ORDER_RELAXED = __ATOMIC_RELAXED,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_ACQUIRE = __ATOMIC_ACQUIRE,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_RELEASE = __ATOMIC_RELEASE,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_ACQ_REL = __ATOMIC_ACQ_REL,
  CONCURRENCY_ATOMIC_MEMORY_ORDER_SEQ_CST = __ATOMIC_SEQ_CST
} TMemoryOrder;

#define ATOMIC_U32_INIT(VALUE) { (uint32_t)(VALUE) }
#if CONCURRENCY_HAS_ATOMIC_U64
#define ATOMIC_U64_INIT(VALUE) { (uint64_t)(VALUE) }
#endif
#define ATOMIC_PTR_INIT(VALUE) { (uintptr_t)(VALUE) }
#define ATOMIC_BOOL_INIT(VALUE) { (uint32_t)((VALUE) != false) }

#define ATOMIC_DECLARE(TYPE, SUFFIX)                                  \
  inline static TYPE ATOMIC_FUNC(SUFFIX##_Load)(const TAtomic##SUFFIX *atomic,  \
                                               TMemoryOrder order);            \
  inline static void ATOMIC_FUNC(SUFFIX##_Store)(TAtomic##SUFFIX *atomic,       \
                                                TYPE value,                     \
                                                TMemoryOrder order);           \
  inline static TYPE ATOMIC_FUNC(SUFFIX##_Exchange)(TAtomic##SUFFIX *atomic,    \
                                                   TYPE value,                  \
                                                   TMemoryOrder order);        \
  inline static bool ATOMIC_FUNC(SUFFIX##_CompareExchange)(                      \
      TAtomic##SUFFIX *atomic, TYPE *expected, TYPE desired,                  \
      TMemoryOrder success, TMemoryOrder failure)

ATOMIC_DECLARE(uint32_t, U32);
#if CONCURRENCY_HAS_ATOMIC_U64
ATOMIC_DECLARE(uint64_t, U64);
#endif
ATOMIC_DECLARE(uintptr_t, Ptr);

inline static uint32_t ATOMIC_FUNC(U32_FetchAdd)(TAtomicU32 *atomic,
                                                uint32_t value,
                                                TMemoryOrder order);
inline static uint32_t ATOMIC_FUNC(U32_FetchSub)(TAtomicU32 *atomic,
                                                uint32_t value,
                                                TMemoryOrder order);
inline static uint32_t ATOMIC_FUNC(U32_FetchAnd)(TAtomicU32 *atomic,
                                                uint32_t value,
                                                TMemoryOrder order);
inline static uint32_t ATOMIC_FUNC(U32_FetchOr)(TAtomicU32 *atomic,
                                               uint32_t value,
                                               TMemoryOrder order);
inline static uint32_t ATOMIC_FUNC(U32_FetchXor)(TAtomicU32 *atomic,
                                                uint32_t value,
                                                TMemoryOrder order);

#if CONCURRENCY_HAS_ATOMIC_U64
inline static uint64_t ATOMIC_FUNC(U64_FetchAdd)(TAtomicU64 *atomic,
                                                uint64_t value,
                                                TMemoryOrder order);
inline static uint64_t ATOMIC_FUNC(U64_FetchSub)(TAtomicU64 *atomic,
                                                uint64_t value,
                                                TMemoryOrder order);
inline static uint64_t ATOMIC_FUNC(U64_FetchAnd)(TAtomicU64 *atomic,
                                                uint64_t value,
                                                TMemoryOrder order);
inline static uint64_t ATOMIC_FUNC(U64_FetchOr)(TAtomicU64 *atomic,
                                               uint64_t value,
                                               TMemoryOrder order);
inline static uint64_t ATOMIC_FUNC(U64_FetchXor)(TAtomicU64 *atomic,
                                                uint64_t value,
                                                TMemoryOrder order);

#endif

inline static bool ATOMIC_FUNC(Bool_Load)(const TAtomicBool *atomic,
                                        TMemoryOrder order);
inline static void ATOMIC_FUNC(Bool_Store)(TAtomicBool *atomic, bool value,
                                         TMemoryOrder order);
inline static bool ATOMIC_FUNC(Bool_Exchange)(TAtomicBool *atomic, bool value,
                                             TMemoryOrder order);

#include "Impl/Atomic.impl"

#elif COMPILER_MSVC
#include "Impl/MSVC.impl"
#else
#error "Concurrency/Atomic requires a supported compiler backend"
#endif
