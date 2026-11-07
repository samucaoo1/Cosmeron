#pragma once

#include "Compiling.inc"
#include "Arguments.h"
#include "Operations.h"
#include "Foreach.h"

/* ============================================================
 * PREPROCESSOR - BINARY TREE REDUCTION ENGINE (1..32)
 * Evaluates binary reductions in O(log2 N) recursion depth (max depth: 5).
 * Zero PP_EVAL dependency, fully composable and reentrant.
 * ============================================================ */

/* --- Arity-specific Tree Reducers (op(left, right)) --- */
#define PP_TREE_REDUCE_1(op, _1) _1
#define PP_TREE_REDUCE_2(op, _1, _2) op(_1, _2)
#define PP_TREE_REDUCE_3(op, _1, _2, _3) op(PP_TREE_REDUCE_2(op, _1, _2), _3)
#define PP_TREE_REDUCE_4(op, _1, _2, _3, _4) op(PP_TREE_REDUCE_2(op, _1, _2), PP_TREE_REDUCE_2(op, _3, _4))
#define PP_TREE_REDUCE_5(op, _1, _2, _3, _4, _5) op(PP_TREE_REDUCE_4(op, _1, _2, _3, _4), _5)
#define PP_TREE_REDUCE_6(op, _1, _2, _3, _4, _5, _6) op(PP_TREE_REDUCE_4(op, _1, _2, _3, _4), PP_TREE_REDUCE_2(op, _5, _6))
#define PP_TREE_REDUCE_7(op, _1, _2, _3, _4, _5, _6, _7) op(PP_TREE_REDUCE_4(op, _1, _2, _3, _4), PP_TREE_REDUCE_3(op, _5, _6, _7))
#define PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8) op(PP_TREE_REDUCE_4(op, _1, _2, _3, _4), PP_TREE_REDUCE_4(op, _5, _6, _7, _8))
#define PP_TREE_REDUCE_9(op, _1, _2, _3, _4, _5, _6, _7, _8, _9) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), _9)
#define PP_TREE_REDUCE_10(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_2(op, _9, _10))
#define PP_TREE_REDUCE_11(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_3(op, _9, _10, _11))
#define PP_TREE_REDUCE_12(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_4(op, _9, _10, _11, _12))
#define PP_TREE_REDUCE_13(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_5(op, _9, _10, _11, _12, _13))
#define PP_TREE_REDUCE_14(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_6(op, _9, _10, _11, _12, _13, _14))
#define PP_TREE_REDUCE_15(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_7(op, _9, _10, _11, _12, _13, _14, _15))
#define PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) op(PP_TREE_REDUCE_8(op, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_8(op, _9, _10, _11, _12, _13, _14, _15, _16))
#define PP_TREE_REDUCE_17(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), _17)
#define PP_TREE_REDUCE_18(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_2(op, _17, _18))
#define PP_TREE_REDUCE_19(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_3(op, _17, _18, _19))
#define PP_TREE_REDUCE_20(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_4(op, _17, _18, _19, _20))
#define PP_TREE_REDUCE_21(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_5(op, _17, _18, _19, _20, _21))
#define PP_TREE_REDUCE_22(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_6(op, _17, _18, _19, _20, _21, _22))
#define PP_TREE_REDUCE_23(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_7(op, _17, _18, _19, _20, _21, _22, _23))
#define PP_TREE_REDUCE_24(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_8(op, _17, _18, _19, _20, _21, _22, _23, _24))
#define PP_TREE_REDUCE_25(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_9(op, _17, _18, _19, _20, _21, _22, _23, _24, _25))
#define PP_TREE_REDUCE_26(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_10(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26))
#define PP_TREE_REDUCE_27(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_11(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27))
#define PP_TREE_REDUCE_28(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_12(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28))
#define PP_TREE_REDUCE_29(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_13(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29))
#define PP_TREE_REDUCE_30(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_14(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30))
#define PP_TREE_REDUCE_31(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_15(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31))
#define PP_TREE_REDUCE_32(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) op(PP_TREE_REDUCE_16(op, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_16(op, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32))

/* --- Arity-specific Tree Reducers with Context (op(ctx, left, right)) --- */
#define PP_TREE_REDUCE_CTX_1(op, ctx, _1) _1
#define PP_TREE_REDUCE_CTX_2(op, ctx, _1, _2) op(ctx, _1, _2)
#define PP_TREE_REDUCE_CTX_3(op, ctx, _1, _2, _3) op(ctx, PP_TREE_REDUCE_CTX_2(op, ctx, _1, _2), _3)
#define PP_TREE_REDUCE_CTX_4(op, ctx, _1, _2, _3, _4) op(ctx, PP_TREE_REDUCE_CTX_2(op, ctx, _1, _2), PP_TREE_REDUCE_CTX_2(op, ctx, _3, _4))
#define PP_TREE_REDUCE_CTX_5(op, ctx, _1, _2, _3, _4, _5) op(ctx, PP_TREE_REDUCE_CTX_4(op, ctx, _1, _2, _3, _4), _5)
#define PP_TREE_REDUCE_CTX_6(op, ctx, _1, _2, _3, _4, _5, _6) op(ctx, PP_TREE_REDUCE_CTX_4(op, ctx, _1, _2, _3, _4), PP_TREE_REDUCE_CTX_2(op, ctx, _5, _6))
#define PP_TREE_REDUCE_CTX_7(op, ctx, _1, _2, _3, _4, _5, _6, _7) op(ctx, PP_TREE_REDUCE_CTX_4(op, ctx, _1, _2, _3, _4), PP_TREE_REDUCE_CTX_3(op, ctx, _5, _6, _7))
#define PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8) op(ctx, PP_TREE_REDUCE_CTX_4(op, ctx, _1, _2, _3, _4), PP_TREE_REDUCE_CTX_4(op, ctx, _5, _6, _7, _8))
#define PP_TREE_REDUCE_CTX_9(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), _9)
#define PP_TREE_REDUCE_CTX_10(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_2(op, ctx, _9, _10))
#define PP_TREE_REDUCE_CTX_11(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_3(op, ctx, _9, _10, _11))
#define PP_TREE_REDUCE_CTX_12(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_4(op, ctx, _9, _10, _11, _12))
#define PP_TREE_REDUCE_CTX_13(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_5(op, ctx, _9, _10, _11, _12, _13))
#define PP_TREE_REDUCE_CTX_14(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_6(op, ctx, _9, _10, _11, _12, _13, _14))
#define PP_TREE_REDUCE_CTX_15(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_7(op, ctx, _9, _10, _11, _12, _13, _14, _15))
#define PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) op(ctx, PP_TREE_REDUCE_CTX_8(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8), PP_TREE_REDUCE_CTX_8(op, ctx, _9, _10, _11, _12, _13, _14, _15, _16))
#define PP_TREE_REDUCE_CTX_17(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), _17)
#define PP_TREE_REDUCE_CTX_18(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_2(op, ctx, _17, _18))
#define PP_TREE_REDUCE_CTX_19(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_3(op, ctx, _17, _18, _19))
#define PP_TREE_REDUCE_CTX_20(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_4(op, ctx, _17, _18, _19, _20))
#define PP_TREE_REDUCE_CTX_21(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_5(op, ctx, _17, _18, _19, _20, _21))
#define PP_TREE_REDUCE_CTX_22(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_6(op, ctx, _17, _18, _19, _20, _21, _22))
#define PP_TREE_REDUCE_CTX_23(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_7(op, ctx, _17, _18, _19, _20, _21, _22, _23))
#define PP_TREE_REDUCE_CTX_24(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_8(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24))
#define PP_TREE_REDUCE_CTX_25(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_9(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25))
#define PP_TREE_REDUCE_CTX_26(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_10(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26))
#define PP_TREE_REDUCE_CTX_27(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_11(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27))
#define PP_TREE_REDUCE_CTX_28(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_12(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28))
#define PP_TREE_REDUCE_CTX_29(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_13(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29))
#define PP_TREE_REDUCE_CTX_30(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_14(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30))
#define PP_TREE_REDUCE_CTX_31(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_15(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31))
#define PP_TREE_REDUCE_CTX_32(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) op(ctx, PP_TREE_REDUCE_CTX_16(op, ctx, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16), PP_TREE_REDUCE_CTX_16(op, ctx, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32))

/* ============================================================
 * PREPROCESSOR - PUBLIC TREE DISPATCHERS (1..32)
 * ============================================================ */

#define PP_TREE_REDUCE(op, ...)                                               \
  PP_OP_CAT2(PP_TREE_REDUCE_, PP_ARG_COUNT(__VA_ARGS__))(op, __VA_ARGS__)

#define PP_TREE_REDUCE_CTX(op, ctx, ...)                                       \
  PP_OP_CAT2(PP_TREE_REDUCE_CTX_, PP_ARG_COUNT(__VA_ARGS__))(op, ctx, __VA_ARGS__)

/* Helpers for token gluing */
#define PP_TREE_GLUE_RAW(a, b) PP_OP_CAT2(a, b)
#define PP_TREE_GLUE_SEP(sep, a, b) PP_OP_CAT3(a, sep, b)
#define PP_TREE_GLUE_US(a, b) PP_OP_CAT3(a, _, b)

/* Glues tokens directly: PP_TREE_CAT(A, B, C, D) -> ABCD */
#define PP_TREE_CAT(...) PP_TREE_REDUCE(PP_TREE_GLUE_RAW, __VA_ARGS__)

/* Glues tokens with custom separator: PP_TREE_JOIN(sep, A, B, C) -> A##sep##B##sep##C */
#define PP_TREE_JOIN(sep, ...) PP_TREE_REDUCE_CTX(PP_TREE_GLUE_SEP, sep, __VA_ARGS__)

/* Glues tokens with underscore: PP_TREE_JOIN_UNDERSCORE(A, B, C) -> A_B_C */
#define PP_TREE_JOIN_UNDERSCORE(...) PP_TREE_REDUCE(PP_TREE_GLUE_US, __VA_ARGS__)

/* ============================================================
 * PREPROCESSOR - MULTIDIMENSIONAL TREE CHUNKS ENGINE (2D, 3D, 4D)
 * Scales tree reductions exponentially without compiler bloat:
 * - 2D Chunks: 32 * 32 = 1,024 elements
 * - 3D Chunks: 32 * 32 * 32 = 32,768 elements
 * - 4D Chunks: 32 * 32 * 32 * 32 = 1,048,576 elements
 * ============================================================ */

/* --- 2D Tree Chunks (D2) --- */
#define COSMERON_MACRO_INTERNAL_TREE_CHUNK_2D(op, chunk)                      \
  PP_TREE_REDUCE(op, PP_UNWRAP chunk)

#define PP_TREE_REDUCE_CHUNKS_2D(op, ...)                                     \
  PP_TREE_REDUCE(op, PP_FOREACH_D2_COMMA_CTX(                                 \
      COSMERON_MACRO_INTERNAL_TREE_CHUNK_2D, op, __VA_ARGS__))

#define PP_TREE_REDUCE_CHUNKS(op, ...) PP_TREE_REDUCE_CHUNKS_2D(op, __VA_ARGS__)

/* --- 3D Tree Chunks (D3 -> D2) --- */
#define COSMERON_MACRO_INTERNAL_TREE_CHUNK_3D(op, block)                      \
  PP_TREE_REDUCE_CHUNKS_2D(op, PP_UNWRAP block)

#define PP_TREE_REDUCE_CHUNKS_3D(op, ...)                                     \
  PP_TREE_REDUCE(op, PP_FOREACH_D3_COMMA_CTX(                                 \
      COSMERON_MACRO_INTERNAL_TREE_CHUNK_3D, op, __VA_ARGS__))

/* --- 4D Tree Chunks (D4 -> D3 -> D2) --- */
#define COSMERON_MACRO_INTERNAL_TREE_CHUNK_4D(op, hyper)                      \
  PP_TREE_REDUCE_CHUNKS_3D(op, PP_UNWRAP hyper)

#define PP_TREE_REDUCE_CHUNKS_4D(op, ...)                                     \
  PP_TREE_REDUCE(op, PP_FOREACH_D4_COMMA_CTX(                                 \
      COSMERON_MACRO_INTERNAL_TREE_CHUNK_4D, op, __VA_ARGS__))

/* --- Token Gluing Chunks (2D, 3D, 4D) --- */
#define PP_TREE_CAT_CHUNKS_2D(...) PP_TREE_REDUCE_CHUNKS_2D(PP_TREE_GLUE_RAW, __VA_ARGS__)
#define PP_TREE_CAT_CHUNKS_3D(...) PP_TREE_REDUCE_CHUNKS_3D(PP_TREE_GLUE_RAW, __VA_ARGS__)
#define PP_TREE_CAT_CHUNKS_4D(...) PP_TREE_REDUCE_CHUNKS_4D(PP_TREE_GLUE_RAW, __VA_ARGS__)
#define PP_TREE_CAT_CHUNKS(...)    PP_TREE_CAT_CHUNKS_2D(__VA_ARGS__)

#define PP_TREE_JOIN_UNDERSCORE_CHUNKS_2D(...) PP_TREE_REDUCE_CHUNKS_2D(PP_TREE_GLUE_US, __VA_ARGS__)
#define PP_TREE_JOIN_UNDERSCORE_CHUNKS_3D(...) PP_TREE_REDUCE_CHUNKS_3D(PP_TREE_GLUE_US, __VA_ARGS__)
#define PP_TREE_JOIN_UNDERSCORE_CHUNKS_4D(...) PP_TREE_REDUCE_CHUNKS_4D(PP_TREE_GLUE_US, __VA_ARGS__)
#define PP_TREE_JOIN_UNDERSCORE_CHUNKS(...)    PP_TREE_JOIN_UNDERSCORE_CHUNKS_2D(__VA_ARGS__)

/* EOF */
