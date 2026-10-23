# Module

The **Module** core subsystem provides Cosmeron's declarative architectural framework. It defines unified declarative schemas for modules, free-function namespaces (**Package**), object-oriented structures with receiver dispatch (**Object**), contract-driven member tuples, implementation prototypes, and universal generic template instantiation.

```c
#include "Cosmeron/Core/Module/Module.h"
```

---

# Overview

`Module.h` serves as the central umbrella header for module creation and code generation across the library. It coordinates several underlying components:

| Component | File | Purpose |
| --- | --- | --- |
| [Package](Package.md) | `Package.h` | Declarative namespaces of free functions with optional function tables |
| [Object](Object.md) | `Object.h` | Declarative object-oriented classes, inheritance, receivers, and dispatch tables |
| Tuple | `Common/Tuple.impl` | Member tuple specifications (`OPERATION`, `PREDICATE`, `FUNCTION`, `PROCEDURE`) |
| Dispatch | `Common/Dispatch.impl` | Unified implementation prototypes and dispatchers (`*_IMPLEMENT_EX`, `*_PROTOTYPEEX`) |
| Template | `Common/Template.impl` | Universal template engine (`TEMPLATE_DECLARE`, `TEMPLATE_IMPLEMENT`) |
| FunctionTable | `Common/FunctionTable.impl` | FunctionTable struct definitions and singleton table accessors |
| Namespace | `Module.space` | Namespace prefixes and macro definitions for the Module core subsystem |

### Member contract classification

Member specifications inside declarative blocks follow Cosmeron's contract guidelines:

| Specifier | Result Type | Contract / Purpose |
| --- | --- | --- |
| `OPERATION(NAME, PARAMS)` | `OPSTATUS` | Fallible operation returning a status code |
| `PREDICATE(NAME, PARAMS)` | `bool` | Proposition or query returning a boolean |
| `FUNCTION(RET, NAME, PARAMS)` | `RET` | General function returning a computed value |
| `PROCEDURE(NAME, PARAMS)` | `void` | Infallible side-effecting action returning void |

Each member category supports inline and const variants:
- **Inline variants:** `OPERATION_INLINE`, `PREDICATE_INLINE`, `FUNCTION_INLINE`, `PROCEDURE_INLINE` accept a `{ ... }` body directly.
- **Const variants:** `CONST_OPERATION`, `CONST_PREDICATE`, `CONST_FUNCTION`, `CONST_PROCEDURE` (and their `*_INLINE` counterparts) enforce a `const` receiver pointer (`const CLASS * self`) when used within an `OBJECT` block.

### Compile-time configuration

Every Function Table family supports compile-time disabling to eliminate static dispatch table overhead:

| Macro Switch | Effect |
| --- | --- |
| `MODULE_DISABLE_FUNCTION_TABLE` | Omits function table structs and accessors across all packages and objects |
| `PACKAGE_DISABLE_FUNCTION_TABLE` | Omits function tables specifically for packages |
| `OBJECT_DISABLE_FUNCTION_TABLE` | Omits function tables specifically for objects |

---

# API reference

## FIELDS

Specifies the member field declarations of an object structure.

### Syntax

```c
FIELDS(...)
```

### Parameters

- `...`: Struct field declarations (types, names, and semicolons).

### Return value

A preprocessor expansion defining the member fields within the target struct; does not evaluate to a runtime value.

### Remarks

Used as the second argument of `OBJECT` or the third argument of `OBJECT_EXTENDS` to define fields cleanly in a structured block.

### Example

```c
OBJECT(Point,
  FIELDS(
    int x;
    int y;
  )
);
```

---

## OPERATION

Declares a fallible member operation returning `OPSTATUS`.

### Syntax

```c
OPERATION(NAME, PARAMS)
```

### Parameters

- `NAME`: Operation identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Expands to a declarative member tuple `(0, 0, OPSTATUS, NAME, PARAMS, { return STATUS_CONST(SUCCESS); })`.

### Remarks

Used inside `OBJECT` or `PACKAGE` declarations. The operation contract requires returning `OPSTATUS` to indicate operational success or recoverable failure. For inline definitions, use `OPERATION_INLINE(NAME, PARAMS, BODY)`. For objects requiring a const receiver, use `CONST_OPERATION` or `CONST_OPERATION_INLINE`.

### Example

```c
OBJECT(Device,
  FIELDS(int fd;),
  OPERATION(Open, (const char *path)),
  OPERATION(Close, ())
);
```

---

## PREDICATE

Declares a boolean predicate member returning `bool`.

### Syntax

```c
PREDICATE(NAME, PARAMS)
```

### Parameters

- `NAME`: Predicate identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Expands to a declarative member tuple `(0, 0, bool, NAME, PARAMS, { return false; })`.

### Remarks

Answers a proposition or state query. For inline implementations, use `PREDICATE_INLINE(NAME, PARAMS, BODY)`. For const receiver queries, use `CONST_PREDICATE` or `CONST_PREDICATE_INLINE`.

### Example

```c
OBJECT(Buffer,
  FIELDS(size_t count;),
  CONST_PREDICATE_INLINE(IsEmpty, (), {
    return self->count == 0U;
  })
);
```

---

## FUNCTION

Declares a general function member returning a value of type `RET`.

### Syntax

```c
FUNCTION(RET, NAME, PARAMS)
```

### Parameters

- `RET`: Return type token.
- `NAME`: Function identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Expands to a declarative member tuple `(0, 0, RET, NAME, PARAMS, { (void)0; })`.

### Remarks

Used for computations or queries returning a specific type. For inline definitions, use `FUNCTION_INLINE(RET, NAME, PARAMS, BODY)`. For objects requiring a const receiver, use `CONST_FUNCTION` or `CONST_FUNCTION_INLINE`.

### Example

```c
PACKAGE(MathUtils,
  FUNCTION_INLINE(int, Clamp, (int v, int minVal, int maxVal), {
    return v < minVal ? minVal : (v > maxVal ? maxVal : v);
  })
);
```

---

## PROCEDURE

Declares an infallible procedure member returning `void`.

### Syntax

```c
PROCEDURE(NAME, PARAMS)
```

### Parameters

- `NAME`: Procedure identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Expands to a declarative member tuple `(0, 0, void, NAME, PARAMS, { (void)0; })`.

### Remarks

Used for infallible actions or state updates without a return value. For inline definitions, use `PROCEDURE_INLINE(NAME, PARAMS, BODY)`. For const receivers, use `CONST_PROCEDURE` or `CONST_PROCEDURE_INLINE`.

### Example

```c
OBJECT(Logger,
  FIELDS(int level;),
  PROCEDURE(Log, (const char *message))
);
```

---

## OPERATION_IMPLEMENT_EX

Defines the implementation prototype for a fallible operation returning `OPSTATUS`.

### Syntax

```c
OPERATION_IMPLEMENT_EX(TARGET, NAME, PARAMS)
```

### Parameters

- `TARGET`: Identifier of the enclosing class or package.
- `NAME`: Operation identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Emits a static inline function prototype returning `OPSTATUS`.

### Remarks

Dispatches automatically to `OBJECT_OPERATION_IMPLEMENT_EX` (binding `CLASS * self`) if `TARGET` is detected as an object via trait tags (`<TARGET>_IS_OBJECT 1`, `COSMERON_TAG_OBJ_<TARGET> 1`, or `TARGET_IS_OBJECT 1`); otherwise dispatches to `PACKAGE_OPERATION_IMPLEMENT_EX`.

### Example

```c
OPERATION_IMPLEMENT_EX(Counter, Reset, ()) {
  if (!self) return STATUS_CONST(INVALID_ARGUMENT);
  self->value = 0;
  return STATUS_CONST(SUCCESS);
}
```

---

## PREDICATE_IMPLEMENT_EX

Defines the implementation prototype for a boolean predicate returning `bool`.

### Syntax

```c
PREDICATE_IMPLEMENT_EX(TARGET, NAME, PARAMS)
```

### Parameters

- `TARGET`: Identifier of the enclosing class or package.
- `NAME`: Predicate identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Emits a static inline function prototype returning `bool`.

### Remarks

Dispatches to an object receiver or package free function based on object trait tags. Follows the implementation signature convention of returning `bool`.

### Example

```c
PREDICATE_IMPLEMENT_EX(Counter, IsAtLimit, (int limit)) {
  if (!self) return false;
  return self->value >= limit;
}
```

---

## FUNCTION_IMPLEMENT_EX

Defines the implementation prototype for a function returning type `RET`.

### Syntax

```c
FUNCTION_IMPLEMENT_EX(TARGET, RET, NAME, PARAMS)
```

### Parameters

- `TARGET`: Identifier of the enclosing class or package.
- `RET`: Return type token.
- `NAME`: Function identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Emits a static inline function prototype returning `RET`.

### Remarks

Unified prototype generator for computing functions belonging to an object or package.

### Example

```c
FUNCTION_IMPLEMENT_EX(MathUtils, int, Square, (int x)) {
  return x * x;
}
```

---

## PROCEDURE_IMPLEMENT_EX

Defines the implementation prototype for an infallible procedure returning `void`.

### Syntax

```c
PROCEDURE_IMPLEMENT_EX(TARGET, NAME, PARAMS)
```

### Parameters

- `TARGET`: Identifier of the enclosing class or package.
- `NAME`: Procedure identifier.
- `PARAMS`: Parameter list enclosed in parentheses `(...)`.

### Return value

Emits a static inline function prototype returning `void`.

### Remarks

Unified prototype generator for infallible side-effecting operations.

### Example

```c
PROCEDURE_IMPLEMENT_EX(Counter, Clear, ()) {
  if (self) self->value = 0;
}
```

---

## TEMPLATE_DECLARE

Instantiates the declarative interface of a generic template without generating method bodies.

### Syntax

```c
TEMPLATE_DECLARE(NAME, SUFFIX, TYPE)
```

### Parameters

- `NAME`: Template base name identifier token.
- `SUFFIX`: Semantic type suffix identifier for the specialized name.
- `TYPE`: C type used for specialization.

### Return value

Emits the declarative interface invocation `GNS2(TEMPLATE, NAME)(SUFFIX, TYPE)`.

### Remarks

Used in header files when declaring specialized types and function tables for a generic template without emitting separate implementation bodies.

### Example

```c
TEMPLATE_DECLARE(TSTACK, int, int);
```

---

## TEMPLATE_IMPLEMENT

Instantiates both the declarative interface and the implementation functions of a generic template.

### Syntax

```c
TEMPLATE_IMPLEMENT(NAME, SUFFIX, TYPE)
```

### Parameters

- `NAME`: Template base name identifier token.
- `SUFFIX`: Semantic type suffix identifier for the specialized name.
- `TYPE`: C type used for specialization.

### Return value

Expands both `GNS2(TEMPLATE, NAME)(SUFFIX, TYPE)` and `GNS3(TEMPLATE, NAME, IMPLEMENT)(SUFFIX, TYPE)`.

### Remarks

Instantiates the complete specialization in an implementation file (`.impl` or `.c`). If `TEMPLATE_<NAME>_INLINE_ONLY` is defined as 1, the implementation call is automatically suppressed for inline-only templates.

### Example

```c
TEMPLATE_IMPLEMENT(TSTACK, int, int);
```

---

# C11 smoke test

The following complete example is compiled and executed by `Tools/check_doc_examples.py`. It demonstrates package definition, object definition with fields and operations, template instantiation, and dispatch via function tables.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/* 1. Declarative Package */
PACKAGE(MathOps,
  FUNCTION_INLINE(int, Add, (int a, int b), {
    return a + b;
  }),
  PREDICATE_INLINE(IsPositive, (int x), {
    return x > 0;
  })
);
PACKAGE_IMPLEMENT(MathOps);

/* 2. Declarative Object */
OBJECT(Counter,
  FIELDS(
    int value;
  ),
  OPERATION(Increment, ()),
  CONST_PREDICATE_INLINE(IsZero, (), {
    return self->value == 0;
  }),
  CONST_FUNCTION_INLINE(int, Get, (), {
    return self->value;
  })
);

OBJECT_PROTOTYPEEX(Counter, Increment) {
  self->value++;
  return STATUS_CONST(SUCCESS);
}
OBJECT_IMPLEMENT(Counter);

/* 3. Universal Template */
#define TEMPLATE_TBOX(SUFFIX, TYPE) \
  OBJECT( \
    GNS2(TBox, SUFFIX), \
    FIELDS( \
      TYPE item; \
    ), \
    OPERATION(Set, (TYPE value)), \
    CONST_FUNCTION_INLINE(TYPE, Get, (), { \
      return self->item; \
    }) \
  )

#define TBox_int_IS_OBJECT 1

#define TEMPLATE_TBOX_IMPLEMENT(SUFFIX, TYPE) \
  OPERATION_IMPLEMENT_EX(GNS2(TBox, SUFFIX), Set, (TYPE value)) { \
    if (!self) return STATUS_CONST(INVALID_ARGUMENT); \
    self->item = value; \
    return STATUS_CONST(SUCCESS); \
  }

TEMPLATE_IMPLEMENT(TBOX, int, int);

int main(void) {
  /* Verify Package dispatch */
  assert(PACKAGE_CALL(MathOps, Add, 10, 20) == 30);
  assert(PACKAGE_CALL(MathOps, IsPositive, 5) == true);
  assert(PACKAGE_CALL(MathOps, IsPositive, -2) == false);

  /* Verify Object lifecycle and dispatch */
  Counter c;
  assert(Counter_Init(&c) == STATUS_CONST(SUCCESS));
  c.value = 0;
  assert(OBJECT_CALL_0(&c, IsZero) == true);
  assert(OBJECT_CALL_0(&c, Increment) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&c, Get) == 1);
  assert(OBJECT_CALL_0(&c, IsZero) == false);

  /* Verify Template specialization */
  TBox_int box;
  assert(TBox_int_Init(&box) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&box, Set, 123) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&box, Get) == 123);

  return 0;
}
```

---

# Requirements

- C11 compiler with standard macro support.
- Header dependencies: `Cosmeron/Core/Module/Module.h` includes `Module.space`, `Common/Tuple.impl`, `Common/FunctionTable.impl`, `Common/Dispatch.impl`, `Common/Template.impl`, `Object.h`, and `Package.h`.
- No dynamic memory allocation or external libraries required.

---

# See also

- [Object](Object.md): Detailed reference for `OBJECT`, `OBJECT_EXTENDS`, and `TObject`.
- [Package](Package.md): Detailed reference for `PACKAGE` and `PACKAGE_CALL`.
- [Preprocessor](Preprocessor.md): Underlying preprocessor macro toolkit.
- [Error](Error.md): Status codes (`OPSTATUS`) returned by operations.
