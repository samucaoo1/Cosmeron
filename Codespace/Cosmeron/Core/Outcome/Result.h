#pragma once

#include "Outcome.space"
#include "Status.h"
#include "../Condition/Error.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Common/Template.impl"
#include "../Module/Object.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define RESULT_MOD NS(OUTCOME_MOD, Result)
#define RESULT_TYPE(SUFFIX) NS(TResult, SUFFIX)

#define RESULT_MAP_CALLBACK(SUFFIX)                                            \
  TCALLBACK(NS(RESULT_TYPE(SUFFIX), MapFunction))

#define RESULT_BIND_CALLBACK(SUFFIX)                                           \
  TCALLBACK(NS(RESULT_TYPE(SUFFIX), BindFunction))

#define RESULT_MAP_BIND(SUFFIX, FN, CTX)                                       \
  CALLBACK_FUNC(NS(RESULT_TYPE(SUFFIX), MapFunction_Bind))((FN), (CTX))

#define RESULT_BIND_BIND(SUFFIX, FN, CTX)                                      \
  CALLBACK_FUNC(NS(RESULT_TYPE(SUFFIX), BindFunction_Bind))((FN), (CTX))

/* ============================================================
 * Universal Template Definition: TRESULT
 * ============================================================ */

#define TEMPLATE_TRESULT(SUFFIX, TYPE)                                         \
  typedef struct RESULT_TYPE(SUFFIX) RESULT_TYPE(SUFFIX);                      \
  OBJECT(                                                                      \
      RESULT_TYPE(SUFFIX),                                                     \
      FIELDS(                                                                  \
          bool isOk;                                                           \
          union {                                                              \
            TYPE value;                                                        \
            ERROR_TYPE(Error) error;                                           \
          } storage;                                                           \
      ),                                                                       \
      CALLBACK(                                                                \
          FUNCTION(TYPE, MapFunction, (TYPE value))                            \
      ),                                                                       \
      CALLBACK(                                                                \
          OPERATION(BindFunction,                                              \
                    (TYPE value, struct RESULT_TYPE(SUFFIX) * outResult))       \
      ),                                                                       \
      OPERATION_INLINE(Ok, (TYPE value), {                                     \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        self->isOk = true;                                                     \
        self->storage.value = value;                                           \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      OPERATION_INLINE(Err, (const ERROR_TYPE(Error) * error), {               \
        if (!self || !error)                                                   \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        self->isOk = false;                                                    \
        self->storage.error = *error;                                          \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      OPERATION_INLINE(FromStatus, (OPSTATUS status), {                        \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (status == STATUS_CONST(SUCCESS)) {                                 \
          self->isOk = true;                                                   \
          memset(&self->storage, 0, sizeof(self->storage));                   \
        } else {                                                               \
          self->isOk = false;                                                  \
          NS(ERROR_TYPE(Error), FromStatus)(&self->storage.error, status);   \
        }                                                                      \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      CONST_PREDICATE_INLINE(IsOk, (), {                                       \
        return self ? self->isOk : false;                                      \
      }),                                                                      \
      CONST_PREDICATE_INLINE(IsError, (), {                                    \
        return self ? !self->isOk : true;                                      \
      }),                                                                      \
      CONST_OPERATION_INLINE(Value, (TYPE * outValue), {                       \
        if (!self || !outValue)                                                \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!self->isOk)                                                       \
          return self->storage.error.code;                                     \
        *outValue = self->storage.value;                                       \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      CONST_OPERATION_INLINE(Error, (ERROR_TYPE(Error) * outError), {          \
        if (!self || !outError)                                                \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (self->isOk)                                                        \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        *outError = self->storage.error;                                       \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      CONST_FUNCTION_INLINE(TYPE, ValueOr, (TYPE fallback), {                  \
        return (self && self->isOk) ? self->storage.value : fallback;          \
      }),                                                                      \
      CONST_OPERATION_INLINE(                                                  \
          Map,                                                                 \
          (RESULT_MAP_CALLBACK(SUFFIX) mapper,                                 \
           struct RESULT_TYPE(SUFFIX) * outResult),                            \
          {                                                                    \
            if (!self || !outResult || !CALLBACK_IS_VALID(mapper))                \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            if (!self->isOk) {                                                 \
              return NS(RESULT_TYPE(SUFFIX), Err)(outResult,                 \
                                                    &self->storage.error);     \
            }                                                                  \
            TYPE mapped = CALLBACK_INVOKE(mapper, self->storage.value);        \
            return NS(RESULT_TYPE(SUFFIX), Ok)(outResult, mapped);           \
          }),                                                                  \
      CONST_OPERATION_INLINE(                                                  \
          AndThen,                                                             \
          (RESULT_BIND_CALLBACK(SUFFIX) binder,                                \
           struct RESULT_TYPE(SUFFIX) * outResult),                            \
          {                                                                    \
            if (!self || !outResult || !CALLBACK_IS_VALID(binder))                \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            if (!self->isOk) {                                                 \
              return NS(RESULT_TYPE(SUFFIX), Err)(outResult,                 \
                                                    &self->storage.error);     \
            }                                                                  \
            return CALLBACK_INVOKE(binder, self->storage.value, outResult);    \
          })                                                                   \
  )

#define TEMPLATE_TRESULT_INLINE_ONLY 1
#define TEMPLATE_TRESULT_IMPLEMENT(SUFFIX, TYPE) /* empty */

/* ============================================================
 * Default Instantiations via Universal Template Engine
 * ============================================================ */

TEMPLATE_IMPLEMENT(TRESULT, int, int);
TEMPLATE_IMPLEMENT(TRESULT, int32, int32_t);
TEMPLATE_IMPLEMENT(TRESULT, int64, int64_t);
TEMPLATE_IMPLEMENT(TRESULT, uint32, uint32_t);
TEMPLATE_IMPLEMENT(TRESULT, uint64, uint64_t);
TEMPLATE_IMPLEMENT(TRESULT, float, float);
TEMPLATE_IMPLEMENT(TRESULT, double, double);
TEMPLATE_IMPLEMENT(TRESULT, size, size_t);
TEMPLATE_IMPLEMENT(TRESULT, bool, bool);

/* EOF */

