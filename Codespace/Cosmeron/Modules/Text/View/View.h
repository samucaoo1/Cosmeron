#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "../Encoding/UTF8.h"
#include "../Unicode/Codepoint.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "View.def"

TEMPLATE_INSTANTIATE(Text, View, 8, char);
TEMPLATE_INSTANTIATE(Text, View, 16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text, View, 32, MODULE_TYPE(Text, Char32));

/* UTF-8 specialized operations for TText_View8 */
MODULE(DOMAIN(Modules), Text,
    PACKAGE(View8,
        NO_FUNCTION_TABLE,
        PREDICATE(Validate, (MODULE_TYPE(Text, View8) view)),
        OPERATION(CountCodepoints,
                  (MODULE_TYPE(Text, View8) view, size_t *outCount)),
        OPERATION(NextCodepoint,
                  (MODULE_TYPE(Text, View8) *view,
                   MODULE_TYPE(Text, Char32) *outCodepoint)),
        VARIANT(Trim,
                FUNCTION(MODULE_TYPE(Text, View8),
                         (MODULE_TYPE(Text, View8) view)),
                FUNCTION(MODULE_TYPE(Text, View8), Left,
                         (MODULE_TYPE(Text, View8) view)),
                FUNCTION(MODULE_TYPE(Text, View8), Right,
                         (MODULE_TYPE(Text, View8) view))
        ),
        OPERATION(SliceCodepoints,
                  (MODULE_TYPE(Text, View8) view, size_t codepointStart,
                   size_t codepointCount, MODULE_TYPE(Text, View8) *outView))
    )
);

#include "Impl/View.impl"

/* EOF */
