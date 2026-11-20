#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Encoding/UTF8.h"
#include "Codepoint.h"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_WIDTH_DISABLE_FUNCTION_TABLE) &&                              \
    !defined(COSMERON_DISABLE_FT_Text_Width)
#define COSMERON_DISABLE_FT_Text_Width 1
#endif

MODULE(DOMAIN(Modules), Text,
    PACKAGE(Width,
        OPERATION(Codepoint, (MODULE_TYPE(Text, Char32) codepoint, int *outWidth)),
        OPERATION(UTF8, (const unsigned char *input, size_t size,
                         size_t *outColumns, size_t *outUnits))
    )
);

#include "Impl/Width.impl"

/* EOF */
