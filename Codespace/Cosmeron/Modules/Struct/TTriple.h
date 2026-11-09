#pragma once

#include "Common.inc"

/* ==========================================================================
 * Declarative Struct Specification: TTriple (Pure POD)
 * ========================================================================== */

MODULE(DOMAIN(Modules), Struct,
    TEMPLATE_STRUCT(Triple, (SUFFIX, TYPE),
        FIELDS(
            union {
              struct {
                union {
                  TYPE value1;
                  TYPE a;
                  TYPE x1;
                  TYPE x;
                  TYPE length;
                  TYPE red;
                };
                union {
                  TYPE value2;
                  TYPE b;
                  TYPE x2;
                  TYPE y;
                  TYPE width;
                  TYPE green;
                };
                union {
                  TYPE value3;
                  TYPE c;
                  TYPE x3;
                  TYPE z;
                  TYPE height;
                  TYPE blue;
                };
              };
              union {
                TYPE value[3];
                TYPE values[3];
              };
            };
        )
    )
);

TEMPLATE_INSTANTIATE_TABLE(Triple, STRUCT_TYPE_TABLE);

/* EOF */
