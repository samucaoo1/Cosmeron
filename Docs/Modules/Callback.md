# Callback

`Callback` represents a pure, stateless typed function pointer in Cosmeron with zero memory or dispatch overhead. It is strictly 8 bytes on 64-bit architectures (`sizeof == sizeof(void *)`).

## Contract

Each callback declaration generates a clean C function pointer type:

- Root declaration: `TCALLBACK(NAME)` expanding to `TCallback_NAME`.
- Scoped declaration inside `MODULE`, `PACKAGE`, or `OBJECT`: expands to `MOD_TCallback_NAME` or `MOD_PKG_TCallback_NAME`.
- Direct pointer assignment: `TCALLBACK(Scale) cb = MyFunction;`
- Validity check: `CALLBACK_IS_VALID(cb)` evaluates `cb != NULL`.
- Invalidation / Empty: `CALLBACK_EMPTY` evaluates to `NULL`.
- Invocation: `CALLBACK_CALL(cb, args...)` or `CALLBACK_INVOKE(cb, args...)` or direct `cb(args...)`.

Callbacks are strictly stateless. When state or a context pointer is required, use `Closure` (`TCLOSURE`). Pure callbacks can be promoted directly into closures without glue code or allocation via `CLOSURE_FUNC(NAME_FromCallback)(cb)`.

## Descriptors

Callbacks correspond directly to Cosmeron function categories:

| Descriptor | Return Type | Arguments Specification |
|---|---|---|
| `CALLBACK_FUNCTION(RET, NAME, PARAMS)` | `RET` | Conventional C parameters `(Type1 a, Type2 b)` |
| `CALLBACK_PROCEDURE(NAME, PARAMS)` | `void` | Conventional C parameters `(Type1 a)` |
| `CALLBACK_OPERATION(NAME, PARAMS)` | `OPSTATUS` | Conventional C parameters `(Type1 a)` |
| `CALLBACK_PREDICATE(NAME, PARAMS)` | `bool` | Conventional C parameters `(Type1 a)` |
| `CALLBACK_COMPARISON(NAME, PARAMS)` | `CMPOUT` | Conventional C parameters `(Type1 a, Type2 b)` |

Nullary callbacks use standard empty parentheses `()` without requiring special macro variants.

## Usage Examples

### 1. Standalone Callbacks

```c
#include "Cosmeron/Core/Module/Callback/Callback.h"

CALLBACK_FUNCTION(int, BinaryOp, (int a, int b));
CALLBACK_PROCEDURE(Logger, (int code));

static int Add(int a, int b) {
    return a + b;
}

static void PrintCode(int code) {
    printf("Code: %d\n", code);
}

int main(void) {
    TCALLBACK(BinaryOp) op = Add;
    assert(CALLBACK_IS_VALID(op));
    assert(CALLBACK_CALL(op, 10, 20) == 30);

    TCALLBACK(Logger) log = PrintCode;
    CALLBACK_CALL(log, 200);

    return 0;
}
```

### 2. Grouped Declarations in Modules (`CALLBACK(...)` block)

Callbacks can be integrated directly inside `MODULE`, `PACKAGE`, or `OBJECT` declarations using the `CALLBACK(...)` variadic block:

```c
MODULE(DOMAIN(Core), Math,
    CALLBACK(
        FUNCTION(int, Transform, (int x)),
        PREDICATE(IsPrime, (int x))
    ),
    PACKAGE(Operations,
        CALLBACK(
            PROCEDURE(OnStep, (size_t step))
        )
    )
);
```

Accessing scoped callback types:
- Module scope: `MODULE_CALLBACK(Core, Math, Transform)`
- Package scope: `MODULE_CALLBACK(Math, Operations, OnStep)`
