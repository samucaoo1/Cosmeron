#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"
#include "../Impl/Common.inc"
#include "Writer.h"

#include <stddef.h>
#include <stdint.h>

#define TEXT_BYTES_HUMAN_UNIT_COL_NAME(NAME, VALUE) NAME
#define TEXT_BYTES_HUMAN_UNIT_COL_VALUE(NAME, VALUE) VALUE
#define TEXT_BYTES_HUMAN_UNIT_GET(FIELD, ROW)                                  \
  PP_OP_CAT2(TEXT_BYTES_HUMAN_UNIT_COL_, FIELD) ROW

#define TEXT_BYTES_HUMAN_UNIT_TABLE(X)                                         \
  X((IEC, 0U))                                                                 \
  X((SI, 1U))

ENUM_DEF_EX(MODULE_NS(Text, BytesHuman), Unit, TEXT_BYTES_HUMAN_UNIT_TABLE,
            QUALIFIED)

MODULE(DOMAIN(Modules), Text,
    PACKAGE(BytesHuman,
        VARIANT(Write,
            OPERATION((MODULE_TYPE(Text, Writer) *writer, uint64_t bytes)),
            OPERATION(Ex,
                      (MODULE_TYPE(Text, Writer) *writer, uint64_t bytes,
                       MODULE_ENUM(Text, BytesHuman, Unit) unitStandard,
                       unsigned precision))
        ),

        VARIANT(Format,
            OPERATION((char *buffer, size_t capacity, size_t *outWritten,
                       uint64_t bytes)),
            OPERATION(Ex,
                      (char *buffer, size_t capacity, size_t *outWritten,
                       uint64_t bytes,
                       MODULE_ENUM(Text, BytesHuman, Unit) unitStandard,
                       unsigned precision))
        ),

        OPERATION(Parse, (const char *str, uint64_t *outBytes))
    )
);

#include "Impl/BytesHuman.impl"

/* EOF */
