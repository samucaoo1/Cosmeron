#pragma once

#include "Error.space"
#include "Stacktrace.h"
#include "Status.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Package.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PANIC_MOD GNS2(ERROR_MOD, Panic)
#define PANIC_CMOD PP_OP_CAT3(ERROR_CMOD, _, PANIC)

#define PANIC_NS(NAME) GNS3(LIB_PREFIX(ERROR_MOD), Panic, NAME)
#define PANIC_CNS(NAME) CNS3(LIB_PREFIX_CONST(ERROR_CMOD), PANIC, NAME)
#define PANIC_INS(NAME) GNS4(LIB_PREFIX(ERROR_MOD), Panic, Internal, NAME)
#define PANIC_CINS(NAME) CNS4(LIB_PREFIX_CONST(ERROR_CMOD), PANIC, INTERNAL, NAME)

#define PANIC_TYPE(NAME) PANIC_NS(NAME)
#define PANIC_FUNC(NAME) PANIC_NS(NAME)
#define PANIC_CONST(NAME) PANIC_CNS(NAME)

#define PANIC_KIND_COL_NAME(NAME, MESSAGE) NAME
#define PANIC_KIND_COL_MESSAGE(NAME, MESSAGE) MESSAGE
#define PANIC_KIND_GET(FIELD, ROW) PP_OP_CAT2(PANIC_KIND_COL_, FIELD) ROW

#define PANIC_KIND_TABLE(X)                                                    \
  X((PANIC, "Explicit panic"))                                                 \
  X((ASSERT, "Development assertion failed"))                                  \
  X((VERIFY, "Runtime verification failed"))                                   \
  X((ENSURE, "Precondition contract failed"))                                  \
  X((UNREACHABLE, "Unreachable code path executed"))

typedef enum PANIC_TYPE(Kind) {
#define PANIC_KIND_ENUM_ITEM(ROW)                                              \
  PANIC_CONST(PP_OP_CAT2(KIND_, PANIC_KIND_GET(NAME, ROW))),
  PANIC_KIND_TABLE(PANIC_KIND_ENUM_ITEM)
#undef PANIC_KIND_ENUM_ITEM
} PANIC_TYPE(Kind);

typedef struct PANIC_TYPE(Info) {
  PANIC_TYPE(Kind) kind;
  const char *message;
  const char *expression;
  const char *file;
  uint32_t line;
  const char *function;
} PANIC_TYPE(Info);

CALLBACK_DECLARE_PROCEDURE(PANIC_INS(PanicHandler_Function),
                           (const PANIC_TYPE(Info) * info), (info));

#define PANIC_HANDLER_CALLBACK TCALLBACK(PANIC_INS(PanicHandler_Function))
#define PANIC_HANDLER_BIND(FN, CTX)                                            \
  GNS2(CALLBACK_FUNC(PANIC_INS(PanicHandler_Function)), Bind)((FN), (CTX))

PACKAGE(LIB_PREFIX(PANIC_MOD),
    PROCEDURE(SetHandler, (PANIC_HANDLER_CALLBACK handler)),
    FUNCTION(PANIC_HANDLER_CALLBACK, GetHandler, ()),
    PROCEDURE(ResetHandler, ()),
    FUNCTION(const char *, KindName, (PANIC_TYPE(Kind) kind)),
    PROCEDURE(Trigger,
              (PANIC_TYPE(Kind) kind, const char *message,
               const char *expression, const char *file, uint32_t line,
               const char *function)),
    PROCEDURE(Now, (const char *message))
);

/* Public contract macros */
#define PANIC(message)                                                         \
  PANIC_FUNC(Trigger)(PANIC_CONST(KIND_PANIC), (message), NULL, __FILE__,      \
                      (uint32_t)__LINE__, __func__)

#if defined(COSMERON_DISABLE_DEBUG_ASSERT)
#define ASSERT(condition) ((void)sizeof(condition))
#else
#define ASSERT(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      PANIC_FUNC(Trigger)(PANIC_CONST(KIND_ASSERT),                            \
                          "Assertion failed: " #condition, #condition,         \
                          __FILE__, (uint32_t)__LINE__, __func__);             \
    }                                                                          \
  } while (0)
#endif

#define VERIFY(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      PANIC_FUNC(Trigger)(PANIC_CONST(KIND_VERIFY),                            \
                          "Verification failed: " #condition, #condition,      \
                          __FILE__, (uint32_t)__LINE__, __func__);             \
    }                                                                          \
  } while (0)

#define ENSURE(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      PANIC_FUNC(Trigger)(PANIC_CONST(KIND_ENSURE),                            \
                          "Precondition contract failed: " #condition,         \
                          #condition, __FILE__, (uint32_t)__LINE__, __func__); \
    }                                                                          \
  } while (0)

#define UNREACHABLE()                                                          \
  do {                                                                         \
    PANIC_FUNC(Trigger)(PANIC_CONST(KIND_UNREACHABLE),                         \
                        "Unreachable code executed", NULL, __FILE__,           \
                        (uint32_t)__LINE__, __func__);                         \
  } while (0)

#define PANIC_ASSERT(condition) ASSERT(condition)

#include "Impl/Panic.impl"
/* EOF */
