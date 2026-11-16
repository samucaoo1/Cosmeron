# Cosmeron Architectural & Coding Pattern

This document is the **single canonical specification** for the architecture, naming, declarations, and implementation patterns across the entire Cosmeron ecosystem.

> **Core Philosophy:** Prefer native C constructs over preprocessor macros. Use macros only for namespace binding, declarative code generation, and internal preprocessing. Magic numbers and obsolete compatibility layers are strictly prohibited.

---

## 1. Architectural Tiers & Hierarchy

Cosmeron organizes code into four declarative tiers:

```
DOMAIN (Architectural / Filesystem Grouping: Core, Container, Text, Security, IO)
  └── MODULE (Fundamental Unit of Compilation & Semantics: Memory, File, Bit, Text)
        ├── PACKAGE (Static Collections of Free Functions: Alloc, Operations, Swap)
        ├── STRUCT (Pure POD Data Containers without self/vtable: Point, Range, Header)
        └── OBJECT (Stateful Instances with Receiver 'self' and FunctionTable: Stream, Pool)
              ├── SPACE (Nested Sub-Namespaces / Sub-Tables: Transform, Codec)
              └── VARIANT (Flat Overload Families: Alloc, AllocAndFill, AllocZeroed)
```

### 1.1 The Semantic Role of `MODULE`
- `MODULE` is the fundamental semantic and compilation unit of the library (`Memory`, `Text`, `Algorithm`, `Sort`).
- A module does not require a domain to possess complete semantic identity. `DOMAIN` exists purely as an organizational grouping (drawer).
- Both forms are first-class:
  - With organizational domain: `MODULE(DOMAIN(DomainName), ModuleName, ...)`
  - Standalone without domain: `MODULE(ModuleName, ...)`
- In both cases, the emitted canonical C symbols use `[Library_]ModuleName_Entity` format.

### 1.2 Packages vs. Structs vs. Objects
- **`PACKAGE`**: A stateless, cohesive collection of free functions, events, callbacks, and sub-namespaces. It does **not** bind a receiver `self` and does **not** emit an instance struct or a `T`-prefixed type.
- **`STRUCT`**: A pure Plain Old Data (POD) struct container. It emits a namespaced `T`-prefixed type (`ModuleName_TStructName`), but contains **zero overhead**: no `api` pointer, no FunctionTable, and no receiver `self`. Fully compatible with C designated initializers (`.field = val`), ABI register passing, and `memcpy`.
- **`ISTRUCT`**: An internal Plain Old Data (POD) struct container. It emits an internal namespaced type (`ModuleName_Internal_StructName`) without the `T` prefix, respecting Cosmeron's rule that internal types omit `T`. Contains zero FunctionTable overhead.
- **`INTERNAL` / `INTERNAL_EX`**: An internal declarative scope for helper routines and module infrastructure. Emits pure `static inline` functions with **zero FunctionTable overhead**.
  - Module-level: `INTERNAL(...)` emits `ModuleName_Internal_FuncName`.
  - Component-level: `INTERNAL_EX(Comp, ...)` or `INTERNAL(Comp, ...)` emits `ModuleName_Comp_Internal_FuncName`.
  - Invocations: use `MODULE_IFUNC(Mod, ...)` or `MODULE_ICALL(Mod, ..., (args...))`.
- **`OBJECT`**: A stateful type with structured fields, lifecycle helpers (`Init`), receiver-bound methods (`self`), and automatic user type generation with the `T` prefix (`ModuleName_TObjectName`).

### 1.3 Sub-Namespaces (`SPACE`) & Overload Families (`VARIANT`)
- **`SPACE(SubName, ...)`**: Declares a nested sub-namespace inside a `PACKAGE` or `OBJECT`. Emits nested symbols (`Module_Package_SubName_Func`) and nested sub-tables in the `FunctionTable`.
- **`VARIANT(FamilyName, ...)`**: Declares a flat overload family sharing a common name prefix without raw empty tokens. The base function omits the sub-name (`FUNCTION(RET, PARAMS)`), while variants append a suffix (`FUNCTION(RET, Suffix, PARAMS)`).

---

## 2. Naming & Case Conventions

Everything in Cosmeron must reside inside the module/package namespace whenever C allows it. Exceptions exist only for public sugar macros (which require prior approval), `OPSTATUS`, and short `*OUT` comparison aliases.

### 2.1 Case Table
| Element | Case | Example |
|---|---|---|
| Domain / Module / Package / Object token | `PascalCase` | `Core`, `Memory`, `Algorithm`, `Repeat` |
| User-facing data type | `PascalCase` with `T` | `TVector_uint32`, `TMemoryPool`, `TString` |
| Output enum / Internal type | `PascalCase` without `T` | `ComparisonResult` (`CMPOUT`), `SeekOrigin` |
| Function / Method | `PascalCase` | `Memory_Copy`, `Vector_Push`, `Repeat_Procedure` |
| Constant / enum item | `SNAKE_CASE` | `SUCCESS`, `OUT_OF_RANGE`, `LOWER` |
| Preprocessor macro | `SNAKE_CASE` | `MODULE`, `PACKAGE_CALL`, `MODULE_SCOPE` |
| Internal preprocessor macro | `COSMERON_MACRO_INTERNAL_*` | `COSMERON_MACRO_INTERNAL_PKG_CALL_0` |
| Parameter / local variable / field | `camelCase` | `destination`, `count`, `bufferCapacity` |

### 2.2 The `T` Prefix Rule
- Use `T` prefix **only** for types intended to be directly handled by the user (e.g. `TMemoryPool`, `TVector_uint32`).
- Generated object types automatically acquire the `T` prefix (`OBJECT(String)` emits `Text_TString`).
- Output enums and internal types do **not** use `T` (e.g. `SeekOrigin`, `ComparisonResult`).
- Unambiguous naming: never use generic colliding names like `TPool` or `TNode`. Explicitly qualify domain semantics (`TMemoryPool`, `TGraphNode`).

### 2.3 Strict Prohibition of Local Header Namespace Boilerplate & O(1) Semantic Macros
- **Prefer `MODULE_*` over raw `NS`**: Always prefer semantic macros (`MODULE_TYPE`, `MODULE_TYPED`, `MODULE_ENUM`, `MODULE_CONST`, `MODULE_FUNC`, `MODULE_NS`, `DOMAIN_NS`) over bare `NS(...)` for all public and consumer-facing module interactions.
- **O(1) Flat Joiner (`NS_JOIN`)**: Fast, non-recursive single-substitution concatenation with underscore separator (`_`) in $O(1)$ preprocessor steps.
  - `MODULE_NS(Modulo, Pacote)`: Emits `[LIB_]Modulo_Pacote` in $O(1)$.
  - `MODULE_FUNC(Modulo, Pacote, Func)`: Emits `[LIB_]Modulo_Pacote_Func` in $O(1)$.
  - `DOMAIN_NS(Domain, ...)`: Emits `Domain_...` in $O(1)$.
- **No `.space` files**: All legacy `.space` files are permanently deleted.
- **No manual header-level namespace macros**: Never define:
  ```c
  /* FORBIDDEN: */
  #define FOO_NS(NAME)     ...
  #define FOO_CNS(NAME)    ...
  #define FOO_INS(NAME)    ...
  #define FOO_CINS(NAME)   ...
  #define FOO_TYPE(NAME)   ...
  #define FOO_FUNC(NAME)   ...
  #define FOO_CONST(NAME)  ...
  #define FOO_MOD / FOO_CMOD
  #define GNS2..GNS5 / CNS2..CNS5
  ```
- Use canonical `MODULE_*` macros or `NS(...)` / `CNS(...)` from `Core/Module/Namespace.h`.

### 2.4 File Organization & `Common.inc`
- **No synthetic `Types.h` files**: Types and enums live in the header of their corresponding feature.
- **`Common.inc`**: Reserved **strictly** for internal shared infrastructure of a module (inline helpers, calculations, internal shared types like `Memory/Common.inc`) to prevent duplicate code across packages within the same module folder. The `.inc` extension makes it explicit that it is an internal include, never to be included directly by users.

### 2.5 Library Namespace Configuration (`LIBRARY_NAMESPACE_CONFIG`)
Cosmeron symbols can be branded or configured globally via `LIBRARY_NAMESPACE_CONFIG` or compiler flags (`-DCOSMERON_NAMESPACE=...`, `-DCOSMERON_NAMESPACE_MODE=...`):
- `BARE`: `Text_TString`, `Text_String_Init`, `Status_SUCCESS` (zero library prefix).
- `SHORT`: `Cos_Text_TString`, `Cos_Text_String_Init`.
- `STANDARD`: `Cosmeron_Text_TString`, `Cosmeron_Text_String_Init` (default).
- `FULL`: `Cosmeron_Core_Text_TString` (includes domain in symbol).
When unconfigured, `LIB_PREFIX(...)` resolves cleanly via `NS(...)`.

---

## 3. Function & Method Contracts

Functions and methods belong to declarative blocks (`MODULE`, `PACKAGE`, or `OBJECT`) and must follow strict return contracts:

| Contract | Macro | Return Type | Meaning |
|---|---|---|---|
| Fallible Operation | `OPERATION(Name, Params)` | `OPSTATUS` | Fallible action reporting status |
| Predicate / Condition | `PREDICATE(Name, Params)` | `bool` | Proposition or condition query |
| Value Function | `FUNCTION(Ret, Name, Params)` | `Ret` | Value-producing computation |
| Infallible Procedure | `PROCEDURE(Name, Params)` | `void` | Infallible action / side-effect |
| Three-Way Comparison | `COMPARISON(Name, Params)` | `CMPOUT` | Compares items: `LOWER`, `EQUAL`, `HIGHER` |

### 3.1 Receiver & Inline Variants
- In `OBJECT`: methods bind `self` (`ClassName *self`). Use `CONST_*` for read-only methods (`const ClassName *self`).
- Inline definitions use the `*_INLINE` suffix and embed the body directly:
  ```c
  OPERATION_INLINE(Clear, (), { self->count = 0; return STATUS(SUCCESS); })
  PREDICATE_INLINE(IsEmpty, (size_t size), { return size == 0U; })
  ```

### 3.2 Error Handling & Parameter Contracts
- **`out` parameters**:
  - Required `out == NULL` $\implies$ return `STATUS(INVALID_ARGUMENT)`.
  - An `out` parameter is modified **only** on `STATUS(SUCCESS)`.
- **Borrowed pointers**: Default to borrowed semantics; always mark input pointers with `const`.
- **Zero magic numbers**: Return status codes via named constants (`STATUS(SUCCESS)`, `STATUS(OUT_OF_RANGE)`). Never return raw integers as status.

---

## 4. Header Declarations (`.h`)

Headers declare features using `MODULE(...)` wrapping `PACKAGE`, `OBJECT`, `ENUM`, `SPACE`, `VARIANT`, or templates.

```c
#pragma once

#include "Core/Module/Module.h"
#include "Core/Outcome/Status.h"
#include "Core/Algorithm/Comparison.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Top-level module declaration (DOMAIN is optional) */
MODULE(DOMAIN(Core), StreamBuffer,

    /* 1. Enum via X-Macro table */
    ENUM(SeekOrigin, SEEK_TABLE),

    /* 1.5 Pure POD Struct (Zero vtable overhead) */
    STRUCT(Point,
        FIELDS(
            int32_t x;
            int32_t y;
        )
    ),

    /* 2. Stateful Object */
    OBJECT(Buffer,
        FIELDS(
            char data[64];
            size_t size;
        ),
        OPERATION(Clear, ()),
        OPERATION(Write, (const char *src, size_t count)),
        CONST_FUNCTION(size_t, Length, ()),
        CONST_PREDICATE(IsEmpty, ()),
        CONST_COMPARISON(Compare, (const StreamBuffer_TBuffer *other))
    ),

    /* 3. Static Package */
    PACKAGE(Utils,
        FUNCTION(size_t, StrLen, (const char *str)),
        PREDICATE(IsStrEmpty, (const char *str)),

        /* Sub-Namespace */
        SPACE(Transform,
            PROCEDURE(ToUpper, (char *dest, const char *src))
        ),

        /* Overload Family */
        VARIANT(Alloc,
            FUNCTION(void *, (size_t size)),
            FUNCTION(void *, AndFill, (size_t size, char fillChar))
        )
    ),

    /* 4. Parameterized Package Template (Direct in MODULE, zero blueprints) */
    TEMPLATE_PACKAGE(BitOps, (SUFFIX, TYPE),
        OPERATION(Set, (TYPE *value, uint8_t bit)),
        PREDICATE(Check, (TYPE value, uint8_t bit))
    )
);

#include "Impl/StreamBuffer.impl"
```

---

## 5. Templates (`TEMPLATE_OBJECT`, `TEMPLATE_PACKAGE`, `TEMPLATE_STRUCT` & `TEMPLATE_ISTRUCT`)

Cosmeron uses a **canonical, single-source-of-truth Dual-Mode `.def` template engine**.

### 5.1 Architecture: The Dual-Mode `.def` Definition File
Templates are defined in their canonical `.def` definition file: `<Target>.def` (e.g. `Codespace/Cosmeron/Modules/Struct/Triple.def`, `Codespace/Cosmeron/Core/Memory/Swap.def`).

The `.def` file is the **Single Source of Truth** for the template. The body is declared **once and only once** using the canonical declarative DSL (`TEMPLATE_STRUCT`, `TEMPLATE_PACKAGE`, `TEMPLATE_OBJECT`), and serves two modes seamlessly:

```c
/* Triple.def - Dual Mode Engine (Zero Duplication) */

#ifndef TEMPLATE_Struct_Triple
#define TEMPLATE_Struct_Triple(SUFFIX, TYPE)                                    \
  TEMPLATE_STRUCT(Struct, Triple, SUFFIX, FIELDS(                               \
      union {                                                                   \
        struct { TYPE first; TYPE second; TYPE third; };                        \
        TYPE values[3];                                                         \
      };                                                                        \
  ))
#endif

/* Mode 2 (Direct Include): Auto-instantiates and cleans up environment */
#if defined(SUFFIX) && defined(TYPE)
  TEMPLATE_Struct_Triple(SUFFIX, TYPE);
  #undef SUFFIX
  #undef TYPE
#endif
```

#### Dual-Mode Operation:
1. **Mode 1 (Macro Export - Primary & Recommended):**  
   Included normally without parameters defined (e.g. `#include "Triple.def"` inside the public header). It exports the `TEMPLATE_<Module>_<Target>` macro, enabling:
   - On-demand instantiation: `TEMPLATE_INSTANTIATE(Module, Target, Suffix, ConcreteType, ...)`.
   - Table-driven batch instantiation: `TEMPLATE_INSTANTIATE_TABLE(Module, Target, TYPE_TABLE)`.
2. **Mode 2 (Direct Include Instantiation):**  
   If included with parameters defined beforehand (e.g. `#define SUFFIX MyType` / `#define TYPE MyType` then `#include "Triple.def"`), the `.def` automatically expands the template macro and `#undef`s the parameter variables to restore a clean environment.

### 5.2 Rules for Templates
1. **Zero Duplication & Single Source of Truth**: The template body is written once and only once inside its `.def` file. Never duplicate member lists or field declarations.
2. **Naming Convention**:
   - Macro: `TEMPLATE_<Module>_<Target>(PARAMS...)` (e.g. `TEMPLATE_Memory_Swap`, `TEMPLATE_Struct_Triple`, `TEMPLATE_Container_Vector`).
   - For structs, the concept name is used without `T` in the template macro name (`TEMPLATE_Struct_Triple`).
   - Generated user types: `T<Target>_<suffix>` (e.g. `Struct_TTriple_uint32`, `Container_TVector_int`).
   - Generated internal types: `<Target>_<suffix>` (omit `T`).
3. **Instantiation Syntax**:
   - On-demand instantiation:  
     `TEMPLATE_INSTANTIATE(Module, Target, Suffix, ConcreteType, ...);`
   - Table-driven batch instantiation:  
     `TEMPLATE_INSTANTIATE_TABLE(Module, Target, TYPE_TABLE);`
4. **Homogeneous Parameter Order (3-Tier Rule)**:
   The parameter list across all templates follows a strict homogeneous hierarchy:
   $$\text{Order: } (\underbrace{\text{SUFFIX}}_{1\text{st: Identifier}}, \quad \underbrace{\text{DATA TYPES...}}_{2\text{nd: Primary Types}}, \quad \underbrace{\text{METADATA...}}_{3\text{rd: Static Capacities / Policies}})$$
   - **Arity 2 (Single Type)**: `(SUFFIX, TYPE)` (e.g. `Swap`, `Triple`, `Vector`, `Stack`, `Queue`, `Result`, `Optional`).
   - **Arity 3 (Dual / Heterogeneous Types)**: `(SUFFIX, FIRST_TYPE, SECOND_TYPE)` or `(SUFFIX, KEY_TYPE, VALUE_TYPE)` (e.g. `Dual`, `Map`, `Dict`, `HashMap`).
   - **Arity 3 / 4 (Static Capacity / Policy)**: `(SUFFIX, TYPE, CAPACITY)` (e.g. `StaticArray`, `RingBuffer`) or `(SUFFIX, KEY_TYPE, VALUE_TYPE, CAPACITY)`.
   - Matching X-Macro rows unpack directly without glue code via `PP_UNWRAP ROW`:
     ```c
     #define STRUCT_TYPE_TABLE(X) \
       X((uint8, uint8_t))        \
       X((uint16, uint16_t))      \
       X((uint32, uint32_t))
     ```
5. **Autocontained Inline & Struct Templates**:
   - Structs (pure POD) and templates with inline methods (`OPERATION_INLINE`, `PROCEDURE_INLINE`, `FUNCTION_INLINE`) are completely self-contained.
   - The template engine (`Template.impl`) never requires stub/dummy `_IMPLEMENT` macros for inline templates or structs. Non-inline template methods are implemented in `.impl` files only when non-inline members exist.


---

## 6. Implementation Scoping (`.impl` / `.c`)

Implementation files use **lexical scoping** via `*_SCOPE` and `*_SCOPE_END()`. No finalizer macros (`PACKAGE_IMPLEMENT` / `OBJECT_IMPLEMENT`) are used.

### 6.1 Package Implementations
```c
#pragma once

PACKAGE_SCOPE(StreamBuffer, Utils)

PACKAGE_FUNCTION_IMPLEMENT_EX(StreamBuffer_Utils, size_t, StrLen, (const char *str)) {
    if (!str) return 0U;
    return strlen(str);
}

PACKAGE_PREDICATE_IMPLEMENT_EX(StreamBuffer_Utils, IsStrEmpty, (const char *str)) {
    return !str || str[0] == '\0';
}

SPACE_SCOPE(Transform)
PACKAGE_PROCEDURE_IMPLEMENT_EX(StreamBuffer_Utils_Transform, ToUpper,
                               (char *dest, const char *src)) {
    if (!dest || !src) return;
    for (size_t i = 0; src[i] != '\0'; ++i) {
        char c = src[i];
        if (c >= 'a' && c <= 'z') c = (char)(c - ('a' - 'A'));
        dest[i] = c;
    }
}
SPACE_SCOPE_END()

PACKAGE_SCOPE_END()
```

### 6.2 Object Implementations
```c
#pragma once

OBJECT_SCOPE(StreamBuffer, Buffer)

OBJECT_OPERATION_IMPLEMENT_EX(StreamBuffer_Buffer, Clear, ()) {
    if (!self) return STATUS(INVALID_ARGUMENT);
    self->size = 0U;
    self->data[0] = '\0';
    return STATUS(SUCCESS);
}

OBJECT_FUNCTION_IMPLEMENT_EX(StreamBuffer_Buffer, size_t, Length, ()) {
    if (!self) return 0U;
    return self->size;
}

OBJECT_PREDICATE_IMPLEMENT_EX(StreamBuffer_Buffer, IsEmpty, ()) {
    return !self || self->size == 0U;
}

OBJECT_SCOPE_END()
```

---

## 7. Declarative Callbacks, Closures & Events

### 7.1 First-Class Pure Callbacks (`TCALLBACK`)
Callbacks represent pure, stateless function pointers with zero overhead (exactly `sizeof(void *)`, 8 bytes on 64-bit platforms). They are declared standalone or in variadic blocks across `MODULE`, `PACKAGE`, or `OBJECT`:

- Declarations:
  - `CALLBACK_OPERATION(Name, (params))`
  - `CALLBACK_PREDICATE(Name, (params))`
  - `CALLBACK_FUNCTION(Ret, Name, (params))`
  - `CALLBACK_PROCEDURE(Name, (params))`
  - `CALLBACK_COMPARISON(Name, (params))`
- Variadic block syntax:
  ```c
  MODULE(DOMAIN(Core), MyModule,
    CALLBACK(
      FUNCTION(int, Transform, (int x)),
      PROCEDURE(Log, (int code))
    )
  );
  ```
- Universal invocation & validation:
  - Invocation: `CALLBACK_CALL(cb, args...)` or `CALLBACK_INVOKE(cb, args...)`
  - Validation: `CALLBACK_IS_VALID(cb)`
  - Empty: `CALLBACK_EMPTY` (`NULL`)

### 7.2 Contextual Fat Closures (`TCLOSURE`)
Closures represent contextual callables combining a function pointer and an optional borrowed context in a 16-byte Pure POD structure (`sizeof == 2 * sizeof(void *)`):

```c
typedef struct TClosure_Name {
  union {
    TCallback_Name callback;
    Name_Function function;
  };
  void *context;
} TClosure_Name;
```

- Declarations:
  - `CLOSURE_OPERATION(Name, (params))`
  - `CLOSURE_PREDICATE(Name, (params))`
  - `CLOSURE_FUNCTION(Ret, Name, (params))`
  - `CLOSURE_PROCEDURE(Name, (params))`
  - `CLOSURE_COMPARISON(Name, (params))`
- Variadic block syntax:
  ```c
  MODULE(DOMAIN(Core), MyModule,
    CLOSURE(
      FUNCTION(int, Compute, (int x)),
      PREDICATE(Validate, (int x))
    )
  );
  ```
- Construction & Binding:
  - `CLOSURE_FUNC(Name_Bind)(fn, ctx)`: Binds a stateful target taking trailing `void *context`.
  - `CLOSURE_FUNC(Name_FromCallback)(cb)`: Promotes a pure stateless callback without wrappers.
- Sentinel Dual-Dispatch:
  - `CLOSURE_CALL(cl, args...)` dynamically checks `cl.context == NULL`. If null, invokes `cl.callback(args...)` directly. If non-null, unwraps the sentinel `(context == (void *)1 ? NULL : context)` and invokes `cl.function(args..., ctx)`.
- Method Adapter:
  - `CLOSURE_ADAPT_OBJECT_METHOD(CLOSURE_TYPE, CLASS_TYPE, METHOD_FUNC, RET, PARAMS, ARGS)` bridges object receiver methods `(CLASS *self, ...)` to closure signatures `(..., void *context)`.
  - `CLOSURE_AS_CLOSURE(METHOD_FUNC, self)` creates a ready-to-use closure from an object instance.

### 7.3 Declarative Events (`EVENT_PROCEDURE` & `EVENT_OPERATION`)
- Zero-allocation 1-to-N observer notification powered natively by `TCLOSURE`.
- Static slots embedded in struct or package scope with automatic cache-line decay (4, 8, 12, or 16 slots).
- Supports subscribing both stateful closures (`Bind`) and pure callbacks (`FromCallback`).
- Re-entrancy safe: `Emit` takes a stack snapshot of active listeners before dispatching.
- Reactive field mutation: `OBJECT_MUTATE(self, ClassName, fieldName, newValue, EventName)` updates state and emits on change.

---

## 8. Function Tables & Compile-Time Disabling

Every package and object supports dispatch via `FunctionTable`:
- Packages: singleton table `Package_GetFunctionTable()` invoked via `PACKAGE_CALL(Package, Func, ...)`.
- Objects: instance pointer `obj->api` invoked via `OBJECT_CALL(obj, Method, ...)`.

### Zero-Cost Disabling
In resource-constrained or embedded environments, dispatch tables can be eliminated at compile time without changing code:
- `-DMODULE_DISABLE_FUNCTION_TABLE`: Disables all tables globally.
- `-DPACKAGE_DISABLE_FUNCTION_TABLE`: Disables tables for static packages.
- `-DOBJECT_DISABLE_FUNCTION_TABLE`: Disables tables for objects.

When disabled, no `FunctionTable` struct or table instance is emitted in memory.

---

## 9. Enums & X-Macro Table Pattern

Every enum must be implemented via a decoupled X-Macro table using the **Row-Tuple Pattern**:

```c
#define SEEK_COL_NAME(NAME, VALUE, DESC)  NAME
#define SEEK_COL_VALUE(NAME, VALUE, DESC) VALUE
#define SEEK_COL_DESC(NAME, VALUE, DESC)  DESC
#define SEEK_GET(FIELD, ROW) PP_OP_CAT2(SEEK_COL_, FIELD) ROW

#define SEEK_TABLE(X) \
  X((SET, 0, "Seek from beginning")) \
  X((CUR, 1, "Seek from current position")) \
  X((END, 2, "Seek from end"))
```

### 9.1 Declarative Enums (`ENUM` / `ENUM_EX`)

Enums are declared natively inside `MODULE(...)`, `PACKAGE(...)`, or `OBJECT(...)`, or standalone via `ENUM_DEF` / `ENUM_DEF_EX`:

```c
MODULE(DOMAIN(Core), File,
  /* Top-level module enum: generates File_SeekOrigin and File_SET = 0, File_CUR = 1... */
  ENUM(SeekOrigin, SEEK_TABLE),

  PACKAGE(Stream,
    /* Inside package with PLAIN dispatch: generates File_Stream_SET = 0... */
    ENUM(Seek, SEEK_TABLE),

    /* Inside package with QUALIFIED dispatch: generates File_Stream_QualifiedSeek_SET = 0... */
    ENUM_EX(QualifiedSeek, SEEK_TABLE, QUALIFIED),

    FUNCTION(int, Position, (void))
  ),

  OBJECT(Handle,
    FIELDS(
      File_Handle_State state;
    ),
    /* Inside object: enum is available before struct fields and methods */
    ENUM(State, STATE_TABLE)
  )
);
```

### 9.2 Dispatch Presets
- `PLAIN` (or `NAME_VALUE`, default for `ENUM`): Emits `CNS(TARGET, NAME) = VALUE,`.
- `QUALIFIED`: Emits `CNS(TARGET, ENUM_NAME, NAME) = VALUE,` to prevent constant collisions when multiple enums share names (e.g. `DEFAULT`, `NONE`, `IDLE`).
- `NAME_ONLY`: Emits `CNS(TARGET, NAME),` letting C auto-increment from 0.
- `QUALIFIED_NAME`: Emits `CNS(TARGET, ENUM_NAME, NAME),` without values.
- **Custom Dispatch**: Pass any macro `DISPATCH(TARGET, ENUM_NAME, ROW)` for custom transformations (e.g. bit-shifts `1U << VALUE`).

Standalone declarations outside `MODULE`:
```c
ENUM_DEF(MODULE_NS(File, Stream), Seek, SEEK_TABLE);
ENUM_DEF_EX(MODULE_NS(File, Stream), Seek, SEEK_TABLE, QUALIFIED);
```

---

## 10. Canonical Consumption Grammar

External consumers interact with modules using the unified grammar of `Namespace.h`:

```c
/* 1. Types (Objects and Structs receive automatic 'T' prefix) */
MODULE_TYPE(StreamBuffer, Buffer) buf; /* Expands to: StreamBuffer_TBuffer */
MODULE_TYPE(Geometry, Point) pt = { .x = 10, .y = 20 }; /* Pure POD Struct */
MODULE_TYPED(Geometry, Pair, int) pair = { .first = 1, .second = 2 }; /* Template Struct */
MODULE_TYPED(Outcome, Optional, int32) opt; /* Template Object */

/* 2. Enums and Constants */
MODULE_ENUM(File, SeekOrigin) origin = MODULE_CONST(File, CUR);

/* 3. Semantic Namespace & Function Symbols in O(1) */
MODULE_NS(StreamBuffer, Utils) /* Expands to: StreamBuffer_Utils */
MODULE_FUNC(StreamBuffer, Utils, StrLen)("test") /* Expands to: StreamBuffer_Utils_StrLen("test") */
DOMAIN_NS(Core, Algorithm, Repeat) /* Expands to: Core_Algorithm_Repeat */

/* 4. Status Output */
OPSTATUS status = STATUS(SUCCESS);

/* 5. Unified Invocation (MODULE_CALL in O(1)) */
MODULE_CALL(StreamBuffer, Buffer, Init, (&buf));
MODULE_CALL(StreamBuffer, Buffer, Clear, (&buf));
size_t len = MODULE_CALL(StreamBuffer, Utils, StrLen, ("test"));
MODULE_CALL(StreamBuffer, Utils, Transform, ToUpper, (dest, src));

/* 6. Direct C Invocation */
StreamBuffer_Buffer_Clear(&buf);
StreamBuffer_Utils_StrLen("test");
```

---

## 11. Strict Refactoring Rules (Zero Retrocompatibilidade)

1. **Delete all `.space` files**: Never create, maintain, or `#include` `.space` files.
2. **Zero header boilerplate macros**: Remove all `#define FOO_NS`, `FOO_CNS`, `FOO_FUNC`, etc.
3. **Zero synthetic `Types.h` files**: Keep types with their features; reserve `Common.inc` strictly for internal shared utilities.
4. **Zero blueprint macros**: Never create `#define TEMPLATE_<Name>(...)` macros duplicating declarations.
5. **Zero backward compatibility layers**: Never emit wrappers or legacy typedefs (`typedef ERROR_TYPE(TError) TError;` is strictly forbidden).
6. **Zero magic numbers**: Use named status constants (`STATUS(...)`) and X-Macro enums.
7. **Zero ad-hoc platform/compiler checks**: All OS, compiler, libc, and processor architecture detection must be conducted strictly via `Codespace/Cosmeron/Core/Preprocessor/Detect.h`. Never write `#ifdef _WIN32`, `#ifdef __linux__`, `#if defined(__clang__)`, etc.
8. **English only**: All code identifiers, comments, commit messages, and documentation must be written in English.

---

## 12. Centralized Platform & Environment Detection (`Detect.h`)

All environment-dependent code must include `Core/Preprocessor/Detect.h` (or `Detect/Detect.h`). Detection provides normalized `0` or `1` compile-time constants:

- **Operating System (`OperationSystem.h`):** `OS_WINDOWS`, `OS_LINUX`, `OS_MAC`, `OS_IOS`, `OS_BSD`, `OS_FREEBSD`, `OS_OPENBSD`, `OS_NETBSD`, `OS_DRAGONFLY`, `OS_SOLARIS`, `OS_AIX`, `OS_HAIKU`, `OS_QNX`, `OS_HURD`, `OS_REDOX`, `OS_WASI`, `OS_EMSCRIPTEN`, `OS_POSIX`, `OS_UNIX`, `OS_ANDROID`, `OS_OHOS`.
- **Compiler Family (`Compiler.h`):** `COMPILER_CLANG`, `COMPILER_GCC`, `COMPILER_MSVC`, `COMPILER_INTEL`, `COMPILER_KNOWN`.
- **Processor Architecture (`Processor.h`):** `PROCESSOR_X86`, `PROCESSOR_X64`, `PROCESSOR_ARM`, `PROCESSOR_ARM64`, `PROCESSOR_RISCV`, `PROCESSOR_WASM`, `PROCESSOR_PPC`, `PROCESSOR_PPC64`, `PROCESSOR_MIPS`, `PROCESSOR_LOONGARCH64`, `PROCESSOR_S390X`, `PROCESSOR_KNOWN`.
- **C Library (`Library.h`):** `LIBC_GLIBC`, `LIBC_MUSL`, `LIBC_BIONIC`, `LIBC_MSVC`, `LIBC_APPLE`, `LIBC_BSD`, `LIBC_UCLIBC`, `LIBC_NEWLIB`, `LIBC_WASI`, `LIBC_EMSCRIPTEN`, `LIBC_RELIBC`, `LIBC_KNOWN`.
