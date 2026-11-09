#pragma once

#include "Common.inc"

/* ==========================================================================
 * Declarative Struct Specification: TDual (Pure POD)
 * ========================================================================== */

MODULE(DOMAIN(Modules), Struct,
    TEMPLATE_STRUCT(Dual, (SUFFIX, TYPE),
        FIELDS(
            union {
              struct {
                union {
                  TYPE value1;
                  TYPE a;
                  TYPE x1;
                  TYPE x;
                  TYPE col;
                  TYPE length;
                  TYPE real;
                  TYPE state1;
                  TYPE begin;
                  TYPE first;
                  TYPE start;
                };
                union {
                  TYPE value2;
                  TYPE b;
                  TYPE x2;
                  TYPE y;
                  TYPE row;
                  TYPE width;
                  TYPE imaginary;
                  TYPE state2;
                  TYPE end;
                  TYPE second;
                  TYPE last;
                };
              };
              union {
                TYPE value[2];
                TYPE values[2];
              };
            };
        )
    )
);

TEMPLATE_INSTANTIATE_TABLE(Dual, STRUCT_TYPE_TABLE);

/* EOF */
