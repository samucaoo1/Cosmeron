#pragma once

#if defined(TUPLE_DISABLE_FUNCTION_TABLE) &&                                   \
    !defined(COSMERON_DISABLE_FT_Tuple)
#define COSMERON_DISABLE_FT_Tuple 1
#endif

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Preprocessor/Detect/Detect.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ==========================================================================
 * Tuple Primitive Instantiation Table (Row-Tuple Pattern)
 * ========================================================================== */

#if !COMPILER_MSVC
#define TUPLE_COMPLEX_TYPE_ROWS(X)                                             \
  X((FloatComplex, float _Complex))                                            \
  X((DoubleComplex, double _Complex))                                          \
  X((LongDoubleComplex, long double _Complex))
#else
#define TUPLE_COMPLEX_TYPE_ROWS(X)
#endif

#define TUPLE_TYPE_TABLE(X)                                                    \
  X((Boolean, bool))                                                           \
  X((Int8, int8_t))                                                            \
  X((Int16, int16_t))                                                          \
  X((Int32, int32_t))                                                          \
  X((Int64, int64_t))                                                          \
  X((UInt8, uint8_t))                                                          \
  X((UInt16, uint16_t))                                                        \
  X((UInt32, uint32_t))                                                        \
  X((UInt64, uint64_t))                                                        \
  X((Char, char))                                                              \
  X((Char8, uint8_t))                                                          \
  X((Char16, uint16_t))                                                        \
  X((Char32, uint32_t))                                                        \
  X((Float32, float))                                                          \
  X((Float64, double))                                                         \
  X((Float, float))                                                            \
  X((Double, double))                                                          \
  X((LongDouble, long double))                                                 \
  X((Int, int))                                                                \
  X((UInt, unsigned int))                                                      \
  X((Size, size_t))                                                            \
  X((PtrDiff, ptrdiff_t))                                                      \
  X((IntPtr, intptr_t))                                                        \
  X((UIntPtr, uintptr_t))                                                      \
  TUPLE_COMPLEX_TYPE_ROWS(X)

#ifndef TEMPLATE_Tuple
#define TEMPLATE_Tuple(TARGET, ...) NS(TEMPLATE_Tuple, TARGET)(__VA_ARGS__)
#endif

/* EOF */
