#pragma once

#include "Boolean.h"
#include "Compiling.inc"
#include "Eval.h"

#define PP_WHILE(pred, op, state) PP_EVAL(PP_WHILE_IMPL(pred, op, state))

#define PP_WHILE_IMPL(pred, op, state)                                        \
  PP_BOOL_IF_ELSE(pred(state))                                                \
  (PP_OBSTRUCT(PP_WHILE_INDIRECT)()(pred, op, op(state)))(state)

#define PP_WHILE_INDIRECT() PP_WHILE_IMPL
/* EOF */
