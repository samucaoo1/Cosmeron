#pragma once

#include "../../Core/Module/Module.h"
#include "../../Core/Outcome/Status.h"
#include "../../Core/Preprocessor/Compiling.inc"
#include "../../Core/Preprocessor/Detect/Compiler.h"

#include <complex.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Standard Struct Types Table (Row-Tuple Pattern)
 * ========================================================================== */

#define STRUCT_TYPE_COL_SUFFIX(SUFFIX, TYPE) SUFFIX
#define STRUCT_TYPE_COL_TYPE(SUFFIX, TYPE)   TYPE
#define STRUCT_TYPE_GET(FIELD, ROW) PP_OP_CAT2(STRUCT_TYPE_COL_, FIELD) ROW

#define STRUCT_STANDARD_TYPE_TABLE(X)                                          \
  X((uint8, uint8_t))                                                          \
  X((uint16, uint16_t))                                                        \
  X((uint32, uint32_t))                                                        \
  X((uint64, uint64_t))                                                        \
  X((size, size_t))                                                            \
  X((int8, int8_t))                                                            \
  X((int16, int16_t))                                                          \
  X((int32, int32_t))                                                          \
  X((int64, int64_t))                                                          \
  X((float, float))                                                            \
  X((double, double))                                                          \
  X((longdouble, long double))

#if !COMPILER_MSVC
#define STRUCT_COMPLEX_TYPE_TABLE(X)                                           \
  X((float_complex, float _Complex))                                           \
  X((double_complex, double _Complex))                                         \
  X((longdouble_complex, long double _Complex))
#else
#define STRUCT_COMPLEX_TYPE_TABLE(X)
#endif

#define STRUCT_TYPE_TABLE(X)                                                   \
  STRUCT_STANDARD_TYPE_TABLE(X)                                                \
  STRUCT_COMPLEX_TYPE_TABLE(X)

/* ==========================================================================
 * Module Template Forwarding Helper
 * Enables: TEMPLATE_INSTANTIATE(Struct, Target, Params...)
 * ========================================================================== */

#ifndef TEMPLATE_Struct
#define TEMPLATE_Struct(TARGET, ...) NS(TEMPLATE_Struct, TARGET)(__VA_ARGS__)
#endif

/* EOF */
