#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Object.h"
#include "../Grid/Attribute.h"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(TEXT_ATTRIBUTED_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

/* Attributed text span definition */
typedef struct MODULE_TYPE(Text, Attributed, Span) {
  size_t start;
  size_t length;
  MODULE_TYPE(Text, Attribute, Cell) attribute;
} MODULE_TYPE(Text, Attributed, Span);

typedef MODULE_TYPE(Text, Attributed, Span) TAttributedSpan;

/* Object-oriented Attributed Text structure definition */
MODULE(Text,
    OBJECT(AttributedText,
           FIELDS(char *text;
                  size_t length;
                  size_t capacity;
                  MODULE_TYPE(Text, Attributed, Span) * spans;
                  size_t spanCount;
                  size_t spanCapacity;),
       METHOD(OPSTATUS, Create, (size_t initialCapacity)),
       METHOD(OPSTATUS, Destroy, ()),
       METHOD(OPSTATUS, Clear, ()),
       METHOD(OPSTATUS, SetText, (const char *text, size_t length)),
       METHOD(OPSTATUS, AppendText, (const char *text, size_t length)),
       METHOD(OPSTATUS, AddSpan,
              (size_t start, size_t length,
               MODULE_TYPE(Text, Attribute, Cell) attribute)),
       METHOD(OPSTATUS, ClearSpans, ()),
       CONST_METHOD(OPSTATUS, GetAttributeAt,
                    (size_t index,
                     MODULE_TYPE(Text, Attribute, Cell) * outAttribute)),
       CONST_METHOD(OPSTATUS, RenderAnsi,
                    (char *outBuffer, size_t bufferCapacity,
                     size_t * outWritten)),
       CONST_METHOD(const char *, Text, ()),
       CONST_METHOD(size_t, Length, ()),
       CONST_METHOD(size_t, SpanCount, ()),
       CONST_METHOD(const MODULE_TYPE(Text, Attributed, Span) *, Spans, ()))
);

#include "Impl/Attributed.impl"

/* EOF */
