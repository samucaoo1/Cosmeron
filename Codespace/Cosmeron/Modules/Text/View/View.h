#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Encoding/UTF8.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "View.def"

TEMPLATE_INSTANTIATE(Text_View, 8, char);
TEMPLATE_INSTANTIATE(Text_View, 16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text_View, 32, MODULE_TYPE(Text, Char32));

/* UTF-8 specialized operations for Text_TView_8 */
MODULE(DOMAIN(Modules), Text,
    PACKAGE(View8,
        PREDICATE(Validate, (MODULE_TYPED(Text, View, 8) view)),
        OPERATION(CountCodepoints,
                  (MODULE_TYPED(Text, View, 8) view, size_t *outCount)),
        OPERATION(NextCodepoint,
                  (MODULE_TYPED(Text, View, 8) *view,
                   MODULE_TYPE(Text, Char32) *outCodepoint)),
        VARIANT(Trim,
                FUNCTION(MODULE_TYPED(Text, View, 8),
                         (MODULE_TYPED(Text, View, 8) view)),
                FUNCTION(MODULE_TYPED(Text, View, 8), Left,
                         (MODULE_TYPED(Text, View, 8) view)),
                FUNCTION(MODULE_TYPED(Text, View, 8), Right,
                         (MODULE_TYPED(Text, View, 8) view))
        ),
        OPERATION(SliceCodepoints,
                  (MODULE_TYPED(Text, View, 8) view, size_t codepointStart,
                   size_t codepointCount, MODULE_TYPED(Text, View, 8) *outView))
    )
);

#include "Impl/View.impl"
