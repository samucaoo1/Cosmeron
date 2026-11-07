#pragma once

#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Arguments.h"
#include "../Preprocessor/Operations.h"
#include "../Preprocessor/Tree.h"

/* ============================================================
 * 1. Universal Namespaces & Delimiters (Powered by Tree Engine)
 * ============================================================ */

#define COSMERON_SEPARATOR_TOKEN _

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
#ifndef COSMERON_NAMESPACE
#define COSMERON_NAMESPACE Cosmeron
#endif

#ifndef COSMERON_NAMESPACE_SHORT
#define COSMERON_NAMESPACE_SHORT Cos
#endif

#ifndef COSMERON_NAMESPACE_CONST
#define COSMERON_NAMESPACE_CONST COSMERON
#endif

#ifndef COSMERON_NAMESPACE_SHORT_CONST
#define COSMERON_NAMESPACE_SHORT_CONST COS
#endif

/* Resolve Current Depth Mode */
#if defined(COSMERON_NAMESPACE_MODE_BARE) ||                                   \
    (defined(COSMERON_NAMESPACE_MODE) && (COSMERON_NAMESPACE_MODE == BARE))
#define LIB_PREFIX(...) NS(__VA_ARGS__)
#define LIB_PREFIX_CONST(...) CNS(__VA_ARGS__)
#elif defined(COSMERON_NAMESPACE_MODE_SHORT) ||                                \
    (defined(COSMERON_NAMESPACE_MODE) && (COSMERON_NAMESPACE_MODE == SHORT))
#define LIB_PREFIX(...) NS(COSMERON_NAMESPACE_SHORT, __VA_ARGS__)
#define LIB_PREFIX_CONST(...) CNS(COSMERON_NAMESPACE_SHORT_CONST, __VA_ARGS__)
#else
/* STANDARD (Default) and FULL */
#define LIB_PREFIX(...) NS(COSMERON_NAMESPACE, __VA_ARGS__)
#define LIB_PREFIX_CONST(...) CNS(COSMERON_NAMESPACE_CONST, __VA_ARGS__)
#endif

/* ============================================================
 * 3. Granular FunctionTable Policy Switches
 * Policies: ENABLED (0), OBJECT_ONLY (1), PACKAGE_ONLY (2), DISABLED (3)
 * ============================================================ */

#if defined(COSMERON_TABLE_DISABLED) ||                                        \
    (defined(COSMERON_TABLE) && (COSMERON_TABLE == DISABLED || COSMERON_TABLE == 0))
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

/* 1. MODULE_TYPE: Generates user type with automatic 'T' prefix for objects */
#define MODULE_TYPE(MOD, NAME) LIB_PREFIX(PP_OP_CAT4(MOD, _, T, NAME))

/* 2. MODULE_TYPED: Specialization by another type */
#define MODULE_TYPED(MOD, NAME, SUFFIX)                                        \
  LIB_PREFIX(PP_OP_CAT6(MOD, _, T, NAME, _, SUFFIX))

/* 3. MODULE_ENUM: Enum type strictly WITHOUT 'T' prefix */
#define MODULE_ENUM(MOD, NAME) LIB_PREFIX(PP_OP_CAT3(MOD, _, NAME))

/* 4. MODULE_CONST: Public constant identifier */
#define MODULE_CONST(MOD, NAME) LIB_PREFIX_CONST(PP_OP_CAT3(MOD, _, NAME))

/* 5. Universal Status Shortcut */
#ifndef STATUS
#define STATUS(CODE) CNS(LIB_PREFIX_CONST(STATUS), CODE)
#endif
#ifndef STATUS_CONST
#define STATUS_CONST(CODE) STATUS(CODE)
#endif

/* 6. Cross-Module Callback */
#define MODULE_CALLBACK(MOD, ...) LIB_PREFIX(NS(MOD, __VA_ARGS__))

/* 7. Forward Declarations */
#define FORWARD_OBJECT(MOD, NAME)                                              \
  typedef struct MODULE_TYPE(MOD, NAME) MODULE_TYPE(MOD, NAME)

#define FORWARD_TEMPLATE_OBJECT(MOD, NAME, SUFFIX)                             \
  typedef struct MODULE_TYPED(MOD, NAME, SUFFIX) MODULE_TYPED(MOD, NAME, SUFFIX)

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L) ||              \
    (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#define FORWARD_ENUM(MOD, NAME)                                                \
  typedef enum MODULE_ENUM(MOD, NAME) MODULE_ENUM(MOD, NAME)
#endif

/* ============================================================
 * 5. Unified Cross-Module Invocations (MODULE_CALL)
 * Resolves paths of any depth (2, 3, 4, 5+ levels) directly to C symbols.
 * ============================================================ */

#define COSMERON_INTERNAL_CALL_2(A, ARGS) LIB_PREFIX(A) ARGS
#define COSMERON_INTERNAL_CALL_3(A, B, ARGS) LIB_PREFIX(NS(A, B)) ARGS
#define COSMERON_INTERNAL_CALL_4(A, B, C, ARGS)                                \
  LIB_PREFIX(NS(A, B, C)) ARGS
#define COSMERON_INTERNAL_CALL_5(A, B, C, D, ARGS)                             \
  LIB_PREFIX(NS(A, B, C, D)) ARGS
#define COSMERON_INTERNAL_CALL_6(A, B, C, D, E, ARGS)                          \
  LIB_PREFIX(NS(A, B, C, D, E)) ARGS
#define COSMERON_INTERNAL_CALL_7(A, B, C, D, E, F, ARGS)                       \
  LIB_PREFIX(NS(A, B, C, D, E, F)) ARGS
#define COSMERON_INTERNAL_CALL_8(A, B, C, D, E, F, G, ARGS)                    \
  LIB_PREFIX(NS(A, B, C, D, E, F, G)) ARGS

#define MODULE_CALL_DISPATCH(count, ...)                                       \
  PP_OP_CAT2(COSMERON_INTERNAL_CALL_, count)(__VA_ARGS__)

#define MODULE_CALL(...)                                                       \
  MODULE_CALL_DISPATCH(PP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

/* EOF */
