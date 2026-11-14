#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Module/Module.h"

#include <stddef.h>
#include <stdint.h>

enum {
  CNS(Text, Format, BYTE_UNIT_IEC) = 0U,
  CNS(Text, Format, BYTE_UNIT_SI) = 1U,
  CNS(Text, Format, BYTES_DEFAULT_PRECISION) = 2U,
  CNS(Text, Format, BYTES_MAX_PRECISION) = 6U,
  CNS(Text, Format, BYTES_IEC_BASE) = 1024U,
  CNS(Text, Format, BYTES_SI_BASE) = 1000U
};

#if defined(TEXT_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(TEXT_BYTES_HUMAN_DISABLE_FUNCTION_TABLE)
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE
#endif
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
