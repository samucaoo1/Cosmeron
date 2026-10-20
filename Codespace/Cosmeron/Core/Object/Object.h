#pragma once

#include "Object.space"

/* ============================================================
 * Base TObject
 * ============================================================ */

#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_FUNCTION_TABLE_FORWARD struct OBJECT_TYPE(TObject_FunctionTable);
#define OBJECT_API_MEMBER const struct OBJECT_TYPE(TObject_FunctionTable) * api;
#define OBJECT_API_INIT(OBJ, TABLE) (OBJ)->api = (TABLE);
#else
#define OBJECT_FUNCTION_TABLE_FORWARD
#define OBJECT_API_MEMBER char _unused;
#define OBJECT_API_INIT(OBJ, TABLE)
#endif

typedef struct OBJECT_TYPE(TObject) {
  OBJECT_API_MEMBER
} OBJECT_TYPE(TObject);

typedef OBJECT_TYPE(TObject) TObject;

/* ============================================================
 * Base TObject Prototypes
 * ============================================================ */

#define OBJECT_INIT_PROTOTYPE                                                  \
  static inline OPSTATUS OBJECT_FUNC(Init)(OBJECT_TYPE(TObject) * object)

#define OBJECT_DESTROY_PROTOTYPE                                               \
  static inline OPSTATUS OBJECT_FUNC(Destroy)(void *object)

#define OBJECT_TO_STRING_PROTOTYPE                                             \
  static inline OPSTATUS OBJECT_FUNC(ToString)(const void *object, char *dest, \
                                               size_t capacity)

#define OBJECT_EQUALS_PROTOTYPE                                                \
  static inline bool OBJECT_FUNC(Equals)(const void *left, const void *right)

#define OBJECT_HASH_PROTOTYPE                                                  \
  static inline uint64_t OBJECT_FUNC(Hash)(const void *object)

OBJECT_INIT_PROTOTYPE;
OBJECT_DESTROY_PROTOTYPE;
OBJECT_TO_STRING_PROTOTYPE;
OBJECT_EQUALS_PROTOTYPE;
OBJECT_HASH_PROTOTYPE;

#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#include "FunctionTable/Object.h"
#endif

#include "Impl/Object.impl"

#ifndef OBJECT_DISABLE_FUNCTION_TABLE
OBJECT_FUNCTION_TABLE_INSTANCE
#endif

/* ============================================================
 * Object-Oriented Macros & Generators
 * ============================================================ */

#define OBJECT_BASE(CLASS) const struct GNS2(CLASS, FunctionTable) * api

#define OBJECT_AS(OBJ, TARGET_TYPE) ((TARGET_TYPE *)(OBJ))

#define OBJECT_CALL_0(OBJ, METHOD_NAME) ((OBJ)->api->METHOD_NAME(OBJ))

#define OBJECT_CALL(OBJ, METHOD_NAME, ...)                                     \
  ((OBJ)->api->METHOD_NAME((OBJ), __VA_ARGS__))

/* ------------------------------------------------------------
 * Unified Declarative OOP Syntax (Alternativa B)
 * ------------------------------------------------------------ */

#define FIELDS(...) __VA_ARGS__

/* Tuple: (IS_CONST, IS_INLINE, RET, NAME, PARAMS, BODY) */
#define METHOD(RET, NAME, PARAMS) (0, 0, RET, NAME, PARAMS, { (void)0; })
#define METHOD_CONST(RET, NAME, PARAMS) (1, 0, RET, NAME, PARAMS, { (void)0; })
#define CONST_METHOD(RET, NAME, PARAMS) (1, 0, RET, NAME, PARAMS, { (void)0; })

#define METHOD_INLINE(RET, NAME, PARAMS, BODY) (0, 1, RET, NAME, PARAMS, BODY)
#define METHOD_CONST_INLINE(RET, NAME, PARAMS, BODY)                           \
  (1, 1, RET, NAME, PARAMS, BODY)
#define CONST_METHOD_INLINE(RET, NAME, PARAMS, BODY)                           \
  (1, 1, RET, NAME, PARAMS, BODY)

#define COSMERON_MACRO_INTERNAL_PARAM_SELF_0(CLASS) CLASS *self
#define COSMERON_MACRO_INTERNAL_PARAM_SELF_1(CLASS) const CLASS *self

#define COSMERON_MACRO_INTERNAL_PARAM_SELF_TYPE_0(CLASS) CLASS
#define COSMERON_MACRO_INTERNAL_PARAM_SELF_TYPE_1(CLASS) const CLASS

#define COSMERON_MACRO_INTERNAL_PARAM_EMIT_EMPTY_1(CLASS, IS_CONST, PARAMS)    \
  (PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PARAM_SELF_, IS_CONST)(CLASS))

#define COSMERON_MACRO_INTERNAL_PARAM_EMIT_EMPTY_0(CLASS, IS_CONST, PARAMS)    \
  (PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PARAM_SELF_, IS_CONST)(CLASS),           \
   PP_UNWRAP PARAMS)

#define COSMERON_MACRO_INTERNAL_EMIT_PARAMS(CLASS, IS_CONST, PARAMS)           \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PARAM_EMIT_EMPTY_,                        \
             PP_IS_EMPTY_PARAMS(PARAMS))(CLASS, IS_CONST, PARAMS)

/* ------------------------------------------------------------
 * OBJECT_PROTOTYPE (Introspection with __typeof__)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PROTO_ARG(CLASS_METHOD, ARG)                  \
  , __typeof__(((struct GNS2(CLASS_METHOD, sig) *)0)->ARG) ARG

#define COSMERON_MACRO_INTERNAL_PROTO_DISPATCH_0(CLASS, METHOD)                \
  GNS3(CLASS, METHOD, ret_t)                                                   \
  GNS2(CLASS, METHOD)(GNS3(CLASS, METHOD, self_t) * self)

#define COSMERON_MACRO_INTERNAL_PROTO_DISPATCH_1(CLASS, METHOD, ...)           \
  GNS3(CLASS, METHOD, ret_t)                                                   \
  GNS2(CLASS, METHOD)(GNS3(CLASS, METHOD, self_t) * self                       \
                      PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PROTO_ARG,        \
                                     GNS2(CLASS, METHOD), __VA_ARGS__))

#define OBJECT_PROTOTYPE(...)                                                  \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PROTO_DISPATCH_,                          \
             PP_HAS_MORE_THAN_2(__VA_ARGS__))(__VA_ARGS__)

/* ------------------------------------------------------------
 * OBJECT_PROTOTYPEEX (Does NOT require __typeof__)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PROTOEX_EMIT_PARAMS_EMPTY_1(CLASS, METHOD,     \
                                                            ...)               \
  (GNS3(CLASS, METHOD, self_t) * self)

#define COSMERON_MACRO_INTERNAL_PROTOEX_EMIT_PARAMS_EMPTY_0(CLASS, METHOD,     \
                                                            PARAMS)            \
  (GNS3(CLASS, METHOD, self_t) * self, PP_UNWRAP PARAMS)

#define COSMERON_MACRO_INTERNAL_PROTOEX_EMIT_PARAMS(CLASS, METHOD, PARAMS)     \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PROTOEX_EMIT_PARAMS_EMPTY_,               \
             PP_IS_EMPTY_PARAMS(PARAMS))(CLASS, METHOD, PARAMS)

#define COSMERON_MACRO_INTERNAL_PROTOEX_2(CLASS, METHOD)                       \
  GNS3(CLASS, METHOD, ret_t)                                                   \
  GNS2(CLASS, METHOD)                                                         \
      COSMERON_MACRO_INTERNAL_PROTOEX_EMIT_PARAMS(CLASS, METHOD, ())

#define COSMERON_MACRO_INTERNAL_PROTOEX_3(CLASS, METHOD, PARAMS)               \
  GNS3(CLASS, METHOD, ret_t)                                                   \
  GNS2(CLASS, METHOD)                                                         \
      COSMERON_MACRO_INTERNAL_PROTOEX_EMIT_PARAMS(CLASS, METHOD, PARAMS)

#define OBJECT_PROTOTYPEEX(...)                                                \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PROTOEX_,                                 \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/* Forward declarations */
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO(CLASS, M)                     \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_E(CLASS, PP_UNWRAP M)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_E(CLASS, ...)                 \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_I(CLASS, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_I(CLASS, C, I, R, N, P, B)    \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_, I)(CLASS, C, R, N, P)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_0(CLASS, C, R, N, P)          \
  typedef R GNS3(CLASS, N, ret_t);                                             \
  typedef PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PARAM_SELF_TYPE_, C)(CLASS)       \
      GNS3(CLASS, N, self_t);                                                  \
  struct GNS3(CLASS, N, sig) {                                                 \
    PP_PARAMS_TO_FIELDS(P)                                                     \
  };                                                                           \
  R GNS2(CLASS, N) COSMERON_MACRO_INTERNAL_EMIT_PARAMS(CLASS, C, P);

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO_1(CLASS, C, R, N, P)          \
  typedef R GNS3(CLASS, N, ret_t);                                             \
  typedef PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PARAM_SELF_TYPE_, C)(CLASS)       \
      GNS3(CLASS, N, self_t);                                                  \
  struct GNS3(CLASS, N, sig) {                                                 \
    PP_PARAMS_TO_FIELDS(P)                                                     \
  };                                                                           \
  static inline R GNS2(CLASS, N)                                               \
      COSMERON_MACRO_INTERNAL_EMIT_PARAMS(CLASS, C, P);

/* Inline implementation definitions */
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF(CLASS, M)                \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_E(CLASS, PP_UNWRAP M)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_E(CLASS, ...)            \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_I(CLASS, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_I(CLASS, C, I, R, N, P,  \
                                                        B)                     \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_, I)(CLASS, C, R,   \
                                                                N, P, B)

#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_0(CLASS, C, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF_1(CLASS, C, R, N, P, B)  \
  static inline R GNS2(CLASS, N)                                               \
      COSMERON_MACRO_INTERNAL_EMIT_PARAMS(CLASS, C, P) B

/* FunctionTable field */
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD(CLASS, M)                  \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD_E(CLASS, PP_UNWRAP M)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD_E(CLASS, ...)              \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD_I(CLASS, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD_I(CLASS, C, I, R, N, P, B) \
  R(*N) COSMERON_MACRO_INTERNAL_EMIT_PARAMS(CLASS, C, P);

/* FunctionTable designated initializer entry */
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY(CLASS, M)                  \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY_E(CLASS, PP_UNWRAP M)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY_E(CLASS, ...)              \
  COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY_I(CLASS, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY_I(CLASS, C, I, R, N, P, B) \
  .N = GNS2(CLASS, N),

#define COSMERON_MACRO_INTERNAL_OBJECT_FOREACH_METHOD(macro, CLASS, ...)      \
  PP_FOREACH_CTX(macro, CLASS, __VA_ARGS__)

#ifndef OBJECT_DISABLE_FUNCTION_TABLE

#define OBJECT(CLASS, FIELDS_BLOCK, ...)                                       \
  typedef struct CLASS CLASS;                                                  \
  typedef struct GNS2(CLASS, FunctionTable) GNS2(CLASS, FunctionTable);        \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_OBJECT_GEN_PROTO, CLASS, __VA_ARGS__) \
  struct GNS2(CLASS, FunctionTable) {                                          \
    PP_FOREACH_CTX(                                                            \
        COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD, CLASS, __VA_ARGS__)       \
  };                                                                           \
  struct CLASS {                                                               \
    const struct GNS2(CLASS, FunctionTable) * api;                             \
    FIELDS_BLOCK                                                               \
  };                                                                           \
  PP_FOREACH_CTX(                                                              \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF, CLASS, __VA_ARGS__)       \
  static inline const struct GNS2(CLASS, FunctionTable) *GNS2(                 \
      CLASS, GetFunctionTable)(void) {                                         \
    static const struct GNS2(CLASS, FunctionTable) table = {                   \
        PP_FOREACH_CTX(                                                        \
            COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY, CLASS, __VA_ARGS__)}; \
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
    PP_FOREACH_CTX(                                                            \
        COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_FIELD, CLASS, __VA_ARGS__)       \
  };                                                                           \
  struct CLASS {                                                               \
    BASE_CLASS base;                                                           \
    const struct GNS2(CLASS, FunctionTable) * api;                             \
    FIELDS_BLOCK                                                               \
  };                                                                           \
  PP_FOREACH_CTX(                                                              \
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF, CLASS, __VA_ARGS__)       \
  static inline const struct GNS2(CLASS, FunctionTable) *GNS2(                 \
      CLASS, GetFunctionTable)(void) {                                         \
    static const struct GNS2(CLASS, FunctionTable) table = {                   \
        PP_FOREACH_CTX(                                                        \
            COSMERON_MACRO_INTERNAL_OBJECT_GEN_FT_ENTRY, CLASS, __VA_ARGS__)}; \
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
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF, CLASS, __VA_ARGS__)       \
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
      COSMERON_MACRO_INTERNAL_OBJECT_GEN_INLINE_DEF, CLASS, __VA_ARGS__)       \
  static inline OPSTATUS GNS2(CLASS, Init)(CLASS * self) {                     \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  typedef int GNS3(_cosmeron_object_semi, CLASS, __LINE__)

#endif

/* ------------------------------------------------------------
 * Legacy / Manual X-Macro Form (Backwards Compatibility)
 * ------------------------------------------------------------ */

#ifndef OBJECT_DISABLE_FUNCTION_TABLE

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

#define OBJECT_IMPLEMENT(CLASS)                                                \
  typedef int GNS3(_cosmeron_object_implemented, CLASS, __LINE__)

#define OBJECT_IMPLEMENT_INLINE(CLASS) OBJECT_IMPLEMENT(CLASS)

/* EOF */
