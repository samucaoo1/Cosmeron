#pragma once

#include "Compiling.inc"

#define PP_OP_EXPAND(...) __VA_ARGS__

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
/* EOF */
