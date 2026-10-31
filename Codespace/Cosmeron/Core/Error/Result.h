#pragma once

#include "Error.h"
#include "Status.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Common/Template.impl"
#include "../Module/Object.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define RESULT_TYPE(SUFFIX) GNS3(LIB_PREFIX(ERROR_MOD), TResult, SUFFIX)

#define RESULT_MAP_CALLBACK(SUFFIX)                                            \
  TCALLBACK(ERROR_INS(PP_OP_CAT3(ResultMap_, SUFFIX, _Function)))

#define RESULT_BIND_CALLBACK(SUFFIX)                                           \
  TCALLBACK(ERROR_INS(PP_OP_CAT3(ResultBind_, SUFFIX, _Function)))

#define RESULT_MAP_BIND(SUFFIX, FN, CTX)                                       \
  GNS2(CALLBACK_FUNC(ERROR_INS(PP_OP_CAT3(ResultMap_, SUFFIX, _Function))),   \
       Bind)((FN), (CTX))

#define RESULT_BIND_BIND(SUFFIX, FN, CTX)                                      \
  GNS2(CALLBACK_FUNC(ERROR_INS(PP_OP_CAT3(ResultBind_, SUFFIX, _Function))),  \
       Bind)((FN), (CTX))

/* ============================================================
 * Universal Template Definition: TRESULT
 * ============================================================ */

#define TEMPLATE_TRESULT(SUFFIX, TYPE)                                         \
  typedef struct RESULT_TYPE(SUFFIX) RESULT_TYPE(SUFFIX);                      \
  CALLBACK_DECLARE(ERROR_INS(PP_OP_CAT3(ResultMap_, SUFFIX, _Function)), TYPE, \
                   (TYPE value), (value));                                     \
  CALLBACK_DECLARE_OPERATION(                                                  \
      ERROR_INS(PP_OP_CAT3(ResultBind_, SUFFIX, _Function)),                  \
      (TYPE value, RESULT_TYPE(SUFFIX) * outResult), (value, outResult));      \
  OBJECT(                                                                      \
      RESULT_TYPE(SUFFIX),                                                     \
      FIELDS(                                                                  \
          bool isOk;                                                           \
          union {                                                              \
            TYPE value;                                                        \
            ERROR_TYPE(TError) error;                                          \
          } storage;                                                           \
      ),                                                                       \
      OPERATION_INLINE(Ok, (TYPE value), {                                     \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        self->isOk = true;                                                     \
        self->storage.value = value;                                           \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      OPERATION_INLINE(Err, (const ERROR_TYPE(TError) * error), {              \
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
          GNS2(ERROR_TYPE(TError), FromStatus)(&self->storage.error, status);               \
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
      CONST_OPERATION_INLINE(Error, (ERROR_TYPE(TError) * outError), {         \
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
            if (!self || !outResult ||                                         \
                !GNS2(CALLBACK_FUNC(ERROR_INS(                                 \
                          PP_OP_CAT3(ResultMap_, SUFFIX, _Function))),         \
                      IsValid)(mapper))                                        \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            if (!self->isOk) {                                                 \
              return GNS2(RESULT_TYPE(SUFFIX), Err)(outResult,                 \
                                                    &self->storage.error);     \
            }                                                                  \
            TYPE mapped =                                                      \
                GNS2(CALLBACK_FUNC(ERROR_INS(                                  \
                         PP_OP_CAT3(ResultMap_, SUFFIX, _Function))),          \
                     Invoke)(mapper, self->storage.value);                     \
            return GNS2(RESULT_TYPE(SUFFIX), Ok)(outResult, mapped);           \
          }),                                                                  \
      CONST_OPERATION_INLINE(                                                  \
          AndThen,                                                             \
          (RESULT_BIND_CALLBACK(SUFFIX) binder,                                \
           struct RESULT_TYPE(SUFFIX) * outResult),                            \
          {                                                                    \
            if (!self || !outResult ||                                         \
                !GNS2(CALLBACK_FUNC(ERROR_INS(                                 \
                          PP_OP_CAT3(ResultBind_, SUFFIX, _Function))),        \
                      IsValid)(binder))                                        \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            if (!self->isOk) {                                                 \
              return GNS2(RESULT_TYPE(SUFFIX), Err)(outResult,                 \
                                                    &self->storage.error);     \
            }                                                                  \
            return GNS2(CALLBACK_FUNC(ERROR_INS(                               \
                            PP_OP_CAT3(ResultBind_, SUFFIX, _Function))),      \
                        Invoke)(binder, self->storage.value, outResult);       \
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
