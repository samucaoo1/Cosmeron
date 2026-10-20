# Object

The **Object** core module provides an object-oriented foundation and declarative class definition syntax for Cosmeron, supporting dispatch tables, inheritance, and unified method declarations.

```c
#include "Cosmeron/Core/Object/Object.h"
```

---

# Overview

The Object system supports base objects (`TObject`) and structured type hierarchies through macro definitions:

| Macro / Type | Purpose |
| --- | --- |
| `TObject` | Base object type containing the virtual dispatch table pointer |
| `OBJECT(CLASS, FIELDS, ...)` | Defines a declarative class with fields and methods |
| `OBJECT_EXTENDS(CLASS, BASE, FIELDS, ...)` | Defines a derived class inheriting from a base class |
| `METHOD(RET, NAME, PARAMS)` | Declares a method within an object specification |
| `METHOD_INLINE(RET, NAME, PARAMS, BODY)` | Declares and implements an inline method |

---

# API reference

## OBJECT

Declares a new class structure and its associated function table.

### Syntax

#### Declarative form

```c
OBJECT(ClassName,
  FIELDS(
    int value;
  ),
  METHOD(OPSTATUS, Compute, (int factor))
);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `CLASS` | Identifier for the generated class structure |
| `FIELDS_BLOCK` | Member fields enclosed in `FIELDS(...)` |
| `...` | List of methods declared with `METHOD` or `METHOD_INLINE` |

---

### Return value

Generates type definitions and inline function table initializers; does not evaluate to a runtime value.

---

### Remarks

Expands to the structure definition, the `FunctionTable` struct, and initialization helpers `ClassName_Init` and `ClassName_GetFunctionTable`.

---

### Example

```c
OBJECT(Counter,
  FIELDS(
    int count;
  ),
  METHOD_INLINE(int, Get, (), { return self->count; })
);
```

---

## OBJECT_EXTENDS

Declares a derived class that embeds a base class instance as its first member.

### Syntax

#### Declarative form

```c
OBJECT_EXTENDS(DerivedName, BaseName,
  FIELDS(
    int extraField;
  ),
  METHOD(OPSTATUS, Action, ())
);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `CLASS` | Identifier for the derived class |
| `BASE_CLASS` | Identifier of the base class to inherit from |
| `FIELDS_BLOCK` | Additional fields declared in `FIELDS(...)` |
| `...` | Method specifications |

---

### Return value

Generates derived structure definitions and dispatch metadata; does not return a runtime value.

---

### Remarks

The base object is stored in the `base` member field, enabling polymorphism and safe casting with `OBJECT_AS`.

---

### Example

```c
OBJECT_EXTENDS(SpecialCounter, Counter,
  FIELDS(
    int step;
  ),
  METHOD_INLINE(int, Step, (), { return self->step; })
);
```
