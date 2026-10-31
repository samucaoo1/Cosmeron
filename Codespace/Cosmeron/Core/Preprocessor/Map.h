#pragma once

#include "Compiling.inc"
#include "Arguments.h"
#include "Operations.h"
#include "Tokens.h"
#include "Eval.h"

/* ==========================================================================
 * MOTOR 2: PP_MAP (Eval Machine - Scalable and Nestable)
 * Fully disjoint from PP_FOREACH: allows PP_MAP inside PP_FOREACH.
 * ========================================================================== */

/* Base step (1 argument remaining): ends recursion without trailing separator */
#define PP_MAP_NEXT_0(c, m, s, ctx, a1) \
  c(m, ctx, a1)

/* Inductive step (2+ arguments remaining): emits item and defers remainder */
#define PP_MAP_NEXT_1(c, m, s, ctx, a1, ...) \
  c(m, ctx, a1) s() \
  PP_OBSTRUCT(PP_MAP_INDIRECT)()(c, m, s, ctx, __VA_ARGS__)

#define PP_MAP_STEP(c, m, s, ctx, ...) \
  PP_OP_CAT2(PP_MAP_NEXT_, PP_HAS_COMMA(__VA_ARGS__))(c, m, s, ctx, __VA_ARGS__)
#define PP_MAP_INDIRECT() PP_MAP_STEP

#define PP_MAP_GEN(call, m, sep, ctx, ...) \
  PP_EVAL(PP_MAP_STEP(call, m, sep, ctx, __VA_ARGS__))

/* --- Public Map APIs (comma-separated by default) --- */
#define PP_MAP(m, ...) \
  PP_MAP_GEN(PP_CALL_MAP_1, m, PP_SEP_COMMA, ~, __VA_ARGS__)

#define PP_MAP_CTX(m, ctx, ...) \
  PP_MAP_GEN(PP_CALL_MAP_2, m, PP_SEP_COMMA, ctx, __VA_ARGS__)

#define PP_MAP_SEMI(m, ...) \
  PP_MAP_GEN(PP_CALL_MAP_1, m, PP_SEP_SEMI, ~, __VA_ARGS__)

#define PP_MAP_SEMI_CTX(m, ctx, ...) \
  PP_MAP_GEN(PP_CALL_MAP_2, m, PP_SEP_SEMI, ctx, __VA_ARGS__)

#define PP_MAP_NONE(m, ...) \
  PP_MAP_GEN(PP_CALL_MAP_1, m, PP_SEP_NONE, ~, __VA_ARGS__)

#define PP_MAP_NONE_CTX(m, ctx, ...) \
  PP_MAP_GEN(PP_CALL_MAP_2, m, PP_SEP_NONE, ctx, __VA_ARGS__)

/* EOF */
