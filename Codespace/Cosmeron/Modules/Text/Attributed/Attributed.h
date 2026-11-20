#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../Grid/Attribute.h"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_ATTRIBUTED_DISABLE_FUNCTION_TABLE) &&                         \
    !defined(COSMERON_DISABLE_FT_Text_Attributed)
#define COSMERON_DISABLE_FT_Text_Attributed 1
#endif

/* ========================================================================== */
/* Attributed Text Configuration Table                                        */
/* ========================================================================== */

#define TEXT_ATTRIBUTED_CONFIG_COL_NAME(NAME, VALUE) NAME
#define TEXT_ATTRIBUTED_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_ATTRIBUTED_CONFIG_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(TEXT_ATTRIBUTED_CONFIG_COL_, FIELD) ROW

#define TEXT_ATTRIBUTED_CONFIG_TABLE(X)                                        \
  X((INITIAL_TEXT_CAPACITY, 16U))                                              \
  X((INITIAL_SPAN_CAPACITY, 4U))                                               \
  X((GROWTH_FACTOR, 2U))

/* ========================================================================== */
/* Attributed Text Span & Object Declaration                                  */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Text,
    STRUCT(Attributed_Span,
        FIELDS(
            size_t start;
            size_t length;
            MODULE_TYPE(Text, Attribute, Cell) attribute;
        )
    ),
    OBJECT(Attributed,
        FIELDS(
            char *text;
            size_t length;
            size_t capacity;
            MODULE_TYPE(Text, Attributed, Span) *spans;
            size_t spanCount;
            size_t spanCapacity;
        ),
        ENUM(Config, TEXT_ATTRIBUTED_CONFIG_TABLE),
        OPERATION(Create, (size_t initialCapacity)),
        OPERATION(Destroy, ()),
        VARIANT(Clear,
            OPERATION(()),
            OPERATION(Spans, ())
        ),
        OPERATION(SetText, (const char *text, size_t length)),
        OPERATION(AppendText, (const char *text, size_t length)),
        OPERATION(AddSpan,
                  (size_t start, size_t length,
                   MODULE_TYPE(Text, Attribute, Cell) attribute)),
        CONST_OPERATION(GetAttributeAt,
                        (size_t index,
                         MODULE_TYPE(Text, Attribute, Cell) *outAttribute)),
        CONST_OPERATION(RenderAnsi,
                        (char *outBuffer, size_t bufferCapacity,
                         size_t *outWritten)),
        CONST_FUNCTION_INLINE(const char *, Text, (), {
          return (self && self->text) ? self->text : "";
        }),
        CONST_FUNCTION_INLINE(size_t, Length, (), {
          return self ? self->length : 0U;
        }),
        CONST_FUNCTION_INLINE(size_t, SpanCount, (), {
          return self ? self->spanCount : 0U;
        }),
        CONST_FUNCTION_INLINE(const MODULE_TYPE(Text, Attributed, Span) *,
                              Spans, (), {
          return self ? self->spans : NULL;
        })
    )
);

#include "Impl/Attributed.impl"

/* EOF */
