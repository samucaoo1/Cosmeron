#pragma once

#include "Package.space"

/* ============================================================
 * Package Declarative Syntax
 * ============================================================ */

/* Tuple: (IS_INLINE, RET, NAME, PARAMS, BODY) */
#define FUNCTION(RET, NAME, PARAMS) (0, RET, NAME, PARAMS, { (void)0; })
#define FUNCTION_INLINE(RET, NAME, PARAMS, BODY) (1, RET, NAME, PARAMS, BODY)
#define INLINE_FUNCTION(RET, NAME, PARAMS, BODY) (1, RET, NAME, PARAMS, BODY)

#define COSMERON_MACRO_INTERNAL_PKG_PARAMS_EMPTY_1(PARAMS) (void)
#define COSMERON_MACRO_INTERNAL_PKG_PARAMS_EMPTY_0(PARAMS) PARAMS
#define COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(PARAMS)                        \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_PARAMS_EMPTY_,                        \
             PP_IS_EMPTY_PARAMS(PARAMS))(PARAMS)

/* ------------------------------------------------------------
 * 1. Declarations & Signatures Generation
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO(PKG, F)                          \
  COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO_E(PKG, PP_UNWRAP F)
#define COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO_E(PKG, ...)                      \
  COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO_I(PKG, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO_I(PKG, I, R, N, P, B)            \
  typedef R GNS3(PKG, N, ret_t);                                               \
  struct GNS3(PKG, N, sig) {                                                   \
    PP_PARAMS_TO_FIELDS(P)                                                     \
  };                                                                           \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_DECL_, I)(PKG, R, N, P)

#define COSMERON_MACRO_INTERNAL_PKG_DECL_0(PKG, R, N, P)                       \
  R GNS2(PKG, N) COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);
#define COSMERON_MACRO_INTERNAL_PKG_DECL_1(PKG, R, N, P)                       \
  static inline R GNS2(PKG, N) COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);

/* ------------------------------------------------------------
 * 2. Inline Definitions Generation
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE(PKG, F)                         \
  COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE_E(PKG, PP_UNWRAP F)
#define COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE_E(PKG, ...)                     \
  COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE_I(PKG, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE_I(PKG, I, R, N, P, B)           \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_INLINE_DEF_, I)(PKG, R, N, P, B)

#define COSMERON_MACRO_INTERNAL_PKG_INLINE_DEF_0(PKG, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_PKG_INLINE_DEF_1(PKG, R, N, P, B)              \
  static inline R GNS2(PKG, N) COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P) B

/* ------------------------------------------------------------
 * 3. FunctionTable Generation
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_FT_FIELD(PKG, F)                           \
  COSMERON_MACRO_INTERNAL_PKG_FT_FIELD_E(PKG, PP_UNWRAP F)
#define COSMERON_MACRO_INTERNAL_PKG_FT_FIELD_E(PKG, ...)                       \
  COSMERON_MACRO_INTERNAL_PKG_FT_FIELD_I(PKG, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_PKG_FT_FIELD_I(PKG, I, R, N, P, B)             \
  R (*N) COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);

#define COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY(PKG, F)                           \
  COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY_E(PKG, PP_UNWRAP F)
#define COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY_E(PKG, ...)                       \
  COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY_I(PKG, __VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY_I(PKG, I, R, N, P, B)             \
  .N = GNS2(PKG, N),

/* ------------------------------------------------------------
 * 4. PACKAGE_PROTOTYPE (Introspection with __typeof__)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_PROTO_ARG(PKG_FUNC, ARG)                  \
  __typeof__(((struct GNS2(PKG_FUNC, sig) *)0)->ARG) ARG

#define COSMERON_MACRO_INTERNAL_PKG_PROTO_DISPATCH_0(PKG, FUNC)                \
  GNS3(PKG, FUNC, ret_t) GNS2(PKG, FUNC)(void)

#define COSMERON_MACRO_INTERNAL_PKG_PROTO_DISPATCH_1(PKG, FUNC, ...)           \
  GNS3(PKG, FUNC, ret_t) GNS2(PKG, FUNC)(                                      \
      PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_PKG_PROTO_ARG,              \
                           GNS2(PKG, FUNC), __VA_ARGS__))

#define PACKAGE_PROTOTYPE(...)                                                 \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_PROTO_DISPATCH_,                      \
             PP_HAS_MORE_THAN_2(__VA_ARGS__))(__VA_ARGS__)

/* ------------------------------------------------------------
 * 5. PACKAGE_PROTOTYPEEX (Pure standard C11 without __typeof__)
 * ------------------------------------------------------------ */

#define PACKAGE_PROTOTYPEEX(PKG, FUNC, PARAMS)                                 \
  GNS3(PKG, FUNC, ret_t) GNS2(PKG, FUNC)                                       \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(PARAMS)

/* ------------------------------------------------------------
 * 6. Invocation Helpers
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_CALL_0(PKG, FUNC_NAME)                     \
  (GNS2(PKG, GetFunctionTable)()->FUNC_NAME())

#define COSMERON_MACRO_INTERNAL_PKG_CALL_1(PKG, FUNC_NAME, ...)                \
  (GNS2(PKG, GetFunctionTable)()->FUNC_NAME(__VA_ARGS__))

#define PACKAGE_CALL(...)                                                      \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_CALL_,                                \
             PP_HAS_MORE_THAN_2(__VA_ARGS__))(__VA_ARGS__)

#define PACKAGE_CALL_0(PKG, FUNC_NAME)                                          \
  (GNS2(PKG, GetFunctionTable)()->FUNC_NAME())

/* ------------------------------------------------------------
 * 7. Top-Level PACKAGE & PACKAGE_IMPLEMENT Macros
 * ------------------------------------------------------------ */

#ifndef PACKAGE_DISABLE_FUNCTION_TABLE

#define PACKAGE(PKG, ...)                                                      \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO, PKG, __VA_ARGS__)     \
  struct GNS2(PKG, FunctionTable) {                                            \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_FT_FIELD, PKG, __VA_ARGS__)     \
  };                                                                           \
  typedef struct GNS2(PKG, FunctionTable) GNS2(PKG, FunctionTable);            \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE, PKG, __VA_ARGS__)    \
  static inline const struct GNS2(PKG, FunctionTable) *GNS2(                   \
      PKG, GetFunctionTable)(void) {                                           \
    static const struct GNS2(PKG, FunctionTable) table = {                     \
        PP_FOREACH_CTX(                                                        \
            COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY, PKG, __VA_ARGS__)};         \
    return &table;                                                             \
  }                                                                            \
  typedef int GNS3(_cosmeron_package_semi, PKG, __LINE__)

#else

#define PACKAGE(PKG, ...)                                                      \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_GEN_PROTO, PKG, __VA_ARGS__)     \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_GEN_INLINE, PKG, __VA_ARGS__)    \
  typedef int GNS3(_cosmeron_package_semi, PKG, __LINE__)

#endif

#define PACKAGE_IMPLEMENT(PKG)                                                 \
  typedef int GNS3(_cosmeron_package_implemented, PKG, __LINE__)

/* EOF */
