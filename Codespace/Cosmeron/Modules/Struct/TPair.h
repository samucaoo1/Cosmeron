#pragma once

#include "Common.inc"

/* ==========================================================================
 * Declarative Struct Specification: TPair (Pure POD)
 * ========================================================================== */

MODULE(DOMAIN(Modules), Struct,
    TEMPLATE_STRUCT(Pair, (SUFFIX, TYPE),
        FIELDS(
            union {
              struct {
                union {
                  TYPE value1;
                  TYPE first;
                  TYPE a;
                  TYPE x1;
                  TYPE x;
                };
                union {
                  TYPE value2;
                  TYPE second;
                  TYPE b;
                  TYPE x2;
                  TYPE y;
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

TEMPLATE_INSTANTIATE_TABLE(Pair, STRUCT_TYPE_TABLE);

/* EOF */
