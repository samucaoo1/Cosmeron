# Callback

Callback represents a typed function pointer together with an optional borrowed context. It provides a type-safe, declarative abstraction for callbacks across Cosmeron modules.

## Contract

Each declaration generates a specialized callback type `TCALLBACK(NAME)` containing:

- A concrete function pointer with the exact signature specified plus `void *context`.
- A non-owning `void *context`.
- `Bind(&cb, function, context)` to package a function pointer and its context.
- `IsValid(&cb)` or `CALLBACK_IS_VALID(cb)` to check whether a function is bound.
- `CALLBACK_INVOKE(cb, args...)` to invoke the callback with stored context.

The context is borrowed. Callback does not allocate, copy, or release it. The caller must keep the pointed-to context alive for every invocation.

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

### 1. Value-Returning Callback

```c
#include "Cosmeron/Core/Module/Callback/Callback.h"

CALLBACK_FUNCTION(int, Scale, (int value));

typedef struct {
    int factor;
} ScaleContext;

static int ScaleValue(int value, void *context) {
    ScaleContext *state = context;
    return value * state->factor;
}

int main(void) {
    ScaleContext state = { 3 };
    TCALLBACK(Scale) callback =
        CALLBACK_FUNC(Scale_Bind)(ScaleValue, &state);

    if (!CALLBACK_IS_VALID(callback))
        return 1;

    return CALLBACK_INVOKE(callback, 4) == 12 ? 0 : 1;
}
```

### 2. Procedures, Operations and Predicates

```c
/* Infallible procedure returning void */
CALLBACK_PROCEDURE(Notify, (int code));

/* Fallible operation returning OPSTATUS */
CALLBACK_OPERATION(Commit, (int value));

/* Predicate proposition returning bool */
CALLBACK_PREDICATE(IsPositive, (int value));

/* Three-way comparison returning CMPOUT */
CALLBACK_COMPARISON(CompareKeys, (int left, int right));
```

### 3. Nullary Callbacks

Callbacks without ordinary parameters use standard empty parentheses `()`:

```c
CALLBACK_FUNCTION(int, ReadSetting, ());
CALLBACK_PROCEDURE(ResetSetting, ());
CALLBACK_OPERATION(Flush, ());
CALLBACK_PREDICATE(IsEnabled, ());
```

### 4. Inlining Callbacks in Modules (`PACKAGE` and `OBJECT`)

Callbacks can be integrated directly inside `PACKAGE` and `OBJECT` declarations using the `CALLBACK(...)` capsule:

```c
PACKAGE(EventBus,
    CALLBACK(
        OPERATION(OnEvent, (int code))
    )
);

OBJECT(Widget,
    FIELDS(
        int id;
    ),
    CALLBACK(
        PREDICATE(Filter, (int value))
    )
);
```

### 5. Automatic Comparator Adapters

Any object comparison returning `CMPOUT` automatically generates a type-erased bridge to `TCALLBACK(Comparison)`:

```c
OBJECT(Person,
    FIELDS(
        int age;
    ),
    COMPARISON_NUMERIC(CompareAge, age)
);

/* Automatically generated: */
TCALLBACK(Comparison) cb = Person_CompareAge_AsComparator();
CALLBACK_INVOKE(cb, &person1, &person2);
```

## Invocation and Validity

- **Universal invocation:** `CALLBACK_CALL(callback, ...)` or `CALLBACK_INVOKE(callback, ...)` accepts an arbitrary number of arguments and automatically forwards the stored context as the final parameter.
- **Validity checking:** Check via `CALLBACK_IS_VALID(callback)` or `callback.IsValid(&callback)`.
- **Value semantics:** Callbacks are small value types (two pointers plus member function pointers) and can be passed and copied by value.
