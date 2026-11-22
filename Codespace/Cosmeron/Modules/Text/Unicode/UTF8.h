#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "Codepoint.h"

MODULE(DOMAIN(Modules), Text,
    PACKAGE(UTF8,
        OPERATION(EncodedLength,
                  (MODULE_TYPE(Text, Char32) codepoint, size_t *outUnits)),
        OPERATION(Encode,
                  (MODULE_TYPE(Text, Char32) codepoint, unsigned char *output,
                   size_t capacity, size_t *outUnits)),
        OPERATION(Decode,
                  (const unsigned char *input, size_t size,
                   MODULE_TYPE(Text, Char32) *outCodepoint, size_t *outUnits)),
        PREDICATE(Validate, (const unsigned char *input, size_t size)),
        OPERATION(Count,
                  (const unsigned char *input, size_t size,
                   size_t *outCodepoints, size_t *outUnits))
    )
);

#include "Impl/UTF8.impl"

/* EOF */
