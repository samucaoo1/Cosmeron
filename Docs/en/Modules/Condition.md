# Condition

The **Condition** module centralizes operation status codes, structured errors, scope-exit cleanup, panic reporting, and exception-style control flow. It belongs to **Core**. Ordinary recoverable failures use `OPSTATUS`; the optional `TRY`/`CATCH` layer is built on C's `setjmp`/`longjmp`.

```c
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Condition/Defer.h"
#include "Cosmeron/Core/Condition/Panic.h"
#include "Cosmeron/Core/Condition/Exception.h"
```

Include only the headers needed. `Exception.h` includes `Error.h` and `Panic.h`.

---

# Overview

| Header | Purpose | Principal interface |
| --- | --- | --- |
| `Status.h` | Operation results and status metadata | `OPSTATUS`, `STATUS_CONST` |
| `Error.h` | Status plus message, context and cause | `MODULE_TYPE(Condition, Error)` |
| `Defer.h` | Scope-exit cleanup | `DEFER_SCOPE`, `DEFER`, `DEFER_TRAITS` |
| `Panic.h` | Contract violations and panic reporting | `PANIC`, `ASSERT`, `VERIFY`, `ENSURE` |
| `Exception.h` | Try/catch/finally-style control flow | `TRY`, `CATCH`, `FINALLY`, `THROW` |

# Status codes

`OPSTATUS` is the library's common result type. Successful operations return `STATUS_CONST(SUCCESS)`. Status values are not Boolean values: callers should compare them with named constants or use the Status package.

The package exposes `IsSuccess`, `IsFailure`, `IsSpecial`, `CategoryOf`, `IsCategory`, `CategoryName`, `Name` and `Message`. `IsSpecial` identifies the nonterminal statuses `WOULD_BLOCK` and `INCOMPLETE_SEQUENCE`.

```c
OPSTATUS status = STATUS_CONST(OUT_OF_RANGE);

if (MODULE_FUNC(Condition, Status, IsFailure)(status)) {
    const char *name = MODULE_FUNC(Condition, Status, Name)(status);
    const char *message = MODULE_FUNC(Condition, Status, Message)(status);
    (void)name;
    (void)message;
}
```

`STATUS_CONST(NAME)` accesses a status constant and `STATUS_CATEGORY_CONST(NAME)` accesses a category constant. A status is a compact outcome, not a complete diagnostic record.

---

# Structured errors

`MODULE_TYPE(Condition, Error)` stores a status code, optional message pointer, embedded `Diagnostic::Context` and optional cause pointer.

| Operation / observer | Purpose |
| --- | --- |
| `Create(code, message)` | Creates an error from a status and message |
| `FromStatus(code)` | Uses the status' default message |
| `Clear()` | Resets the object |
| `Code()` / `Message()` | Reads status and message |
| `Is(code)` | Compares the status |
| `Is.Success()` / `Is.Failure()` | Tests the success state |
| `Is.Category(category)` / `Category()` | Tests or reads the status category |
| `Set.Context(context)` | Copies a diagnostic context |
| `Set.Origin(file, line, function, description, userData)` | Sets source-origin fields |
| `Set.Cause(cause)` | Attaches a causal error |
| `Context()` / `HasContext()` | Reads/tests the context |
| `Cause()` / `HasCause()` | Reads/tests the direct cause |
| `RootCause()` / `Depth()` | Inspects the cause chain |

```c
MODULE_TYPE(Condition, Error) error = {0};

OPSTATUS status = API(&error, Create,
                      STATUS_CONST(OUT_OF_RANGE),
                      "Requested index is outside the collection");

if (status == STATUS_CONST(SUCCESS)) {
    API(&error, Set, Origin, __FILE__, (uint32_t)__LINE__,
        __func__, "Reading an element", NULL);
    status = API(&error, Code);
}
```

Message, context strings and user data are borrowed pointers. Keep their storage alive while the Error is used. A cause pointer is borrowed as well; do not attach a short-lived error to a longer-lived one.

---

# Deferred cleanup

`Defer.h` provides portable loop-based scope macros and GCC/Clang cleanup-attribute macros.

## `DEFER_SCOPE`

Runs an exit statement when the generated scope finishes:

```c
DEFER_SCOPE(close_resource()) {
    use_resource();
}
```

Avoid jumping into or out of the generated loop with `goto`.

## `DEFER_TRY_SCOPE`

Combines the exit action with an exception frame:

```c
DEFER_TRY_SCOPE(release_temporary()) {
    acquire_temporary();
    do_work_that_may_throw();
}
```

## `DEFER` and `DEFER_NAMED`

On GCC and Clang, `DEFER(function, context)` registers a callback for scope exit. `DEFER_NAMED(variable, function, context)` gives the node an explicit identifier. `DEFER_CANCEL(variable)` disables a pending cleanup. `DEFER_TRAITS(traits, instance)` and `DEFER_TRAITS_NAMED(...)` register the Type Traits disposal operation.

```c
static void release_handle(void *context)
{
    Handle_Close((Handle *)context);
}

void process(Handle *handle)
{
    DEFER(release_handle, handle);
    perform_work(handle);
}
```

The Defer package also exposes `Cancel`, `IsActive` and `Execute`. The compiler-cleanup macros are only defined for GCC and Clang; use `DEFER_SCOPE` where that extension is unavailable.

---

# Panic and contracts

Panic reports a failed assumption or contract that should not be treated as an ordinary recoverable status. `Panic_Info` carries the kind, message, expression, file, line and function. The package provides `SetHandler`, `GetHandler`, `ResetHandler`, `KindName`, `Trigger` and `Now`.

Public macros capture source location: `PANIC(message)`, `ASSERT(condition)`, `VERIFY(condition)`, `ENSURE(condition)`, `UNREACHABLE()` and `PANIC_ASSERT(condition)`. `ASSERT` is a development assertion and is compiled out when `COSMERON_DISABLE_DEBUG_ASSERT` is defined; `VERIFY` and `ENSURE` are runtime checks. These macros do not return `OPSTATUS`.

---

# Exception-style control flow

`Exception.h` manages `Exception_Frame` objects using `setjmp`/`longjmp`.

```c
TRY {
    perform_operation();
    if (failed)
        THROW_STATUS(STATUS_CONST(OUT_OF_RANGE), "Invalid input");
}
CATCH {
    const MODULE_TYPE(Condition, Error) *error = CURRENT_ERROR();
    log_error(error);
}
FINALLY {
    release_temporary_resources();
}
```

`THROW(error)` throws an existing `Condition_Error`; `THROW_STATUS(status, message)` constructs one with the current source location. `CATCH` handles the active frame's error, `CURRENT_ERROR()` exposes it and `RETHROW()` propagates it. `FINALLY` is the final clause.

The Exception package exposes `PushFrame`, `PopFrame`, `RegisterCleanup`, `Throw`, `ThrowStatus`, `Rethrow`, `CurrentError` and `HasActiveFrame`. Frame observers include `Error`, `Threw` and `Caught`; `SetCleanup` registers a cleanup closure.

**C rule:** automatic local variables modified after `setjmp` may become indeterminate after `longjmp` unless declared `volatile`. Do not jump across functions that have returned. Use this facility only when its non-local control flow is intentional; normal recoverable failures should use `OPSTATUS`.

---

# Choosing the right mechanism

| Situation | Use |
| --- | --- |
| Caller should decide how to recover | `OPSTATUS` |
| Caller needs a message, origin or cause chain | `Condition_Error` |
| A local resource needs scope-exit cleanup | `DEFER_SCOPE` or `DEFER` |
| A contract or invariant is violated | `ASSERT`, `VERIFY`, `ENSURE` |
| Code intentionally needs throw/catch control flow | `TRY`, `CATCH`, `FINALLY` |

# Build and portability notes

These interfaces are header-included C APIs. `DEFER_NAMED`, `DEFER`, `DEFER_TRAITS` and `DEFER_CANCEL` rely on GCC/Clang's `cleanup` attribute. Exception handling requires C's `setjmp` support. The status/error interface remains the ordinary portable path for recoverable failures.
