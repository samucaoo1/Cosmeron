#pragma once

#include "../../../Core/Algorithm/Comparison.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../Encoding/UTF8.h"
#include "../Format/Format.h"
#include "../View/View.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TEXT_STRING_INTERNAL_TABLE(X)                                          \
  X((INITIAL_CAPACITY, 16U))                                                   \
  X((GROWTH_FACTOR, 2U))

IENUM_DEF(Text_String, Config, TEXT_STRING_INTERNAL_TABLE)

#include "String.def"

TEMPLATE_INSTANTIATE(Text_String, 8, char);
TEMPLATE_INSTANTIATE(Text_String, 16, MODULE_TYPE(Text, Char16));
TEMPLATE_INSTANTIATE(Text_String, 32, MODULE_TYPE(Text, Char32));

/* UTF-8 specialized operations for Text_TString_8 */
MODULE(DOMAIN(Modules), Text,
    PACKAGE(String8,
        OPERATION(AppendCodepoint,
                  (MODULE_TYPED(Text, String, 8) *str,
                   MODULE_TYPE(Text, Char32) codepoint)),
        OPERATION(CountCodepoints,
                  (const MODULE_TYPED(Text, String, 8) *str, size_t *outCount)),
        PREDICATE(Validate, (const MODULE_TYPED(Text, String, 8) *str)),
        VARIANT(Trim,
                OPERATION((MODULE_TYPED(Text, String, 8) *str)),
                OPERATION(Left, (MODULE_TYPED(Text, String, 8) *str)),
                OPERATION(Right, (MODULE_TYPED(Text, String, 8) *str))
        ),
        VARIANT(Format,
                OPERATION((MODULE_TYPED(Text, String, 8) *str, const char *format,
                           ...)),
                OPERATION(Variadic,
                          (MODULE_TYPED(Text, String, 8) *str, const char *format,
                           va_list arguments))
        ),
        VARIANT(AppendFormat,
                OPERATION((MODULE_TYPED(Text, String, 8) *str, const char *format,
                           ...)),
                OPERATION(Variadic,
                          (MODULE_TYPED(Text, String, 8) *str, const char *format,
                           va_list arguments))
        ),
        OPERATION(SliceCodepoints,
                  (const MODULE_TYPED(Text, String, 8) *str,
                   size_t codepointStart, size_t codepointCount,
                   MODULE_TYPED(Text, View, 8) *outView))
    )
);

#include "Impl/String.impl"
