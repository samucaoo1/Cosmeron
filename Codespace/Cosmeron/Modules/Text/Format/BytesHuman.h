#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"

#include <stddef.h>
#include <stdint.h>

#define TEXT_FORMAT_BYTES_HUMAN_TABLE(X)                                       \
  X((BYTE_UNIT_IEC, 0U))                                                       \
  X((BYTE_UNIT_SI, 1U))                                                        \
  X((BYTES_DEFAULT_PRECISION, 2U))                                             \
  X((BYTES_MAX_PRECISION, 6U))                                                 \
  X((BYTES_IEC_BASE, 1024U))                                                   \
  X((BYTES_SI_BASE, 1000U))

ENUM_DEF(Text_Format, BytesHumanConfig, TEXT_FORMAT_BYTES_HUMAN_TABLE)

#if defined(TEXT_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Text)
#define COSMERON_DISABLE_FT_Text 1
#endif
#if defined(TEXT_BYTES_HUMAN_DISABLE_FUNCTION_TABLE) &&                        \
    !defined(COSMERON_DISABLE_FT_Text_BytesHuman)
#define COSMERON_DISABLE_FT_Text_BytesHuman 1
#endif

MODULE(DOMAIN(Modules), Text,
    PACKAGE(BytesHuman,
        VARIANT(Format,
                OPERATION((char *buffer, size_t capacity, size_t *outWritten,
                           uint64_t bytes)),
                OPERATION(Ex,
                          (char *buffer, size_t capacity, size_t *outWritten,
                           uint64_t bytes, unsigned unitStandard,
                           unsigned precision))
        ),
        OPERATION(Parse, (const char *str, uint64_t *outBytes))
    )
);

#include "Impl/BytesHuman.impl"
