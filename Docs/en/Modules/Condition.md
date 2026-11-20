# Condition

The **Condition** module groups operational statuses, rich error values, scoped cleanup, panic handling and structured exception frames. Use the lightest mechanism that fits the failure: return an `OPSTATUS` for ordinary fallible operations, attach a `Condition_Error` when context or a cause chain is useful, and reserve panic/exception facilities for control-flow contracts that require them. The Condition headers disable function-table dispatch; call package functions and `Error` methods directly through `MODULE_FUNC(Condition, ...).

## Headers

~~~c
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Condition/Defer.h"
#include "Cosmeron/Core/Condition/Panic.h"
#include "Cosmeron/Core/Condition/Exception.h"
~~~

---

# Overview

| Component | Purpose |
| --- | --- |
| `Status` | Defines `OPSTATUS`, status categories and symbolic/message lookup. |
| `Error` | Stores a status, message, diagnostic origin and optional cause. |
| `Defer` | Runs cleanup actions on lexical-scope exit or exception unwinding. |
| `Panic` | Reports a violated assertion, precondition, invariant or unreachable path. |
| `Exception` | Provides `TRY`/`CATCH`/`FINALLY` control flow based on `setjmp`/`longjmp`. |

---

# API reference

## Status package

Header: `Status.h`.

`OPSTATUS` is the canonical status type. Status and category values are generated from `StatusTable/StatusTable.h`; use the public helpers rather than hard-coding numeric values.

| Function | Description |
| --- | --- |
| `IsSuccess(status)` | True only for `STATUS_CONST(SUCCESS)`. |
| `IsFailure(status)` | True for any status other than success. |
| `IsSpecial(status)` | True for the special flow statuses declared by the table. |
| `CategoryOf(status)` | Returns the status category. |
| `IsCategory(status, category)` | Tests the status category. |
| `CategoryName(category)` | Returns the category name. |
| `Name(status)` | Returns the symbolic status name. |
| `Message(status)` | Returns the default human-readable message. |

~~~c
OPSTATUS status = STATUS_CONST(INVALID_ARGUMENT);

if (MODULE_FUNC(Condition, Status, IsFailure)(status)) {
    const char *name = MODULE_FUNC(Condition, Status, Name)(status);
    const char *message = MODULE_FUNC(Condition, Status, Message)(status);
}
~~~

## Error object

Header: `Error.h`.

`MODULE_TYPE(Condition, Error)` stores an `OPSTATUS` code, a message pointer, a `Diagnostic_Context` value and an optional pointer to a cause error.

| Method | Contract |
| --- | --- |
| `Create(code, message)` | Sets the status and caller-supplied message. |
| `FromStatus(code)` | Creates an error using the status table's default message. |
| `Clear()` | Clears the error value. |
| `Code()`, `Message()` | Reads the status and message. |
| `Is(code)`, `Is.Success()`, `Is.Failure()`, `Is.Category(category)` | Queries the stored code. |
| `Category()` | Returns the category associated with the status. |
| `Set.Context(context)` | Copies diagnostic context into the error. |
| `Set.Origin(file, line, function, description, userData)` | Sets source-origin metadata. |
| `Set.Cause(cause)` | Associates a cause error. |
| `Context()`, `HasContext()` | Reads or tests diagnostic context. |
| `Cause()`, `HasCause()`, `RootCause()`, `Depth()` | Inspects the cause chain. |

~~~c
MODULE_TYPE(Condition, Error) error = {0};

OPSTATUS status = MODULE_FUNC(Condition, Error, Create)(
    &error, STATUS_CONST(INVALID_ARGUMENT), "Invalid account identifier");

if (status == STATUS_CONST(SUCCESS)) {
    const char *message =
        MODULE_FUNC(Condition, Error, Message)(&error);
}
~~~

The message and diagnostic string fields are borrowed pointers; the Error object does not duplicate the strings. Keep their storage valid for as long as the error uses them. The cause pointer is also non-owning.

## Defer package

Header: `Defer.h`.

The package exposes `Cancel(node)`, `IsActive(node)` and `Execute(node)` for the defer-node representation. Most callers use the macros instead of manipulating nodes directly.

| Macro | Availability | Behavior |
| --- | --- | --- |
| `DEFER_NAMED(name, function, context)` | GCC and Clang | Registers a named cleanup node using the compiler cleanup attribute. |
| `DEFER(function, context)` | GCC and Clang | Registers an automatically named cleanup. |
| `DEFER_TRAITS_NAMED(name, traits, instance)` | GCC and Clang | Registers disposal through a `Type_Traits` descriptor. |
| `DEFER_TRAITS(traits, instance)` | GCC and Clang | Automatically named traits-based cleanup. |
| `DEFER_CANCEL(name)` | GCC and Clang | Cancels a named cleanup before scope exit. |
| `DEFER_SCOPE(action)` | C11 | Executes the supplied action when the macro's single-pass scope is exited. |
| `DEFER_TRY_SCOPE(action)` | C11 with Exception | Combines a try frame with a scope-exit action. |

~~~c
static void CloseHandle(void *context) {
    int *handle = (int *)context;
    if (handle != NULL) {
        *handle = -1;
    }
}

void UseHandle(void) {
    int handle = 12;
    DEFER(CloseHandle, &handle);
    /* handle is released when this lexical scope exits */
}
~~~

The `DEFER` cleanup-attribute macros are compiler extensions available only under GCC/Clang. Use `DEFER_SCOPE(action)` when a portable C11 single-pass scope is preferable. The latter has the control-flow shape of nested `for` statements; avoid jumps that bypass the macro-generated loop increment.

## Panic package

Header: `Panic.h`.

`Panic_Info` carries the panic kind, message, failed expression, file, line and function. The package exposes `SetHandler(handler)`, `GetHandler()`, `ResetHandler()`, `KindName(kind)`, `Trigger(...)` and `Now(message)`.

Public macros capture source-location fields automatically:

| Macro | Intended contract |
| --- | --- |
| `PANIC(message)` | Explicitly reports a panic. |
| `ASSERT(condition)` | Development assertion; it is disabled when `COSMERON_DISABLE_DEBUG_ASSERT` is defined. |
| `VERIFY(condition)` | Checks a runtime condition and triggers the configured handler on failure. |
| `ENSURE(condition)` | Enforces a precondition contract. |
| `UNREACHABLE()` | Reports execution of a path that should not be reached. |

A panic is not an ordinary error return. The configured handler determines what happens after the report; do not assume that the handler returns or that execution can safely continue.

## Exception package

Header: `Exception.h`.

The package uses `setjmp`/`longjmp` and a stack-allocated `Exception_Frame`. Public methods are `PushFrame`, `PopFrame`, `RegisterCleanup`, `Throw`, `ThrowStatus`, `Rethrow`, `CurrentError` and `HasActiveFrame`.

| Macro | Purpose |
| --- | --- |
| `TRY` | Starts a protected block. |
| `CATCH` | Handles an exception thrown from the active try block. |
| `FINALLY` | Runs the final block after try/catch processing. |
| `THROW(errorPtr)` | Throws a previously populated Error. |
| `THROW_STATUS(status, message)` | Creates and throws an error from a status and source location. |
| `RETHROW()` | Marks the current caught exception for propagation after the frame completes. |
| `CURRENT_ERROR()` | Provides access to the error stored in the active frame. |

~~~c
TRY {
    /* fallible work */
    if (failed) {
        THROW_STATUS(STATUS_CONST(INVALID_ARGUMENT), "Bad input");
    }
} CATCH {
    const MODULE_TYPE(Condition, Error) *error = CURRENT_ERROR();
    (void)error;
} FINALLY {
    /* finalization for this frame */
}
~~~

A `longjmp` does not unwind arbitrary C stack objects or run C++-style destructors. Register cleanup explicitly when needed. Do not jump across scopes with objects whose invariants require ordinary C cleanup. The frame is stack-local and must remain alive throughout the try/catch/finally sequence.

---

# Contracts and choosing a mechanism

- Return `OPSTATUS` for routine validation, I/O, allocation and arithmetic failures.
- Use `Condition_Error` when a status needs a message, diagnostic origin or causal chain.
- Use `DEFER`/`DEFER_SCOPE` for deterministic cleanup in C control flow.
- Use `PANIC`/`ASSERT`/`VERIFY`/`ENSURE` for violated programming contracts, not recoverable user input.
- Use `TRY`/`CATCH` only when non-local error propagation is deliberately part of the design.

## Related modules

- [Diagnostic](Diagnostic.md) — source context and error formatting.
- [Types](Types.md) — traits-based lifecycle operations.
- [Module](Module.md) — generated object, package and closure declarations.

## Source headers

- `Codespace/Cosmeron/Core/Condition/Status.h`
- `Codespace/Cosmeron/Core/Condition/Error.h`
- `Codespace/Cosmeron/Core/Condition/Defer.h`
- `Codespace/Cosmeron/Core/Condition/Panic.h`
- `Codespace/Cosmeron/Core/Condition/Exception.h`
