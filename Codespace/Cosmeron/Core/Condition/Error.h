#pragma once

#include "Condition.space"
#include "../Diagnostic/Context.h"
#include "../Outcome/Status.h"
#include "../Module/Object.h"

OBJECT(ERROR_TYPE(Error),
    FIELDS(
        OPSTATUS code;
        const char *message;
        DIAGNOSTIC_TYPE(Context) context;
        const struct ERROR_TYPE(Error) *cause;
    ),
    OPERATION(Create, (OPSTATUS code, const char *message)),
    OPERATION(FromStatus, (OPSTATUS code)),
    PROCEDURE(Clear, ()),
    CONST_FUNCTION_INLINE(OPSTATUS, Code, (), {
      return self ? self->code : STATUS_CONST(INVALID_ARGUMENT);
    }),
    CONST_FUNCTION(const char *, Message, ()),
    CONST_PREDICATE_INLINE(Is, (OPSTATUS code), {
      return self ? (self->code == code) : false;
    }),
    CONST_PREDICATE_INLINE(IsSuccess, (), {
      return self ? (self->code == STATUS_CONST(SUCCESS)) : false;
    }),
    CONST_PREDICATE_INLINE(IsFailure, (), {
      return self ? (self->code != STATUS_CONST(SUCCESS)) : true;
    }),
    CONST_FUNCTION_INLINE(STATUS_TYPE(Category), Category, (), {
      return self ? STATUS_FUNC(CategoryOf)(self->code)
                  : STATUS_CATEGORY_CONST(GENERIC);
    }),
    CONST_PREDICATE_INLINE(IsCategory, (STATUS_TYPE(Category) category), {
      return self ? STATUS_FUNC(IsCategory)(self->code, category) : false;
    }),
    OPERATION(SetContext, (const DIAGNOSTIC_TYPE(Context) *context)),
    OPERATION(SetOrigin, (const char *file, uint32_t line,
                          const char *function, const char *description,
                          const void *userData)),
    CONST_FUNCTION_INLINE(const DIAGNOSTIC_TYPE(Context) *, Context, (), {
      return self ? &self->context : NULL;
    }),
    CONST_PREDICATE_INLINE(HasContext, (), {
      return self ? NS(DIAGNOSTIC_TYPE(Context), HasOrigin)(&self->context)
                  : false;
    }),
    OPERATION(SetCause, (const struct ERROR_TYPE(Error) *cause)),
    CONST_PREDICATE_INLINE(HasCause, (), {
      return self && self->cause != NULL;
    }),
    CONST_FUNCTION_INLINE(const struct ERROR_TYPE(Error) *, Cause, (), {
      return self ? self->cause : NULL;
    }),
    CONST_FUNCTION(const struct ERROR_TYPE(Error) *, RootCause, ()),
    CONST_FUNCTION(size_t, Depth, ())
);

#include "Impl/Error.impl"
/* EOF */

