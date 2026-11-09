#pragma once

#include "Common.inc"

/* ==========================================================================
 * Declarative Struct Specification: TPenta (Pure POD)
 * ========================================================================== */

MODULE(DOMAIN(Modules), Struct,
    TEMPLATE_STRUCT(Penta, (SUFFIX, TYPE),
        FIELDS(
            union {
              struct {
                union { TYPE value1; TYPE a; };
                union { TYPE value2; TYPE b; };
                union { TYPE value3; TYPE c; };
                union { TYPE value4; TYPE d; };
                union { TYPE value5; TYPE e; };
              };
              union {
                TYPE value[5];
                TYPE values[5];
              };
            };
        )
    )
);

TEMPLATE_INSTANTIATE_TABLE(Penta, STRUCT_TYPE_TABLE);

/* EOF */
