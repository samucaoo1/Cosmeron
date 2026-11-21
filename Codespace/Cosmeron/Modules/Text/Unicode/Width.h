#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "Codepoint.h"
#include "UTF8.h"

MODULE(DOMAIN(Modules), Text,
    PACKAGE(Width,
        OPERATION(Codepoint, (MODULE_TYPE(Text, Char32) codepoint, int *outWidth)),
        OPERATION(UTF8, (const unsigned char *input, size_t size,
                         size_t *outColumns, size_t *outUnits))
    )
);

#include "Impl/Width.impl"

/* EOF */
