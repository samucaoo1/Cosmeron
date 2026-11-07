#pragma once

#include "../Diagnostic/Context.h"
#include "../Outcome/Status.h"
#include "../Module/Object.h"

#define ERROR_CONFIG_COL_NAME(NAME, VALUE) NAME
#define ERROR_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define ERROR_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(ERROR_CONFIG_COL_, FIELD) ROW

#define ERROR_CONFIG_TABLE(X)                                                  \
  X((MAX_CHAIN_DEPTH, 16))

enum CNS(LIB_PREFIX_CONST(CONDITION), ERROR, Config) {
#define ERROR_CONFIG_ENUM_ITEM(ROW)                                            \
  CNS(LIB_PREFIX_CONST(CONDITION), ERROR, ERROR_CONFIG_GET(NAME, ROW)) =        \
      ERROR_CONFIG_GET(VALUE, ROW),
  ERROR_CONFIG_TABLE(ERROR_CONFIG_ENUM_ITEM)
#undef ERROR_CONFIG_ENUM_ITEM
};

#ifndef ERROR_TYPE
#define ERROR_TYPE(NAME) PP_OP_CAT2(T, NAME)
#endif
#ifndef ERROR_FUNC
#define ERROR_FUNC(NAME) NS(ERROR_TYPE(Error), NAME)
#endif
#ifndef ERROR_CONST
#define ERROR_CONST(NAME) CNS(LIB_PREFIX_CONST(CONDITION), ERROR, NAME)
#endif

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
    CONST_FUNCTION_INLINE(MODULE_ENUM(Status, Category), Category, (), {
      return self ? Status_CategoryOf(self->code)
                  : STATUS_CONST(CATEGORY_GENERIC);
    }),
    CONST_PREDICATE_INLINE(IsCategory, (MODULE_ENUM(Status, Category) category), {
      return self ? Status_IsCategory(self->code, category) : false;
    }),
    OPERATION(SetContext, (const DIAGNOSTIC_TYPE(Context) * context)),
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
    OPERATION(SetCause, (const struct ERROR_TYPE(Error) * cause)),
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
