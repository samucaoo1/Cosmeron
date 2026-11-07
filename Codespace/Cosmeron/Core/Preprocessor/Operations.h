#pragma once

#include "Compiling.inc"

#define PP_OP_EXPAND(...) __VA_ARGS__

#define PP_OP_STRING_RAW(VALUE) #VALUE
#define PP_OP_STRING(VALUE) PP_OP_STRING_RAW(VALUE)

/* --- Primitive 2-token paste --- */
#define PP_OP_CAT_RAW(a, b) a##b
#define PP_OP_CAT2(a, b) PP_OP_CAT_RAW(a, b)

/* --- Fixed-arity expanded token pastes (3..32) --- */
#define PP_OP_CAT3(a, b, c) PP_OP_CAT2(PP_OP_CAT2(a, b), c)
#define PP_OP_CAT4(a, b, c, d) PP_OP_CAT2(PP_OP_CAT3(a, b, c), d)
#define PP_OP_CAT5(a, b, c, d, e) PP_OP_CAT2(PP_OP_CAT4(a, b, c, d), e)
#define PP_OP_CAT6(a, b, c, d, e, f) PP_OP_CAT2(PP_OP_CAT5(a, b, c, d, e), f)
#define PP_OP_CAT7(a, b, c, d, e, f, g) PP_OP_CAT2(PP_OP_CAT6(a, b, c, d, e, f), g)
#define PP_OP_CAT8(a, b, c, d, e, f, g, h) PP_OP_CAT2(PP_OP_CAT7(a, b, c, d, e, f, g), h)
#define PP_OP_CAT9(a, b, c, d, e, f, g, h, i) PP_OP_CAT2(PP_OP_CAT8(a, b, c, d, e, f, g, h), i)
#define PP_OP_CAT10(a, b, c, d, e, f, g, h, i, j) PP_OP_CAT2(PP_OP_CAT9(a, b, c, d, e, f, g, h, i), j)
#define PP_OP_CAT11(a, b, c, d, e, f, g, h, i, j, k) PP_OP_CAT2(PP_OP_CAT10(a, b, c, d, e, f, g, h, i, j), k)
#define PP_OP_CAT12(a, b, c, d, e, f, g, h, i, j, k, l) PP_OP_CAT2(PP_OP_CAT11(a, b, c, d, e, f, g, h, i, j, k), l)
#define PP_OP_CAT13(a, b, c, d, e, f, g, h, i, j, k, l, m) PP_OP_CAT2(PP_OP_CAT12(a, b, c, d, e, f, g, h, i, j, k, l), m)
#define PP_OP_CAT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) PP_OP_CAT2(PP_OP_CAT13(a, b, c, d, e, f, g, h, i, j, k, l, m), n)
#define PP_OP_CAT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) PP_OP_CAT2(PP_OP_CAT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n), o)
#define PP_OP_CAT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) PP_OP_CAT2(PP_OP_CAT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o), p)
#define PP_OP_CAT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) PP_OP_CAT2(PP_OP_CAT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p), q)
#define PP_OP_CAT18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) PP_OP_CAT2(PP_OP_CAT17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q), r)
#define PP_OP_CAT19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) PP_OP_CAT2(PP_OP_CAT18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r), s)
#define PP_OP_CAT20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) PP_OP_CAT2(PP_OP_CAT19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s), t)
#define PP_OP_CAT21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) PP_OP_CAT2(PP_OP_CAT20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t), u)
#define PP_OP_CAT22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) PP_OP_CAT2(PP_OP_CAT21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u), v)
#define PP_OP_CAT23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) PP_OP_CAT2(PP_OP_CAT22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v), w)
#define PP_OP_CAT24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) PP_OP_CAT2(PP_OP_CAT23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w), x)
#define PP_OP_CAT25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) PP_OP_CAT2(PP_OP_CAT24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x), y)
#define PP_OP_CAT26(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z) PP_OP_CAT2(PP_OP_CAT25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y), z)
#define PP_OP_CAT27(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1) PP_OP_CAT2(PP_OP_CAT26(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z), _1)
#define PP_OP_CAT28(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2) PP_OP_CAT2(PP_OP_CAT27(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1), _2)
#define PP_OP_CAT29(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3) PP_OP_CAT2(PP_OP_CAT28(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2), _3)
#define PP_OP_CAT30(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3, _4) PP_OP_CAT2(PP_OP_CAT29(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3), _4)
#define PP_OP_CAT31(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3, _4, _5) PP_OP_CAT2(PP_OP_CAT30(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3, _4), _5)
#define PP_OP_CAT32(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3, _4, _5, _6) PP_OP_CAT2(PP_OP_CAT31(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, _1, _2, _3, _4, _5), _6)

#define PP_OP_EMPTY()
#define PP_OP_COMMA() ,
#define PP_OP_IDENTITY(x) x

/* ============================================================
 * PREPROCESSOR - CALL INVOCATION HELPERS
 * ============================================================ */

#define PP_CALL_1(m, ctx, x) m(x)
#define PP_CALL_2(m, ctx, x) m(ctx, x)

/* Dedicated caller tokens to avoid blue-paint recursion suppression during nesting */
#define PP_CALL_D1_1(m, ctx, x) m(x)
#define PP_CALL_D1_2(m, ctx, x) m(ctx, x)

#define PP_CALL_D2_1(m, ctx, x) m(x)
#define PP_CALL_D2_2(m, ctx, x) m(ctx, x)

#define PP_CALL_D3_1(m, ctx, x) m(x)
#define PP_CALL_D3_2(m, ctx, x) m(ctx, x)

#define PP_CALL_D4_1(m, ctx, x) m(x)
#define PP_CALL_D4_2(m, ctx, x) m(ctx, x)

#define PP_CALL_D5_1(m, ctx, x) m(x)
#define PP_CALL_D5_2(m, ctx, x) m(ctx, x)

#define PP_CALL_MAP_1(m, ctx, x) m(x)
#define PP_CALL_MAP_2(m, ctx, x) m(ctx, x)

/* ============================================================
 * PREPROCESSOR - ARBITRARY TOKEN CONCATENATION (PP_OP_CAT)
 * Uses the O(log2 N) Binary Tree Reduction Engine (Tree.h)
 * ============================================================ */
#include "Tree.h"

#define PP_OP_CAT(...) PP_TREE_CAT(__VA_ARGS__)

/* EOF */
