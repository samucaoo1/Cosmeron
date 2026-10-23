#pragma once

#include "Module.space"
#include "Common/Tuple.impl"
#include "Common/FunctionTable.impl"
#include "Common/Dispatch.impl"
#include "Common/Template.impl"

/* ============================================================
 * Object Declarative Framework (Core/Module/Object.h)
 * Handles object structs, receivers, inline methods, and function tables.
 * ============================================================ */

#define OBJECT_MOD Object
#define OBJECT_CMOD OBJECT

#define OBJECT_NS(NAME) GNS2(LIB_PREFIX(OBJECT_MOD), NAME)
#define OBJECT_CNS(NAME) CNS2(LIB_PREFIX_CONST(OBJECT_CMOD), NAME)
#define OBJECT_INS(NAME) GNS3(LIB_PREFIX(OBJECT_MOD), Internal, NAME)
#define OBJECT_CINS(NAME) CNS3(LIB_PREFIX_CONST(OBJECT_CMOD), INTERNAL, NAME)

#define OBJECT_TYPE(NAME) OBJECT_NS(NAME)
#define OBJECT_FUNC(NAME) OBJECT_NS(NAME)
#define OBJECT_CONST(NAME) OBJECT_CNS(NAME)

/* ============================================================
 * Base TObject
 * ============================================================ */

#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)
typedef struct OBJECT_TYPE(TObject_FunctionTable) {
  OPSTATUS (*destroy)(void *self);
  OPSTATUS (*toString)(const void *self, char *dest, size_t capacity);
  bool (*equals)(const void *self, const void *other);
  uint64_t (*hash)(const void *self);
} OBJECT_TYPE(TObject_FunctionTable);

#define OBJECT_API_MEMBER const struct OBJECT_TYPE(TObject_FunctionTable) * api;
#define OBJECT_API_INIT(OBJ, TABLE) (OBJ)->api = (TABLE);
#else
#define OBJECT_API_MEMBER char _unused;
#define OBJECT_API_INIT(OBJ, TABLE)
#endif

typedef struct OBJECT_TYPE(TObject) {
  OBJECT_API_MEMBER
} OBJECT_TYPE(TObject);

typedef OBJECT_TYPE(TObject) TObject;

/* ------------------------------------------------------------
 * Base TObject Inline Implementations
 * ------------------------------------------------------------ */

static inline OPSTATUS OBJECT_FUNC(Destroy)(void *object) {
  if (!object) return STATUS_CONST(INVALID_ARGUMENT);
  return STATUS_CONST(SUCCESS);
}

static inline OPSTATUS OBJECT_FUNC(ToString)(const void *object, char *dest, size_t capacity) {
  if (!object || !dest || capacity == 0U) return STATUS_CONST(INVALID_ARGUMENT);
  dest[0] = '\0';
  return STATUS_CONST(SUCCESS);
}

static inline bool OBJECT_FUNC(Equals)(const void *left, const void *right) {
  return left == right;
}

static inline uint64_t OBJECT_FUNC(Hash)(const void *object) {
  return (uint64_t)(uintptr_t)object;
}

#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)
static const OBJECT_TYPE(TObject_FunctionTable) OBJECT_FUNC(functions) = {
    .destroy = OBJECT_FUNC(Destroy),
    .toString = OBJECT_FUNC(ToString),
    .equals = OBJECT_FUNC(Equals),
    .hash = OBJECT_FUNC(Hash)
};
#endif

static inline OPSTATUS OBJECT_FUNC(Init)(OBJECT_TYPE(TObject) * object) {
  if (!object) return STATUS_CONST(INVALID_ARGUMENT);
#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)
  object->api = &OBJECT_FUNC(functions);
#endif
  return STATUS_CONST(SUCCESS);
}

/* ------------------------------------------------------------
 * 1. Invocation & Casting Helpers
 * ------------------------------------------------------------ */

#define OBJECT_AS(OBJ, TARGET_TYPE) ((TARGET_TYPE *)(OBJ))

#define OBJECT_CALL_0(OBJ, METHOD_NAME) ((OBJ)->api->METHOD_NAME(OBJ))

#define OBJECT_CALL(OBJ, METHOD_NAME, ...) \
  ((OBJ)->api->METHOD_NAME((OBJ), __VA_ARGS__))

/* ------------------------------------------------------------
 * 2. Internal Method Generators (Prototypes & Inlines)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_I(CLASS, C, I, R, N, P, B) \
  typedef R GNS3(CLASS, N, ret_t);                                           \
  typedef PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PARAM_SELF_TYPE_, C)(CLASS)     \
      GNS3(CLASS, N, self_t);                                                \
  struct GNS3(CLASS, N, sig) {                                               \
    PP_PARAMS_TO_FIELDS(P)                                                   \
  };                                                                         \
  static inline R GNS2(CLASS, N)                                             \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, C, P);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO(CLASS, M) \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_I, CLASS, M)

#define COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_0(CLASS, C, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_1(CLASS, C, R, N, P, B) \
  static inline R GNS2(CLASS, N)                                           \
      COSMERON_MACRO_INTERNAL_OBJ_EMIT_PARAMS(CLASS, C, P) B

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_I(CLASS, C, I, R, N, P, B) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_INLINE_DEF_, I)(CLASS, C, R, N, P, B)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE(CLASS, M) \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_I, CLASS, M)

/* ------------------------------------------------------------
 * 3. Top-Level OBJECT Declarator
 * ------------------------------------------------------------ */

#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)

#define OBJECT(CLASS, FIELDS_BLOCK, ...)                                       \
  typedef struct CLASS CLASS;                                                  \
  typedef struct GNS2(CLASS, FunctionTable) GNS2(CLASS, FunctionTable);        \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct GNS2(CLASS, FunctionTable) {                                          \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJ_FT_FIELD, CLASS, __VA_ARGS__)   \
  };                                                                           \
  struct CLASS {                                                               \
    const struct GNS2(CLASS, FunctionTable) * api;                             \
    FIELDS_BLOCK                                                               \
  };                                                                           \
  PP_FOREACH_CTX(                                                              \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS, __VA_ARGS__)          \
  static inline const struct GNS2(CLASS, FunctionTable) *GNS2(                 \
      CLASS, GetFunctionTable)(void) {                                         \
    static const struct GNS2(CLASS, FunctionTable) table = {                   \
        PP_FOREACH_CTX(                                                        \
            COSMERON_MACRO_INTERNAL_OBJ_FT_ENTRY, CLASS, __VA_ARGS__)};        \
    return &table;                                                             \
  }                                                                            \
  static inline OPSTATUS GNS2(CLASS, Init)(CLASS * self) {                     \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->api = GNS2(CLASS, GetFunctionTable)();                               \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int GNS3(_cosmeron_object_semi, CLASS, __LINE__)

#define OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS_BLOCK, ...)                   \
  typedef struct CLASS CLASS;                                                  \
  typedef struct GNS2(CLASS, FunctionTable) GNS2(CLASS, FunctionTable);        \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct GNS2(CLASS, FunctionTable) {                                          \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJ_FT_FIELD, CLASS, __VA_ARGS__)   \
  };                                                                           \
  struct CLASS {                                                               \
    BASE_CLASS base;                                                           \
    const struct GNS2(CLASS, FunctionTable) * api;                             \
    FIELDS_BLOCK                                                               \
  };                                                                           \
  PP_FOREACH_CTX(                                                              \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS, __VA_ARGS__)          \
  static inline const struct GNS2(CLASS, FunctionTable) *GNS2(                 \
      CLASS, GetFunctionTable)(void) {                                         \
    static const struct GNS2(CLASS, FunctionTable) table = {                   \
        PP_FOREACH_CTX(                                                        \
            COSMERON_MACRO_INTERNAL_OBJ_FT_ENTRY, CLASS, __VA_ARGS__)};        \
    return &table;                                                             \
  }                                                                            \
  static inline OPSTATUS GNS2(CLASS, Init)(CLASS * self) {                     \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->api = GNS2(CLASS, GetFunctionTable)();                               \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int GNS3(_cosmeron_object_semi, CLASS, __LINE__)

#else

#define OBJECT(CLASS, FIELDS_BLOCK, ...)                                       \
  typedef struct CLASS CLASS;                                                  \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct CLASS {                                                               \
    char _unused;                                                              \
    FIELDS_BLOCK                                                               \
  };                                                                           \
  PP_FOREACH_CTX(                                                              \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS, __VA_ARGS__)          \
  static inline OPSTATUS GNS2(CLASS, Init)(CLASS * self) {                     \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int GNS3(_cosmeron_object_semi, CLASS, __LINE__)

#define OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS_BLOCK, ...)                   \
  typedef struct CLASS CLASS;                                                  \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct CLASS {                                                               \
    BASE_CLASS base;                                                           \
    char _unused;                                                              \
    FIELDS_BLOCK                                                               \
  };                                                                           \
  PP_FOREACH_CTX(                                                              \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE, CLASS, __VA_ARGS__)          \
  static inline OPSTATUS GNS2(CLASS, Init)(CLASS * self) {                     \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int GNS3(_cosmeron_object_semi, CLASS, __LINE__)

#endif

/* ------------------------------------------------------------
 * 4. Legacy / Manual Form (Backwards Compatibility)
 * ------------------------------------------------------------ */

#if !defined(OBJECT_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT(CLASS, METHODS)                  \
  typedef struct GNS2(CLASS, FunctionTable) {                                  \
    METHODS(COSMERON_MACRO_INTERNAL_OBJECT_FT_FIELD, CLASS)                    \
  } GNS2(CLASS, FunctionTable);

#define COSMERON_MACRO_INTERNAL_OBJECT_FT_FIELD(METHOD_NAME, CLASS)            \
  __typeof__(GNS2(CLASS, METHOD_NAME)) *METHOD_NAME;

#define COSMERON_MACRO_INTERNAL_OBJECT_API_FIELD(CLASS)                        \
  const struct GNS2(CLASS, FunctionTable) * api;

#define COSMERON_MACRO_INTERNAL_OBJECT_FT_ENTRY(METHOD_NAME, CLASS)            \
  .METHOD_NAME = GNS2(CLASS, METHOD_NAME),

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INSTANCE(CLASS)                     \
  static const struct GNS2(CLASS, FunctionTable)                               \
      GNS2(CLASS, functions) = {GNS2(CLASS, METHODS)(                          \
          COSMERON_MACRO_INTERNAL_OBJECT_FT_ENTRY, CLASS)};

#define COSMERON_MACRO_INTERNAL_OBJECT_SET_API(CLASS, SELF)                    \
  (SELF)->api = &GNS2(CLASS, functions);

#else

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT(CLASS, METHODS)
#define COSMERON_MACRO_INTERNAL_OBJECT_API_FIELD(CLASS) char _unused;
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INSTANCE(CLASS)
#define COSMERON_MACRO_INTERNAL_OBJECT_SET_API(CLASS, SELF)

#endif

#define COSMERON_MACRO_INTERNAL_OBJECT_DECL_PROTOS(CLASS, METHODS)             \
  METHODS(COSMERON_MACRO_INTERNAL_OBJECT_PROTO_DECL, CLASS)

#define COSMERON_MACRO_INTERNAL_OBJECT_PROTO_DECL(METHOD_NAME, CLASS)          \
  typedef void GNS3(CLASS, METHOD_NAME, ret_t);                                \
  typedef CLASS GNS3(CLASS, METHOD_NAME, self_t);                              \
  struct GNS3(CLASS, METHOD_NAME, sig) { int _unused; };                       \
  GNS3(CLASS, METHOD_NAME, PROTOTYPE);

#define OBJECT_STRUCT(CLASS, ...)                                              \
  typedef struct CLASS CLASS;                                                  \
  COSMERON_MACRO_INTERNAL_OBJECT_DECL_PROTOS(CLASS,                            \
                                             GNS2(CLASS, METHODS))             \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT(CLASS, GNS2(CLASS, METHODS))           \
  struct CLASS {                                                               \
    COSMERON_MACRO_INTERNAL_OBJECT_API_FIELD(CLASS)                            \
    __VA_ARGS__                                                                \
  };                                                                           \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_INSTANCE(CLASS)                           \
  static inline OPSTATUS GNS2(CLASS, Init)(CLASS * self) {                     \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    COSMERON_MACRO_INTERNAL_OBJECT_SET_API(CLASS, self)                        \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int GNS3(_cosmeron_object_struct_semi, CLASS, __LINE__)

/* ------------------------------------------------------------
 * 5. Object Implementation Finalizer
 * ------------------------------------------------------------ */

#define OBJECT_IMPLEMENT(CLASS) \
  typedef int GNS3(_cosmeron_object_implemented, CLASS, __LINE__)

#define OBJECT_IMPLEMENT_INLINE(CLASS) OBJECT_IMPLEMENT(CLASS)

/* EOF */
