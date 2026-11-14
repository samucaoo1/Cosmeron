#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Unicode/Codepoint.h"

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
                  (const MODULE_TYPE(Text, Char16) *input, size_t size))
    )
);

#include "Impl/UTF16.impl"
