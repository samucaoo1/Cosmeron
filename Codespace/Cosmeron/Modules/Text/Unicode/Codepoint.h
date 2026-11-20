#pragma once

#include <stdbool.h>

#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_CODEPOINT_DISABLE_FUNCTION_TABLE) &&                          \
    !defined(COSMERON_DISABLE_FT_Text_Codepoint)
#define COSMERON_DISABLE_FT_Text_Codepoint 1
#endif

MODULE(DOMAIN(Modules), Text,
    PACKAGE(Codepoint,
        VARIANT(Is,
                PREDICATE(Valid, (MODULE_TYPE(Text, Char32) codepoint)),
                PREDICATE(Scalar, (MODULE_TYPE(Text, Char32) codepoint)),
                PREDICATE(ASCII, (MODULE_TYPE(Text, Char32) codepoint)),
                PREDICATE(Control, (MODULE_TYPE(Text, Char32) codepoint)),
                PREDICATE(Whitespace, (MODULE_TYPE(Text, Char32) codepoint))
        )
    )
);

#include "Impl/Codepoint.impl"

/* EOF */
