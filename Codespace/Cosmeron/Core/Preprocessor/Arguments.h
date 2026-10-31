#pragma once

#include "Compiling.inc"
#include "Operations.h"

#define PP_ARG_RSEQ() \
  151, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, \
      132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, \
      113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, \
      94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, \
      75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, \
      56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, \
      37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, \
      18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define PP_ARG_COUNT_EXPAND(args) PP_ARG_COUNT_SELECT args
#define PP_ARG_COUNT(...) PP_ARG_COUNT_EXPAND((__VA_ARGS__, PP_ARG_RSEQ(), 0))

#define PP_ARG_COUNT_IMPL(...) PP_ARG_COUNT_SELECT(__VA_ARGS__, 0)

#define PP_ARG_COUNT_SELECT( \
    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, \
    _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, \
    _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, \
    _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, \
    _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, \
    _119, _120, _121, _122, _123, _124, _125, _126, _127, _128, _129, _130, _131, _132, _133, _134, _135, \
    _136, _137, _138, _139, _140, _141, _142, _143, _144, _145, _146, _147, _148, _149, _150, N, ...) \
  N

#define PP_UNWRAP(...) __VA_ARGS__
#define PP_ARG_UNWRAP(...) __VA_ARGS__

#define PP_ARG16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, \
                 _15, _16, ...)                                                \
  _16

#define PP_ARG16_EXPAND(args) PP_ARG16 args

#define PP_HAS_COMMA_RSEQ() \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0

#define PP_HAS_COMMA(...)                                                      \
  PP_ARG_COUNT_EXPAND((__VA_ARGS__, PP_HAS_COMMA_RSEQ()))

#define PP_TRIGGER_PAREN(...) ,

#define PP_IS_PAREN(x) PP_HAS_COMMA(PP_TRIGGER_PAREN x)

#define PP_CASE_0001 , 1

#define PP_IS_EMPTY_IMPL(_0, _1, _2, _3)                                       \
  PP_HAS_COMMA(PP_OP_CAT2(PP_CASE_, PP_OP_CAT4(_0, _1, _2, _3)))

#define PP_IS_EMPTY(...)                                                       \
  PP_IS_EMPTY_IMPL(                                                            \
      PP_HAS_COMMA(__VA_ARGS__),                                               \
      PP_HAS_COMMA(PP_TRIGGER_PAREN __VA_ARGS__),                              \
      PP_HAS_COMMA(__VA_ARGS__(~)),                                            \
      PP_HAS_COMMA(PP_TRIGGER_PAREN __VA_ARGS__(~)))

#define PP_IS_EMPTY_PARAMS(PARAMS) PP_IS_EMPTY(PP_UNWRAP PARAMS)

#define PP_HAS_MORE_THAN_2_RSEQ() \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0

#define PP_HAS_MORE_THAN_2(...)                                                \
  PP_ARG_COUNT_EXPAND((__VA_ARGS__, PP_HAS_MORE_THAN_2_RSEQ(), 0))

/* Convert parameter declaration lists (e.g. (int a, void *b)) to struct fields */
#define PP_PARAM_TO_FIELD_1(a) a;
#define PP_PARAM_TO_FIELD_2(a, b) a; b;
#define PP_PARAM_TO_FIELD_3(a, b, c) a; b; c;
#define PP_PARAM_TO_FIELD_4(a, b, c, d) a; b; c; d;
#define PP_PARAM_TO_FIELD_5(a, b, c, d, e) a; b; c; d; e;
#define PP_PARAM_TO_FIELD_6(a, b, c, d, e, f) a; b; c; d; e; f;
#define PP_PARAM_TO_FIELD_7(a, b, c, d, e, f, g) a; b; c; d; e; f; g;
#define PP_PARAM_TO_FIELD_8(a, b, c, d, e, f, g, h) a; b; c; d; e; f; g; h;
#define PP_PARAM_TO_FIELD_9(a, b, c, d, e, f, g, h, i) a; b; c; d; e; f; g; h; i;
#define PP_PARAM_TO_FIELD_10(a, b, c, d, e, f, g, h, i, j) a; b; c; d; e; f; g; h; i; j;
#define PP_PARAM_TO_FIELD_11(a, b, c, d, e, f, g, h, i, j, k) a; b; c; d; e; f; g; h; i; j; k;
#define PP_PARAM_TO_FIELD_12(a, b, c, d, e, f, g, h, i, j, k, l) a; b; c; d; e; f; g; h; i; j; k; l;
#define PP_PARAM_TO_FIELD_13(a, b, c, d, e, f, g, h, i, j, k, l, m) a; b; c; d; e; f; g; h; i; j; k; l; m;
#define PP_PARAM_TO_FIELD_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) a; b; c; d; e; f; g; h; i; j; k; l; m; n;
#define PP_PARAM_TO_FIELD_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) a; b; c; d; e; f; g; h; i; j; k; l; m; n; o;
#define PP_PARAM_TO_FIELD_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) a; b; c; d; e; f; g; h; i; j; k; l; m; n; o; p;

/* EOF */
