# Package

The **Package** core module enables declarative grouping of module-level functions with optional function table dispatch.

```c
#include "Cosmeron/Core/Module/Package.h"
```

---

# Overview

`Package.h` provides macro utilities for declaring structured namespaces of free functions and generating corresponding function tables.

| Macro | Purpose |
| --- | --- |
| `PACKAGE(PKG, ...)` | Defines a declarative package interface and its functions |
| `FUNCTION(RET, NAME, PARAMS)` | Declares a normal function inside a package |
| `FUNCTION_INLINE(RET, NAME, PARAMS, BODY)` | Declares and implements an inline function inside a package |
| `PACKAGE_CALL(PKG, FUNC, ...)` | Dispatches a call through the package function table |

---

# API reference

## PACKAGE

Declares a package namespace, function prototypes, and an associated function table.

### Syntax

#### Declarative form

```c
PACKAGE(MyPackage,
  FUNCTION(int, Add, (int a, int b)),
  FUNCTION_INLINE(int, Sub, (int a, int b), { return a - b; })
);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `PKG` | Package name token |
| `...` | List of functions defined with `FUNCTION` or `FUNCTION_INLINE` |

---

### Return value

Generates function prototypes, inline definitions, and function table accessors.

---

### Remarks

When `PACKAGE_DISABLE_FUNCTION_TABLE` is defined, only function prototypes and inline implementations are emitted, without generating the dispatch table structure.

---

### Example

```c
PACKAGE(MathHelper,
  FUNCTION_INLINE(int, Square, (int x), { return x * x; })
);
```

---

## PACKAGE_CALL

Invokes a package function through its generated function table.

### Syntax

#### Direct call

```c
int res = PACKAGE_CALL(MyPackage, Add, 2, 3);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `PKG` | Package name token |
| `FUNC_NAME` | Function name identifier |
| `...` | Arguments forwarded to the function |

---

### Return value

Returns the result produced by the target function.

---

### Remarks

Fetches the static table via `GetFunctionTable` and dispatches through the corresponding function pointer.

---

### Example

```c
int val = PACKAGE_CALL(MathHelper, Square, 5);
```
