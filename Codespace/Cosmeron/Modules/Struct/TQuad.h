#pragma once

#include "Common.inc"

/* ==========================================================================
 * Declarative Struct Specification: TQuad (Pure POD)
 * ========================================================================== */

MODULE(DOMAIN(Modules), Struct,
    TEMPLATE_STRUCT(Quad, (SUFFIX, TYPE),
        FIELDS(
            union {
              struct {
                union {
                  TYPE value1;
                  TYPE a;
                  TYPE x1;
                  TYPE x;
                  TYPE left;
                  TYPE red;
                };
                union {
                  TYPE value2;
                  TYPE b;
                  TYPE x2;
                  TYPE y;
                  TYPE right;
                  TYPE green;
                };
                union {
                  TYPE value3;
                  TYPE c;
                  TYPE x3;
                  TYPE z;
                  TYPE top;
                  TYPE blue;
                };
                union {
                  TYPE value4;
                  TYPE d;
                  TYPE x4;
                  TYPE w;
                  TYPE bottom;
                  TYPE alpha;
                };
              };
              union {
                TYPE value[4];
                TYPE values[4];
              };
            };
        )
    )
);

TEMPLATE_INSTANTIATE_TABLE(Quad, STRUCT_TYPE_TABLE);

/* EOF */
