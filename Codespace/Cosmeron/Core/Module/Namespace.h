#pragma once

#include "../Preprocessor/Arguments.h"
#include "../Preprocessor/Boolean.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Detect/Detect.h"
#include "../Preprocessor/Operations.h"
#include "../Preprocessor/Tree.h"

/* ============================================================
 * 1. O(1) Fast Flat Namespace Joiner (NS_JOIN)
 * Expands directly in a single substitution step via PP_OP_CAT arity dispatch.
 * ============================================================ */

#define COSMERON_SEPARATOR_TOKEN _

#define COSMERON_INTERNAL_NS_JOIN_1(_1) _1
#define COSMERON_INTERNAL_NS_JOIN_2(_1, _2) PP_OP_CAT3(_1, _, _2)
#define COSMERON_INTERNAL_NS_JOIN_3(_1, _2, _3) PP_OP_CAT5(_1, _, _2, _, _3)
#define COSMERON_INTERNAL_NS_JOIN_4(_1, _2, _3, _4)                            \
  PP_OP_CAT7(_1, _, _2, _, _3, _, _4)
#define COSMERON_INTERNAL_NS_JOIN_5(_1, _2, _3, _4, _5)                        \
  PP_OP_CAT9(_1, _, _2, _, _3, _, _4, _, _5)
#define COSMERON_INTERNAL_NS_JOIN_6(_1, _2, _3, _4, _5, _6)                    \
  PP_OP_CAT11(_1, _, _2, _, _3, _, _4, _, _5, _, _6)
#define COSMERON_INTERNAL_NS_JOIN_7(_1, _2, _3, _4, _5, _6, _7)                \
  PP_OP_CAT13(_1, _, _2, _, _3, _, _4, _, _5, _, _6, _, _7)
#define COSMERON_INTERNAL_NS_JOIN_8(_1, _2, _3, _4, _5, _6, _7, _8)            \
  PP_OP_CAT15(_1, _, _2, _, _3, _, _4, _, _5, _, _6, _, _7, _, _8)

#define NS_JOIN(...)                                                           \
  PP_OP_CAT2(COSMERON_INTERNAL_NS_JOIN_, PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/* Universal Namespaces & Delimiters (Powered by Tree Engine) */
#define NS(...) PP_TREE_JOIN_UNDERSCORE(__VA_ARGS__)
#define CNS(...) PP_TREE_JOIN_UNDERSCORE(__VA_ARGS__)

/* ============================================================
 * 2. Declarative Root Configuration (LIBRARY_NAMESPACE_CONFIG)
 * ============================================================ */

#define LIBRARY_NAMESPACE_CONFIG(LIB, ...)                                     \
  typedef int PP_OP_CAT3(_cosmeron_config_registered_, LIB, __LINE__)

#define NAME(X) X
#define SHORT(X) X
#define MODE(X) X
#define CASE(X) X
#define TABLE(X) X

/* Depth Modes: BARE, SHORT, STANDARD, FULL */
#define COSMERON_MODE_BARE 1
#define COSMERON_MODE_SHORT 2
#define COSMERON_MODE_STANDARD 3
#define COSMERON_MODE_FULL 4

/* Default Configuration Fallbacks */
#ifndef COSMERON_NAMESPACE_SHORT
#define COSMERON_NAMESPACE_SHORT Cos
#endif

#ifndef COSMERON_NAMESPACE_SHORT_CONST
#define COSMERON_NAMESPACE_SHORT_CONST COS
#endif

/* Resolve Current Depth Mode */
#if !defined(COSMERON_NAMESPACE) || defined(COSMERON_NAMESPACE_MODE_BARE) ||   \
    (defined(COSMERON_NAMESPACE_MODE) && (COSMERON_NAMESPACE_MODE == BARE))
#define LIB_PREFIX(...) NS(__VA_ARGS__)
#define LIB_PREFIX_CONST(...) CNS(__VA_ARGS__)
#elif defined(COSMERON_NAMESPACE_MODE_SHORT) ||                                \
    (defined(COSMERON_NAMESPACE_MODE) && (COSMERON_NAMESPACE_MODE == SHORT))
#define LIB_PREFIX(...) NS(COSMERON_NAMESPACE_SHORT, __VA_ARGS__)
#define LIB_PREFIX_CONST(...) CNS(COSMERON_NAMESPACE_SHORT_CONST, __VA_ARGS__)
#else
/* STANDARD and FULL (when COSMERON_NAMESPACE is defined) */
#define LIB_PREFIX(...) NS(COSMERON_NAMESPACE, __VA_ARGS__)
#define LIB_PREFIX_CONST(...) CNS(COSMERON_NAMESPACE_CONST, __VA_ARGS__)
#endif

/* ============================================================
 * 3. Granular FunctionTable Policy Switches
 * Policies: ENABLED (0), OBJECT_ONLY (1), PACKAGE_ONLY (2), DISABLED (3)
 * ============================================================ */

#if defined(COSMERON_TABLE_DISABLED) ||                                        \
    (defined(COSMERON_TABLE) &&                                                \
     (COSMERON_TABLE == DISABLED || COSMERON_TABLE == 0))
#ifndef MODULE_DISABLE_FUNCTION_TABLE
#define MODULE_DISABLE_FUNCTION_TABLE 1
#endif
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#endif
#elif defined(COSMERON_TABLE_OBJECT_ONLY) ||                                   \
    (defined(COSMERON_TABLE) && COSMERON_TABLE == OBJECT_ONLY)
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#endif
#elif defined(COSMERON_TABLE_PACKAGE_ONLY) ||                                  \
    (defined(COSMERON_TABLE) && COSMERON_TABLE == PACKAGE_ONLY)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif
#endif

/* ============================================================
 * 4. Cross-Module Consumption Grammar
 * ============================================================ */

/* 1. MODULE_TYPE: Generates user type with automatic 'T' prefix on 1st token */
#define COSMERON_INTERNAL_MOD_TYPE_1(OBJ) LIB_PREFIX(PP_OP_CAT2(T, OBJ))
#define COSMERON_INTERNAL_MOD_TYPE_2(MOD, OBJ)                                 \
  LIB_PREFIX(PP_OP_CAT4(T, MOD, _, OBJ))
#define COSMERON_INTERNAL_MOD_TYPE_3(MOD, PKG, OBJ)                            \
  LIB_PREFIX(PP_OP_CAT6(T, MOD, _, PKG, _, OBJ))
#define MODULE_TYPE(...)                                                       \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_TYPE_,                                      \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/* 2. MODULE_TYPED: Specialization by another type ('T' on 1st token) */
#define COSMERON_INTERNAL_MOD_TYPED_2(OBJ, SUFFIX)                             \
  LIB_PREFIX(PP_OP_CAT4(T, OBJ, _, SUFFIX))
#define COSMERON_INTERNAL_MOD_TYPED_3(MOD, OBJ, SUFFIX)                        \
  LIB_PREFIX(PP_OP_CAT6(T, MOD, _, OBJ, _, SUFFIX))
#define COSMERON_INTERNAL_MOD_TYPED_4(MOD, PKG, OBJ, SUFFIX)                   \
  LIB_PREFIX(PP_OP_CAT8(T, MOD, _, PKG, _, OBJ, _, SUFFIX))
#define MODULE_TYPED(...)                                                      \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_TYPED_,                                     \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/* 2b. MODULE_SUBTYPE / MODULE_SUBTYPED: 'T' on 2nd token (e.g. Tree_TAVL_Set)
 */
#define MODULE_SUBTYPE(MOD, SUB, OBJ)                                          \
  LIB_PREFIX(PP_OP_CAT6(MOD, _, T, SUB, _, OBJ))
#define MODULE_SUBTYPED(MOD, SUB, OBJ, SUFFIX)                                 \
  LIB_PREFIX(PP_OP_CAT8(MOD, _, T, SUB, _, OBJ, _, SUFFIX))

/* 2c. Multi-type naming utilities for Map/Pair/Graph specializations */
#define MODULE_NAME_KV(KEY, VAL) PP_OP_CAT5(K, KEY, _, V, VAL)
#define MODULE_NAME_AND(A, B) PP_OP_CAT3(A, And, B)
#define MODULE_NAME_VW(VERTEX, WEIGHT) PP_OP_CAT5(V, VERTEX, _, W, WEIGHT)

/* 3. MODULE_ENUM: Enum type strictly WITHOUT 'T' prefix in O(1) */
#define MODULE_ENUM(...) LIB_PREFIX(NS_JOIN(__VA_ARGS__))

/* 4. MODULE_CONST: Public constant identifier in O(1) */
#define MODULE_CONST(...) LIB_PREFIX_CONST(NS_JOIN(__VA_ARGS__))

/* 5. Universal Status Shortcut */
#ifndef STATUS
#define STATUS(CODE) CNS(LIB_PREFIX_CONST(OPSTATUS), CODE)
#endif
#ifndef STATUS_CONST
#define STATUS_CONST(CODE) STATUS(CODE)
#endif

/* 6. Cross-Module Callable & Delegate */
#define COSMERON_INTERNAL_MOD_CALLABLE_2(MOD, NAME)                            \
  LIB_PREFIX(NS_JOIN(MOD, TCallable, NAME))
#define COSMERON_INTERNAL_MOD_CALLABLE_3(MOD, PKG, NAME)                       \
  LIB_PREFIX(NS_JOIN(MOD, PKG, TCallable, NAME))
#define COSMERON_INTERNAL_MOD_CALLABLE_4(MOD, PKG, SUB, NAME)                  \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, TCallable, NAME))
#define COSMERON_INTERNAL_MOD_CALLABLE_5(MOD, PKG, SUB1, SUB2, NAME)           \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB1, SUB2, TCallable, NAME))

#define MODULE_CALLABLE(...)                                                   \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_CALLABLE_,                                  \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define COSMERON_INTERNAL_MOD_CALLABLE_FUNC_2(MOD, NAME)                       \
  LIB_PREFIX(NS_JOIN(MOD, Callable, NAME))
#define COSMERON_INTERNAL_MOD_CALLABLE_FUNC_3(MOD, PKG, NAME)                  \
  LIB_PREFIX(NS_JOIN(MOD, PKG, Callable, NAME))
#define COSMERON_INTERNAL_MOD_CALLABLE_FUNC_4(MOD, PKG, SUB, NAME)             \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, Callable, NAME))
#define COSMERON_INTERNAL_MOD_CALLABLE_FUNC_5(MOD, PKG, SUB1, SUB2, NAME)      \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB1, SUB2, Callable, NAME))

#define MODULE_CALLABLE_FUNC(...)                                              \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_CALLABLE_FUNC_,                             \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define COSMERON_INTERNAL_MOD_DELEGATE_2(MOD, NAME)                            \
  LIB_PREFIX(NS_JOIN(MOD, TDelegate, NAME))
#define COSMERON_INTERNAL_MOD_DELEGATE_3(MOD, PKG, NAME)                       \
  LIB_PREFIX(NS_JOIN(MOD, PKG, TDelegate, NAME))
#define COSMERON_INTERNAL_MOD_DELEGATE_4(MOD, PKG, SUB, NAME)                  \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, TDelegate, NAME))
#define COSMERON_INTERNAL_MOD_DELEGATE_5(MOD, PKG, SUB1, SUB2, NAME)           \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB1, SUB2, TDelegate, NAME))

#define MODULE_DELEGATE(...)                                                   \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_DELEGATE_,                                  \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define COSMERON_INTERNAL_MOD_DELEGATE_FUNC_2(MOD, NAME)                       \
  LIB_PREFIX(NS_JOIN(MOD, Delegate, NAME))
#define COSMERON_INTERNAL_MOD_DELEGATE_FUNC_3(MOD, PKG, NAME)                  \
  LIB_PREFIX(NS_JOIN(MOD, PKG, Delegate, NAME))
#define COSMERON_INTERNAL_MOD_DELEGATE_FUNC_4(MOD, PKG, SUB, NAME)             \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, Delegate, NAME))
#define COSMERON_INTERNAL_MOD_DELEGATE_FUNC_5(MOD, PKG, SUB1, SUB2, NAME)      \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB1, SUB2, Delegate, NAME))

#define MODULE_DELEGATE_FUNC(...)                                              \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_DELEGATE_FUNC_,                             \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/* 7. Forward Declarations */
#define FORWARD_OBJECT(...)                                                    \
  typedef struct MODULE_TYPE(__VA_ARGS__) MODULE_TYPE(__VA_ARGS__)

#define FORWARD_STRUCT(...) FORWARD_OBJECT(__VA_ARGS__)

#define FORWARD_DELEGATE(...)                                                  \
  typedef struct MODULE_DELEGATE(__VA_ARGS__) MODULE_DELEGATE(__VA_ARGS__)

#define FORWARD_TEMPLATE_OBJECT(...)                                           \
  typedef struct MODULE_TYPED(__VA_ARGS__) MODULE_TYPED(__VA_ARGS__)

#define FORWARD_TEMPLATE_STRUCT(...) FORWARD_TEMPLATE_OBJECT(__VA_ARGS__)

#define FORWARD_ISTRUCT(MOD, NAME)                                             \
  typedef struct MODULE_ITYPE(MOD, NAME) MODULE_ITYPE(MOD, NAME)

#define FORWARD_TEMPLATE_ISTRUCT(MOD, NAME, SUFFIX)                            \
  typedef struct MODULE_ITYPED(MOD, NAME, SUFFIX)                              \
      MODULE_ITYPED(MOD, NAME, SUFFIX)

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) ||              \
    ((COMPILER_GCC || COMPILER_CLANG) && !defined(__STRICT_ANSI__))
#define FORWARD_ENUM(MOD, NAME)                                                \
  typedef enum MODULE_ENUM(MOD, NAME) MODULE_ENUM(MOD, NAME)
#endif

/* 8. MODULE_NS: Semantic module namespace identifier with LIB_PREFIX in O(1) */
#define MODULE_NS(...) LIB_PREFIX(NS_JOIN(__VA_ARGS__))

/* 9. MODULE_FUNC: Semantic function identifier with LIB_PREFIX in O(1) */
#define MODULE_FUNC(...) LIB_PREFIX(NS_JOIN(__VA_ARGS__))

/* 10. DOMAIN_NS: Domain-level namespace identifier in O(1) */
#define DOMAIN_NS(DOMAIN, ...) NS_JOIN(DOMAIN, __VA_ARGS__)

/* ============================================================
 * 4.5 Internal Namespace Variants (Single 'I' Discriminator)
 * Automatically injects the canonical 'Internal' / 'INTERNAL' token in O(1).
 * ============================================================ */

#define COSMERON_INTERNAL_MOD_INS_1(NAME) LIB_PREFIX(NS_JOIN(Internal, NAME))
#define COSMERON_INTERNAL_MOD_INS_2(MOD, NAME)                                 \
  LIB_PREFIX(NS_JOIN(MOD, Internal, NAME))
#define COSMERON_INTERNAL_MOD_INS_3(MOD, PKG, NAME)                            \
  LIB_PREFIX(NS_JOIN(MOD, PKG, Internal, NAME))
#define COSMERON_INTERNAL_MOD_INS_4(MOD, PKG, SUB, NAME)                       \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, Internal, NAME))
#define COSMERON_INTERNAL_MOD_INS_5(MOD, PKG, SUB, SUB2, NAME)                 \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, SUB2, Internal, NAME))

#define MODULE_INS(...)                                                        \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_INS_, PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define MODULE_IFUNC(...) MODULE_INS(__VA_ARGS__)
#define MODULE_ITYPE(...) MODULE_INS(__VA_ARGS__)
#define MODULE_ITYPED(...) MODULE_INS(__VA_ARGS__)
#define MODULE_IENUM(...) MODULE_INS(__VA_ARGS__)

#define COSMERON_INTERNAL_MOD_ICALLABLE_2(MOD, NAME)                           \
  LIB_PREFIX(NS_JOIN(MOD, Internal, TCallable, NAME))
#define COSMERON_INTERNAL_MOD_ICALLABLE_3(MOD, PKG, NAME)                      \
  LIB_PREFIX(NS_JOIN(MOD, PKG, Internal, TCallable, NAME))
#define COSMERON_INTERNAL_MOD_ICALLABLE_4(MOD, PKG, SUB, NAME)                 \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, Internal, TCallable, NAME))

#define MODULE_ICALLABLE(...)                                                  \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_ICALLABLE_,                                 \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define COSMERON_INTERNAL_MOD_IDELEGATE_2(MOD, NAME)                           \
  LIB_PREFIX(NS_JOIN(MOD, Internal, TDelegate, NAME))
#define COSMERON_INTERNAL_MOD_IDELEGATE_3(MOD, PKG, NAME)                      \
  LIB_PREFIX(NS_JOIN(MOD, PKG, Internal, TDelegate, NAME))
#define COSMERON_INTERNAL_MOD_IDELEGATE_4(MOD, PKG, SUB, NAME)                 \
  LIB_PREFIX(NS_JOIN(MOD, PKG, SUB, Internal, TDelegate, NAME))

#define MODULE_IDELEGATE(...)                                                  \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_IDELEGATE_,                                 \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define COSMERON_INTERNAL_MOD_ICONST_1(NAME)                                   \
  LIB_PREFIX_CONST(NS_JOIN(INTERNAL, NAME))
#define COSMERON_INTERNAL_MOD_ICONST_2(MOD, NAME)                              \
  LIB_PREFIX_CONST(NS_JOIN(MOD, INTERNAL, NAME))
#define COSMERON_INTERNAL_MOD_ICONST_3(MOD, PKG, NAME)                         \
  LIB_PREFIX_CONST(NS_JOIN(MOD, PKG, INTERNAL, NAME))
#define COSMERON_INTERNAL_MOD_ICONST_4(MOD, PKG, SUB, NAME)                    \
  LIB_PREFIX_CONST(NS_JOIN(MOD, PKG, SUB, INTERNAL, NAME))
#define COSMERON_INTERNAL_MOD_ICONST_5(MOD, PKG, SUB, SUB2, NAME)              \
  LIB_PREFIX_CONST(NS_JOIN(MOD, PKG, SUB, SUB2, INTERNAL, NAME))

#define MODULE_ICONST(...)                                                     \
  PP_OP_CAT2(COSMERON_INTERNAL_MOD_ICONST_,                                    \
             PP_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define MODULE_ICNS(...) MODULE_ICONST(__VA_ARGS__)

/* ============================================================
 * 5. Unified Cross-Module Invocations (MODULE_CALL & MODULE_ICALL)
 * Resolves paths of any depth (2, 3, 4, 5+ levels) directly to C symbols in
 * O(1).
 * ============================================================ */

#define COSMERON_INTERNAL_CALL_2(A, ARGS) LIB_PREFIX(A) ARGS
#define COSMERON_INTERNAL_CALL_3(A, B, ARGS) MODULE_FUNC(A, B) ARGS
#define COSMERON_INTERNAL_CALL_4(A, B, C, ARGS) MODULE_FUNC(A, B, C) ARGS
#define COSMERON_INTERNAL_CALL_5(A, B, C, D, ARGS) MODULE_FUNC(A, B, C, D) ARGS
#define COSMERON_INTERNAL_CALL_6(A, B, C, D, E, ARGS)                          \
  MODULE_FUNC(A, B, C, D, E) ARGS
#define COSMERON_INTERNAL_CALL_7(A, B, C, D, E, F, ARGS)                       \
  MODULE_FUNC(A, B, C, D, E, F) ARGS
#define COSMERON_INTERNAL_CALL_8(A, B, C, D, E, F, G, ARGS)                    \
  MODULE_FUNC(A, B, C, D, E, F, G) ARGS

#define MODULE_CALL_DISPATCH(count, ...)                                       \
  PP_OP_CAT2(COSMERON_INTERNAL_CALL_, count)(__VA_ARGS__)

#define MODULE_CALL(...)                                                       \
  MODULE_CALL_DISPATCH(PP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define COSMERON_INTERNAL_ICALL_2(A, ARGS) MODULE_IFUNC(A) ARGS
#define COSMERON_INTERNAL_ICALL_3(A, B, ARGS) MODULE_IFUNC(A, B) ARGS
#define COSMERON_INTERNAL_ICALL_4(A, B, C, ARGS) MODULE_IFUNC(A, B, C) ARGS
#define COSMERON_INTERNAL_ICALL_5(A, B, C, D, ARGS)                            \
  MODULE_IFUNC(A, B, C, D) ARGS
#define COSMERON_INTERNAL_ICALL_6(A, B, C, D, E, ARGS)                         \
  MODULE_IFUNC(A, B, C, D, E) ARGS

#define MODULE_ICALL_DISPATCH(count, ...)                                      \
  PP_OP_CAT2(COSMERON_INTERNAL_ICALL_, count)(__VA_ARGS__)

#define MODULE_ICALL(...)                                                      \
  MODULE_ICALL_DISPATCH(PP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

/* EOF */
