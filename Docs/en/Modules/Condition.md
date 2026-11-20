# Condition

The **Condition** module provides Cosmeron's canonical error-handling, operational status, scoped cleanup, panic, and structured exception primitives.

```c
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Condition/Defer.h"
#include "Cosmeron/Core/Condition/Panic.h"
#include "Cosmeron/Core/Condition/Exception.h"
```

---

## Architecture & Design

1. **Operational Status (`OPSTATUS`)**:
   Every fallible `OPERATION` returns `OPSTATUS`. Success is `STATUS_CONST(SUCCESS)`. Categories include generic, memory, I/O, parsing, and arithmetic errors.
2. **Rich Error Object (`Condition_TError`)**:
   `MODULE_TYPE(Condition, Error)` encapsulates an `OPSTATUS` code, a human-readable message, and query methods (`Is`, `IsSuccess`, `IsFailure`, `Category`, `IsCategory`).
3. **Scoped Cleanup (`DEFER`)**:
   `DEFER(func, context)` and `DEFER_SCOPE(action)` guarantee deterministic resource disposal on scope exit.
4. **Panic & Exception**:
   `Panic` handles unrecoverable contract violations with configurable handlers; `Exception` provides `setjmp`/`longjmp`-based structured frames.

---

## Status Reference (`Status.h`)

### `Condition_Status_Name`

Returns the symbolic name of an `OPSTATUS` code.

```c
const char *Condition_Status_Name(OPSTATUS status);
/* Macro form: MODULE_FUNC(Condition, Status, Name)(status) */
```

- **Parameters**: `status` — An `OPSTATUS` value.
- **Return value**: Null-terminated constant string (e.g., `"SUCCESS"`, `"INVALID_ARGUMENT"`).

### `Condition_Status_Message`

Returns a human-readable description of an `OPSTATUS` code.

```c
const char *Condition_Status_Message(OPSTATUS status);
```

### `Condition_Status_IsSuccess` / `Condition_Status_IsFailure`

```c
bool Condition_Status_IsSuccess(OPSTATUS status);
bool Condition_Status_IsFailure(OPSTATUS status);
```

---

## Error Object Reference (`Error.h`)

### `Condition_Error_Init`

Initializes a `MODULE_TYPE(Condition, Error)` instance to `SUCCESS`.

```c
OPSTATUS Condition_Error_Init(MODULE_TYPE(Condition, Error) *self);
```

### `Condition_Error_Create`

Populates an error object with a status code and custom message.

```c
OPSTATUS Condition_Error_Create(MODULE_TYPE(Condition, Error) *self,
                                OPSTATUS code,
                                const char *message);
```

### `Condition_Error_FromStatus`

Populates an error object using the default message for `code`.

```c
OPSTATUS Condition_Error_FromStatus(MODULE_TYPE(Condition, Error) *self,
                                    OPSTATUS code);
```

### Query Methods

```c
OPSTATUS Condition_Error_Code(const MODULE_TYPE(Condition, Error) *self);
const char *Condition_Error_Message(const MODULE_TYPE(Condition, Error) *self);
bool Condition_Error_Is(const MODULE_TYPE(Condition, Error) *self, OPSTATUS code);
bool Condition_Error_IsSuccess(const MODULE_TYPE(Condition, Error) *self);
bool Condition_Error_IsFailure(const MODULE_TYPE(Condition, Error) *self);
void Condition_Error_Clear(MODULE_TYPE(Condition, Error) *self);
```

---

## Usage Example

```c
#include "Cosmeron/Core/Condition/Defer.h"
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Condition/Status.h"
#include <stdio.h>

static void Cleanup(void *ctx) {
  int *handle = (int *)ctx;
  if (handle) *handle = 0;
}

int main(void) {
  MODULE_TYPE(Condition, Error) err = {0};
  MODULE_FUNC(Condition, Error, Init)(&err);
  MODULE_FUNC(Condition, Error, Create)(&err, STATUS_CONST(INVALID_ARGUMENT), "Value must be positive");

  printf("Error [%s]: %s\n",
         MODULE_FUNC(Condition, Status, Name)(OBJECT_CALL_0(&err, Code)),
         OBJECT_CALL_0(&err, Message));

  int handle = 123;
  {
    DEFER(Cleanup, &handle);
    /* handle is automatically cleaned up on scope exit */
  }
  return 0;
}
```

See [Examples/Core/Condition.c](../../../Codespace/Examples/Core/Condition.c).
