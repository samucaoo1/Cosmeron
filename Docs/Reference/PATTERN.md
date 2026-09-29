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

## 3. Functions

Every public or normal function has a prototype macro.

```c
*_OPERATION_PROTOTYPE(...)
```

The prototype macro is the single source of truth for the function signature. It does **not** imply that an implementation macro must exist.

For a normal one-off function, reuse the prototype directly:

```c
#define ADDRESS_EQUAL_PROTOTYPE \
    static inline bool ADDRESS_FUNC(Equal)( \
        const ADDRESS_TYPE(Address) *left,  \
        const ADDRESS_TYPE(Address) *right)

ADDRESS_EQUAL_PROTOTYPE;
```

And in the implementation:

```c
ADDRESS_EQUAL_PROTOTYPE {
    /* ... */
}
```

Use implementation-generation macros only when they actually generate or specialize code:

```c
*_OPERATION_IMPLEMENT(...)
*_IMPLEMENT_ALL(...)
```

Example:

```c
#define VECTOR_PUSH_PROTOTYPE(TYPE) \
    static inline OPSTATUS VECTOR_FUNC(Push)(...)

#define VECTOR_PUSH_IMPLEMENT(TYPE) \
    VECTOR_PUSH_PROTOTYPE(TYPE) {   \
        /* ... */                   \
    }

#define VECTOR_IMPLEMENT_ALL(TYPE) \
    VECTOR_INIT_IMPLEMENT(TYPE)    \
    VECTOR_PUSH_IMPLEMENT(TYPE)    \
    VECTOR_POP_IMPLEMENT(TYPE)
```

Do not create `*_IMPLEMENT` aliases that merely rename `*_PROTOTYPE`, and do not create `*_IMPLEMENT_ALL` when there is only one fixed implementation.

### Readability

Prototype macros must remain as readable as normal C declarations.

- keep the signature visibly declaration-shaped;
- break long parameter lists across lines;
- do not collapse large prototypes into unreadable one-line macro blocks;
- do not add macro layers that make the declaration harder to understand than native C.

Internal functions do not require a separate prototype macro:

```c
*_IMPLEMENT_INTERNAL(...)
```

They contain signature and body together.

Function names use:

```c
*_FUNC(NAME)
*_TYPED_FUNC(...)
```

Adapters and wrappers are normal functions, not a separate category.

## 4. Structs and Function Tables

Generated structures use:

```c
*_STRUCT(...)
```

Function tables use:

```c
*_FUNCTION_TABLE_STRUCT(...)
*_FUNCTION_TABLE_INSTANCE(...)
```

Every Function Table family must provide a compile-time switch:

```c
*_DISABLE_FUNCTION_TABLE
```

When defined, the Function Table struct, instance, bindings, and related generated code must not be emitted.

Use `FunctionTable`, not `VTable`.

Recommended filesystem form:

```text
FunctionTable/
    Vector.inc
    Queue.inc
```

or, when stored beside the package:

```text
Vector.FunctionTable.inc
```

## 5. Enums

Enums exist primarily for named constant domains and function signatures.

A raw integer must not represent a categorical state when an enum can express it.

User-facing enums may use `T`:

```c
TText_Underline_Style
```

Output enums do not use `T`. Their descriptive type remains namespaced; only the short `*OUT` alias is exempt:

```c
typedef enum COMPARISON_TYPE(Result) {
    COMPARISON_CONST(LOWER)  = -1,
    COMPARISON_CONST(EQUAL)  = 0,
    COMPARISON_CONST(HIGHER) = 1
} COMPARISON_TYPE(Result);

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
COMPARISON_CONST(LOWER)
STATUS_CONST(SUCCESS)
```

Magic numbers are prohibited:

```c
return -1;                       /* wrong */
return COMPARISON_CONST(LOWER); /* correct */
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

OPERATION_PROTOTYPE(...)
OPERATION_IMPLEMENT(...)
IMPLEMENT_INTERNAL(...)
IMPLEMENT_ALL(...)

STRUCT(...)
FUNCTION_TABLE_STRUCT(...)
FUNCTION_TABLE_INSTANCE(...)

LIBRARY_MACRO_INTERNAL_*
```

If a new macro does not clearly belong to this grammar, reconsider whether it should exist.

## 9. X-Macros

Use X-macros when one semantic table must generate multiple representations.

```c
#define COMPARISON_TABLE(X)\
  X(COMPARISON_CONST(LESS),-1, "Less than")\
  X(COMPARISON_CONST(SAME), 0, "Same")\
  X(COMPARISON_CONST(GREATER), 1, "Greater than")
```
and

```c
typedef enum COMPARISON_TYPE(Result) {
"#define X(NAME, VALUE, DESC) NAME = VALUE,
COMPARISON_TABLE(X)
#undef X"
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
