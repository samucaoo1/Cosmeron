# Call

Call represents a typed function together with a borrowed context. It uses the declarative package system from `Core/Module/Package.h` to generate the callable operations.

## Contract

Each declaration creates a distinct callback type containing:

- A signature-specific function pointer.
- A non-owning `void *context`.
- `Bind`, to package a function and its context.
- `IsValid`, to check whether a function is bound.
- `Invoke`, to call the function with the stored context.

These operations are declared through `PACKAGE` and the matching `FUNCTION_INLINE`, `PREDICATE_INLINE`, or `PROCEDURE_INLINE` descriptors. The normal Cosmeron package function-table behavior and its compile-time disable switch therefore apply.

The context is borrowed. Call does not allocate, copy, or release it. The caller must keep the pointed-to object alive for every invocation.

## Value-returning callable

```c
#include "Cosmeron/Core/Module/Call/Call.h"

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
    Scale call = COSMERON_Call_Scale_Bind(ScaleValue, &state);

    if (!COSMERON_Call_Scale_IsValid(call))
        return 1;

    return COSMERON_Call_Scale_Invoke(call, 4) == 12 ? 0 : 1;
}
```

The generated callback signature is `int (*)(int value, void *context)`. The caller of `Invoke` supplies only the ordinary argument; the stored context is forwarded automatically.

## Multiple arguments

```c
CALL_DECLARE(AddTo, int, (int left, int right), (left, right))

static int AddToContext(int left, int right, void *context) {
    int offset = *(int *)context;
    return left + right + offset;
}
```

## Procedures

Use `CALL_DECLARE_PROCEDURE` for callbacks returning `void`:

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

`IsValid` checks only whether the function pointer is non-null. It does not validate the context or guarantee the callback's preconditions. Calling `Invoke` on an invalid callable is a caller error; check `IsValid` first when a binding may be empty.

Callables can be copied by value. Copies share the same context pointer and do not duplicate the context data. The context may be `NULL` when the callback does not need extra state.

## Scope

Call does not provide automatic variable capture, context ownership, destruction, or type erasure across arbitrary function signatures. Each declared callable preserves a concrete function-pointer type, so invocation remains type-checked by C.
