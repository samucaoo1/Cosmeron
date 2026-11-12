#pragma once

#include "../Module/Callback/Callback.h"
#include "../Module/Module.h"
#include "../Outcome/Status.h"
#include "Error.h"
#include "Panic.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Declarative Module: Condition (ExceptionFrame Object & Exception Package)*/
/* ========================================================================== */

MODULE(DOMAIN(Core), Condition,
    CLOSURE(
        PROCEDURE(ExceptionCleanup, ())
    ),
    OBJECT(ExceptionFrame,
        FIELDS(
            jmp_buf env;
            struct MODULE_TYPE(Condition, ExceptionFrame) *previous;
            MODULE_TYPE(Condition, Error) error;
            int step;
            bool threw;
            bool caught;
            bool rethrow;
            MODULE_CLOSURE(Condition, ExceptionCleanup) cleanup;
        ),
        CONST_FUNCTION_INLINE(const MODULE_TYPE(Condition, Error) *, Error, (), {
          return self ? &self->error : NULL;
        }),
        CONST_PREDICATE_INLINE(Threw, (), {
          return self ? self->threw : false;
        }),
        CONST_PREDICATE_INLINE(Caught, (), {
          return self ? self->caught : false;
        }),
        PROCEDURE_INLINE(SetCleanup,
                         (MODULE_CLOSURE(Condition, ExceptionCleanup) cleanup), {
          if (self)
            self->cleanup = cleanup;
        })
    ),
    PACKAGE(Exception,
        PROCEDURE(PushFrame, (MODULE_TYPE(Condition, ExceptionFrame) *frame)),
        PROCEDURE(PopFrame, ()),
        PROCEDURE(RegisterCleanup,
                  (MODULE_CLOSURE(Condition, ExceptionCleanup) cleanup)),
        PROCEDURE(Throw, (const MODULE_TYPE(Condition, Error) *error)),
        PROCEDURE(ThrowStatus,
                  (OPSTATUS status, const char *message, const char *file,
                   uint32_t line, const char *function)),
        PROCEDURE(Rethrow, ()),
        FUNCTION(const MODULE_TYPE(Condition, Error) *, CurrentError, ()),
        PREDICATE(HasActiveFrame, ())
    ),
    INTERNAL_EX(Exception,
        FUNCTION_INLINE(int, Begin,
                        (MODULE_TYPE(Condition, ExceptionFrame) *frame, int jumpResult), {
          if (jumpResult == 0) {
            frame->step = 0;
            frame->threw = false;
            frame->caught = false;
            frame->rethrow = false;
            frame->cleanup.function = NULL;
            frame->cleanup.context = NULL;
            MODULE_FUNC(Condition, Exception, PushFrame)(frame);
            return 0;
          } else {
            frame->threw = true;
            return 1;
          }
        }),

        FUNCTION_INLINE(int, NextStep,
                        (MODULE_TYPE(Condition, ExceptionFrame) *frame, int currentStep), {
          (void)frame;
          if (currentStep == 0 || currentStep == 1) {
            return 2;
          }
          return 3;
        }),

        PROCEDURE_INLINE(Finish,
                         (MODULE_TYPE(Condition, ExceptionFrame) *frame), {
          frame->step = 1;
          MODULE_FUNC(Condition, Exception, PopFrame)();
          if (frame->threw && (!frame->caught || frame->rethrow)) {
            MODULE_TYPE(Condition, Error) err = frame->error;
            MODULE_FUNC(Condition, Exception, Throw)(&err);
          }
        })
    )
);

/* ========================================================================== */
/* 3. Public Control-Flow Macros                                              */
/* ========================================================================== */

#define TRY \
  for (MODULE_TYPE(Condition, ExceptionFrame) _cosm_frame = {0}; \
       _cosm_frame.step == 0; \
       MODULE_IFUNC(Condition, Exception, Finish)(&_cosm_frame)) \
    for (int _cosm_step = \
             MODULE_IFUNC(Condition, Exception, Begin)( \
                 &_cosm_frame, setjmp(_cosm_frame.env)); \
         _cosm_step < 3; \
         _cosm_step = MODULE_IFUNC(Condition, Exception, NextStep)( \
             &_cosm_frame, _cosm_step)) \
      if (_cosm_step == 0)

#define CATCH else if (_cosm_step == 1 && (_cosm_frame.caught = true))

#define FINALLY else if (_cosm_step == 2)

#define THROW(ERROR_PTR) MODULE_FUNC(Condition, Exception, Throw)(ERROR_PTR)

#define THROW_STATUS(STATUS, MSG) \
  MODULE_FUNC(Condition, Exception, ThrowStatus)( \
      (STATUS), (MSG), __FILE__, (uint32_t)__LINE__, __func__)

#define RETHROW() \
  do { \
    _cosm_frame.rethrow = true; \
  } while (0)

#define CURRENT_ERROR() (&_cosm_frame.error)

#include "Impl/Exception.impl"

/* EOF */
