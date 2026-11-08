#pragma once

#include "Common.inc"
#include "Context.h"
#include "../Condition/Error.h"
#include "../Outcome/Status.h"
#include "../Module/Module.h"

#include <stddef.h>

/* ========================================================================== */
/* Declarative Package: Diagnostic_Format                                     */
/* ========================================================================== */

MODULE(DOMAIN(Core), Diagnostic,
    PACKAGE(Format,
        OPERATION(Simple,
                  (const MODULE_TYPE(Condition, Error) *error, char *outBuffer,
                   size_t capacity, size_t *outWritten)),
        OPERATION(Detailed,
                  (const MODULE_TYPE(Condition, Error) *error, char *outBuffer,
                   size_t capacity, size_t *outWritten)),
        OPERATION(Chain,
                  (const MODULE_TYPE(Condition, Error) *error, char *outBuffer,
                   size_t capacity, size_t *outWritten)),
        OPERATION(Context,
                  (const MODULE_TYPE(Diagnostic, Context) *context, char *outBuffer,
                   size_t capacity, size_t *outWritten))
    )
);

#include "Impl/Format.impl"

/* EOF */

