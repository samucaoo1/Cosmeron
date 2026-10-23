# Call

Call represents a typed function together with a non-owning context. It provides a small C99 building block for callbacks and closure-like values without allocating memory or converting incompatible function-pointer types.

## Contract

Each callable type has:

- A signature-specific function pointer.
- A `void *context` associated with that function.
- A `Bind` function that packages function and context.
- An `IsValid` predicate that checks whether a function is bound.
- An `Invoke` function generated with the exact signature.

The context is borrowed, not owned. Call does not allocate, copy, or release it. The caller must keep the pointed-to object alive for every invocation.

## Value-returning callable

```c
#include "Call/Call.h"

CALL_DECLARE(Scale, int, (int value), (value))

typedef struct {
    int factor;
} ScaleContext;

static int ScaleValue(int value, void *context) {
    ScaleContext *state = context;
    return value * state->factor;
}

int main(void) {
    ScaleContext state = { 3 };
    Scale call = Scale_Bind(ScaleValue, &state);

    if (!Scale_IsValid(call))
        return 1;

    return Scale_Invoke(call, 4) == 12 ? 0 : 1;
}
```

The generated callback signature is `int (*)(int value, void *context)`. The caller of `Scale_Invoke` supplies only the ordinary argument; the stored context is forwarded automatically.

## Multiple arguments

```c
CALL_DECLARE(AddTo, int, (int left, int right), (left, right))
```

Implement the callback with the declared arguments followed by `void *context`:

```c
static int AddToContext(int left, int right, void *context) {
    int offset = *(int *)context;
    return left + right + offset;
}
```

## Procedures

Use `CALL_DECLARE_PROCEDURE` for functions returning `void`:

```c
CALL_DECLARE_PROCEDURE(Notify, (int code), (code))

static void NotifyHandler(int code, void *context) {
    int *lastCode = context;
    *lastCode = code;
}
```

## Callables without ordinary arguments

Use `CALL_DECLARE_NULLARY` for a value-returning callback that only receives its context, or `CALL_DECLARE_NULLARY_PROCEDURE` for a void callback.

```c
CALL_DECLARE_NULLARY(ReadSetting, int)
CALL_DECLARE_NULLARY_PROCEDURE(ResetSetting)
```

## Validity and lifetime

`IsValid` checks only whether the function pointer is non-null. It does not validate the context or guarantee the function's preconditions. Calling `Invoke` on an invalid callable is a caller error; check `IsValid` first when the binding may be empty.

Callables can be copied by value. Copies share the same context pointer; they do not duplicate the context data. A context may be `NULL` when the callback does not need extra state.

## Scope

Call does not provide automatic variable capture, closure allocation, context ownership, destruction, or type erasure across arbitrary function signatures. Each declared callable preserves a concrete function-pointer type, so invocation remains type-checked by C.
