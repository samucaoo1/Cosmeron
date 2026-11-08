#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Callback/Callback.h"
#include "Common/ComparisonEx.impl"
#include "Common/Dispatch.impl"
#include "Common/FunctionTable.impl"
#include "Common/Template.impl"
#include "Common/Tuple.impl"
#include "Namespace.h"

/* ============================================================
 * Object Declarative Framework (Core/Module/Object.h)
 * Handles object structs, receivers, inline methods, and function tables.
 * ============================================================ */

/* ------------------------------------------------------------
 * 1. Invocation & Casting Helpers
 * ------------------------------------------------------------ */

#define OBJECT_AS(OBJ, TARGET_TYPE) ((TARGET_TYPE *)(OBJ))

#define COSMERON_MACRO_INTERNAL_OBJECT_API_0(OBJ, METHOD_NAME)                 \
  ((OBJ)->api->METHOD_NAME(OBJ))

#define COSMERON_MACRO_INTERNAL_OBJECT_API_1(OBJ, METHOD_NAME, ...)            \
  ((OBJ)->api->METHOD_NAME((OBJ), __VA_ARGS__))

#define API(...)                                                               \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_API_,                              \
             PP_HAS_MORE_THAN_2(__VA_ARGS__))(__VA_ARGS__)

/* Backwards-compatible object invocation aliases. */
#define OBJECT_CALL_0(OBJ, METHOD_NAME) API(OBJ, METHOD_NAME)
#define OBJECT_CALL(...) API(__VA_ARGS__)

/* ------------------------------------------------------------
 * 2. Internal Method Generators (Prototypes & Inlines)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_C_0(CLASS, R, N, P)           \
  typedef R NS(CLASS, N, ret_t);                                               \
  typedef CLASS NS(CLASS, N, self_t);                                          \
  struct NS(CLASS, N, sig) {                                                   \
    PP_PARAMS_TO_FIELDS(P)                                                     \
  };                                                                           \
  static inline R NS(CLASS, N)                                                 \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, 0, P);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_C_1(CLASS, R, N, P)           \
  typedef R NS(CLASS, N, ret_t);                                               \
  typedef const CLASS NS(CLASS, N, self_t);                                    \
  struct NS(CLASS, N, sig) {                                                   \
    PP_PARAMS_TO_FIELDS(P)                                                     \
  };                                                                           \
  static inline R NS(CLASS, N)                                                 \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, 1, P);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_C_2(CLASS, R, N, P)           \
  CALLBACK_FUNCTION(R, NS(CLASS, N), P);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_C_3(CLASS, R, N, P)           \
  CALLBACK_PROCEDURE(NS(CLASS, N), P);                                         \
  static inline OPSTATUS NS(CLASS, N, Subscribe)(                              \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener);                         \
  static inline OPSTATUS NS(CLASS, N, Unsubscribe)(                            \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener);                         \
  static inline void NS(CLASS, N, Emit)                                        \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, 0, P);                    \
  static inline OPSTATUS NS(CLASS, N, Clear)(CLASS * self);                    \
  static inline uint8_t NS(CLASS, N, Count)(const CLASS *self);                \
  static inline uint8_t NS(CLASS, N, Capacity)(const CLASS *self);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_C_4(CLASS, R, N, P)           \
  CALLBACK_OPERATION(NS(CLASS, N), P);                                         \
  static inline OPSTATUS NS(CLASS, N, Subscribe)(                              \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener);                         \
  static inline OPSTATUS NS(CLASS, N, Unsubscribe)(                            \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener);                         \
  static inline OPSTATUS NS(CLASS, N, Emit)                                    \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, 0, P);                    \
  static inline OPSTATUS NS(CLASS, N, Clear)(CLASS * self);                    \
  static inline uint8_t NS(CLASS, N, Count)(const CLASS *self);                \
  static inline uint8_t NS(CLASS, N, Capacity)(const CLASS *self);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_I(CLASS, C, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_C_, C)(CLASS, R, N, P)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO(CLASS, M)                     \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(                                       \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_I, CLASS, M)

/* --- Event Fields in Struct --- */
#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_0(CLASS, I, N)
#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_1(CLASS, I, N)
#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_2(CLASS, I, N)
#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_3(CLASS, I, N)               \
  _Static_assert((I) <= 16, "Cosmeron Event capacity cannot exceed 16 slots"); \
  struct {                                                                     \
    uint8_t count;                                                             \
    TCALLBACK(NS(CLASS, N)) listeners[COSMERON_EVENT_NORMALIZE_CAPACITY(I)];   \
  } NS(event, N);
#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_4(CLASS, I, N)               \
  COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_3(CLASS, I, N)

#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_I(CLASS, C, I, R, N, P, B)     \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_C_, C)(CLASS, I, N)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_EVENT_FIELD(CLASS, M)               \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(                                       \
      COSMERON_MACRO_INTERNAL_OBJ_EVENT_FIELD_I, CLASS, M)

/* --- Event Initializer in Init --- */
#define COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_C_0(CLASS, N)
#define COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_C_1(CLASS, N)
#define COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_C_2(CLASS, N)
#define COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_C_3(CLASS, N)                   \
  self->NS(event, N).count = 0;
#define COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_C_4(CLASS, N)                   \
  self->NS(event, N).count = 0;

#define COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_I(CLASS, C, I, R, N, P, B)      \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_C_, C)(CLASS, N)

#define COSMERON_MACRO_INTERNAL_OBJECT_INIT_EVENT(CLASS, M)                    \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(                                       \
      COSMERON_MACRO_INTERNAL_OBJ_INIT_EVENT_I, CLASS, M)

#define COSMERON_MACRO_INTERNAL_PROBE_CMP_CMPOUT ,
#define COSMERON_MACRO_INTERNAL_IS_CMP_TYPE(r)                                 \
  PP_HAS_COMMA(PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PROBE_CMP_, r))

#define COSMERON_MACRO_INTERNAL_OBJ_CMP_ADAPTER_1(CLASS, N)                    \
  static inline CMPOUT NS(CLASS, N, ComparatorAdapter)(                        \
      const void *left, const void *right, void *context) {                    \
    (void)context;                                                             \
    if (left == right)                                                         \
      return COMPARISON_CONST(EQUAL);                                          \
    if (!left)                                                                 \
      return COMPARISON_CONST(LOWER);                                          \
    if (!right)                                                                \
      return COMPARISON_CONST(HIGHER);                                         \
    return NS(CLASS, N)((const CLASS *)left, (const CLASS *)right);            \
  }                                                                            \
  static inline TCALLBACK(Comparison) NS(CLASS, N, AsComparator)(void) {       \
    return NS(CALLBACK_FUNC(Comparison),                                       \
              Bind)(NS(CLASS, N, ComparatorAdapter), NULL);                    \
  }

#define COSMERON_MACRO_INTERNAL_OBJ_CMP_ADAPTER_0(CLASS, N)

#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_INLINE_PROC(CLASS, CAP, N, P, ARGS)  \
  static inline OPSTATUS NS(CLASS, N, Subscribe)(                              \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener) {                        \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (!CALLBACK_IS_VALID(listener))                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (self->NS(event, N).count >= COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)) {  \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    self->NS(event, N).listeners[self->NS(event, N).count++] = listener;       \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, N, Unsubscribe)(                            \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener) {                        \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    for (uint8_t i = 0; i < self->NS(event, N).count; ++i) {                   \
      if (self->NS(event, N).listeners[i].function == listener.function &&     \
          self->NS(event, N).listeners[i].context == listener.context) {       \
        for (uint8_t j = i; j + 1 < self->NS(event, N).count; ++j) {           \
          self->NS(event, N).listeners[j] =                                    \
              self->NS(event, N).listeners[j + 1];                             \
        }                                                                      \
        self->NS(event, N).count--;                                            \
        return STATUS_CONST(SUCCESS);                                          \
      }                                                                        \
    }                                                                          \
    return STATUS_CONST(NOT_FOUND);                                            \
  }                                                                            \
  static inline void NS(CLASS, N, Emit)                                        \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, 0, P) {                   \
    if (!self || self->NS(event, N).count == 0)                                \
      return;                                                                  \
    uint8_t activeCount = self->NS(event, N).count;                            \
    TCALLBACK(NS(CLASS, N)) snapshot[COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)];  \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      snapshot[i] = self->NS(event, N).listeners[i];                           \
    }                                                                          \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      COSMERON_MACRO_INTERNAL_EV_INVOKE(snapshot[i], ARGS);                    \
    }                                                                          \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, N, Clear)(CLASS * self) {                   \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->NS(event, N).count = 0;                                              \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline uint8_t NS(CLASS, N, Count)(const CLASS *self) {               \
    return self ? self->NS(event, N).count : 0;                                \
  }                                                                            \
  static inline uint8_t NS(CLASS, N, Capacity)(const CLASS *self) {            \
    (void)self;                                                                \
    return (uint8_t)COSMERON_EVENT_NORMALIZE_CAPACITY(CAP);                    \
  }

#define COSMERON_MACRO_INTERNAL_OBJ_EVENT_INLINE_OP(CLASS, CAP, N, P, ARGS)    \
  static inline OPSTATUS NS(CLASS, N, Subscribe)(                              \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener) {                        \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (!CALLBACK_IS_VALID(listener))                                          \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (self->NS(event, N).count >= COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)) {  \
      return STATUS_CONST(OUT_OF_RANGE);                                       \
    }                                                                          \
    self->NS(event, N).listeners[self->NS(event, N).count++] = listener;       \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, N, Unsubscribe)(                            \
      CLASS * self, TCALLBACK(NS(CLASS, N)) listener) {                        \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    for (uint8_t i = 0; i < self->NS(event, N).count; ++i) {                   \
      if (self->NS(event, N).listeners[i].function == listener.function &&     \
          self->NS(event, N).listeners[i].context == listener.context) {       \
        for (uint8_t j = i; j + 1 < self->NS(event, N).count; ++j) {           \
          self->NS(event, N).listeners[j] =                                    \
              self->NS(event, N).listeners[j + 1];                             \
        }                                                                      \
        self->NS(event, N).count--;                                            \
        return STATUS_CONST(SUCCESS);                                          \
      }                                                                        \
    }                                                                          \
    return STATUS_CONST(NOT_FOUND);                                            \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, N, Emit)                                    \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, 0, P) {                   \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (self->NS(event, N).count == 0)                                         \
      return STATUS_CONST(SUCCESS);                                            \
    uint8_t activeCount = self->NS(event, N).count;                            \
    TCALLBACK(NS(CLASS, N)) snapshot[COSMERON_EVENT_NORMALIZE_CAPACITY(CAP)];  \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      snapshot[i] = self->NS(event, N).listeners[i];                           \
    }                                                                          \
    for (uint8_t i = 0; i < activeCount; ++i) {                                \
      OPSTATUS status = COSMERON_MACRO_INTERNAL_EV_INVOKE(snapshot[i], ARGS);  \
      if (status != STATUS_CONST(SUCCESS)) {                                   \
        return status;                                                         \
      }                                                                        \
    }                                                                          \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, N, Clear)(CLASS * self) {                   \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->NS(event, N).count = 0;                                              \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  static inline uint8_t NS(CLASS, N, Count)(const CLASS *self) {               \
    return self ? self->NS(event, N).count : 0;                                \
  }                                                                            \
  static inline uint8_t NS(CLASS, N, Capacity)(const CLASS *self) {            \
    (void)self;                                                                \
    return (uint8_t)COSMERON_EVENT_NORMALIZE_CAPACITY(CAP);                    \
  }

#define COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_0(CLASS, C, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_1(CLASS, C, R, N, P, B)      \
  static inline R NS(CLASS, N)                                                 \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, C, P)                     \
          B PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJ_CMP_ADAPTER_,               \
                       COSMERON_MACRO_INTERNAL_IS_CMP_TYPE(R))(CLASS, N)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_C_0(CLASS, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_, I)(CLASS, 0, R, N, P, \
                                                            B)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_C_1(CLASS, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_, I)(CLASS, 1, R, N, P, \
                                                            B)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_C_2(CLASS, I, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_C_3(CLASS, I, R, N, P, B)    \
  COSMERON_MACRO_INTERNAL_OBJ_EVENT_INLINE_PROC(CLASS, I, N, P, B)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_C_4(CLASS, I, R, N, P, B)    \
  COSMERON_MACRO_INTERNAL_OBJ_EVENT_INLINE_OP(CLASS, I, N, P, B)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_I(CLASS, C, I, R, N, P, B)   \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_C_, C)(CLASS, I, R, N,  \
                                                              P, B)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE(CLASS, M)                    \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(                                       \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_I, CLASS, M)

/* ------------------------------------------------------------
 * 3. Top-Level OBJECT Declarator
 * ------------------------------------------------------------ */

#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) &&                                 \
    !defined(MODULE_DISABLE_FUNCTION_TABLE)

#define OBJECT_DEF(CLASS, FIELDS_BLOCK, ...)                                   \
  typedef struct CLASS CLASS;                                                  \
  typedef struct NS(CLASS, FunctionTable) NS(CLASS, FunctionTable);            \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct NS(CLASS, FunctionTable) {                                            \
    int _reserved;                                                             \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJ_FT_FIELD, CLASS, __VA_ARGS__)   \
  };                                                                           \
  struct CLASS {                                                               \
    const struct NS(CLASS, FunctionTable) * api;                               \
    FIELDS_BLOCK                                                               \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_EVENT_FIELD, CLASS,      \
                   __VA_ARGS__)                                                \
  };                                                                           \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS,             \
                 __VA_ARGS__)                                                  \
  static inline const struct NS(CLASS, FunctionTable) *                        \
      NS(CLASS, GetFunctionTable)(void) {                                      \
    static const struct NS(CLASS, FunctionTable)                               \
        table = {._reserved = 0,                                               \
                 PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJ_FT_ENTRY, CLASS,   \
                                __VA_ARGS__)};                                 \
    return &table;                                                             \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, Init)(CLASS * self) {                       \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->api = NS(CLASS, GetFunctionTable)();                                 \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_INIT_EVENT, CLASS,           \
                   __VA_ARGS__)                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int NS(_cosmeron_object_semi, CLASS, __LINE__)

#define OBJECT(CLASS, FIELDS_BLOCK, ...)                                       \
  OBJECT_DEF(CLASS, FIELDS_BLOCK, __VA_ARGS__)

#define OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS_BLOCK, ...)                   \
  typedef struct CLASS CLASS;                                                  \
  typedef struct NS(CLASS, FunctionTable) NS(CLASS, FunctionTable);            \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct NS(CLASS, FunctionTable) {                                            \
    int _reserved;                                                             \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJ_FT_FIELD, CLASS, __VA_ARGS__)   \
  };                                                                           \
  struct CLASS {                                                               \
    BASE_CLASS base;                                                           \
    const struct NS(CLASS, FunctionTable) * api;                               \
    FIELDS_BLOCK                                                               \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_EVENT_FIELD, CLASS,      \
                   __VA_ARGS__)                                                \
  };                                                                           \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS,             \
                 __VA_ARGS__)                                                  \
  static inline const struct NS(CLASS, FunctionTable) *                        \
      NS(CLASS, GetFunctionTable)(void) {                                      \
    static const struct NS(CLASS, FunctionTable)                               \
        table = {._reserved = 0,                                               \
                 PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJ_FT_ENTRY, CLASS,   \
                                __VA_ARGS__)};                                 \
    return &table;                                                             \
  }                                                                            \
  static inline OPSTATUS NS(CLASS, Init)(CLASS * self) {                       \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->api = NS(CLASS, GetFunctionTable)();                                 \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_INIT_EVENT, CLASS,           \
                   __VA_ARGS__)                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int NS(_cosmeron_object_semi, CLASS, __LINE__)

#else

#define OBJECT_DEF(CLASS, FIELDS_BLOCK, ...)                                   \
  typedef struct CLASS CLASS;                                                  \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct CLASS {                                                               \
    FIELDS_BLOCK                                                               \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_EVENT_FIELD, CLASS,      \
                   __VA_ARGS__)                                                \
  };                                                                           \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS,             \
                 __VA_ARGS__)                                                  \
  static inline OPSTATUS NS(CLASS, Init)(CLASS * self) {                       \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_INIT_EVENT, CLASS,           \
                   __VA_ARGS__)                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int NS(_cosmeron_object_semi, CLASS, __LINE__)

#define OBJECT(CLASS, FIELDS_BLOCK, ...)                                       \
  OBJECT_DEF(CLASS, FIELDS_BLOCK, __VA_ARGS__)

#define OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS_BLOCK, ...)                   \
  typedef struct CLASS CLASS;                                                  \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct CLASS {                                                               \
    BASE_CLASS base;                                                           \
    FIELDS_BLOCK                                                               \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_EVENT_FIELD, CLASS,      \
                   __VA_ARGS__)                                                \
  };                                                                           \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS,             \
                 __VA_ARGS__)                                                  \
  static inline OPSTATUS NS(CLASS, Init)(CLASS * self) {                       \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_INIT_EVENT, CLASS,           \
                   __VA_ARGS__)                                                \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int NS(_cosmeron_object_semi, CLASS, __LINE__)

#endif

/* ------------------------------------------------------------
 * 4. Legacy / Manual Form (Backwards Compatibility)
 * ------------------------------------------------------------ */

#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) &&                                 \
    !defined(MODULE_DISABLE_FUNCTION_TABLE)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT(CLASS, METHODS)                  \
  typedef struct NS(CLASS, FunctionTable) {                                    \
    METHODS(COSMERON_MACRO_INTERNAL_OBJECT_FT_FIELD, CLASS)                    \
  } NS(CLASS, FunctionTable);

#define COSMERON_MACRO_INTERNAL_OBJECT_FT_FIELD(METHOD_NAME, CLASS)            \
  __typeof__(NS(CLASS, METHOD_NAME)) *METHOD_NAME;

#define COSMERON_MACRO_INTERNAL_OBJECT_API_FIELD(CLASS)                        \
  const struct NS(CLASS, FunctionTable) * api;

#define COSMERON_MACRO_INTERNAL_OBJECT_FT_ENTRY(METHOD_NAME, CLASS)            \
  .METHOD_NAME = NS(CLASS, METHOD_NAME),

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INSTANCE(CLASS)                     \
  static const struct NS(CLASS, FunctionTable) NS(CLASS, functions) = {        \
      NS(CLASS, METHODS)(COSMERON_MACRO_INTERNAL_OBJECT_FT_ENTRY, CLASS)};

#define COSMERON_MACRO_INTERNAL_OBJECT_SET_API(CLASS, SELF)                    \
  (SELF)->api = &NS(CLASS, functions);

#else

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT(CLASS, METHODS)
#define COSMERON_MACRO_INTERNAL_OBJECT_API_FIELD(CLASS) char _unused;
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INSTANCE(CLASS)
#define COSMERON_MACRO_INTERNAL_OBJECT_SET_API(CLASS, SELF)

#endif

#define COSMERON_MACRO_INTERNAL_OBJECT_DECL_PROTOS(CLASS, METHODS)             \
  METHODS(COSMERON_MACRO_INTERNAL_OBJECT_PROTO_DECL, CLASS)

#define COSMERON_MACRO_INTERNAL_OBJECT_PROTO_DECL(METHOD_NAME, CLASS)          \
  typedef void NS(CLASS, METHOD_NAME, ret_t);                                  \
  typedef CLASS NS(CLASS, METHOD_NAME, self_t);                                \
  struct NS(CLASS, METHOD_NAME, sig) {                                         \
    int _unused;                                                               \
  };                                                                           \
  NS(CLASS, METHOD_NAME, PROTOTYPE);

#define OBJECT_STRUCT(CLASS, ...)                                              \
  typedef struct CLASS CLASS;                                                  \
  COSMERON_MACRO_INTERNAL_OBJECT_DECL_PROTOS(CLASS, NS(CLASS, METHODS))        \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT(CLASS, NS(CLASS, METHODS))             \
  struct CLASS {                                                               \
    COSMERON_MACRO_INTERNAL_OBJECT_API_FIELD(CLASS)                            \
    __VA_ARGS__                                                                \
  };                                                                           \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_INSTANCE(CLASS)                           \
  static inline OPSTATUS NS(CLASS, Init)(CLASS * self) {                       \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    COSMERON_MACRO_INTERNAL_OBJECT_SET_API(CLASS, self)                        \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int NS(_cosmeron_object_struct_semi, CLASS, __LINE__)

/* ------------------------------------------------------------
 * 5. Object Implementation Finalizer
 * ------------------------------------------------------------ */

#define OBJECT_IMPLEMENT(CLASS)                                                \
  typedef int NS(_cosmeron_object_implemented, CLASS, __LINE__)

#define OBJECT_IMPLEMENT_INLINE(CLASS) OBJECT_IMPLEMENT(CLASS)

/* EOF */
