#pragma once

#include "Compiling.inc"

#define PP_SEQ_FIRST(first, ...) first
#define PP_SEQ_REST(first, ...) __VA_ARGS__

#define PP_SEQ_GET_1(_1, ...) _1
#define PP_SEQ_GET_2(_1, _2, ...) _2
#define PP_SEQ_GET_3(_1, _2, _3, ...) _3
#define PP_SEQ_GET_4(_1, _2, _3, _4, ...) _4
#define PP_SEQ_GET_5(_1, _2, _3, _4, _5, ...) _5
#define PP_SEQ_GET_6(_1, _2, _3, _4, _5, _6, ...) _6
#define PP_SEQ_GET_7(_1, _2, _3, _4, _5, _6, _7, ...) _7
#define PP_SEQ_GET_8(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define PP_SEQ_GET_9(_1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9
#define PP_SEQ_GET_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10
/* EOF */
