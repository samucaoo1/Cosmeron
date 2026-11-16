# Closure

`Closure` represents a typed, contextual callable fat pointer in Cosmeron. It is a 16-byte Pure POD structure (`sizeof == 2 * sizeof(void *)`) consisting of a dual-mode code pointer union and a borrowed context pointer.

## Architecture & Layout

```c
typedef struct TClosure_Name {
  union {
    TCallback_Name callback;
    Name_Function function;
  };
  void *context;
} TClosure_Name;
```

Zero vtable pointer overhead: `TClosure` contains strictly `(code_pointer, context_pointer)` and complies with C11 designated initializers and register passing.

### Sentinel Dual-Dispatch

To unify stateful closures and pure stateless callbacks without duplicate storage or allocation, `Closure` employs sentinel dual-dispatch:

1. **Promotion from pure callback (`FromCallback`)**:
   `cl.context = NULL`.
   When invoked via `CLOSURE_CALL(cl, args...)`, `cl.context == NULL` triggers the direct dispatch branch `cl.callback(args...)`.
2. **Contextual binding (`Bind`)**:
   - When `context != NULL`: `cl.context = context`.
   - When `context == NULL`: mapped to the internal sentinel pointer `(void *)(uintptr_t)1`.
   When invoked via `CLOSURE_CALL(cl, args...)`, `cl.context != NULL` triggers the function branch `cl.function(args..., ctx)` where sentinel `(void *)1` is safely decoded back to `NULL`.

## Descriptors

Closures correspond directly to Cosmeron function categories:

| Descriptor | Return Type | Arguments Specification |
|---|---|---|
| `CLOSURE_FUNCTION(RET, NAME, PARAMS)` | `RET` | Conventional C parameters `(Type1 a, Type2 b)` |
| `CLOSURE_PROCEDURE(NAME, PARAMS)` | `void` | Conventional C parameters `(Type1 a)` |
| `CLOSURE_OPERATION(NAME, PARAMS)` | `OPSTATUS` | Conventional C parameters `(Type1 a)` |
| `CLOSURE_PREDICATE(NAME, PARAMS)` | `bool` | Conventional C parameters `(Type1 a)` |
| `CLOSURE_COMPARISON(NAME, PARAMS)` | `CMPOUT` | Conventional C parameters `(Type1 a, Type2 b)` |

## Usage Examples

### 1. Standalone Closure

```c
#include "Cosmeron/Core/Module/Closure/Closure.h"

CLOSURE_FUNCTION(int, Scale, (int value));

static int ScaleByState(int value, void *context) {
    int factor = *(int *)context;
    return value * factor;
}

static int PureSquare(int value) {
    return value * value;
}

int main(void) {
    /* Stateful binding */
    int factor = 4;
    TCLOSURE(Scale) cl1 = CLOSURE_FUNC(Scale_Bind)(ScaleByState, &factor);
    assert(CLOSURE_CALL(cl1, 5) == 20);

    /* Direct promotion from stateless callback */
    TCLOSURE(Scale) cl2 = CLOSURE_FUNC(Scale_FromCallback)(PureSquare);
    assert(CLOSURE_CALL(cl2, 5) == 25);

    return 0;
}
```

### 2. Grouped Declarations in Modules (`CLOSURE(...)` block)

```c
MODULE(DOMAIN(Core), Pipeline,
    CLOSURE(
        FUNCTION(int, Step, (int input)),
        PREDICATE(Filter, (int item))
    ),
    OBJECT(Worker,
        FIELDS(int id;),
        CLOSURE(
            OPERATION(Execute, (int taskId))
        )
    )
);
```

Accessing scoped closure types:
- Module scope: `MODULE_CLOSURE(Core, Pipeline, Step)`
- Object scope: `MODULE_CLOSURE(Pipeline, Worker, Execute)`
- Function constructor: `NS_JOIN(MODULE_CLOSURE_FUNC(Core, Pipeline, Step), Bind)(fn, ctx)`

### 3. Object Method Adapters (`CLOSURE_ADAPT_OBJECT_METHOD`)

Bridges object receiver methods `(CLASS *self, ...)` to closure signatures `(..., void *context)` on demand:

```c
/* Adapts Worker_Add(Worker *self, int delta) to closure (int delta, void *context) */
CLOSURE_ADAPT_OBJECT_METHOD(
    TCLOSURE(IntProc),
    MODULE_TYPE(Pipeline, Worker),
    MODULE_FUNC(Pipeline, Worker, Add),
    void,
    (int delta),
    (delta)
)

Worker worker;
TCLOSURE(IntProc) cl = CLOSURE_AS_CLOSURE(MODULE_FUNC(Pipeline, Worker, Add), &worker);
CLOSURE_CALL(cl, 10); /* mutates worker.accumulator by +10 */
```

