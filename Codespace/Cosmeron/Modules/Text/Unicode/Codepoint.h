#pragma once

#include <stdbool.h>

#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"

MODULE(DOMAIN(Modules), Text,
    PACKAGE(Codepoint,
        VARIANT(Is,
            PREDICATE(Valid, (MODULE_TYPE(Text, Char32) codepoint)),
            PREDICATE(Scalar, (MODULE_TYPE(Text, Char32) codepoint)),
            PREDICATE(Ascii, (MODULE_TYPE(Text, Char32) codepoint)),
            PREDICATE(Control, (MODULE_TYPE(Text, Char32) codepoint)),
            PREDICATE(Whitespace, (MODULE_TYPE(Text, Char32) codepoint))
        )
    )
);

#include "Impl/Codepoint.impl"

/* EOF */
