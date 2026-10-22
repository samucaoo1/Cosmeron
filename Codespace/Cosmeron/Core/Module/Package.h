#pragma once

#include "Module.space"
#include "Common/Tuple.impl"
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

#define PACKAGE_NS(NAME) GNS2(LIB_PREFIX(PACKAGE_MOD), NAME)
#define PACKAGE_CNS(NAME) CNS2(LIB_PREFIX_CONST(PACKAGE_CMOD), NAME)
#define PACKAGE_INS(NAME) GNS3(LIB_PREFIX(PACKAGE_MOD), Internal, NAME)
#define PACKAGE_CINS(NAME) CNS3(LIB_PREFIX_CONST(PACKAGE_CMOD), INTERNAL, NAME)

#define PACKAGE_TYPE(NAME) PACKAGE_NS(NAME)
#define PACKAGE_FUNC(NAME) PACKAGE_NS(NAME)
#define PACKAGE_CONST(NAME) PACKAGE_CNS(NAME)

/* ------------------------------------------------------------
 * 1. Invocation Helpers
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PKG_CALL_0(PKG, FUNC_NAME) \
  (GNS2(PKG, GetFunctionTable)()->FUNC_NAME())

#define COSMERON_MACRO_INTERNAL_PKG_CALL_1(PKG, FUNC_NAME, ...) \
  (GNS2(PKG, GetFunctionTable)()->FUNC_NAME(__VA_ARGS__))

#define PACKAGE_CALL(...) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PKG_CALL_, \
             PP_HAS_MORE_THAN_2(__VA_ARGS__))(__VA_ARGS__)

#define PACKAGE_CALL_0(PKG, FUNC_NAME) \
  (GNS2(PKG, GetFunctionTable)()->FUNC_NAME())

/* ------------------------------------------------------------
 * 2. Internal Function Generators (Prototypes & Inlines)
 * ------------------------------------------------------------ */

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO_I(PKG, C, I, R, N, P, B) \
  typedef R GNS3(PKG, N, ret_t);                                            \
  struct GNS3(PKG, N, sig) {                                                \
    PP_PARAMS_TO_FIELDS(P)                                                  \
  };                                                                        \
  static inline R GNS2(PKG, N)                                              \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P);

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO(PKG, M) \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO_I, PKG, M)

#define COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_0(PKG, C, R, N, P, B)
#define COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_1(PKG, C, R, N, P, B) \
  static inline R GNS2(PKG, N)                                            \
      COSMERON_MACRO_INTERNAL_PKG_EMIT_PARAMS(P) B

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_I(PKG, C, I, R, N, P, B) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PACKAGE_INLINE_DEF_, I)(PKG, C, R, N, P, B)

#define COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE(PKG, M) \
  COSMERON_MACRO_INTERNAL_UNWRAP_MEMBER(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE_I, PKG, M)

/* ------------------------------------------------------------
 * 3. Top-Level PACKAGE Declarator
 * ------------------------------------------------------------ */

#if !defined(PACKAGE_DISABLE_FUNCTION_TABLE) && !defined(MODULE_DISABLE_FUNCTION_TABLE)

#define PACKAGE(PKG, ...)                                                       \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO, PKG, __VA_ARGS__)   \
  struct GNS2(PKG, FunctionTable) {                                             \
    PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PKG_FT_FIELD, PKG, __VA_ARGS__)      \
  };                                                                            \
  typedef struct GNS2(PKG, FunctionTable) GNS2(PKG, FunctionTable);             \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE, PKG, __VA_ARGS__)  \
  static inline const struct GNS2(PKG, FunctionTable) *GNS2(                    \
      PKG, GetFunctionTable)(void) {                                            \
    static const struct GNS2(PKG, FunctionTable) table = {                      \
        PP_FOREACH_CTX(                                                         \
            COSMERON_MACRO_INTERNAL_PKG_FT_ENTRY, PKG, __VA_ARGS__)};          \
    return &table;                                                              \
  }                                                                             \
  typedef int GNS3(_cosmeron_package_semi, PKG, __LINE__)

#else

#define PACKAGE(PKG, ...)                                                       \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_PROTO, PKG, __VA_ARGS__)   \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_PACKAGE_GEN_INLINE, PKG, __VA_ARGS__)  \
  typedef int GNS3(_cosmeron_package_semi, PKG, __LINE__)

#endif

/* ------------------------------------------------------------
 * 4. Package Implementation Finalizer
 * ------------------------------------------------------------ */

#define PACKAGE_IMPLEMENT(PKG) \
  typedef int GNS3(_cosmeron_package_implemented, PKG, __LINE__)

/* EOF */
