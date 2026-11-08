#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"

#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Stacktrace Configuration Table                                          */
/* ========================================================================== */

#define STACKTRACE_CONFIG_COL_NAME(NAME, VALUE) NAME
#define STACKTRACE_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define STACKTRACE_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(STACKTRACE_CONFIG_COL_, FIELD) ROW

#define STACKTRACE_CONFIG_TABLE(X) \
  X((MAX_FRAMES, 32U))

/* ========================================================================== */
/* 2. Declarative Object: Diagnostic_Stacktrace                               */
/* ========================================================================== */

MODULE(DOMAIN(Core), Diagnostic,
    OBJECT(Stacktrace,
        FIELDS(
            void *frames[MODULE_CONST(Diagnostic, Stacktrace, MAX_FRAMES)];
            size_t frameCount;
        ),
        ENUM(Config, STACKTRACE_CONFIG_TABLE),
        OPERATION(Capture, (size_t skipFrames, size_t maxFrames)),
        PROCEDURE(Clear, ()),
        CONST_FUNCTION_INLINE(size_t, FrameCount, (), {
          return self ? self->frameCount : 0U;
        }),
        CONST_OPERATION(FrameAt, (size_t index, void **outAddress)),
        CONST_OPERATION(Format, (char *outBuffer, size_t capacity, size_t *outWritten))
    )
);

typedef MODULE_TYPE(Diagnostic, Stacktrace) TDiagnosticStacktrace;
typedef MODULE_TYPE(Diagnostic, Stacktrace) TStacktrace;

#include "Impl/Stacktrace.impl"

/* EOF */

