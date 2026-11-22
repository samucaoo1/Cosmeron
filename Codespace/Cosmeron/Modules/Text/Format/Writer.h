#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "../Unicode/UTF8.h"
#include "../View/View.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Writer Target Mode Enum                                                    */
/* ========================================================================== */

#define TEXT_WRITER_MODE_COL_NAME(NAME, VALUE) NAME
#define TEXT_WRITER_MODE_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_WRITER_MODE_GET(FIELD, ROW)                                       \
  PP_OP_CAT2(TEXT_WRITER_MODE_COL_, FIELD) ROW

#define TEXT_WRITER_MODE_TABLE(X)                                              \
  X((COUNTER, 0U))                                                             \
  X((BUFFER, 1U))                                                              \
  X((DELEGATE, 2U))

ENUM_DEF_EX(MODULE_NS(Text, Writer), Mode, TEXT_WRITER_MODE_TABLE, QUALIFIED)

/* ========================================================================== */
/* Declarative Writer Sink Delegate, Struct & Package                         */
/* ========================================================================== */

MODULE(DOMAIN(Modules), Text,
    DELEGATE(
        OPERATION(WriterSink, (const char *data, size_t length))
    ),

    STRUCT(Writer,
        FIELDS(
            MODULE_ENUM(Text, Writer, Mode) mode;
            char *buffer;
            size_t capacity;
            size_t written;
            bool truncated;
            OPSTATUS status;
            MODULE_DELEGATE(Text, WriterSink) sink;
        )
    ),

    PACKAGE(Writer,
        FUNCTION(MODULE_TYPE(Text, Writer), ToBuffer,
                 (char *buffer, size_t capacity)),
        FUNCTION(MODULE_TYPE(Text, Writer), Counter, ()),
        FUNCTION(MODULE_TYPE(Text, Writer), ToDelegate,
                 (MODULE_DELEGATE(Text, WriterSink) sink)),

        FUNCTION(size_t, Written,
                 (const MODULE_TYPE(Text, Writer) *writer)),
        PREDICATE(IsTruncated,
                  (const MODULE_TYPE(Text, Writer) *writer)),
        FUNCTION(OPSTATUS, Status,
                 (const MODULE_TYPE(Text, Writer) *writer)),

        OPERATION(WriteBuffer,
                  (MODULE_TYPE(Text, Writer) *writer, const char *data,
                   size_t length)),
        OPERATION(WriteChar,
                  (MODULE_TYPE(Text, Writer) *writer, char ch)),
        OPERATION(WriteCodepoint,
                  (MODULE_TYPE(Text, Writer) *writer,
                   MODULE_TYPE(Text, Char32) codepoint)),
        OPERATION(WriteRepeat,
                  (MODULE_TYPE(Text, Writer) *writer, char ch, size_t count)),
        OPERATION(WriteCStr,
                  (MODULE_TYPE(Text, Writer) *writer, const char *str)),
        OPERATION(WriteView,
                  (MODULE_TYPE(Text, Writer) *writer,
                   MODULE_TYPE(Text, View8) view)),
        OPERATION(WriteBool,
                  (MODULE_TYPE(Text, Writer) *writer, bool value)),
        OPERATION(WriteInt,
                  (MODULE_TYPE(Text, Writer) *writer, intmax_t value,
                   unsigned base, bool uppercase)),
        OPERATION(WriteUInt,
                  (MODULE_TYPE(Text, Writer) *writer, uintmax_t value,
                   unsigned base, bool uppercase)),
        OPERATION(WriteFloat,
                  (MODULE_TYPE(Text, Writer) *writer, double value,
                   char specifier, bool hasPrecision, size_t precision)),
        OPERATION(WriteHex,
                  (MODULE_TYPE(Text, Writer) *writer, const void *bytes,
                   size_t size, bool uppercase)),
        OPERATION(WriteEscaped,
                  (MODULE_TYPE(Text, Writer) *writer, const char *data,
                   size_t length, bool quote)),
        OPERATION(Finish, (MODULE_TYPE(Text, Writer) *writer))
    )
);

#include "Impl/Writer.impl"

/* EOF */
