# Diagnostic

The **Diagnostic** module provides source-location context tracking, structured error formatting, and portable stacktrace capture.

```c
#include "Cosmeron/Core/Diagnostic/Context.h"
#include "Cosmeron/Core/Diagnostic/Format.h"
#include "Cosmeron/Core/Diagnostic/Stacktrace.h"
```

---

## Architecture & Design

1. **Diagnostic Context (`Diagnostic_TContext`)**:
   `MODULE_TYPE(Diagnostic, Context)` records origin metadata (`file`, `line`, `function`, `description`, and optional `userData`).
2. **Formatting (`Diagnostic_Format`)**:
   Formats `Condition_TError` and `Diagnostic_TContext` into caller-owned buffers without heap allocation, returning `STATUS_CONST(INSUFFICIENT_SPACE)` if truncated.
3. **Stacktrace (`Diagnostic_Stacktrace`)**:
   Captures execution frames where supported by the target platform.

---

## Context Reference (`Context.h`)

### `Diagnostic_Context_Init`

Initializes a `MODULE_TYPE(Diagnostic, Context)` instance.

```c
OPSTATUS Diagnostic_Context_Init(MODULE_TYPE(Diagnostic, Context) *self);
```

### `Diagnostic_Context_Create`

Populates a context with source-location metadata.

```c
OPSTATUS Diagnostic_Context_Create(MODULE_TYPE(Diagnostic, Context) *self,
                                   const char *file,
                                   uint32_t line,
                                   const char *function,
                                   const char *description,
                                   void *userData);
```

### Query & Reset Methods

```c
bool Diagnostic_Context_HasOrigin(const MODULE_TYPE(Diagnostic, Context) *self);
const char *Diagnostic_Context_File(const MODULE_TYPE(Diagnostic, Context) *self);
uint32_t Diagnostic_Context_Line(const MODULE_TYPE(Diagnostic, Context) *self);
const char *Diagnostic_Context_Function(const MODULE_TYPE(Diagnostic, Context) *self);
const char *Diagnostic_Context_Description(const MODULE_TYPE(Diagnostic, Context) *self);
void *Diagnostic_Context_UserData(const MODULE_TYPE(Diagnostic, Context) *self);
void Diagnostic_Context_Clear(MODULE_TYPE(Diagnostic, Context) *self);
```

---

## Format Reference (`Format.h`)

### `Diagnostic_Format_Simple`

Formats an error as `[STATUS_NAME] message`.

```c
OPSTATUS Diagnostic_Format_Simple(const MODULE_TYPE(Condition, Error) *error,
                                  char *buffer,
                                  size_t bufferSize,
                                  size_t *outWritten);
```

### `Diagnostic_Format_Context`

Formats a context as `at file:line in func() - description` (or `<no origin>` if empty).

```c
OPSTATUS Diagnostic_Format_Context(const MODULE_TYPE(Diagnostic, Context) *context,
                                   char *buffer,
                                   size_t bufferSize,
                                   size_t *outWritten);
```

### `Diagnostic_Format_Detailed`

Formats an error together with its diagnostic context.

---

## Usage Example

```c
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Diagnostic/Context.h"
#include "Cosmeron/Core/Diagnostic/Format.h"
#include <stdio.h>

int main(void) {
  MODULE_TYPE(Diagnostic, Context) ctx = {0};
  MODULE_FUNC(Diagnostic, Context, Init)(&ctx);
  MODULE_FUNC(Diagnostic, Context, Create)(
      &ctx, __FILE__, (uint32_t)__LINE__, __func__, "Loading user profile", NULL);

  char buffer[256] = {0};
  size_t written = 0U;
  MODULE_FUNC(Diagnostic, Format, Context)(&ctx, buffer, sizeof(buffer), &written);
  printf("%s\n", buffer);
  return 0;
}
```

See [Examples/Core/Diagnostic.c](../../../Codespace/Examples/Core/Diagnostic.c).
