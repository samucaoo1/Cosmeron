#pragma once

#include "Common.h"
#include "Context.h"
#include "../Condition/Error.h"
#include "../Outcome/Status.h"
#include "../Module/Package.h"

#include <stddef.h>

PACKAGE(LIB_PREFIX(Diagnostic_Format),
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

