#pragma once

#include "../Module/Callback/Callback.h"
#include "../Module/Object.h"
#include "../Module/Package.h"
#include "../Outcome/Status.h"
#include "Condition.space"
#include "Error.h"
#include "Panic.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define EXCEPTION_MOD GNS2(CONDITION_MOD, Exception)
#define EXCEPTION_CMOD PP_OP_CAT3(CONDITION_CMOD, _, EXCEPTION)

#define EXCEPTION_NS(NAME) GNS3(LIB_PREFIX(CONDITION_MOD), Exception, NAME)
#define EXCEPTION_CNS(NAME)                                                    \
  CNS3(LIB_PREFIX_CONST(CONDITION_CMOD), EXCEPTION, NAME)
#define EXCEPTION_INS(NAME)                                                    \
  GNS4(LIB_PREFIX(CONDITION_MOD), Exception, Internal, NAME)
#define EXCEPTION_CINS(NAME)                                                   \
  CNS4(LIB_PREFIX_CONST(CONDITION_CMOD), EXCEPTION, INTERNAL, NAME)

#define EXCEPTION_TYPE(NAME) EXCEPTION_NS(NAME)
#define EXCEPTION_FUNC(NAME) EXCEPTION_NS(NAME)
#define EXCEPTION_CONST(NAME) EXCEPTION_CNS(NAME)

#define EXCEPTION_CLEANUP_CALLBACK                                             \
  TCALLBACK(GNS2(EXCEPTION_TYPE(TExceptionFrame), Cleanup))
#define EXCEPTION_CLEANUP_BIND(FN, CTX)                                        \
  CALLBACK_FUNC(GNS2(EXCEPTION_TYPE(TExceptionFrame), Cleanup_Bind))((FN), (CTX))

OBJECT(EXCEPTION_TYPE(TExceptionFrame),
       FIELDS(jmp_buf env; struct EXCEPTION_TYPE(TExceptionFrame) * previous;
              ERROR_TYPE(Error) error; int step; bool threw; bool caught;
              bool rethrow; EXCEPTION_CLEANUP_CALLBACK cleanup;),
       CALLBACK(
           PROCEDURE(Cleanup, ())
       ),
       CONST_FUNCTION_INLINE(const ERROR_TYPE(Error) *, Error, (),
                             { return self ? &self->error : NULL; }),
       CONST_PREDICATE_INLINE(Threw, (),
                              { return self ? self->threw : false; }),
       CONST_PREDICATE_INLINE(Caught, (),
                              { return self ? self->caught : false; }),
       PROCEDURE_INLINE(SetCleanup, (EXCEPTION_CLEANUP_CALLBACK cleanup), {
         if (self)
           self->cleanup = cleanup;
       }));

PACKAGE(LIB_PREFIX(EXCEPTION_MOD),
        PROCEDURE(PushFrame, (EXCEPTION_TYPE(TExceptionFrame) * frame)),
        PROCEDURE(PopFrame, ()),
        PROCEDURE(RegisterCleanup, (EXCEPTION_CLEANUP_CALLBACK cleanup)),
        PROCEDURE(Throw, (const ERROR_TYPE(Error) * error)),
        PROCEDURE(ThrowStatus,
                  (OPSTATUS status, const char *message, const char *file,
                   uint32_t line, const char *function)),
        PROCEDURE(Rethrow, ()),
        FUNCTION(const ERROR_TYPE(Error) *, CurrentError, ()),
        PREDICATE(HasActiveFrame, ()));

static inline int EXCEPTION_INS(Begin)(EXCEPTION_TYPE(TExceptionFrame) * frame,
                                       int jumpResult) {
  if (jumpResult == 0) {
    frame->step = 0;
    frame->threw = false;
    frame->caught = false;
    frame->rethrow = false;
    frame->cleanup.function = NULL;
    frame->cleanup.context = NULL;
    EXCEPTION_FUNC(PushFrame)(frame);
    return 0;
  } else {
    frame->threw = true;
    return 1;
  }
}

static inline int EXCEPTION_INS(NextStep)(EXCEPTION_TYPE(TExceptionFrame) *
                                              frame,
                                          int currentStep) {
  (void)frame;
  if (currentStep == 0 || currentStep == 1) {
    return 2;
  }
  return 3;
}

static inline void EXCEPTION_INS(Finish)(EXCEPTION_TYPE(TExceptionFrame) *
                                         frame) {
  frame->step = 1;
  EXCEPTION_FUNC(PopFrame)();
  if (frame->threw && (!frame->caught || frame->rethrow)) {
    ERROR_TYPE(Error) err = frame->error;
    EXCEPTION_FUNC(Throw)(&err);
  }
}

/* Public control-flow macros */
#define TRY                                                                    \
  for (EXCEPTION_TYPE(TExceptionFrame) _cosm_frame = {0};                      \
       _cosm_frame.step == 0; EXCEPTION_INS(Finish)(&_cosm_frame))             \
    for (int _cosm_step =                                                      \
             EXCEPTION_INS(Begin)(&_cosm_frame, setjmp(_cosm_frame.env));      \
         _cosm_step < 3;                                                       \
         _cosm_step = EXCEPTION_INS(NextStep)(&_cosm_frame, _cosm_step))       \
      if (_cosm_step == 0)

#define CATCH else if (_cosm_step == 1 && (_cosm_frame.caught = true))

#define FINALLY else if (_cosm_step == 2)

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
