#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "Codepoint.h"
#include "UTF8.h"

MODULE(DOMAIN(Modules), Text,
    PACKAGE(UTF16,
        OPERATION(EncodedLength,
                  (MODULE_TYPE(Text, Char32) codepoint, size_t *outUnits)),
        OPERATION(Encode,
                  (MODULE_TYPE(Text, Char32) codepoint,
                   MODULE_TYPE(Text, Char16) *output, size_t capacity,
                   size_t *outUnits)),
        OPERATION(Decode,
                  (const MODULE_TYPE(Text, Char16) *input, size_t size,
                   MODULE_TYPE(Text, Char32) *outCodepoint, size_t *outUnits)),
        PREDICATE(Validate,
                  (const MODULE_TYPE(Text, Char16) *input, size_t size)),
        OPERATION(Count,
                  (const MODULE_TYPE(Text, Char16) *input, size_t size,
                   size_t *outCodepoints, size_t *outUnits)),
        OPERATION(FromUTF8,
                  (const unsigned char *input, size_t size,
                   MODULE_TYPE(Text, Char16) *output, size_t capacity,
                   size_t *outUnits)),
        OPERATION(ToUTF8,
                  (const MODULE_TYPE(Text, Char16) *input, size_t size,
                   unsigned char *output, size_t capacity, size_t *outBytes))
    )
);

#include "Impl/UTF16.impl"

/* EOF */
