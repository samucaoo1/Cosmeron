# Cosmeron Pattern

This document defines the naming and generation grammar used across Cosmeron.

> Prefer native C constructs over preprocessor macros. Use macros only for naming, code generation, internal preprocessing, or useful syntactic sugar.

Magic numbers are prohibited.

## 1. Naming

Everything must live inside the library/module namespace whenever C allows it.

Exceptions exist only where namespacing is not practical or would defeat the purpose, notably:

```text
public sugar macros
OPSTATUS
*OUT aliases
```

### Case table

| Element | Case | Example |
|---|---|---|
| Module / namespace token | `PascalCase` | `Memory`, `Comparison` |
| Type | `PascalCase` | `TVector`, `ComparisonResult` |
| Function | `PascalCase` | `Memory_Copy`, `Vector_Push` |
| Constant / enum value | `SNAKE_CASE` | `SUCCESS`, `OUT_OF_RANGE` |
| Preprocessor macro | `SNAKE_CASE` | `VECTOR_IMPLEMENT_ALL` |
| Internal namespace token | `PascalCase` / `SNAKE_CASE` | `Internal` / `INTERNAL` |
| Parameter / local / field | `camelCase` | `outValue`, `leftSize`, `bufferCapacity` |

```c
#define MEMORY_MOD  Memory
#define MEMORY_CMOD MEMORY
```

`*_MOD` names the normal module namespace.  
`*_CMOD` names the constant namespace.

```c
#define MEMORY_NS(NAME)   GNS2(LIB_PREFIX(MEMORY_MOD), NAME)
#define MEMORY_CNS(NAME)  CNS2(LIB_PREFIX_CONST(MEMORY_CMOD), NAME)

#define MEMORY_INS(NAME)  GNS3(LIB_PREFIX(MEMORY_MOD), Internal, NAME)
#define MEMORY_CINS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), INTERNAL, NAME)
```

- `*_NS` — public C identifiers
- `*_CNS` — public constants
- `*_INS` — internal C identifiers
- `*_CINS` — internal constants

Internal preprocessor macros do not use namespace guards:

```c
COSMERON_MACRO_INTERNAL_*
```

## 2. Types

Use `T` only for types intended to be directly handled by the user.

```c
BIT_TYPE(32)          /* TBit32 */
VECTOR_TYPED(uint32)  /* TVector_uint32 */
```

`*_TYPE(SUFFIX)` uses a semantic suffix.  
`*_TYPED(TYPE)` represents specialization by another type.

The underscore is meaningful:

```text
TBit32          semantic suffix
TVector_uint32  typed specialization
```

## 3. Functions & Declarative Contracts

Functions and methods in Cosmeron belong to declarative blocks (`PACKAGE` or `OBJECT`) and are classified by their return contract:

| Category | Specifier | Return Type | Meaning |
|---|---|---|---|
| Operation | `OPERATION(NAME, PARAMS)` | `OPSTATUS` | Fallible operation reporting status |
| Predicate | `PREDICATE(NAME, PARAMS)` | `bool` | Proposition or condition query |
| Function | `FUNCTION(RET, NAME, PARAMS)` | `RET` | Value-producing computation |
| Procedure | `PROCEDURE(NAME, PARAMS)` | `void` | Infallible action / side-effect |

Each category supports inline variants (`*_INLINE(..., BODY)`) and `const` receiver variants for objects (`CONST_*` and `CONST_*_INLINE`).

### Method & Function Implementation

Member prototypes and bodies are implemented in implementation files (`.impl` or `.c`) using either explicit macros (recommended) or the unified dispatcher:

#### 1. Explicit Family (Recommended - Zero Ambiguity & Zero Configuration)
Explicitly declares whether receiver pointer `self` is bound, requiring no traits or flags:
```c
/* For Objects: binds receiver (Target * self, ...) */
OBJECT_OPERATION_IMPLEMENT_EX(Target, MethodName, (params...)) {
    if (!self) return STATUS_CONST(INVALID_ARGUMENT);
    return STATUS_CONST(SUCCESS);
}
OBJECT_PREDICATE_IMPLEMENT_EX(Target, MethodName, (params...)) { return self != NULL; }
OBJECT_FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (params...)) { return computedValue; }
OBJECT_PROCEDURE_IMPLEMENT_EX(Target, MethodName, (params...)) { /* infallible action */ }

/* For Packages: free functions without self */
PACKAGE_OPERATION_IMPLEMENT_EX(Package, FuncName, (params...)) { return STATUS_CONST(SUCCESS); }
PACKAGE_PREDICATE_IMPLEMENT_EX(Package, FuncName, (params...)) { return true; }
PACKAGE_FUNCTION_IMPLEMENT_EX(Package, ReturnType, FuncName, (params...)) { return computedValue; }
PACKAGE_PROCEDURE_IMPLEMENT_EX(Package, FuncName, (params...)) { /* infallible action */ }
```

#### 2. Unified Family with Trait Detection
```c
OPERATION_IMPLEMENT_EX(Target, MethodName, (params...)) {
    if (!self) return STATUS_CONST(INVALID_ARGUMENT);
    return STATUS_CONST(SUCCESS);
}

PREDICATE_IMPLEMENT_EX(Target, MethodName, (params...)) {
    return self != NULL;
}

FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (params...)) {
    return computedValue;
}

PROCEDURE_IMPLEMENT_EX(Target, MethodName, (params...)) {
    /* infallible action */
}
```

The unified engine detects whether `Target` is an Object or Package using trait tags, automatically binding `self` for objects if **any** of the following conditions hold:
1. Direct type tag: `#define <Target>_IS_OBJECT 1` (e.g. `#define TCalculator_IS_OBJECT 1` or `#define TStack_int_IS_OBJECT 1`)
2. Namespaced tag: `#define COSMERON_TAG_OBJ_<Target> 1`
3. File-wide override: `#define TARGET_IS_OBJECT 1` (at the top of the `.impl` translation unit)

If none of these traits are detected, it automatically emits free functions for packages.

### Implementation Finalizers

Every implementation file must close with the designated finalizer macro to complete bindings:

```c
PACKAGE_IMPLEMENT(PackageName);
/* or */
OBJECT_IMPLEMENT(ObjectName);
```

### Deprecated Compatibility Aliases

Macros under `Backward Compatibility Aliases` in `Tuple.impl` and `Dispatch.impl` (such as `METHOD`, `CONST_METHOD`, `METHOD_INLINE`, `OBJECT_PROTOTYPEEX`, `PACKAGE_PROTOTYPEEX`, and `OBJECT_PROTOTYPE`) exist solely for backward compatibility with legacy code and **must not be used** in new code.

## 4. Structs, Objects & Function Tables

Structured data and modules are declared using declarative blocks:

### Declarative Objects (`OBJECT` and `OBJECT_EXTENDS`)

Objects define structured types with fields, receiver-bound methods, and dispatch tables:

```c
OBJECT(Counter,
    FIELDS(
        int count;
    ),
    OPERATION(Increment, ()),
    CONST_PREDICATE_INLINE(IsZero, (), { return self->count == 0; }),
    CONST_FUNCTION_INLINE(int, Get, (), { return self->count; })
);
```

Derived classes inherit from a base class using `OBJECT_EXTENDS`, embedding `BASE_CLASS base` as the first field:

```c
OBJECT_EXTENDS(SpecialCounter, Counter,
    FIELDS(
        int step;
    ),
    OPERATION(StepIncrement, ())
);
```

- Safe downcasting and polymorphic conversions use `OBJECT_AS(OBJ, TARGET_TYPE)`.
- Method invocation: `OBJECT_CALL(obj, MethodName, args...)` or `OBJECT_CALL_0(obj, MethodName)`.
- Automatic lifecycle helper: `ClassName_Init(&instance)`.

### Declarative Packages (`PACKAGE`)

Packages group free functions under a unified namespace and function table:

```c
PACKAGE(MathUtils,
    FUNCTION_INLINE(int, Clamp, (int val, int min, int max), {
        return val < min ? min : (val > max ? max : val);
    }),
    PREDICATE_INLINE(IsPositive, (int val), { return val > 0; })
);
```

- Package invocation: `PACKAGE_CALL(PackageName, FunctionName, args...)` or direct calls `PackageName_FunctionName(...)`.

### Compile-time Disabling of Function Tables

Function tables allow static singleton dispatch (`Package_GetFunctionTable()` or `object->api`). Every family must provide compile-time disabling:

- `MODULE_DISABLE_FUNCTION_TABLE`: Disables all function tables globally in the translation unit.
- `PACKAGE_DISABLE_FUNCTION_TABLE`: Disables function tables for packages.
- `OBJECT_DISABLE_FUNCTION_TABLE`: Disables function tables for objects.

When defined, no `FunctionTable` struct or table instance is emitted, removing all dispatch table overhead.

Always use the term `FunctionTable`, never `VTable`.

## 5. Enums

Enums exist primarily for named constant domains and function signatures.

A raw integer must not represent a categorical state when an enum can express it.

### Mandatory X-Macro / Table Pattern

Every enum must be implemented via an X-Macro table (`<DOMAIN>_TABLE(X)`). Directly hardcoding raw lists inside `enum { ... }` declarations is strictly prohibited. The table macro serves as the single source of truth for generating enum values, names, string tables, and switch cases.

```c
#define COMPARISON_TABLE(X) \
    X(LOWER,  -1, "Lower")  \
    X(EQUAL,   0, "Equal")  \
    X(HIGHER,  1, "Higher")

typedef enum COMPARISON_TYPE(Result) {
#define COMPARISON_ENUM_ITEM(NAME, VALUE, DESC) COMPARISON_CONST(NAME) = VALUE,
    COMPARISON_TABLE(COMPARISON_ENUM_ITEM)
#undef COMPARISON_ENUM_ITEM
} COMPARISON_TYPE(Result);

typedef COMPARISON_TYPE(Result) CMPOUT;
```

User-facing enums may use `T`:

```c
TText_Underline_Style
```

Output enums do not use `T`. Their descriptive type remains namespaced; only the short `*OUT` alias is exempt:

```c
typedef enum COMPARISON_TYPE(Result) { ... } COMPARISON_TYPE(Result);
typedef COMPARISON_TYPE(Result) CMPOUT;
```

Prefer the output alias in function signatures:

```c
CMPOUT COMPARISON_FUNC(Int)(int left, int right);
```

`TComparisonResult` is invalid because this enum is not intended as a first-class user-facing type.

## 6. Constants

Public constants use:

```c
*_CONST(NAME)
```

Example:

```c
COMPARISON_LOWER_CONST
STATUS_CONST(SUCCESS)
```

Magic numbers are prohibited:

```c
return -1;                       /* wrong */
return COMPARISON_LOWER_CONST; /* correct */
```

Explicit numeric values are allowed only when the representation itself matters, such as bit flags, protocols, file formats, ABI values, or externally defined mappings.

Outside that definition, code must use the named constant.

## 7. Macro Policy

`#define` should be rare.

Expected uses:

- namespace and naming;
- generic/type generation;
- implementation generation;
- internal preprocessing;
- public syntactic sugar when C cannot express the same interface cleanly.

Avoid macros that merely replace ordinary C syntax.

Public sugar should delegate to a real C function whenever practical.

### Preprocessor operations

Preprocessor code must use the helpers from `Core/Preprocessor` instead of spelling preprocessor operators directly.

Normal token composition uses the expanding helper:

```c
PP_OP_CAT2(SUFFIX, _Store)
PP_OP_CAT3(PREFIX, _, NAME)
```

Use the primitive `PP_OP_CAT` only when argument expansion must intentionally be suppressed, such as protecting a generated identifier from a conflicting external macro:

```c
PP_OP_CAT(NETWORK_ERROR_, NAME)
```

Module/package code must not use raw `##` directly. The raw operator belongs inside the preprocessor utility layer. If a required preprocessing operation does not exist yet, add the reusable helper to `Core/Preprocessor` first, then use that helper from the module.


## 8. Core Grammar

```text
MODULE_MOD
MODULE_CMOD

MODULE_NS(NAME)
MODULE_CNS(NAME)
MODULE_INS(NAME)
MODULE_CINS(NAME)

MODULE_TYPE(SUFFIX)
MODULE_TYPED(TYPE)

MODULE_FUNC(NAME)
MODULE_TYPED_FUNC(...)
MODULE_CONST(NAME)

/* Declarative Definitions */
PACKAGE(PKG, ...)
OBJECT(CLASS, FIELDS(...), ...)
OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS(...), ...)
FIELDS(...)

/* Member Specification Tuples */
OPERATION(NAME, PARAMS) / CONST_OPERATION / *_INLINE
PREDICATE(NAME, PARAMS) / CONST_PREDICATE / *_INLINE
FUNCTION(RET, NAME, PARAMS) / CONST_FUNCTION / *_INLINE
PROCEDURE(NAME, PARAMS) / CONST_PROCEDURE / *_INLINE

/* Unified Implementation Prototypes */
OPERATION_IMPLEMENT_EX(TARGET, NAME, PARAMS)
PREDICATE_IMPLEMENT_EX(TARGET, NAME, PARAMS)
FUNCTION_IMPLEMENT_EX(TARGET, RET, NAME, PARAMS)
PROCEDURE_IMPLEMENT_EX(TARGET, NAME, PARAMS)

/* Implementation Finalizers */
PACKAGE_IMPLEMENT(PKG)
OBJECT_IMPLEMENT(CLASS)

/* Invocation & Dispatch */
PACKAGE_CALL(PKG, FUNC, ...)
OBJECT_CALL(OBJ, METHOD, ...)
OBJECT_AS(OBJ, TARGET_TYPE)

/* Generic Template Engine */
TEMPLATE_DECLARE(NAME, SUFFIX, TYPE)
TEMPLATE_IMPLEMENT(NAME, SUFFIX, TYPE)

/* Function Table Switches */
MODULE_DISABLE_FUNCTION_TABLE
PACKAGE_DISABLE_FUNCTION_TABLE
OBJECT_DISABLE_FUNCTION_TABLE

COSMERON_MACRO_INTERNAL_*
```

If a new macro does not clearly belong to this grammar, reconsider whether it should exist.

## 9. X-Macros

Use X-macros when one semantic table must generate multiple representations.

```c
#define COMPARISON_TABLE(X)\
  X(COMPARISON_LOWER_CONST,-1, "Lower")\
  X(COMPARISON_EQUAL_CONST, 0, "Equal")\
  X(COMPARISON_HIGHER_CONST, 1, "Higher")
```
and

```c
typedef enum COMPARISON_TYPE(Result) {
#define X(NAME, VALUE, DESC) NAME = VALUE,
COMPARISON_TABLE(X)
#undef X
} COMPARISON_TYPE(Result);
```
Rules:

- one table is the source of truth;
- do not "duplicate" the same list in enums, strings, mappings, or switches. If the table exist, use. 
- table names and helper macros use `SNAKE_CASE`;
- generated public constants still pass through the constant namespace;
- X-macros are generation tools, not public runtime API.

## 10. Functions Work

Function contracts follow the result they produce.

| Function kind | Return | Output |
|---|---|---|
| Infallible natural result | value directly | — |
| Predicate / proposition | `bool` | — |
| Fallible operation without primary result | `OPSTATUS` | — |
| Fallible operation with primary result | `OPSTATUS` | `out` parameter |
| Infallible mutation with no result | `void` | mutates target |

Examples:

```c
size_t Vector_Size(...);
bool Vector_Contains(...);

OPSTATUS Vector_Push(...);
OPSTATUS Vector_At(..., TYPE *out);

void Vector_Clear(...);
```

`bool` answers a proposition; `OPSTATUS` reports an operational outcome.

An `out` parameter is a produced result, not merely any mutable pointer.

- required `out == NULL` → `INVALID_ARGUMENT`;
- `out` is modified only on `SUCCESS`, unless explicitly documented otherwise.

Recoverable failures return `OPSTATUS`. Contract violations or impossible invariants may `PANIC`.

Pointers are borrowed by default. Use `const` whenever the pointed data is not logically modified.

Do not hide failures in sentinel values or implicit `LastError` state.
