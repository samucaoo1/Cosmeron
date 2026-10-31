#pragma once

#include "Context.h"
#include "Status.h"
#include "../Module/Object.h"

OBJECT(ERROR_TYPE(TError),
    FIELDS(
        OPSTATUS code;
        const char *message;
        ERROR_TYPE(TErrorContext) context;
        const struct ERROR_TYPE(TError) *cause;
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
    OPERATION(SetContext, (const ERROR_TYPE(TErrorContext) *context)),
    OPERATION(SetOrigin, (const char *file, uint32_t line,
                          const char *function, const char *description,
                          const void *userData)),
    CONST_FUNCTION_INLINE(const ERROR_TYPE(TErrorContext) *, Context, (), {
      return self ? &self->context : NULL;
    }),
    CONST_PREDICATE_INLINE(HasContext, (), {
      return self ? GNS2(ERROR_TYPE(TErrorContext), HasOrigin)(&self->context)
                  : false;
    }),
    OPERATION(SetCause, (const struct ERROR_TYPE(TError) *cause)),
    CONST_PREDICATE_INLINE(HasCause, (), {
      return self && self->cause != NULL;
    }),
    CONST_FUNCTION_INLINE(const struct ERROR_TYPE(TError) *, Cause, (), {
      return self ? self->cause : NULL;
    }),
    CONST_FUNCTION(const struct ERROR_TYPE(TError) *, RootCause, ()),
    CONST_FUNCTION(size_t, Depth, ())
);

#include "Impl/Error.impl"
/* EOF */
