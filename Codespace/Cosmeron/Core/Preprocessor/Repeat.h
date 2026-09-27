#pragma once

#include "Compiling.inc"
#include "Operations.h"

#define PP_REPEAT(count, macro) PP_OP_CAT2(PP_REPEAT_, count)(macro)

#define PP_REPEAT_0(macro)
#define PP_REPEAT_1(macro) macro(1) PP_REPEAT_0(macro)
#define PP_REPEAT_2(macro) macro(2) PP_REPEAT_1(macro)
#define PP_REPEAT_3(macro) macro(3) PP_REPEAT_2(macro)
#define PP_REPEAT_4(macro) macro(4) PP_REPEAT_3(macro)
#define PP_REPEAT_5(macro) macro(5) PP_REPEAT_4(macro)

#define PP_DEC_0 0
#define PP_DEC_1 0
#define PP_DEC_2 1
#define PP_DEC_3 2
#define PP_DEC_4 3
#define PP_DEC_5 4

#define PP_DEC(x) PP_OP_CAT2(PP_DEC_, x)
/* EOF */
