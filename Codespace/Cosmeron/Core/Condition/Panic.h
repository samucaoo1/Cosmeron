#pragma once

#include "../Diagnostic/Stacktrace.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Module.h"
#include "../Outcome/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Panic Kind Table                                                        */
/* ========================================================================== */

#define PANIC_KIND_COL_NAME(NAME, MESSAGE) NAME
#define PANIC_KIND_COL_MESSAGE(NAME, MESSAGE) MESSAGE
#define PANIC_KIND_GET(FIELD, ROW) PP_OP_CAT2(PANIC_KIND_COL_, FIELD) ROW

#define PANIC_KIND_TABLE(X) \
  X((PANIC,       "Explicit panic")) \
  X((ASSERT,      "Development assertion failed")) \
  X((VERIFY,      "Runtime verification failed")) \
  X((ENSURE,      "Precondition contract failed")) \
  X((UNREACHABLE, "Unreachable code path executed"))

/* ========================================================================== */
/* 2. Declarative Module: Condition (PanicKind, PanicInfo & Panic Package)    */
/* ========================================================================== */

MODULE(DOMAIN(Core), Condition,
    ENUM_EX(PanicKind, PANIC_KIND_TABLE, NAME_ONLY),
    STRUCT(PanicInfo,
        FIELDS(
            MODULE_ENUM(Condition, PanicKind) kind;
            const char *message;
            const char *expression;
            const char *file;
            uint32_t line;
            const char *function;
        )
    ),
    PACKAGE(Panic,
        CALLBACK(
            PROCEDURE(Handler, (const MODULE_TYPE(Condition, PanicInfo) *info))
        ),
        PROCEDURE(SetHandler, (TCALLBACK(MODULE_NS(Condition, Panic, Handler)) handler)),
        FUNCTION(TCALLBACK(MODULE_NS(Condition, Panic, Handler)), GetHandler, ()),
        PROCEDURE(ResetHandler, ()),
        FUNCTION(const char *, KindName, (MODULE_ENUM(Condition, PanicKind) kind)),
        PROCEDURE(Trigger, (MODULE_ENUM(Condition, PanicKind) kind, const char *message,
                            const char *expression, const char *file,
                            uint32_t line, const char *function)),
        PROCEDURE(Now, (const char *message))
    )
);

/* ========================================================================== */
/* 3. Public Contract Macros                                                  */
/* ========================================================================== */

#define PANIC(message) \
  MODULE_FUNC(Condition, Panic, Trigger)( \
      MODULE_CONST(Condition, PANIC), (message), NULL, __FILE__, \
      (uint32_t)__LINE__, __func__)

#if defined(COSMERON_DISABLE_DEBUG_ASSERT)
#define ASSERT(condition) ((void)sizeof(condition))
#else
#define ASSERT(condition) \
  do { \
    if (!(condition)) { \
      MODULE_FUNC(Condition, Panic, Trigger)( \
          MODULE_CONST(Condition, ASSERT), \
          "Assertion failed: " #condition, #condition, \
          __FILE__, (uint32_t)__LINE__, __func__); \
    } \
  } while (0)
#endif

#define VERIFY(condition) \
  do { \
    if (!(condition)) { \
      MODULE_FUNC(Condition, Panic, Trigger)( \
          MODULE_CONST(Condition, VERIFY), \
          "Verification failed: " #condition, #condition, \
          __FILE__, (uint32_t)__LINE__, __func__); \
    } \
  } while (0)

#define ENSURE(condition) \
  do { \
    if (!(condition)) { \
      MODULE_FUNC(Condition, Panic, Trigger)( \
          MODULE_CONST(Condition, ENSURE), \
          "Precondition contract failed: " #condition, \
          #condition, __FILE__, (uint32_t)__LINE__, __func__); \
    } \
  } while (0)

#define UNREACHABLE() \
  do { \
    MODULE_FUNC(Condition, Panic, Trigger)( \
        MODULE_CONST(Condition, UNREACHABLE), \
        "Unreachable code executed", NULL, __FILE__, \
        (uint32_t)__LINE__, __func__); \
  } while (0)

#define PANIC_ASSERT(condition) ASSERT(condition)

#include "Impl/Panic.impl"

/* EOF */

