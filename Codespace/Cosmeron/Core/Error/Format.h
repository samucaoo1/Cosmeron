#pragma once

#include "Context.h"
#include "Error.h"
#include "Status.h"
#include "../Module/Package.h"

#include <stddef.h>

PACKAGE(ERROR_NS(Format),
    OPERATION(Simple,
              (const ERROR_TYPE(TError) *error, char *outBuffer,
               size_t capacity, size_t *outWritten)),
    OPERATION(Detailed,
              (const ERROR_TYPE(TError) *error, char *outBuffer,
               size_t capacity, size_t *outWritten)),
    OPERATION(Chain,
              (const ERROR_TYPE(TError) *error, char *outBuffer,
               size_t capacity, size_t *outWritten)),
    OPERATION(Context,
              (const ERROR_TYPE(TErrorContext) *context, char *outBuffer,
               size_t capacity, size_t *outWritten))
);

#include "Impl/Format.impl"
/* EOF */

