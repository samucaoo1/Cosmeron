#pragma once

#include "../Module/Callback/Callback.h"
#include "Comparison.space"

CALLBACK_COMPARISON(Comparison, (const void *left, const void *right));

#include "../Module/Package.h"
#include "../Primitive/Types.h"

#define COMPARISON_PRIMITIVE_DECL(ROW)                                         \
  FUNCTION(CMPOUT, PRIMITIVE_TYPE_GET(NAME, ROW),                              \
           (PRIMITIVE_TYPE_GET(TYPE, ROW) left,                                \
            PRIMITIVE_TYPE_GET(TYPE, ROW) right)),

PACKAGE(ALGORITHM_NS(COMPARISON_MOD),
        PRIMITIVE_TYPE_TABLE(COMPARISON_PRIMITIVE_DECL)
            FUNCTION(CMPOUT, CString, (const char *left, const char *right)),
        FUNCTION(CMPOUT, Bytes,
                 (const void *left, size_t leftSize, const void *right,
                  size_t rightSize)));

#undef COMPARISON_PRIMITIVE_DECL

#include "Impl/Comparison.impl"
/* EOF */
