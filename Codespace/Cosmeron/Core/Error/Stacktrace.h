#pragma once

#include "Error.space"
#include "Status.h"
#include "../Module/Object.h"

#include <stddef.h>
#include <stdint.h>

#define STACKTRACE_MOD GNS2(ERROR_MOD, Stacktrace)
#define STACKTRACE_CMOD PP_OP_CAT3(ERROR_CMOD, _, STACKTRACE)

#define STACKTRACE_NS(NAME) GNS3(LIB_PREFIX(ERROR_MOD), Stacktrace, NAME)
#define STACKTRACE_CNS(NAME) CNS3(LIB_PREFIX_CONST(ERROR_CMOD), STACKTRACE, NAME)
#define STACKTRACE_TYPE(NAME) STACKTRACE_NS(NAME)
#define STACKTRACE_CONST(NAME) STACKTRACE_CNS(NAME)

#define STACKTRACE_CONFIG_COL_NAME(NAME, VALUE) NAME
#define STACKTRACE_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define STACKTRACE_CONFIG_GET(FIELD, ROW)                                      \
  PP_OP_CAT2(STACKTRACE_CONFIG_COL_, FIELD) ROW

#define STACKTRACE_CONFIG_TABLE(X)                                             \
  X((MAX_FRAMES, 32))

enum STACKTRACE_TYPE(Config) {
#define STACKTRACE_CONFIG_ENUM_ITEM(ROW)                                       \
  STACKTRACE_CONST(STACKTRACE_CONFIG_GET(NAME, ROW)) =                         \
      STACKTRACE_CONFIG_GET(VALUE, ROW),
  STACKTRACE_CONFIG_TABLE(STACKTRACE_CONFIG_ENUM_ITEM)
#undef STACKTRACE_CONFIG_ENUM_ITEM
};

OBJECT(ERROR_TYPE(TStacktrace),
    FIELDS(
        void *frames[STACKTRACE_CONST(MAX_FRAMES)];
        size_t frameCount;
    ),
    OPERATION(Capture, (size_t skipFrames, size_t maxFrames)),
    PROCEDURE(Clear, ()),
    CONST_FUNCTION_INLINE(size_t, FrameCount, (), {
      return self ? self->frameCount : 0;
    }),
    CONST_OPERATION(FrameAt, (size_t index, void **outAddress)),
    CONST_OPERATION(Format, (char *outBuffer, size_t capacity, size_t *outWritten))
);

#include "Impl/Stacktrace.impl"
/* EOF */

