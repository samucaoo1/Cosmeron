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
 * Template Struct Internal Definitions for Batch Instantiation
 * ========================================================================== */

#define TEMPLATE_Pair_IMPLEMENT(...) /* empty */
#define TEMPLATE_Pair(SUFFIX, TYPE)                                            \
  TEMPLATE_STRUCT(Struct, Pair, SUFFIX, FIELDS(                               \
      union {                                                                  \
        struct {                                                               \
          union { TYPE value1; TYPE first; TYPE a; TYPE x1; TYPE x; };         \
          union { TYPE value2; TYPE second; TYPE b; TYPE x2; TYPE y; };        \
        };                                                                     \
        union { TYPE value[2]; TYPE values[2]; };                              \
      };                                                                       \
  ))

#define TEMPLATE_Dual_IMPLEMENT(...) /* empty */
#define TEMPLATE_Dual(SUFFIX, TYPE)                                            \
  TEMPLATE_STRUCT(Struct, Dual, SUFFIX, FIELDS(                               \
      union {                                                                  \
        struct {                                                               \
          union {                                                              \
            TYPE value1; TYPE a; TYPE x1; TYPE x; TYPE col;                    \
            TYPE length; TYPE real; TYPE state1; TYPE begin;                   \
            TYPE first; TYPE start;                                            \
          };                                                                   \
          union {                                                              \
            TYPE value2; TYPE b; TYPE x2; TYPE y; TYPE row;                    \
            TYPE width; TYPE imaginary; TYPE state2; TYPE end;                 \
            TYPE second; TYPE last;                                            \
          };                                                                   \
        };                                                                     \
        union { TYPE value[2]; TYPE values[2]; };                              \
      };                                                                       \
  ))

#define TEMPLATE_Triple_IMPLEMENT(...) /* empty */
#define TEMPLATE_Triple(SUFFIX, TYPE)                                          \
  TEMPLATE_STRUCT(Struct, Triple, SUFFIX, FIELDS(                             \
      union {                                                                  \
        struct {                                                               \
          union { TYPE value1; TYPE a; TYPE x1; TYPE x; TYPE length; TYPE red; };\
          union { TYPE value2; TYPE b; TYPE x2; TYPE y; TYPE width; TYPE green; };\
          union { TYPE value3; TYPE c; TYPE x3; TYPE z; TYPE height; TYPE blue; };\
        };                                                                     \
        union { TYPE value[3]; TYPE values[3]; };                              \
      };                                                                       \
  ))

#define TEMPLATE_Quad_IMPLEMENT(...) /* empty */
#define TEMPLATE_Quad(SUFFIX, TYPE)                                            \
  TEMPLATE_STRUCT(Struct, Quad, SUFFIX, FIELDS(                               \
      union {                                                                  \
        struct {                                                               \
          union { TYPE value1; TYPE a; TYPE x1; TYPE x; TYPE left; TYPE red; };\
          union { TYPE value2; TYPE b; TYPE x2; TYPE y; TYPE right; TYPE green; };\
          union { TYPE value3; TYPE c; TYPE x3; TYPE z; TYPE top; TYPE blue; };\
          union { TYPE value4; TYPE d; TYPE x4; TYPE w; TYPE bottom; TYPE alpha; };\
        };                                                                     \
        union { TYPE value[4]; TYPE values[4]; };                              \
      };                                                                       \
  ))

#define TEMPLATE_Penta_IMPLEMENT(...) /* empty */
#define TEMPLATE_Penta(SUFFIX, TYPE)                                           \
  TEMPLATE_STRUCT(Struct, Penta, SUFFIX, FIELDS(                              \
      union {                                                                  \
        struct {                                                               \
          union { TYPE value1; TYPE a; };                                      \
          union { TYPE value2; TYPE b; };                                      \
          union { TYPE value3; TYPE c; };                                      \
          union { TYPE value4; TYPE d; };                                      \
          union { TYPE value5; TYPE e; };                                      \
        };                                                                     \
        union { TYPE value[5]; TYPE values[5]; };                              \
      };                                                                       \
  ))

/* EOF */
