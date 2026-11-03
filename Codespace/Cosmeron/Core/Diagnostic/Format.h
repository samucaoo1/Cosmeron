#pragma once

#include "Diagnostic.space"
#include "Context.h"
#include "../Condition/Error.h"
#include "../Outcome/Status.h"
#include "../Module/Package.h"

#include <stddef.h>

#define FORMAT_MOD GNS2(DIAGNOSTIC_MOD, Format)
#define FORMAT_CMOD PP_OP_CAT3(DIAGNOSTIC_CMOD, _, FORMAT)
#define FORMAT_NS(NAME) GNS3(LIB_PREFIX(DIAGNOSTIC_MOD), Format, NAME)
#define FORMAT_FUNC(NAME) FORMAT_NS(NAME)

PACKAGE(DIAGNOSTIC_NS(Format),
    OPERATION(Simple,
              (const ERROR_TYPE(Error) *error, char *outBuffer,
               size_t capacity, size_t *outWritten)),
    OPERATION(Detailed,
              (const ERROR_TYPE(Error) *error, char *outBuffer,
               size_t capacity, size_t *outWritten)),
    OPERATION(Chain,
              (const ERROR_TYPE(Error) *error, char *outBuffer,
               size_t capacity, size_t *outWritten)),
    OPERATION(Context,
              (const DIAGNOSTIC_TYPE(Context) *context, char *outBuffer,
               size_t capacity, size_t *outWritten))
);

#include "Impl/Format.impl"
/* EOF */

