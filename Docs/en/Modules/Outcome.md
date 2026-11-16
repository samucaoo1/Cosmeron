# Outcome

The **Outcome** module provides Cosmeron's core abstractions for operational status reporting, fallible computations, and optional values. It unifies three complementary paradigms:

1. **Status (`Status.h`)**: Universal `OPSTATUS` enumeration, status categories, and queries for recoverable function outcomes.
2. **Result (`Result.h`, `Result.def`)**: Declarative generic object `Result<T>` modeling computation outcome as either success (`Ok(T)`) or diagnostic failure (`Err(Error)`).
3. **Optional (`Optional.h`, `Optional.def`)**: Declarative generic object `Optional<T>` modeling presence (`Some(T)`) or absence (`None`) of a value without the overhead of error structures.

```c
#include "Cosmeron/Core/Outcome/Status.h"
#include "Cosmeron/Core/Outcome/Result.h"
#include "Cosmeron/Core/Outcome/Optional.h"
```

---

## 1. Architectural Design

- **Zero Global State**: No hidden thread-local or global `errno` / `GetLastError` variables. All outcomes are explicitly propagated via return values or output parameters.
- **Dual-Mode Template Engine**: Both `Result` and `Optional` are implemented via canonical `.def` dual-mode template engines, supporting on-demand instantiation (`TEMPLATE_INSTANTIATE`) or direct include instantiation without code duplication.
- **Strict Return Contracts**:
  - `OPERATION`: Returns `OPSTATUS` (`STATUS_CONST(SUCCESS)`, `STATUS_CONST(NOT_FOUND)`, `STATUS_CONST(INVALID_ARGUMENT)`).
  - `PREDICATE`: Returns `bool` (`IsSome`, `IsNone`, `HasValue`, `IsOk`, `IsError`).
  - `FUNCTION`: Returns concrete value (`ValueOr`) or pointer (`ValuePointer`, `MutablePointer`).
  - `PROCEDURE`: Infallible mutation (`Clear`).
- **Zero-Cost Layout in Embedded**: All methods in `Result.def` and `Optional.def` are `*_INLINE`. When built with `-DOBJECT_DISABLE_FUNCTION_TABLE`, no function table pointer (`api`) is emitted, giving objects the exact memory footprint of raw C structs.
- **Output Parameter Safety**: Functions with output parameters (`outValue`) modify the referenced memory strictly upon `STATUS_CONST(SUCCESS)`.

---

## 2. Status API (`Status.h`)

`OPSTATUS` is the universal operational outcome type returned by all fallible operations across the Cosmeron library.

### Categories

Status codes are grouped into decoupled semantic categories via `STATUS_CATEGORY_TABLE`:

| Category | Constant Macro | Purpose |
|---|---|---|
| `SUCCESS` | `STATUS_CATEGORY_CONST(SUCCESS)` | Successful completion |
| `GENERIC` | `STATUS_CATEGORY_CONST(GENERIC)` | General invalid arguments, missing resources, out of range |
| `MEMORY` | `STATUS_CATEGORY_CONST(MEMORY)` | Allocator exhaustion, insufficient capacity |
| `CONCURRENCY` | `STATUS_CATEGORY_CONST(CONCURRENCY)` | Timeouts, locking contention, would block |
| `ARITHMETIC` | `STATUS_CATEGORY_CONST(ARITHMETIC)` | Division by zero, overflow, underflow |
| `SEQUENCE` | `STATUS_CATEGORY_CONST(SEQUENCE)` | Invalid or incomplete byte/character sequences |

### Status Predicates and Helpers

```c
/* Checks */
bool isSuccess = MODULE_FUNC(Outcome, Status, IsSuccess)(status);
bool isFailure = MODULE_FUNC(Outcome, Status, IsFailure)(status);
bool isSpecial = MODULE_FUNC(Outcome, Status, IsSpecial)(status); /* WOULD_BLOCK, INCOMPLETE_SEQUENCE */

/* Metadata */
MODULE_ENUM(Outcome, Status, Category) cat = MODULE_FUNC(Outcome, Status, CategoryOf)(status);
const char *name = MODULE_FUNC(Outcome, Status, Name)(status);       /* e.g. "NOT_FOUND" */
const char *msg  = MODULE_FUNC(Outcome, Status, Message)(status);    /* e.g. "Not found" */
```

---

## 3. Optional API (`Optional.h`, `Optional.def`)

`Optional<T>` represents a value of type `T` that may or may not be present.

### Instantiation

```c
#include "Cosmeron/Core/Outcome/Optional.h"

/* Instantiates Outcome_TOptional_int32 */
TEMPLATE_INSTANTIATE(Optional, int32, int32_t);
```

### Type Definition & Layout

For a specialization `(SUFFIX, TYPE)`:
- Type symbol: `MODULE_TYPED(Outcome, Optional, SUFFIX)` $\to$ `Outcome_TOptional_SUFFIX`
- Data fields:
  ```c
  bool hasValue;
  TYPE value;
  ```

### Interface Summary

| Member | Kind | Signature | Description |
|---|---|---|---|
| `Init` | `OPERATION` | `(self)` | Initializes the optional object. |
| `Some` | `OPERATION` | `(self, TYPE value)` | Sets `hasValue = true` and stores `value`. |
| `None` | `OPERATION` | `(self)` | Sets `hasValue = false` and zeroes the value storage. |
| `FromPointer` | `OPERATION` | `(self, const TYPE *ptr)` | If `ptr != NULL` sets `Some(*ptr)`, otherwise sets `None()`. |
| `Clear` | `PROCEDURE` | `(self)` | Infallibly resets the optional to `None()`. |
| `HasValue` | `CONST_PREDICATE` | `(self)` | Returns `true` if a value is present, `false` otherwise. |
| `IsSome` | `CONST_PREDICATE` | `(self)` | Alias query: returns `true` if a value is present. |
| `IsNone` | `CONST_PREDICATE` | `(self)` | Alias query: returns `true` if no value is present (`!hasValue`). |
| `Value` | `CONST_OPERATION` | `(self, TYPE *outValue)` | Copies stored value into `*outValue` and returns `STATUS_CONST(SUCCESS)`. If empty, returns `STATUS_CONST(NOT_FOUND)` and leaves `*outValue` unchanged. |
| `ValueOr` | `CONST_FUNCTION` | `(self, TYPE fallback) -> TYPE` | Returns the contained value if present; otherwise returns `fallback`. |
| `ValuePointer` | `CONST_FUNCTION` | `(self) -> const TYPE *` | Returns a borrowed pointer `&self->value` if present; otherwise `NULL`. |
| `MutablePointer` | `FUNCTION` | `(self) -> TYPE *` | Returns a mutable pointer `&self->value` if present; otherwise `NULL`. |
| `Map` | `CONST_OPERATION` | `(self, TCALLBACK(MapFunction) mapper, TOptional *outOptional)` | If present, invokes `mapper(value)` and stores result in `outOptional`. If empty, emits empty `outOptional`. |
| `AndThen` | `CONST_OPERATION` | `(self, TCALLBACK(BindFunction) binder, TOptional *outOptional)` | Monadic bind (flat map). Invokes `binder(value, outOptional)` if present; otherwise emits empty `outOptional`. |
| `Filter` | `CONST_OPERATION` | `(self, TCALLBACK(FilterPredicate) pred, TOptional *outOptional)` | If present and `pred(value) == true`, preserves value in `outOptional`; otherwise emits empty `outOptional`. |

---

## 4. Result API (`Result.h`, `Result.def`)

`Result<T>` models fallible operations returning either a valid `TYPE` or a rich `MODULE_TYPE(Condition, Error)`.

### Instantiation

```c
#include "Cosmeron/Core/Outcome/Result.h"

/* Instantiates Outcome_TResult_int32 */
TEMPLATE_INSTANTIATE(Result, int32, int32_t);
```

### Interface Summary

| Member | Kind | Signature | Description |
|---|---|---|---|
| `Init` | `OPERATION` | `(self)` | Initializes the result object. |
| `Ok` | `OPERATION` | `(self, TYPE value)` | Stores success value (`isOk = true`). |
| `Err` | `OPERATION` | `(self, const TError *error)` | Stores diagnostic failure (`isOk = false`). |
| `FromStatus` | `OPERATION` | `(self, OPSTATUS status)` | Converts status to `Ok` if `SUCCESS`, or creates `Error` from `status`. |
| `IsOk` | `CONST_PREDICATE` | `(self)` | Returns `true` if operation succeeded. |
| `IsError` | `CONST_PREDICATE` | `(self)` | Returns `true` if operation failed. |
| `Value` | `CONST_OPERATION` | `(self, TYPE *outValue)` | Copies value into `*outValue` on success; returns error code on failure. |
| `ValueOr` | `CONST_FUNCTION` | `(self, TYPE fallback) -> TYPE` | Returns contained value if ok; otherwise `fallback`. |
| `Error` | `CONST_OPERATION` | `(self, TError *outError)` | Extracts stored error if failed; returns `INVALID_ARGUMENT` if ok. |
| `Map` | `CONST_OPERATION` | `(self, TCALLBACK(MapFunction) mapper, TResult *outResult)` | Transforms success value through `mapper`. |
| `AndThen` | `CONST_OPERATION` | `(self, TCALLBACK(BindFunction) binder, TResult *outResult)` | Monadic bind through `binder`. |

---

## 5. Usage Examples

### 5.1 Basic Optional Creation and Safe Extraction

```c
#include "Cosmeron/Core/Outcome/Optional.h"
#include <stdio.h>

TEMPLATE_INSTANTIATE(Optional, int32, int32_t);

void ProcessOption(MODULE_TYPED(Outcome, Optional, int32) *opt) {
    /* 1. Predicate check */
    if (OBJECT_CALL_0(opt, IsSome)) {
        int32_t value = 0;
        if (OBJECT_CALL(opt, Value, &value) == STATUS_CONST(SUCCESS)) {
            printf("Found value: %d\n", value);
        }
    } else {
        printf("No value present.\n");
    }

    /* 2. Direct fallback without branching */
    int32_t valOrDefault = OBJECT_CALL(opt, ValueOr, -1);
    printf("Value or default: %d\n", valOrDefault);

    /* 3. Zero-copy pointer inspection */
    const int32_t *ptr = OBJECT_CALL_0(opt, ValuePointer);
    if (ptr != NULL) {
        printf("Inspected value via pointer: %d\n", *ptr);
    }
}
```

### 5.2 Monadic Transformations (`Map`, `AndThen`, `Filter`)

```c
#include "Cosmeron/Core/Outcome/Optional.h"
#include <assert.h>

TEMPLATE_INSTANTIATE(Optional, int32, int32_t);

static int32_t DoubleValue(int32_t x, void *ctx) {
    (void)ctx;
    return x * 2;
}

static OPSTATUS KeepPositiveOnly(int32_t x, MODULE_TYPED(Outcome, Optional, int32) *out, void *ctx) {
    (void)ctx;
    if (x > 0) return OBJECT_CALL(out, Some, x);
    return OBJECT_CALL_0(out, None);
}

static bool IsEven(int32_t x, void *ctx) {
    (void)ctx;
    return (x % 2) == 0;
}

void MonadicPipeline(void) {
    MODULE_TYPED(Outcome, Optional, int32) opt;
    MODULE_TYPED(Outcome, Optional, int32) step1;
    MODULE_TYPED(Outcome, Optional, int32) step2;
    MODULE_TYPED(Outcome, Optional, int32) finalResult;

    MODULE_FUNC(Outcome, Optional, int32, Init)(&opt);
    OBJECT_CALL(&opt, Some, 5);

    /* 1. Map: 5 -> 10 */
    TCALLBACK(MODULE_CALLBACK(Outcome, Optional, int32, MapFunction)) mapCb = {
        .function = DoubleValue,
        .context = NULL
    };
    OBJECT_CALL(&opt, Map, mapCb, &step1);

    /* 2. Filter: 10 is even -> Some(10) */
    TCALLBACK(MODULE_CALLBACK(Outcome, Optional, int32, FilterPredicate)) filterCb = {
        .function = IsEven,
        .context = NULL
    };
    OBJECT_CALL(&step1, Filter, filterCb, &step2);

    /* 3. AndThen: 10 > 0 -> Some(10) */
    TCALLBACK(MODULE_CALLBACK(Outcome, Optional, int32, BindFunction)) bindCb = {
        .function = KeepPositiveOnly,
        .context = NULL
    };
    OBJECT_CALL(&step2, AndThen, bindCb, &finalResult);

    assert(OBJECT_CALL(&finalResult, ValueOr, 0) == 10);
}
```

### 5.3 Bridging Between `Optional` and `Result`

```c
#include "Cosmeron/Core/Outcome/Optional.h"
#include "Cosmeron/Core/Outcome/Result.h"

TEMPLATE_INSTANTIATE(Optional, int32, int32_t);
TEMPLATE_INSTANTIATE(Result, int32, int32_t);

void ConvertOptionalToResult(const MODULE_TYPED(Outcome, Optional, int32) *opt,
                             MODULE_TYPED(Outcome, Result, int32) *outRes) {
    int32_t val;
    if (OBJECT_CALL(opt, Value, &val) == STATUS_CONST(SUCCESS)) {
        OBJECT_CALL(outRes, Ok, val);
    } else {
        OBJECT_CALL(outRes, FromStatus, STATUS_CONST(NOT_FOUND));
    }
}

void ConvertResultToOptional(const MODULE_TYPED(Outcome, Result, int32) *res,
                             MODULE_TYPED(Outcome, Optional, int32) *outOpt) {
    int32_t val;
    if (OBJECT_CALL(res, Value, &val) == STATUS_CONST(SUCCESS)) {
        OBJECT_CALL(outOpt, Some, val);
    } else {
        OBJECT_CALL_0(outOpt, None);
    }
}
```

---

## 6. Compilation & Portability

- **C Standard**: Strict C11 (`-std=c11`).
- **Compiler Support**: Clang, GCC, MSVC, Intel C++ Compiler.
- **Zero Dependencies**: Requires only standard headers (`stdbool.h`, `stddef.h`, `stdint.h`, `string.h`) and Cosmeron's Core module infrastructure.
- **Function Table Elimination**: Compile with `-DOBJECT_DISABLE_FUNCTION_TABLE` to eliminate all function table pointers and vtables for minimal code and memory footprint in embedded environments.

