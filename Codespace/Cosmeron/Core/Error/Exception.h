#pragma once

#include "Error.h"
#include "Error.space"
#include "Panic.h"
#include "Status.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Object.h"
#include "../Module/Package.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define EXCEPTION_MOD GNS2(ERROR_MOD, Exception)
#define EXCEPTION_CMOD PP_OP_CAT3(ERROR_CMOD, _, EXCEPTION)

#define EXCEPTION_NS(NAME) GNS3(LIB_PREFIX(ERROR_MOD), Exception, NAME)
#define EXCEPTION_CNS(NAME) CNS3(LIB_PREFIX_CONST(ERROR_CMOD), EXCEPTION, NAME)
#define EXCEPTION_INS(NAME) GNS4(LIB_PREFIX(ERROR_MOD), Exception, Internal, NAME)
#define EXCEPTION_CINS(NAME) CNS4(LIB_PREFIX_CONST(ERROR_CMOD), EXCEPTION, INTERNAL, NAME)

#define EXCEPTION_TYPE(NAME) EXCEPTION_NS(NAME)
#define EXCEPTION_FUNC(NAME) EXCEPTION_NS(NAME)
#define EXCEPTION_CONST(NAME) EXCEPTION_CNS(NAME)

CALLBACK_DECLARE_NULLARY_PROCEDURE(EXCEPTION_INS(CleanupFunction));
#define EXCEPTION_CLEANUP_CALLBACK TCALLBACK(EXCEPTION_INS(CleanupFunction))
#define EXCEPTION_CLEANUP_BIND(FN, CTX)                                        \
  GNS2(CALLBACK_FUNC(EXCEPTION_INS(CleanupFunction)), Bind)((FN), (CTX))

OBJECT(EXCEPTION_TYPE(TExceptionFrame),
    FIELDS(
        jmp_buf env;
        struct EXCEPTION_TYPE(TExceptionFrame) *previous;
        ERROR_TYPE(TError) error;
        int step;
        bool threw;
        bool caught;
        bool rethrow;
        EXCEPTION_CLEANUP_CALLBACK cleanup;
    ),
    CONST_FUNCTION_INLINE(const ERROR_TYPE(TError) *, Error, (), {
      return self ? &self->error : NULL;
    }),
    CONST_PREDICATE_INLINE(Threw, (), {
      return self ? self->threw : false;
    }),
    CONST_PREDICATE_INLINE(Caught, (), {
      return self ? self->caught : false;
    })
);

PACKAGE(LIB_PREFIX(EXCEPTION_MOD),
    PROCEDURE(PushFrame, (EXCEPTION_TYPE(TExceptionFrame) *frame)),
    PROCEDURE(PopFrame, ()),
    PROCEDURE(RegisterCleanup, (EXCEPTION_CLEANUP_CALLBACK cleanup)),
    PROCEDURE(Throw, (const ERROR_TYPE(TError) *error)),
    PROCEDURE(ThrowStatus,
              (OPSTATUS status, const char *message, const char *file,
               uint32_t line, const char *function)),
    PROCEDURE(Rethrow, ()),
    FUNCTION(const ERROR_TYPE(TError) *, CurrentError, ()),
    PREDICATE(HasActiveFrame, ())
);

/* Internal flow-control hooks */
static inline int EXCEPTION_INS(Begin)(
    EXCEPTION_TYPE(TExceptionFrame) *frame, int jumpVal) {
  if (jumpVal == 0) {
    frame->step = 0;
    frame->threw = false;
    frame->caught = false;
    frame->rethrow = false;
    EXCEPTION_FUNC(PushFrame)(frame);
    return 0;
  } else {
    frame->threw = true;
    return 1;
  }
}

static inline int EXCEPTION_INS(NextStep)(
    EXCEPTION_TYPE(TExceptionFrame) *frame, int currentStep) {
  (void)frame;
  if (currentStep == 0 || currentStep == 1) {
    return 2;
  }
  return 3;
}

static inline void EXCEPTION_INS(Finish)(
    EXCEPTION_TYPE(TExceptionFrame) *frame) {
  frame->step = 1;
  EXCEPTION_FUNC(PopFrame)();
  if (frame->threw && (!frame->caught || frame->rethrow)) {
    ERROR_TYPE(TError) err = frame->error;
    EXCEPTION_FUNC(Throw)(&err);
  }
}

/* Public control-flow macros */
#define TRY                                                                    \
  for (EXCEPTION_TYPE(TExceptionFrame) _cosm_frame = {0};                      \
       _cosm_frame.step == 0;                                                  \
       EXCEPTION_INS(Finish)(&_cosm_frame))                                    \
    for (int _cosm_step = EXCEPTION_INS(Begin)(&_cosm_frame,                  \
                                               setjmp(_cosm_frame.env));       \
         _cosm_step < 3;                                                       \
         _cosm_step = EXCEPTION_INS(NextStep)(&_cosm_frame, _cosm_step))       \
      if (_cosm_step == 0)

#define CATCH                                                                  \
  else if (_cosm_step == 1 && (_cosm_frame.caught = true))

#define FINALLY                                                                \
  else if (_cosm_step == 2)

#define THROW(ERROR_PTR) EXCEPTION_FUNC(Throw)(ERROR_PTR)

#define THROW_STATUS(STATUS, MSG)                                              \
  EXCEPTION_FUNC(ThrowStatus)((STATUS), (MSG), __FILE__, (uint32_t)__LINE__,   \
                             __func__)

#define RETHROW()                                                              \
  do {                                                                         \
    _cosm_frame.rethrow = true;                                                \
  } while (0)

#define CURRENT_ERROR() (&_cosm_frame.error)

#include "Impl/Exception.impl"
/* EOF */

