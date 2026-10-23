#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Module/Object.h"
#include "../Grid/Attribute.h"
#include "../Text.space"

#if defined(TEXT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(TEXT_ATTRIBUTED_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

#define TEXT_ATTRIBUTED_NS(NAME) GNS2(TEXT_NS(Attributed), NAME)
#define TEXT_ATTRIBUTED_CNS(NAME) CNS3(LIB_PREFIX_CONST(TEXT_CMOD), ATTRIBUTED, NAME)
#define TEXT_ATTRIBUTED_INS(NAME) GNS3(TEXT_NS(Attributed), Internal, NAME)

#define TEXT_ATTRIBUTED_TYPE(NAME) TEXT_ATTRIBUTED_NS(NAME)
#define TEXT_ATTRIBUTED_FUNC(NAME) GNS2(TEXT_ATTRIBUTED_TYPE(TAttributedText), NAME)
#define TEXT_ATTRIBUTED_CONST(NAME) TEXT_ATTRIBUTED_CNS(NAME)

/* Attributed text span definition */
typedef struct TEXT_ATTRIBUTED_TYPE(Span) {
  size_t start;
  size_t length;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute;
} TEXT_ATTRIBUTED_TYPE(Span);

typedef TEXT_ATTRIBUTED_TYPE(Span) TAttributedSpan;

/* Object-oriented Attributed Text structure definition */
OBJECT(TEXT_ATTRIBUTED_TYPE(TAttributedText),
       FIELDS(char *text;
              size_t length;
              size_t capacity;
              TEXT_ATTRIBUTED_TYPE(Span) * spans;
              size_t spanCount;
              size_t spanCapacity;),
       METHOD(OPSTATUS, Create, (size_t initialCapacity)),
       METHOD(OPSTATUS, Destroy, ()),
       METHOD(OPSTATUS, Clear, ()),
       METHOD(OPSTATUS, SetText, (const char *text, size_t length)),
       METHOD(OPSTATUS, AppendText, (const char *text, size_t length)),
       METHOD(OPSTATUS, AddSpan,
              (size_t start, size_t length,
               TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)),
       METHOD(OPSTATUS, ClearSpans, ()),
       CONST_METHOD(OPSTATUS, GetAttributeAt,
                    (size_t index,
                     TEXT_GRID_ATTRIBUTE_TYPE(Cell) * outAttribute)),
       CONST_METHOD(OPSTATUS, RenderAnsi,
                    (char *outBuffer, size_t bufferCapacity,
                     size_t * outWritten)),
       CONST_METHOD(const char *, Text, ()),
       CONST_METHOD(size_t, Length, ()),
       CONST_METHOD(size_t, SpanCount, ()),
       CONST_METHOD(const TEXT_ATTRIBUTED_TYPE(Span) *, Spans, ()));

typedef TEXT_ATTRIBUTED_TYPE(TAttributedText) TAttributedText;

#include "Impl/Attributed.impl"

/* EOF */
