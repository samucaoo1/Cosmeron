#pragma once

#include "Compiling.inc"
/* ============================================================
 * PREPROCESSOR - EXPANSION CONTROL
 * ============================================================ */

#define PP_EMPTY()

#define PP_DEFER(id) id PP_EMPTY()

#define PP_OBSTRUCT(...) __VA_ARGS__ PP_DEFER(PP_EMPTY)()

/* === EVAL EXPANSION === */

#define PP_EVAL(...) PP_EVAL1(PP_EVAL1(PP_EVAL1(__VA_ARGS__)))
#define PP_EVAL1(...) PP_EVAL2(PP_EVAL2(PP_EVAL2(__VA_ARGS__)))
#define PP_EVAL2(...) PP_EVAL3(PP_EVAL3(PP_EVAL3(__VA_ARGS__)))
#define PP_EVAL3(...) __VA_ARGS__
/* EOF */
