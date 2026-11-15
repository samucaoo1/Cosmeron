#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Module.h"
#include "../Grid/Attribute.h"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(TEXT_ATTRIBUTED_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

/* Attributed text span and object definition */
MODULE(DOMAIN(Modules), Text,
    STRUCT(Attributed_Span,
        FIELDS(
            size_t start;
            size_t length;
            MODULE_TYPE(Text, Attribute, Cell) attribute;
        )
    ),
    OBJECT(AttributedText,
           FIELDS(char *text;
                  size_t length;
                  size_t capacity;
                  MODULE_TYPE(Text, Attributed, Span) * spans;
                  size_t spanCount;
                  size_t spanCapacity;),
       OPERATION(Create, (size_t initialCapacity)),
       OPERATION(Destroy, ()),
       OPERATION(Clear, ()),
       OPERATION(SetText, (const char *text, size_t length)),
       OPERATION(AppendText, (const char *text, size_t length)),
       OPERATION(AddSpan,
                 (size_t start, size_t length,
                  MODULE_TYPE(Text, Attribute, Cell) attribute)),
       OPERATION(ClearSpans, ()),
       CONST_OPERATION(GetAttributeAt,
                       (size_t index,
                        MODULE_TYPE(Text, Attribute, Cell) * outAttribute)),
       CONST_OPERATION(RenderAnsi,
                       (char *outBuffer, size_t bufferCapacity,
                        size_t * outWritten)),
       CONST_FUNCTION(const char *, Text, ()),
       CONST_FUNCTION(size_t, Length, ()),
       CONST_FUNCTION(size_t, SpanCount, ()),
       CONST_FUNCTION(const MODULE_TYPE(Text, Attributed, Span) *, Spans, ()))
);

#include "Impl/Attributed.impl"

/* EOF */
