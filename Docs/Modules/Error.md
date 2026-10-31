# Error

The **Error** module provides two complementary mechanisms for reporting failures in Cosmeron: **Status** for recoverable errors and **Panic** for unrecoverable conditions.

A function can return an `OPSTATUS` to let its caller decide how to handle an error. When an invariant is violated or execution cannot safely continue, `PANIC` and `PANIC_ASSERT` report the failure location and terminate the process.

The module does not require a global *last-error* variable.

```c
#include "Cosmeron/Core/Error/Status.h"
#include "Cosmeron/Core/Error/Context.h"
#include "Cosmeron/Core/Error/Error.h"
#include "Cosmeron/Core/Error/Panic.h"
```

---

# Overview

| Package | Interface | Purpose | Execution continues |
| --- | --- | --- | --- |
| Status | `OPSTATUS`, `STATUS_CONST(...)` | Reports normal success and recoverable errors | Yes |
| Panic | `PANIC(message)` | Reports a critical failure and aborts | No |
| Panic | `PANIC_ASSERT(condition)` | Aborts if a required condition is false | Only if the condition is true |

The Status API follows the library's configurable namespacing scheme.

### Macro form

```c
OPSTATUS status = STATUS_CONST(SUCCESS);
```

### Direct form

```c
OPSTATUS status = STATUS_SUCCESS;
```

These two expressions refer to the same enum constant when the default namespace is used.

To configure namespacing, define the namespace macros **before** including the header:

```c
#define COSMERON_NAMESPACE MyProject
#define COSMERON_NAMESPACE_CONST MYPROJECT

#include "Cosmeron/Core/Error/Status.h"
```

The direct constant becomes `MYPROJECT_STATUS_SUCCESS`, while the macro form stays unchanged:

```c
OPSTATUS status = STATUS_CONST(SUCCESS);
```

Unlike Status constants, **`PANIC` and `PANIC_ASSERT` are themselves public macros**. Their underlying functions are internal implementation details, not alternate public entry points.

---

# API reference

## Interface summary

| Interface | Description | Terminates process |
| --- | --- | --- |
| [`OPSTATUS`](#opstatus) | Public status-result type | No |
| [`STATUS_CONST`](#status_const) | Accesses a status constant using the configured namespace | No |
| [`Status_Category`](#status_category) | Enumeration of status categories | No |
| [`STATUS_CATEGORY_CONST`](#status_category_const) | Accesses a category constant | No |
| [`STATUS_TABLE`](#status_table) | X-macro listing all predefined status codes and messages | No |
| [`Status Package Functions`](#status-package-functions) | Classification, naming, and diagnostic inspection functions | No |
| [`TErrorContext`](#terrorcontext) | Source location and context metadata object | No |
| [`TError`](#terror) | Declarative structured error object with cause chaining and context | No |
| [`TResult`](#tresult) | Parameterized, allocation-free typed outcome type (`RESULT_TYPE(SUFFIX)`) | No |
| [`Error_Format`](#error_format) | Allocation-free textual error, chain, and context formatting (`PACKAGE(ERROR_NS(Format))`) | No |
| [`TStacktrace`](#tstacktrace) | Portable call stack frame capture and formatting object (`ERROR_TYPE(TStacktrace)`) | No |
| [`Panic`](#panic) | Fatal failures, contract checks (`ASSERT`, `VERIFY`, `ENSURE`, `UNREACHABLE`, `PANIC`) | Yes (unless handler intervenes) |
| [`Exception`](#exception) | Structured non-local control flow (`TRY`, `CATCH`, `FINALLY`, `THROW`, `RETHROW`) | No (unless unhandled) |

---

# Status

`Status.h` declares an enumeration of operational outcomes. Use it for expected, recoverable failure conditions rather than terminating the process.

A function with a fallible operation and no primary return value can return `OPSTATUS` directly. A fallible function with a separate result should generally return `OPSTATUS` and write the result to an output parameter only on success.

---

# OPSTATUS

The standard return type for an operation that can fail.

### Syntax

#### Macro / public alias

```c
OPSTATUS status;
```

#### Direct enum tag

```c
enum Status_Status status;
```

`Status_Status` above is the default-namespaced **enum tag**, not a separate typedef. `OPSTATUS` is the preferred public type alias and does not depend on the custom namespace spelling.

---

### Values

The module currently defines **18** status constants.

| Constant | Description |
| --- | --- |
| `SUCCESS` | Operation completed successfully |
| `GENERIC_ERROR` | Error not represented by a more specific status |
| `INVALID_ARGUMENT` | Invalid argument supplied |
| `OUT_OF_RANGE` | Value or index outside the allowed range |
| `OUT_OF_MEMORY` | Required memory could not be obtained |
| `NOT_FOUND` | Requested value or resource was not found |
| `ALREADY_EXISTS` | Value or resource already exists |
| `NOT_SUPPORTED` | Requested operation is not supported |
| `NOT_AVAILABLE` | Required resource or operation is unavailable |
| `BUSY` | Resource is busy |
| `WOULD_BLOCK` | Operation would block |
| `TIMEOUT` | Operation exceeded a time limit |
| `CANCELLED` | Operation was cancelled |
| `ARITHMETIC_OVERFLOW` | Arithmetic result cannot be represented |
| `DIVISION_BY_ZERO` | Attempted division by zero |
| `INVALID_SEQUENCE` | Input sequence is invalid |
| `INCOMPLETE_SEQUENCE` | Input sequence is incomplete |
| `INSUFFICIENT_SPACE` | Supplied space is insufficient |

Each entry is accessed through `STATUS_CONST(NAME)`. The direct spelling under the default namespace is `STATUS_NAME`.

---

### Return value

`OPSTATUS` is a **type**, not a function; functions returning it use `STATUS_CONST(SUCCESS)` for success or one of the failure constants when an operation does not complete.

---

### Remarks

- An `OPSTATUS` value is not a `bool`; compare it with a named status constant.
- Codes are generated by an enum without explicit numeric assignments. Do **not** depend on particular integer values in serialized data or external protocols.
- The status itself does not automatically record a message, a stacktrace, or a global error.
- Handling and propagating an `OPSTATUS` does not terminate the process.

---

### Example

```c
static OPSTATUS CheckCapacity(size_t used, size_t capacity) {
    if (used > capacity)
        return STATUS_CONST(OUT_OF_RANGE);

    return STATUS_CONST(SUCCESS);
}

OPSTATUS status = CheckCapacity(12, 8);

if (status != STATUS_CONST(SUCCESS)) {
    /* Handle the recoverable error. */
}
```

---

# STATUS_CONST

Expands to the configured namespace-qualified constant associated with a status name.

### Syntax

#### Macro form

```c
STATUS_CONST(INVALID_ARGUMENT)
```

#### Direct form

```c
STATUS_INVALID_ARGUMENT
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `NAME` | One of the uppercase identifiers listed in the status table |

---

### Return value

Expands to an **enum constant**. It is a preprocessor naming macro and is not called at runtime.

---

### Remarks

- The direct form depends on the constant namespace configuration.
- Using the macro form avoids hard-coding a namespace into reusable code.
- The status constants should be preferred over integer literals.

---

### Example

```c
static OPSTATUS ValidatePointer(const void *pointer) {
    if (pointer == NULL)
        return STATUS_CONST(INVALID_ARGUMENT);

    return STATUS_CONST(SUCCESS);
}
```

---

# STATUS_TABLE

The X-macro providing a single source of truth for the status identifiers and their human-readable English messages.

### Syntax

#### Macro form

```c
STATUS_TABLE(X)
```

#### Direct form

There is no corresponding direct function. `STATUS_TABLE` is a **code-generation macro**, not a runtime callable.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `X` | User-supplied macro accepting `(ROW)`, where fields are accessed via `STATUS_GET(FIELD, ROW)` (e.g., `NAME`, `MESSAGE`) |

---

### Return value

None. Expands the supplied macro once per status-table entry.

---

### Remarks

- The current table contains 18 entries aggregated across categorical sub-tables (`Generic`, `Memory`, `Concurrency`, `Arithmetic`, `Sequence`).
- Each entry is passed as a row tuple `((NAME, MESSAGE))`, completely decoupling consumers from column order and count.
- Field values are retrieved using `STATUS_GET(FIELD, ROW)` (such as `STATUS_GET(NAME, ROW)` and `STATUS_GET(MESSAGE, ROW)`).
- The enum in `Status.h` is generated from this table.
- The messages are string literals; no public `Status_ToString` function is currently declared.
- A caller may reuse the table to create mappings, switch cases, or diagnostics without duplicating its entries.

---

### Example

The following helper demonstrates deriving messages from the existing X-macro using `STATUS_GET`:

```c
static const char *StatusMessage(OPSTATUS status) {
    switch (status) {
#define STATUS_MESSAGE_CASE(ROW) \
        case STATUS_CONST(STATUS_GET(NAME, ROW)): return STATUS_GET(MESSAGE, ROW);
        STATUS_TABLE(STATUS_MESSAGE_CASE)
#undef STATUS_MESSAGE_CASE
        default: return "Unknown status";
    }
}

const char *message = StatusMessage(STATUS_CONST(OUT_OF_MEMORY));
/* message points to "Out of memory" */
```

---

# Status_Category

Enumeration of domain categories classifying every `OPSTATUS`.

### Syntax

```c
Status_Category category = Status_CategoryOf(status);
```

### Values

Defined via `STATUS_CATEGORY_TABLE`:

| Constant | Description |
| --- | --- |
| `SUCCESS` | Successful operational outcomes (`SUCCESS`) |
| `GENERIC` | General invalid argument, not found, or generic errors |
| `MEMORY` | Memory exhaustion and buffer sizing errors |
| `CONCURRENCY` | Synchronization, busy states, timeouts, and cancellation |
| `ARITHMETIC` | Arithmetic overflows and division by zero |
| `SEQUENCE` | Invalid or incomplete input sequences |

Each constant is accessed via `STATUS_CATEGORY_CONST(NAME)` (e.g. `STATUS_CATEGORY_CONST(MEMORY)`).

---

# Status Package Functions

Declared declaratively via `PACKAGE(Status)` in `Status.h`. Can be invoked either as free functions (`Status_FunctionName(...)`) or via `PACKAGE_CALL(Status, FunctionName, ...)`.

| Function | Signature | Description |
| --- | --- | --- |
| `Status_IsSuccess` | `bool (OPSTATUS status)` | Returns `true` if `status == STATUS_CONST(SUCCESS)` |
| `Status_IsFailure` | `bool (OPSTATUS status)` | Returns `true` if `status != STATUS_CONST(SUCCESS)` |
| `Status_IsSpecial` | `bool (OPSTATUS status)` | Returns `true` for non-fatal transient states (`WOULD_BLOCK`, `INCOMPLETE_SEQUENCE`) |
| `Status_CategoryOf` | `Status_Category (OPSTATUS status)` | Returns the categorical classification of `status` |
| `Status_IsCategory` | `bool (OPSTATUS status, Status_Category category)` | Tests whether `status` belongs to `category` |
| `Status_CategoryName` | `const char * (Status_Category category)` | Returns the English label of a category |
| `Status_Name` | `const char * (OPSTATUS status)` | Returns the symbolic name string (e.g. `"OUT_OF_MEMORY"`) |
| `Status_Message` | `const char * (OPSTATUS status)` | Returns the human-readable description (e.g. `"Out of memory"`) |

### Example

```c
OPSTATUS status = DoOperation();

if (Status_IsFailure(status)) {
    if (Status_IsCategory(status, STATUS_CATEGORY_CONST(MEMORY))) {
        /* Handle out-of-memory or buffer space error */
    }
    printf("Failed: %s (%s)\n", Status_Name(status), Status_Message(status));
}
```

---

# TError

`Error.h` provides the structured `ERROR_TYPE(TError)` object (default `Error_TError`), declared via `OBJECT(ERROR_TYPE(TError), ...)`.

`ERROR_TYPE(TError)` represents an operational failure or outcome with associated status code, descriptive message, categorical queries, optional context, and causal chaining.

### Design Principles

- **Zero dynamic memory allocation**: Stack-allocated or embedded directly in values.
- **Borrowed / Static strings**: `message` holds a borrowed literal pointer. If `message == NULL`, `Message()` falls back directly to `Status_Message(code)`.
- **Zero global state**: Errors are explicitly passed and returned; no thread-local or global *last-error* is used.
- **Strict namespacing**: Lives inside `Error_TError_*` without unnamespaced typedef shortcuts.

### Syntax

```c
ERROR_TYPE(TError) err;
OBJECT_CALL(&err, Create, STATUS_CONST(INVALID_ARGUMENT), "Buffer size cannot be zero");

/* Querying properties via OBJECT_CALL */
OPSTATUS code = OBJECT_CALL_0(&err, Code);
const char *msg = OBJECT_CALL_0(&err, Message);
bool isFail = OBJECT_CALL_0(&err, IsFailure);
```

### Methods & Functions

| Method | Signature | Description |
| --- | --- | --- |
| `Create` | `OPSTATUS (ERROR_TYPE(TError) *self, OPSTATUS code, const char *message)` | Initializes the error with an operational code and custom message |
| `FromStatus` | `OPSTATUS (ERROR_TYPE(TError) *self, OPSTATUS code)` | Initializes the error from a status code, defaulting message to `Status_Message(code)` |
| `Clear` | `void (ERROR_TYPE(TError) *self)` | Resets the error to `STATUS_CONST(SUCCESS)` and `NULL` message |
| `Code` | `OPSTATUS (const ERROR_TYPE(TError) *self)` | Returns the stored status code |
| `Message` | `const char * (const ERROR_TYPE(TError) *self)` | Returns the custom message or the standard status description |
| `Is` | `bool (const ERROR_TYPE(TError) *self, OPSTATUS code)` | Tests if the error matches the given status code |
| `IsSuccess` | `bool (const ERROR_TYPE(TError) *self)` | Returns `true` if `code == STATUS_CONST(SUCCESS)` |
| `IsFailure` | `bool (const ERROR_TYPE(TError) *self)` | Returns `true` if `code != STATUS_CONST(SUCCESS)` |
| `Category` | `Status_Category (const ERROR_TYPE(TError) *self)` | Returns the categorical classification of the error |
| `IsCategory` | `bool (const ERROR_TYPE(TError) *self, Status_Category category)` | Tests whether the error belongs to `category` |
| `SetContext` | `OPSTATUS (ERROR_TYPE(TError) *self, const ERROR_TYPE(TErrorContext) *context)` | Attaches contextual metadata to the error |
| `SetOrigin` | `OPSTATUS (ERROR_TYPE(TError) *self, const char *file, uint32_t line, const char *function, const char *description, const void *userData)` | Sets source location and metadata directly |
| `Context` | `const ERROR_TYPE(TErrorContext) * (const ERROR_TYPE(TError) *self)` | Returns the pointer to the attached context |
| `HasContext` | `bool (const ERROR_TYPE(TError) *self)` | Returns `true` if source origin metadata is recorded |
| `SetCause` | `OPSTATUS (ERROR_TYPE(TError) *self, const struct ERROR_TYPE(TError) *cause)` | Chains a causal error with cycle and depth overflow checks |
| `HasCause` | `bool (const ERROR_TYPE(TError) *self)` | Returns `true` if this error wraps a preceding cause |
| `Cause` | `const struct ERROR_TYPE(TError) * (const ERROR_TYPE(TError) *self)` | Returns the immediate causal error, or `NULL` |
| `RootCause` | `const struct ERROR_TYPE(TError) * (const ERROR_TYPE(TError) *self)` | Traverses the causal chain to the initial underlying root error |
| `Depth` | `size_t (const ERROR_TYPE(TError) *self)` | Returns the total count of errors in the chain (self + causes) |

Methods can be invoked as direct C functions (`Error_TError_MethodName(&err, ...)`), via polymorphic object dispatch (`OBJECT_CALL(&err, MethodName, ...)` / `OBJECT_CALL_0(&err, MethodName)`), or through its `api` FunctionTable pointer (`err.api->MethodName(&err, ...)`).

### Example

```c
static OPSTATUS ParseHeader(const void *data, size_t length, ERROR_TYPE(TError) *outError) {
    if (!data || length < 4) {
        if (outError) {
            OBJECT_CALL(outError, Create, STATUS_CONST(INSUFFICIENT_SPACE),
                        "Header requires at least 4 bytes");
        }
        return STATUS_CONST(INSUFFICIENT_SPACE);
    }
    return STATUS_CONST(SUCCESS);
}

ERROR_TYPE(TError) err;
OPSTATUS status = ParseHeader(NULL, 0, &err);

if (OBJECT_CALL_0(&err, IsFailure)) {
    printf("Parsing error: %s (code: %s)\n",
           OBJECT_CALL_0(&err, Message),
           Status_Name(OBJECT_CALL_0(&err, Code)));
}
```

---

# TErrorContext

`Context.h` provides `TErrorContext`, an object encapsulating caller origin and descriptive metadata.

### Design Principles

- **Explicit creation**: Callers explicitly supply `__FILE__`, `__LINE__`, and `__func__` when desired.
- **Optional user data**: An unowned `const void *userData` pointer allows domain-specific structures to be attached without allocation.

### Methods & Functions

| Method | Signature | Description |
| --- | --- | --- |
| `Create` | `OPSTATUS (TErrorContext *self, const char *file, uint32_t line, const char *function, const char *description, const void *userData)` | Initializes the context |
| `Clear` | `void (TErrorContext *self)` | Resets all fields to `NULL` / `0` |
| `HasOrigin` | `bool (const TErrorContext *self)` | Returns `true` if `file != NULL` |
| `File` | `const char * (const TErrorContext *self)` | Returns the file name string |
| `Line` | `uint32_t (const TErrorContext *self)` | Returns the source line number |
| `Function` | `const char * (const TErrorContext *self)` | Returns the function name string |
| `Description` | `const char * (const TErrorContext *self)` | Returns the contextual description |
| `UserData` | `const void * (const TErrorContext *self)` | Returns the attached user data pointer |

---

# Error Chain

`TError` supports causal chaining through `SetCause`. This allows an outer layer (such as a database or transport subsystem) to wrap an inner error (such as an I/O timeout or memory failure) without losing the original failure details.

### Cycle Defense and Depth Limits

- Causal chains are bounded by `ERROR_CONST(MAX_CHAIN_DEPTH)` (configured as `16` via `ERROR_CONFIG_TABLE`).
- Attempting to attach a cause that would form a direct or indirect cycle (e.g. `err1 -> err2 -> err1`) is detected and rejected with `STATUS_CONST(INVALID_ARGUMENT)`.
- Attempting to build a chain exceeding `ERROR_CONST(MAX_CHAIN_DEPTH)` returns `STATUS_CONST(OUT_OF_RANGE)` without modifying the error.

### Example

```c
ERROR_TYPE(TError) ioErr;
OBJECT_CALL(&ioErr, Create, STATUS_CONST(TIMEOUT), "Socket read timed out");
OBJECT_CALL(&ioErr, SetOrigin, __FILE__, __LINE__, __func__, "Network stream", NULL);

ERROR_TYPE(TError) queryErr;
OBJECT_CALL(&queryErr, Create, STATUS_CONST(GENERIC_ERROR), "Failed to fetch user record");
OBJECT_CALL(&queryErr, SetCause, &ioErr);

/* Querying the chain */
size_t depth = OBJECT_CALL_0(&queryErr, Depth); /* 2 */
const ERROR_TYPE(TError) *root = OBJECT_CALL_0(&queryErr, RootCause); /* points to ioErr */
```

---

# TResult

`Result.h` provides typed, parameterized result types generated by Cosmeron's Universal Template Engine (`TEMPLATE_TRESULT`). A result represents either a successful value or a failure carrying a structured `ERROR_TYPE(TError)`.

### Design Principles

- **No heap allocation**: The result stores either the value or the error directly inside an inline tagged union.
- **Strict state encapsulation**: Attempting to extract a value when the result is an error does not modify the output buffer and safely returns the error's status code.
- **Universal Template Engine**: Specialized per type via `TEMPLATE_IMPLEMENT(TRESULT, suffix, type)` without compiler-specific extensions (`typeof`).
- **Functional combinators**: Supports `Map` and `AndThen` using `TCALLBACK` for declarative chaining.

### Default Types

The following types are pre-instantiated in `Result.h`:
- `RESULT_TYPE(int)` (`Error_TResult_int`)
- `RESULT_TYPE(int32)` (`Error_TResult_int32`)
- `RESULT_TYPE(int64)` (`Error_TResult_int64`)
- `RESULT_TYPE(uint32)` (`Error_TResult_uint32`)
- `RESULT_TYPE(uint64)` (`Error_TResult_uint64`)
- `RESULT_TYPE(float)` (`Error_TResult_float`)
- `RESULT_TYPE(double)` (`Error_TResult_double`)
- `RESULT_TYPE(size)` (`Error_TResult_size`)
- `RESULT_TYPE(bool)` (`Error_TResult_bool`)

### Methods & Operations

| Method | Signature | Description |
| --- | --- | --- |
| `Ok` | `OPSTATUS (TResult *self, TYPE value)` | Stores a success value and marks the result as Ok |
| `Err` | `OPSTATUS (TResult *self, const ERROR_TYPE(TError) *error)` | Copies an error and marks the result as Err |
| `FromStatus` | `OPSTATUS (TResult *self, OPSTATUS status)` | Sets Ok if `SUCCESS`, otherwise builds an error from `status` |
| `IsOk` | `bool (const TResult *self)` | Returns `true` if the result holds a valid value |
| `IsError` | `bool (const TResult *self)` | Returns `true` if the result holds a failure |
| `Value` | `OPSTATUS (const TResult *self, TYPE *outValue)` | Copies the value to `outValue` if Ok; returns error code and leaves `outValue` untouched if Err |
| `ValueOr` | `TYPE (const TResult *self, TYPE fallback)` | Returns the stored value if Ok, otherwise returns `fallback` |
| `Error` | `OPSTATUS (const TResult *self, ERROR_TYPE(TError) *outError)` | Copies the error to `outError` if Err; returns `INVALID_ARGUMENT` if Ok |
| `Map` | `OPSTATUS (const TResult *self, RESULT_MAP_CALLBACK(SUFFIX) mapper, TResult *outResult)` | Transforms the value if Ok, or propagates the error |
| `AndThen` | `OPSTATUS (const TResult *self, RESULT_BIND_CALLBACK(SUFFIX) binder, TResult *outResult)` | Invokes a fallible operation on the value if Ok, or propagates the error |

### Example

```c
#include "Cosmeron/Core/Error/Result.h"

RESULT_TYPE(int32) Divide(int32_t a, int32_t b) {
    RESULT_TYPE(int32) result;
    Error_TResult_int32_Init(&result);

    if (b == 0) {
        ERROR_TYPE(TError) err;
        Error_TError_Create(&err, STATUS_CONST(DIVISION_BY_ZERO), "Cannot divide by zero");
        OBJECT_CALL(&result, Err, &err);
        return result;
    }

    OBJECT_CALL(&result, Ok, a / b);
    return result;
}

RESULT_TYPE(int32) res = Divide(10, 2);
if (OBJECT_CALL_0(&res, IsOk)) {
    int32_t value = 0;
    OBJECT_CALL(&res, Value, &value); /* value == 5 */
} else {
    int32_t fallback = OBJECT_CALL(&res, ValueOr, 0);
}
```

---

# Error_Format

`Format.h` provides pure, allocation-free textual formatting routines without `<stdio.h>` or `malloc`. All output is written into a caller-supplied buffer with strict bounds checking and null termination.

### Methods & Operations

| Function | Signature | Description |
| --- | --- | --- |
| `Simple` | `OPSTATUS (const TError *error, char *outBuffer, size_t capacity, size_t *outWritten)` | Formats `[STATUS_NAME] Message` |
| `Context` | `OPSTATUS (const TErrorContext *context, char *outBuffer, size_t capacity, size_t *outWritten)` | Formats `at file:line in func() - description` |
| `Detailed` | `OPSTATUS (const TError *error, char *outBuffer, size_t capacity, size_t *outWritten)` | Formats simple error with origin context if present |
| `Chain` | `OPSTATUS (const TError *error, char *outBuffer, size_t capacity, size_t *outWritten)` | Formats the error and all parent causes in causal order |

### Return Contracts

- **`SUCCESS`**: Output was successfully written and fits within the buffer (null-terminated).
- **`INSUFFICIENT_SPACE`**: Buffer capacity was too small to contain the full text. The output is safely truncated and null-terminated at `capacity - 1`. `*outWritten` holds the count of characters written.
- **`INVALID_ARGUMENT`**: Either `error`/`context` or `outBuffer` was `NULL`, or `capacity == 0`.

---

# TStacktrace

`Stacktrace.h` provides `ERROR_TYPE(TStacktrace)`, an object that captures call stack return addresses portably across platforms without heap allocation.

### Configuration

- Bounded by `STACKTRACE_CONST(MAX_FRAMES)` (configured as `32` via `STACKTRACE_CONFIG_TABLE`).

### Methods & Operations

| Method | Signature | Description |
| --- | --- | --- |
| `Capture` | `OPSTATUS (TStacktrace *self, size_t skipFrames, size_t maxFrames)` | Captures current call stack frames |
| `Clear` | `void (TStacktrace *self)` | Clears all captured frames and sets frame count to 0 |
| `FrameCount` | `size_t (const TStacktrace *self)` | Returns the number of captured frames |
| `FrameAt` | `OPSTATUS (const TStacktrace *self, size_t index, void **outAddress)` | Returns the frame address at `index` |
| `Format` | `OPSTATUS (const TStacktrace *self, char *outBuffer, size_t capacity, size_t *outWritten)` | Formats all frames as `#00  0x...` lines without `stdio` or allocation |

### Example

```c
#include "Cosmeron/Core/Error/Stacktrace.h"

ERROR_TYPE(TStacktrace) trace = {0};
Error_TStacktrace_Init(&trace);

/* Capture up to 16 frames, skipping the Capture call itself */
OBJECT_CALL(&trace, Capture, 0, 16);

char buffer[1024];
size_t written = 0;
OBJECT_CALL(&trace, Format, buffer, sizeof(buffer), &written);
```

---

# Panic

`Panic.h` provides fatal failure reporting, invariant contracts, and configurable panic interceptors via `TCALLBACK(PanicHandler)`.

### Kinds of Panics

Defined via `PANIC_KIND_TABLE(X)`:
- `PANIC_CONST(KIND_PANIC)`: Explicit panic triggered directly by `PANIC("...")` or `Panic_Now("...")`.
- `PANIC_CONST(KIND_ASSERT)`: Development assertion triggered by `ASSERT(condition)`. Can be disabled by defining `COSMERON_DISABLE_DEBUG_ASSERT`.
- `PANIC_CONST(KIND_VERIFY)`: Always-evaluated runtime check triggered by `VERIFY(condition)`.
- `PANIC_CONST(KIND_ENSURE)`: Always-evaluated precondition contract triggered by `ENSURE(condition)`.
- `PANIC_CONST(KIND_UNREACHABLE)`: Indicates impossible code path reached triggered by `UNREACHABLE()`.

### Custom Handlers

Callers can intercept panic events by setting a custom handler:

```c
static void OnPanic(const PANIC_TYPE(Info) *info, void *context) {
    /* Log diagnostic or trigger graceful shutdown */
}

PANIC_FUNC(SetHandler)(PANIC_HANDLER_BIND(OnPanic, NULL));
```

When no custom handler is configured (or if the handler returns without aborting), `Panic_Trigger` prints diagnostic details (kind, expression, file, line, function) and formats the captured `TStacktrace` to `stderr` before calling `abort()`.

### Macros Summary

| Macro | Description | Always Evaluated |
| --- | --- | --- |
| `PANIC(message)` | Unconditional fatal abort | Yes |
| `ASSERT(condition)` | Development assertion; aborts if false | Active unless `COSMERON_DISABLE_DEBUG_ASSERT` defined |
| `VERIFY(condition)` | Runtime verification; aborts if false | Yes |
| `ENSURE(condition)` | Precondition contract check; aborts if false | Yes |
| `UNREACHABLE()` | Aborts indicating impossible code branch | Yes |

---

# Exception

`Exception.h` provides structured, scoped non-local control flow using `setjmp` and `longjmp` with deterministic unwinding and cleanup callbacks.

### Control-Flow Macros

- `TRY { ... }`: Establishes an exception frame on the call stack and executes the block.
- `CATCH { ... }`: Executes only if an exception was thrown inside the corresponding `TRY` block.
- `FINALLY { ... }`: Guaranteed to execute regardless of whether the `TRY` block completed normally or threw an exception.
- `THROW(errorPtr)`: Throws a structured `const ERROR_TYPE(TError) *`.
- `THROW_STATUS(status, message)`: Conveniently creates and throws an error with the current source file, line, and function.
- `RETHROW()`: Re-propagates the currently caught exception to the enclosing outer `TRY` block.
- `CURRENT_ERROR()`: Macro returning a pointer to the active `ERROR_TYPE(TError)` within the `CATCH` block.

### Unwinding and Cleanup Callbacks

Inside a `TRY` block, temporary resources can be safely protected against abrupt unwinding via `Exception_RegisterCleanup`:

```c
FILE *f = fopen("data.bin", "rb");
EXCEPTION_FUNC(RegisterCleanup)(EXCEPTION_CLEANUP_BIND(CloseFileCleanup, f));

/* If any code throws below, CloseFileCleanup is called before unwinding */
ProcessFile(f);
```

### Nested Exception Handling Example

```c
TRY {
    TRY {
        THROW_STATUS(STATUS_CONST(TIMEOUT), "Connection timed out");
    } CATCH {
        /* Inspect error and rethrow */
        if (CURRENT_ERROR()->code == STATUS_CONST(TIMEOUT)) {
            RETHROW();
        }
    } FINALLY {
        /* Always runs for the inner block */
        ReleaseInnerResources();
    }
} CATCH {
    /* Catches the rethrown timeout */
    printf("Handled outer error: %s\n", CURRENT_ERROR()->message);
} FINALLY {
    /* Always runs for the outer block */
    ReleaseOuterResources();
}
```

---

# Choosing Between Status, Exception, and Panic

| Mechanism | Purpose | Recoverable | Typical Use Cases |
| --- | --- | --- | --- |
| `OPSTATUS` / `TResult` | Direct operational outcomes | Yes | Expected domain fallibility (validation, missing keys, I/O timeouts) |
| `Exception` (`TRY`/`CATCH`) | Deep control-flow unwinding | Yes | Multi-layer aborts, parsers, transaction rollbacks |
| `Panic` (`ASSERT`, `PANIC`) | Fatal invariant violation | No | Corrupted memory, contract breaches, impossible code branches |
