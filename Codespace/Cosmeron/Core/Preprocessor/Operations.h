#pragma once

#include "Compiling.inc"

#define PP_OP_EXPAND(...) __VA_ARGS__

#define PP_OP_STRING_RAW(VALUE) #VALUE
#define PP_OP_STRING(VALUE) PP_OP_STRING_RAW(VALUE)

#define PP_OP_CAT(a, b) a##b
#define PP_OP_CAT2(a, b) PP_OP_CAT(a, b)

#define PP_OP_CAT3(a, b, c) PP_OP_CAT2(PP_OP_CAT2(a, b), c)

#define PP_OP_CAT4(a, b, c, d) PP_OP_CAT2(PP_OP_CAT3(a, b, c), d)

#define PP_OP_CAT5(a, b, c, d, e) PP_OP_CAT2(PP_OP_CAT4(a, b, c, d), e)

#define PP_OP_CAT6(a, b, c, d, e, f) PP_OP_CAT2(PP_OP_CAT5(a, b, c, d, e), f)

#define PP_OP_CAT7(a, b, c, d, e, f, g)                                       \
  PP_OP_CAT2(PP_OP_CAT6(a, b, c, d, e, f), g)

#define PP_OP_CAT8(a, b, c, d, e, f, g, h)                                    \
  PP_OP_CAT2(PP_OP_CAT7(a, b, c, d, e, f, g), h)

#define PP_OP_CAT9(a, b, c, d, e, f, g, h, i)                                 \
  PP_OP_CAT2(PP_OP_CAT8(a, b, c, d, e, f, g, h), i)

#define PP_OP_CAT10(a, b, c, d, e, f, g, h, i, j)                             \
  PP_OP_CAT2(PP_OP_CAT9(a, b, c, d, e, f, g, h, i), j)

#define PP_OP_CAT11(a, b, c, d, e, f, g, h, i, j, k)                          \
  PP_OP_CAT2(PP_OP_CAT10(a, b, c, d, e, f, g, h, i, j), k)

#define PP_OP_CAT12(a, b, c, d, e, f, g, h, i, j, k, l)                       \
  PP_OP_CAT2(PP_OP_CAT11(a, b, c, d, e, f, g, h, i, j, k), l)

#define PP_OP_CAT13(a, b, c, d, e, f, g, h, i, j, k, l, m)                    \
  PP_OP_CAT2(PP_OP_CAT12(a, b, c, d, e, f, g, h, i, j, k, l), m)

#define PP_OP_CAT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n)                 \
  PP_OP_CAT2(PP_OP_CAT13(a, b, c, d, e, f, g, h, i, j, k, l, m), n)

#define PP_OP_CAT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)              \
  PP_OP_CAT2(PP_OP_CAT14(a, b, c, d, e, f, g, h, i, j, k, l, m, n), o)

#define PP_OP_CAT16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)           \
  PP_OP_CAT2(PP_OP_CAT15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o), p)

#define PP_OP_EMPTY()
#define PP_OP_COMMA() ,
#define PP_OP_IDENTITY(x) x

/* EOF */
