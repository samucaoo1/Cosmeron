# Diagnostic

The **Diagnostic** module provides source-origin metadata, human-readable formatting for errors and contexts, and stack-trace capture. It belongs to **Core** and complements `Condition`: Condition stores error/status state, while Diagnostic describes where an event originated and how to present it.

```c
#include "Cosmeron/Core/Diagnostic/Context.h"
#include "Cosmeron/Core/Diagnostic/Format.h"
#include "Cosmeron/Core/Diagnostic/Stacktrace.h"
```

---

# Overview

| Header | API | Purpose |
| --- | --- | --- |
| `Context.h` | `Diagnostic_Context` | Source file, line, function, description and user data |
| `Format.h` | `Diagnostic_Format` | Formats an Error or Context into a caller-owned buffer |
| `Stacktrace.h` | `Diagnostic_Stacktrace` | Captures and formats instruction addresses |

Diagnostic does not own strings or user data stored in a Context. Formatting writes into buffers supplied by the caller; no global logger, terminal output or file sink is implied.

---

# Context

`MODULE_TYPE(Diagnostic, Context)` is a lightweight value object with `file`, `line`, `function`, `description` and `userData` fields.

Methods: `Create(file, line, function, description, userData)`, `Clear()`, `HasOrigin()`, `File()`, `Line()`, `Function()`, `Description()` and `UserData()`.

```c
MODULE_TYPE(Diagnostic, Context) context = {0};

OPSTATUS status = API(&context, Create,
                      __FILE__, (uint32_t)__LINE__, __func__,
                      "Parsing a configuration file", NULL);

if (status == STATUS_CONST(SUCCESS) && API(&context, HasOrigin)) {
    const char *file = API(&context, File);
    uint32_t line = API(&context, Line);
    (void)file;
    (void)line;
}
```

A Context is a value container; its pointer fields remain borrowed. Use string literals, static storage or storage whose lifetime is guaranteed to outlive the diagnostic record.

---

# Formatting

`Diagnostic_Format` exposes four operations. Each takes a writable output buffer, its capacity and an `outWritten` pointer:

| Operation | Input | Output |
| --- | --- | --- |
| `Simple(error, buffer, capacity, outWritten)` | `Condition_Error` | Compact error text |
| `Detailed(error, buffer, capacity, outWritten)` | `Condition_Error` | Error text with diagnostic context |
| `Chain(error, buffer, capacity, outWritten)` | `Condition_Error` | Error and its cause chain |
| `Context(context, buffer, capacity, outWritten)` | `Diagnostic_Context` | Source-origin description |

```c
char message[256];
size_t written = 0;

OPSTATUS status = MODULE_FUNC(Diagnostic, Format, Detailed)(
    &error, message, sizeof message, &written);

if (status == STATUS_CONST(SUCCESS)) {
    /* message contains written characters; written is not the capacity. */
}
```

The formatter does not allocate a result string. Always pass the actual destination capacity and check the returned `OPSTATUS`; do not assume a truncated message is complete.

---

# Stacktrace

`MODULE_TYPE(Diagnostic, Stacktrace)` stores a bounded array of instruction addresses. The default public maximum is 32 frames.

| Method | Purpose |
| --- | --- |
| `Capture(skipFrames, maxFrames)` | Captures up to the requested number of frames after skipping leading frames |
| `Clear()` | Resets the trace |
| `FrameCount()` | Returns the captured frame count |
| `FrameAt(index, outAddress)` | Copies an address to an output pointer |
| `Format(buffer, capacity, outWritten)` | Formats captured addresses into a caller-provided buffer |

```c
MODULE_TYPE(Diagnostic, Stacktrace) trace = {0};

OPSTATUS status = API(&trace, Capture, 0U, 16U);
if (status == STATUS_CONST(SUCCESS)) {
    size_t count = API(&trace, FrameCount);
    void *address = NULL;

    if (count > 0U)
        status = API(&trace, FrameAt, 0U, &address);
}

API(&trace, Clear);
```

A captured frame is an address, not automatically a source-level function name. Symbol resolution and source-line lookup are separate responsibilities. The implementation has platform-specific capture paths: Windows uses the Windows API; Linux with glibc, macOS and BSD use `execinfo` where available. Other targets may not support capture; check the operation status.

---

# Integration with Condition

A common pattern is to attach a Context to a Condition Error, then format it at the reporting boundary:

```c
MODULE_TYPE(Diagnostic, Context) origin = {0};
MODULE_TYPE(Condition, Error) error = {0};

API(&origin, Create, __FILE__, (uint32_t)__LINE__, __func__,
    "Loading application settings", NULL);
API(&error, Create, STATUS_CONST(NOT_FOUND), "Settings file was not found");
API(&error, Set, Context, &origin);

char buffer[256];
size_t written = 0;
OPSTATUS status = MODULE_FUNC(Diagnostic, Format, Detailed)(
    &error, buffer, sizeof buffer, &written);
```

The Context is copied into the Error's embedded context by `Set.Context`; its string pointers remain borrowed. This keeps error state separate from presentation.

---

# Notes

- All APIs are header-included and use caller-owned memory.
- Formatting does not print to `stdout` or `stderr`.
- Stack traces contain instruction addresses; symbol resolution is outside this module.
- Treat strings and `userData` stored in a Context as borrowed references.
