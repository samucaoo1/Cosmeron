#pragma once

#include "Module.space"
#include "Common/Tuple.impl"
#include "Common/ComparisonEx.impl"
#include "Callback/Callback.h"
#include "Common/FunctionTable.impl"
#include "Common/Dispatch.impl"
#include "Common/Template.impl"

/* ============================================================
 * Package Declarative Framework (Core/Module/Package.h)
 * Handles collections of free functions (pure procedures,
 * operations, predicates, and functions) with optional FunctionTable.
 * ============================================================ */

#define PACKAGE_MOD Package
#define PACKAGE_CMOD PACKAGE

#define PACKAGE_NS(NAME) NS(LIB_PREFIX(PACKAGE_MOD), NAME)
#define PACKAGE_CNS(NAME) CNS(LIB_PREFIX_CONST(PACKAGE_CMOD), NAME)
#define PACKAGE_INS(NAME) NS(LIB_PREFIX(PACKAGE_MOD), Internal, NAME)
#define PACKAGE_CINS(NAME) CNS(LIB_PREFIX_CONST(PACKAGE_CMOD), INTERNAL, NAME)

#define PACKAGE_TYPE(NAME) PACKAGE_NS(NAME)
#define PACKAGE_FUNC(NAME) PACKAGE_NS(NAME)
#define PACKAGE_CONST(NAME) PACKAGE_CNS(NAME)

/* ------------------------------------------------------------
 * 1. Invocation Helpers
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_CALL_0(PKG, FUNC_NAME) \
  (NS(PKG, GetFunctionTable)()->FUNC_NAME())

#define COSMERON_MACRO_INTERNAL_PKG_CALL_1(PKG, FUNC_NAME, ...) \
  (NS(PKG, GetFunctionTable)()->FUNC_NAME(__VA_ARGS__))

#define PACKAGE_API(...) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_CALL_, \
             PP_HAS_MORE_THAN_2(__VA_ARGS__))(__VA_ARGS__)

/* Backwards-compatible package invocation aliases. */
#define PACKAGE_CALL(...) PACKAGE_API(__VA_ARGS__)

#define PACKAGE_CALL_0(PKG, FUNC_NAME) \
  (NS(PKG, GetFunctionTable)()->FUNC_NAME())

/* ------------------------------------------------------------
 * 2. Internal Function Generators (Prototypes & Inlines)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_0(PKG, R, N, P)                \
  typedef R NS(PKG, N, ret_t);                                               \
  struct NS(PKG, N, sig) {                                                   \
    PP_PARAMS_TO_FIELDS(P)                                                     \
  };                                                                           \
  static inline R NS(PKG, N)                                                 \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);

#define COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_1(PKG, R, N, P)                \
  COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_0(PKG, R, N, P)

#define COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_2(PKG, R, N, P)                \
  CALLBACK_FUNCTION(R, NS(PKG, N), P);

#define COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_3(PKG, R, N, P)                \
  CALLBACK_PROCEDURE(NS(PKG, N), P);                                           \
  static inline OPSTATUS NS(PKG, N, Subscribe)(                                \
      TCALLBACK(NS(PKG, N)) listener);                                         \
  static inline OPSTATUS NS(PKG, N, Unsubscribe)(                              \
      TCALLBACK(NS(PKG, N)) listener);                                         \
  static inline void NS(PKG, N, Emit)                                         \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);                              \
  static inline OPSTATUS NS(PKG, N, Clear)(void);                              \
  static inline uint8_t NS(PKG, N, Count)(void);                               \
  static inline uint8_t NS(PKG, N, Capacity)(void);

#define COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_4(PKG, R, N, P)                \
  CALLBACK_OPERATION(NS(PKG, N), P);                                           \
  static inline OPSTATUS NS(PKG, N, Subscribe)(                                \
      TCALLBACK(NS(PKG, N)) listener);                                         \
  static inline OPSTATUS NS(PKG, N, Unsubscribe)(                              \
      TCALLBACK(NS(PKG, N)) listener);                                         \
  static inline OPSTATUS NS(PKG, N, Emit)                                      \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);                              \
  static inline OPSTATUS NS(PKG, N, Clear)(void);                              \
  static inline uint8_t NS(PKG, N, Count)(void);                               \
  static inline uint8_t NS(PKG, N, Capacity)(void);

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO_I(PKG, C, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PACKAGE_PROTO_C_, C)(PKG, R, N, P)

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO(PKG, M) \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO_I, PKG, M)

#define COSMERON_MACRO_INTERNAL_PKG_EVENT_INLINE_PROC(PKG, CAP, N, P, ARGS)    \
  _Static_assert((CAP) <= 16, "Cosmeron Event capacity cannot exceed 16 slots");\
  static COSMERON_MAYBE_UNUSED struct {                                         \
    uint8_t count;                                                             \
    TCALLBACK(NS(PKG, N)) listeners[COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)];   \
  } NS(PKG, event, N);                                                         \
  static inline OPSTATUS NS(PKG, N, Subscribe)(                                \
      TCALLBACK(NS(PKG, N)) listener) {                                        \
    if (!CALLBACK_IS_VALID(listener))                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (NS(PKG, event, N).count >= COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)) {   \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    NS(PKG, event, N).listeners[NS(PKG, event, N).count++] = listener;         \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS NS(PKG, N, Unsubscribe)(                              \
      TCALLBACK(NS(PKG, N)) listener) {                                        \
    for (uint8_t i = 0; i < NS(PKG, event, N).count; ++i) {                   \
      if (NS(PKG, event, N).listeners[i].function == listener.function &&      \
          NS(PKG, event, N).listeners[i].context == listener.context) {        \
        for (uint8_t j = i; j + 1 < NS(PKG, event, N).count; ++j) {           \
          NS(PKG, event, N).listeners[j] = NS(PKG, event, N).listeners[j + 1]; \
        }                                                                      \
        NS(PKG, event, N).count--;                                             \
        return STATUS_CONST(SUCCESS);                                          \
      }                                                                        \
    }                                                                          \
    return STATUS_CONST(NOT_FOUND);                                            \
  }                                                                            \
  static inline void NS(PKG, N, Emit)                                         \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P) {                             \
    if (NS(PKG, event, N).count == 0) return;                                  \
    uint8_t activeCount = NS(PKG, event, N).count;                             \
    TCALLBACK(NS(PKG, N)) snapshot[COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)];    \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      snapshot[i] = NS(PKG, event, N).listeners[i];                           \
    }                                                                          \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      COSMERON_MACRO_INTERNAL_EV_INVOKE(snapshot[i], ARGS);                    \
    }                                                                          \
  }                                                                            \
  static inline OPSTATUS NS(PKG, N, Clear)(void) {                             \
    NS(PKG, event, N).count = 0;                                               \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline uint8_t NS(PKG, N, Count)(void) {                              \
    return NS(PKG, event, N).count;                                            \
  }                                                                            \
  static inline uint8_t NS(PKG, N, Capacity)(void) {                           \
    return (uint8_t)COSMERON_EVENT_NORMALIZE_CAPACITY(CAP);                    \
  }

#define COSMERON_MACRO_INTERNAL_PKG_EVENT_INLINE_OP(PKG, CAP, N, P, ARGS)      \
  _Static_assert((CAP) <= 16, "Cosmeron Event capacity cannot exceed 16 slots");\
  static COSMERON_MAYBE_UNUSED struct {                                         \
    uint8_t count;                                                             \
    TCALLBACK(NS(PKG, N)) listeners[COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)];   \
  } NS(PKG, event, N);                                                         \
  static inline OPSTATUS NS(PKG, N, Subscribe)(                                \
      TCALLBACK(NS(PKG, N)) listener) {                                        \
    if (!CALLBACK_IS_VALID(listener))                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (NS(PKG, event, N).count >= COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)) {   \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    NS(PKG, event, N).listeners[NS(PKG, event, N).count++] = listener;         \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS NS(PKG, N, Unsubscribe)(                              \
      TCALLBACK(NS(PKG, N)) listener) {                                        \
    for (uint8_t i = 0; i < NS(PKG, event, N).count; ++i) {                   \
      if (NS(PKG, event, N).listeners[i].function == listener.function &&      \
          NS(PKG, event, N).listeners[i].context == listener.context) {        \
        for (uint8_t j = i; j + 1 < NS(PKG, event, N).count; ++j) {           \
          NS(PKG, event, N).listeners[j] = NS(PKG, event, N).listeners[j + 1]; \
        }                                                                      \
        NS(PKG, event, N).count--;                                             \
        return STATUS_CONST(SUCCESS);                                          \
      }                                                                        \
    }                                                                          \
    return STATUS_CONST(NOT_FOUND);                                            \
  }                                                                            \
  static inline OPSTATUS NS(PKG, N, Emit)                                      \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P) {                             \
    if (NS(PKG, event, N).count == 0) return STATUS_CONST(SUCCESS);            \
    uint8_t activeCount = NS(PKG, event, N).count;                             \
    TCALLBACK(NS(PKG, N)) snapshot[COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)];    \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      snapshot[i] = NS(PKG, event, N).listeners[i];                           \
    }                                                                          \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      OPSTATUS status = COSMERON_MACRO_INTERNAL_EV_INVOKE(snapshot[i], ARGS);  \
      if (status != STATUS_CONST(SUCCESS)) {                                   \
        return status;                                                         \
      }                                                                        \
    }                                                                          \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS NS(PKG, N, Clear)(void) {                             \
    NS(PKG, event, N).count = 0;                                               \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline uint8_t NS(PKG, N, Count)(void) {                              \
    return NS(PKG, event, N).count;                                            \
  }                                                                            \
  static inline uint8_t NS(PKG, N, Capacity)(void) {                           \
    return (uint8_t)COSMERON_EVENT_NORMALIZE_CAPACITY(CAP);                    \
  }

#define COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_0(PKG, C, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_1(PKG, C, R, N, P, B) \
  static inline R NS(PKG, N)                                            \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P) B

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_C_0(PKG, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_, I)(PKG, 0, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_C_1(PKG, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_, I)(PKG, 1, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_C_2(PKG, I, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_C_3(PKG, I, R, N, P, B)    \
  COSMERON_MACRO_INTERNAL_PKG_EVENT_INLINE_PROC(PKG, I, N, P, B)
#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_C_4(PKG, I, R, N, P, B)    \
  COSMERON_MACRO_INTERNAL_PKG_EVENT_INLINE_OP(PKG, I, N, P, B)

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_I(PKG, C, I, R, N, P, B)  \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_C_, C)(PKG, I, R, N, P, B)

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE(PKG, M) \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_I, PKG, M)

/* ------------------------------------------------------------
 * 3. Top-Level PACKAGE Declarator
 * ------------------------------------------------------------ */

#if !defined(PACKAGE_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)

#define PACKAGE(PKG, ...)                                                       \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO, PKG, __VA_ARGS__)   \
  struct NS(PKG, FunctionTable) {                                             \
    int _reserved;                                                              \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_FT_FIELD, PKG, __VA_ARGS__)      \
  };                                                                            \
  typedef struct NS(PKG, FunctionTable) NS(PKG, FunctionTable);             \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE, PKG, __VA_ARGS__)  \
  static inline const struct NS(PKG, FunctionTable) *NS(                    \
      PKG, GetFunctionTable)(void) {                                            \
    static const struct NS(PKG, FunctionTable) table = {                      \
        ._reserved = 0,                                                         \
        PP_FOREACH_CTX(                                                         \
            COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY, PKG, __VA_ARGS__)};          \
    return &table;                                                              \
  }                                                                             \
  typedef int NS(_cosmeron_package_semi, PKG, __LINE__)

#else

#define PACKAGE(PKG, ...)                                                       \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO, PKG, __VA_ARGS__)   \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE, PKG, __VA_ARGS__)  \
  typedef int NS(_cosmeron_package_semi, PKG, __LINE__)

#endif

/* ------------------------------------------------------------
 * 4. Package Implementation Finalizer
 * ------------------------------------------------------------ */

#define PACKAGE_IMPLEMENT(PKG) \
  typedef int NS(_cosmeron_package_implemented, PKG, __LINE__)

/* EOF */
