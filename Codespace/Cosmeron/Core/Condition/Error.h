#pragma once

#include "../Diagnostic/Context.h"
#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Error Configuration Table & Enum                                           */
/* ========================================================================== */

#define CONDITION_ERROR_CONFIG_COL_NAME(NAME, VALUE) NAME
#define CONDITION_ERROR_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define CONDITION_ERROR_CONFIG_GET(FIELD, ROW)                                 \
  PP_OP_CAT2(CONDITION_ERROR_CONFIG_COL_, FIELD) ROW

#define CONDITION_ERROR_CONFIG_TABLE(X)                                        \
  X((MAX_CHAIN_DEPTH, 16))

/* ========================================================================== */
/* Object Declaration: Condition_Error                                        */
/* ========================================================================== */

MODULE(DOMAIN(Core), Condition,
    OBJECT(Error,
        FIELDS(
            OPSTATUS code;
            const char *message;
            MODULE_TYPE(Diagnostic, Context) context;
            const MODULE_TYPE(Condition, Error) *cause;
        ),
        ENUM(Config, CONDITION_ERROR_CONFIG_TABLE),
        OPERATION(Create, (OPSTATUS code, const char *message)),
        OPERATION(FromStatus, (OPSTATUS code)),
        PROCEDURE(Clear, ()),
        CONST_FUNCTION_INLINE(OPSTATUS, Code, (), {
          return self ? self->code : STATUS_CONST(INVALID_ARGUMENT);
        }),
        CONST_FUNCTION(const char *, Message, ()),
        VARIANT(Is,
            CONST_PREDICATE_INLINE((OPSTATUS code), {
              return self ? (self->code == code) : false;
            }),
            CONST_PREDICATE_INLINE(Success, (), {
              return self ? (self->code == STATUS_CONST(SUCCESS)) : false;
            }),
            CONST_PREDICATE_INLINE(Failure, (), {
              return self ? (self->code != STATUS_CONST(SUCCESS)) : true;
            }),
            CONST_PREDICATE_INLINE(Category, (MODULE_ENUM(Outcome, Status, Category) category), {
              return self ? MODULE_FUNC(Outcome, Status, IsCategory)(self->code, category) : false;
            })
        ),
        CONST_FUNCTION_INLINE(MODULE_ENUM(Outcome, Status, Category), Category, (), {
          return self ? MODULE_FUNC(Outcome, Status, CategoryOf)(self->code)
                      : STATUS_CATEGORY_CONST(GENERIC);
        }),
        OPERATION(SetContext, (const MODULE_TYPE(Diagnostic, Context) * context)),
        OPERATION(SetOrigin, (const char *file, uint32_t line,
                              const char *function, const char *description,
                              const void *userData)),
        CONST_FUNCTION_INLINE(const MODULE_TYPE(Diagnostic, Context) *, Context, (), {
          return self ? &self->context : NULL;
        }),
        CONST_PREDICATE_INLINE(HasContext, (), {
          return self ? MODULE_FUNC(Diagnostic, Context, HasOrigin)(&self->context)
                      : false;
        }),
        OPERATION(SetCause, (const MODULE_TYPE(Condition, Error) * cause)),
        CONST_PREDICATE_INLINE(HasCause, (), {
          return self && self->cause != NULL;
        }),
        CONST_FUNCTION_INLINE(const MODULE_TYPE(Condition, Error) *, Cause, (), {
          return self ? self->cause : NULL;
        }),
        CONST_FUNCTION(const MODULE_TYPE(Condition, Error) *, RootCause, ()),
        CONST_FUNCTION(size_t, Depth, ())
    )
);

#include "Impl/Error.impl"

/* EOF */
