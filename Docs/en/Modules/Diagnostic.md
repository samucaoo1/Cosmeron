# Diagnostic

The **Diagnostic** module stores source-origin metadata, formats errors into caller-owned buffers and captures stack traces where the target platform provides support. Its `Context`, `Format` and `Stacktrace` headers disable function tables; use direct `MODULE_FUNC(Diagnostic, ...)` calls for these APIs.

## Headers

~~~c
#include "Cosmeron/Core/Diagnostic/Context.h"
#include "Cosmeron/Core/Diagnostic/Format.h"
#include "Cosmeron/Core/Diagnostic/Stacktrace.h"
~~~

---

# Overview

| Component | Purpose |
| --- | --- |
| `Context` | Associates a diagnostic with file, line, function, description and opaque user data. |
| `Format` | Produces simple, detailed, chained or context-only text in a caller-provided buffer. |
| `Stacktrace` | Captures frame addresses and formats them without owning a dynamically allocated frame array. |

Diagnostic metadata is separate from the error code. Use `Condition_Error` for the failure and attach or format a `Diagnostic_Context` when the source location and explanation matter.

---

# API reference

## Context object

Header: `Context.h`.

Type: `MODULE_TYPE(Diagnostic, Context)`.

| Method | Description |
| --- | --- |
| `Create(file, line, function, description, userData)` | Sets the context's source origin and optional opaque data pointer. |
| `Clear()` | Resets all context fields. |
| `HasOrigin()` | Reports whether a source file is present. |
| `File()`, `Line()`, `Function()`, `Description()`, `UserData()` | Returns the stored fields. |

~~~c
MODULE_TYPE(Diagnostic, Context) context = {0};

OPSTATUS status = MODULE_FUNC(Diagnostic, Context, Create)(
    &context, __FILE__, (uint32_t)__LINE__, __func__,
    "Loading configuration", NULL);
~~~

The strings and `userData` pointer are borrowed; the object does not copy their contents. Make sure the referenced storage remains valid for the period in which the context is used.

## Format package

Header: `Format.h`.

Every formatter is an `OPERATION` and receives an output buffer, its capacity and an optional output count for the number of bytes written.

| Function | Description |
| --- | --- |
| `Simple(error, outBuffer, capacity, outWritten)` | Formats a `Condition_Error` status and message. |
| `Detailed(error, outBuffer, capacity, outWritten)` | Formats an error with its diagnostic context. |
| `Chain(error, outBuffer, capacity, outWritten)` | Formats the error's cause chain. |
| `Context(context, outBuffer, capacity, outWritten)` | Formats one diagnostic context value. |

~~~c
char output[256];
size_t written = 0U;

OPSTATUS status = MODULE_FUNC(Diagnostic, Format, Simple)(
    &error, output, sizeof(output), &written);
if (status == STATUS_CONST(SUCCESS)) {
    /* output contains the formatted text */
}
~~~

`Context.h`, `Stacktrace.h` and `Format.h` disable Diagnostic function tables. Invoke object methods with direct `MODULE_FUNC(Diagnostic, Type, Method)` calls; the package formatters are also called directly with `MODULE_FUNC`.

The caller owns the buffer. Always pass its actual capacity and check the returned status before consuming the output. A formatting operation can report insufficient space; do not assume that a truncated buffer contains a complete diagnostic.

## Stacktrace object

Header: `Stacktrace.h`.

Type: `MODULE_TYPE(Diagnostic, Stacktrace)`. The object stores up to the configured maximum number of addresses in its own fixed frame array; the current configuration sets the default maximum to 32.

| Method | Description |
| --- | --- |
| `Capture(skipFrames, maxFrames)` | Captures up to the requested number of frames after skipping the requested leading frames. |
| `Clear()` | Resets the captured frame count. |
| `FrameCount()` | Returns the number of captured addresses. |
| `FrameAt(index, outAddress)` | Copies one frame address to the output pointer. |
| `Format(outBuffer, capacity, outWritten)` | Formats captured addresses into a caller-owned buffer. |

~~~c
MODULE_TYPE(Diagnostic, Stacktrace) trace = {0};

OPSTATUS status = MODULE_FUNC(Diagnostic, Stacktrace, Capture)(
    &trace, 0U, 16U);

size_t count = MODULE_FUNC(Diagnostic, Stacktrace, FrameCount)(&trace);
~~~

Frame capture is platform-dependent and should be treated as best effort. The object stores raw addresses; it does not promise symbol names, source-line resolution or a portable unwinder on every target. Check the status from `Capture`, `FrameAt` and `Format`.

---

# Typical workflow

1. Build a `Condition_Error` with a status and message.
2. Attach a `Diagnostic_Context` to preserve the origin, when available.
3. Use `Format.Simple` for compact logs, `Format.Detailed` for error plus origin, or `Format.Chain` when causes are relevant.
4. Capture a stacktrace at a boundary where the platform supports it, then format it into a caller-owned buffer.

## Related modules

- [Condition](Condition.md) — status values, error objects, panic and exceptions.
- [Memory](Memory.md) — caller-managed buffers and memory ownership.
- [Module](Module.md) — object and package method conventions.

## Source headers

- `Codespace/Cosmeron/Core/Diagnostic/Context.h`
- `Codespace/Cosmeron/Core/Diagnostic/Format.h`
- `Codespace/Cosmeron/Core/Diagnostic/Stacktrace.h`
