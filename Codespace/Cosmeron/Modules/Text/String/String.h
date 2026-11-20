#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "../Encoding/UTF8.h"
#include "../Format/Format.h"
#include "../View/View.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "String.def"

TEMPLATE_INSTANTIATE(Text, String, 8, char);
TEMPLATE_INSTANTIATE(Text, String, 16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text, String, 32, MODULE_TYPE(Text, Char32));

/* UTF-8 specialized operations for TText_String8 */
MODULE(DOMAIN(Modules), Text,
    PACKAGE(String8,
        NO_FUNCTION_TABLE,
        OPERATION(AppendCodepoint,
                  (MODULE_TYPE(Text, String8) *str,
                   MODULE_TYPE(Text, Char32) codepoint)),
        OPERATION(CountCodepoints,
                  (const MODULE_TYPE(Text, String8) *str, size_t *outCount)),
        PREDICATE(Validate, (const MODULE_TYPE(Text, String8) *str)),
        VARIANT(Trim,
                OPERATION((MODULE_TYPE(Text, String8) *str)),
                OPERATION(Left, (MODULE_TYPE(Text, String8) *str)),
                OPERATION(Right, (MODULE_TYPE(Text, String8) *str))
        ),
        VARIANT(Format,
                OPERATION((MODULE_TYPE(Text, String8) *str, const char *format,
                           ...)),
                OPERATION(Variadic,
                          (MODULE_TYPE(Text, String8) *str, const char *format,
                           va_list arguments))
        ),
        VARIANT(AppendFormat,
                OPERATION((MODULE_TYPE(Text, String8) *str, const char *format,
                           ...)),
                OPERATION(Variadic,
                          (MODULE_TYPE(Text, String8) *str, const char *format,
                           va_list arguments))
        ),
        OPERATION(SliceCodepoints,
                  (const MODULE_TYPE(Text, String8) *str,
                   size_t codepointStart, size_t codepointCount,
                   MODULE_TYPE(Text, View8) *outView))
    )
);

#include "Impl/String.impl"

/* EOF */
