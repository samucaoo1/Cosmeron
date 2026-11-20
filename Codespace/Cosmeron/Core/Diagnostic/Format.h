#pragma once

#include "Impl/Common.inc"
#include "Context.h"
#include "../Condition/Error.h"
#include "../Condition/Status.h"
#include "../Module/Module.h"

#include <stddef.h>

/* ========================================================================== */
/* Declarative Package: Diagnostic_Format                                     */
/* ========================================================================== */

#if defined(DIAGNOSTIC_DISABLE_FUNCTION_TABLE) &&                              \
    !defined(COSMERON_DISABLE_FT_Diagnostic)
#define COSMERON_DISABLE_FT_Diagnostic 1
#endif
#if defined(DIAGNOSTIC_FORMAT_DISABLE_FUNCTION_TABLE) &&                       \
    !defined(COSMERON_DISABLE_FT_Diagnostic_Format)
#define COSMERON_DISABLE_FT_Diagnostic_Format 1
#endif

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

