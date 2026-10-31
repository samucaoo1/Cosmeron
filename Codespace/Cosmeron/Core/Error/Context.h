#pragma once

#include "Status.h"
#include "../Module/Object.h"

OBJECT(ERROR_TYPE(TErrorContext),
    FIELDS(
        const char *file;
        uint32_t line;
        const char *function;
        const char *description;
        const void *userData;
    ),
    OPERATION(Create, (const char *file, uint32_t line, const char *function,
                       const char *description, const void *userData)),
    PROCEDURE(Clear, ()),
    CONST_PREDICATE_INLINE(HasOrigin, (), {
      return self && self->file != NULL;
    }),
    CONST_FUNCTION_INLINE(const char *, File, (), {
      return self ? self->file : NULL;
    }),
    CONST_FUNCTION_INLINE(uint32_t, Line, (), {
      return self ? self->line : 0U;
    }),
    CONST_FUNCTION_INLINE(const char *, Function, (), {
      return self ? self->function : NULL;
    }),
    CONST_FUNCTION_INLINE(const char *, Description, (), {
      return self ? self->description : NULL;
    }),
    CONST_FUNCTION_INLINE(const void *, UserData, (), {
      return self ? self->userData : NULL;
    })
);

#include "Impl/Context.impl"
/* EOF */
