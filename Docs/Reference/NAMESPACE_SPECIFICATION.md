# Cosmeron Next-Generation Namespace & Module Specification

## 1. Executive Summary

This document specifies the redesigned, unified namespace and declarative module architecture for the Cosmeron ecosystem. It replaces the legacy `.space` system with a zero-boilerplate, multi-tiered declarative engine supporting configurable symbol depth, custom library branding, decoupled X-Macro reflection, first-class callbacks, declarative comparisons, and unified implementation scoping.

---

## 2. Core Architectural Pillars

### 2.1 The Four Declarative Levels
The ecosystem is organized into four distinct architectural tiers:

```
DOMAIN (Architectural Grouping: Core, Container, Text, Security, IO)
  └── MODULE (Unit of Compilation & Linker Boundary: Memory, File, Bit, Sort, CString)
        ├── PACKAGE (Static Collections of Free Functions: Alloc, Operations, Swap)
        └── OBJECT (Stateful Instances with Receiver 'self' and FunctionTable: Stream, Pool)
              ├── SPACE (Nested Sub-Namespaces / Sub-Tables: Transform, Codec)
              └── VARIANT (Flat Overload Families: Alloc, AllocAndFill, AllocZeroed)
```

> **Semantic Philosophy:** The `MODULE` is the fundamental semantic unit of the Cosmeron architecture (`Memory`, `Sort`, `Random`, `Text`, `Bit`). A module does not need a domain to possess complete semantic value (`Memory` is meaningful without `Core`; `LinkedList` is meaningful without `Container`). `DOMAIN` exists purely as organizational filesystem grouping (drawer). Both standalone declaration `MODULE(ModuleName, ...)` and drawer-tagged declaration `MODULE(DOMAIN(DomainName), ModuleName, ...)` are supported identically. Canonical C symbols are emitted in `[Library_]ModuleName_Entity` format.

### 2.2 Complete Elimination of `.space` Files
- All legacy `.space` files are eliminated.
- The 11 repetitive manual macros (`*_MOD`, `*_CMOD`, `*_NS`, `*_CNS`, `*_INS`, `*_CINS`, `*_TYPE`, `*_FUNC`, `*_CONST`) previously duplicated in every header are obsolete and deleted.
- Shared structs and value objects move to formal `OBJECT(...)` declarations or dedicated `Types.h` headers.
- Enums and X-Macros are detached from namespace infrastructure and declared via `ENUM(...)` and accessed via `MODULE_ENUM(...)`.

---

## 3. Library Namespace Configuration (`LIBRARY_NAMESPACE_CONFIG`)

Cosmeron symbols are entirely decoupled from hardcoded library names. A consumer or engine may brand, alias, or omit the library prefix without altering any module code.

### 3.1 Declarative Configuration
Declared in a root configuration header (e.g. `CosmeronConfig.h`):

```c
LIBRARY_NAMESPACE_CONFIG(COSMERON,
    NAME(Mypotato),     /* Custom library identifier */
    SHORT(MP),          /* Abbreviated short alias */
    MODE(STANDARD),     /* Depth: BARE, SHORT, STANDARD, FULL */
    CASE(HYBRID),       /* Constant casing: HYBRID, UPPER */
    TABLE(ENABLED)      /* Dispatch tables: ENABLED, OBJECT_ONLY, PACKAGE_ONLY, DISABLED */
)
```

#### FunctionTable Policy Options (`TABLE`):
- `ENABLED` / `ALL`: Full dispatch tables for both `OBJECT` instances and static `PACKAGE` modules (default).
- `OBJECT_ONLY`: Tables emitted only for stateful `OBJECT` instances (`obj->api`); static packages emit zero tables and zero overhead.
- `PACKAGE_ONLY`: Tables emitted only for static `PACKAGE` modules (e.g. plugin loading / dynamic export); objects have no `api` pointer.
- `DISABLED` / `NONE`: Eliminates all dispatch tables across the entire library. All calls resolve to direct C symbols.

### 3.2 Compilation / CLI Configuration
Alternatively configured via build system flags:
```bash
-DCOSMERON_NAMESPACE=Mypotato -DCOSMERON_NAMESPACE_SHORT=MP -DCOSMERON_NAMESPACE_MODE=STANDARD -DCOSMERON_TABLE=OBJECT_ONLY
```

#### Per-Module CLI Override:
Individual modules can selectively override the global table policy via build flags:
```bash
-DCOSMERON_TABLE_Math=DISABLED -DCOSMERON_TABLE_Bit=DISABLED
```

### 3.3 Default Fallbacks (Zero-Config)
If unconfigured, the engine defaults to:
- `NAME`: `Cosmeron`
- `SHORT`: `Cos`
- `MODE`: `STANDARD`
- `CASE`: `HYBRID`
- `TABLE`: `ENABLED`

### 3.4 Symbol Depth Modes

| Mode | Type Symbol Format (`MODULE_TYPE`) | Enum Symbol Format (`MODULE_ENUM`) | Function Symbol Format | Constant / Status Format | Ideal Use Case |
|---|---|---|---|---|---|
| `BARE` | `Text_TString` | `Text_SeekOrigin` | `Text_String_Init` | `Status_SUCCESS` | Embedded, lightweight CLI tools, flat builds |
| `SHORT` | `MP_Text_TString` | `MP_Text_SeekOrigin` | `MP_Text_String_Init` | `MP_Status_SUCCESS` | Compact prefixing without namespace collision |
| `STANDARD` | `Mypotato_Text_TString` | `Mypotato_Text_SeekOrigin` | `Mypotato_Text_String_Init` | `Mypotato_Status_SUCCESS` | Canonical production and engine integration |
| `FULL` | `Mypotato_Core_Text_TString` | `Mypotato_Core_Text_SeekOrigin` | `Mypotato_Core_Text_String_Init` | `Mypotato_Core_Status_SUCCESS` | Strict multi-vendor isolation, FFI bindings |

---

## 4. Header Declaration Syntax (`.h`)

All function and method descriptors strictly maintain the canonical Cosmeron parameter order:
- `FUNCTION(RET, NAME, PARAMS)`
- `CONST_FUNCTION(RET, NAME, PARAMS)`
- `OPERATION(NAME, PARAMS)`
- `PREDICATE(NAME, PARAMS)`
- `PROCEDURE(NAME, PARAMS)`
- `COMPARISON(NAME, PARAMS)`

### 4.1 Stateful Objects (`OBJECT` and `EXTENDS`)
- Objects represent types with state, fields, receiver `self`, and optional `FunctionTable`.
- The `T` prefix rule is applied **automatically** to the generated user-facing type (`Text_TString`), while methods and tables use the clean name (`Text_String_Init`, `Text_String_FunctionTable`).
- Inheritance is declared with `EXTENDS(BaseType)`.
- Comparisons use `COMPARISON(Name, Params)` returning `CMPOUT`, with automatic adapter generation for `TCALLBACK(Comparison)`.

```c
#pragma once

#include "Core/Module/Module.h"

MODULE(DOMAIN(Core), Text,
    OBJECT(String,
        FIELDS(
            char *data;
            size_t size;
            size_t capacity;
        ),
        OPERATION(Clear, ()),
        OPERATION(Reserve, (size_t newCapacity)),
        CONST_FUNCTION(size_t, Length, ()),
        CONST_PREDICATE(IsEmpty, ()),
        CONST_COMPARISON(Compare, (const Text_TString *other)),
        COMPARISON_STRING(CompareContent, data)
    ),

    OBJECT(AttributedText, EXTENDS(String),
        FIELDS(
            uint32_t attributes;
        ),
        OPERATION(SetAttribute, (uint32_t attributeMask))
    )
);
```

### 4.2 Static Packages & Namespaces (`PACKAGE`, `SPACE`, `VARIANT`)
- `PACKAGE`: Logical bundle of free functions without instance state.
- `SPACE`: Nested sub-namespace with a nested sub-struct in `FunctionTable`.
- `VARIANT`: Flat family of name-overloaded functions sharing a common prefix.

#### Clean Overload Syntax for `VARIANT`:
The base function omits the sub-name entirely using the 2-argument form `FUNCTION(RET, PARAMS)` or 1-argument `OPERATION(PARAMS)`. Overloaded variants specify the suffix:

```c
MODULE(DOMAIN(Core), Text,
    PACKAGE(CString,
        FUNCTION(size_t, Length, (const char *str)),
        PREDICATE(IsEmpty, (const char *str)),

        /* VARIANT: Flat overload family without raw empty tokens */
        VARIANT(Alloc,
            FUNCTION(void *, (size_t size)),
            FUNCTION(void *, AndFill, (size_t size, char ch)),
            FUNCTION(void *, Zeroed, (size_t size))
        ),

        /* SPACE: Nested sub-namespace with nested FunctionTable struct */
        SPACE(Transform,
            PROCEDURE(ToUpper, (char *dest, const char *src)),
            PROCEDURE(ToLower, (char *dest, const char *src))
        )
    )
);
```

### 4.3 Pure Package Modules (Direct Functions in `MODULE`)
When an entire module is a static collection of functions (e.g. `Math`, `Bit`), functions are placed directly in `MODULE(...)`, eliminating redundant `Math_Math_...` naming:

```c
MODULE(DOMAIN(Core), Math,
    FUNCTION(int, Abs, (int x)),
    FUNCTION(double, Sqrt, (double x)),
    FUNCTION(int, Min, (int a, int b)),
    FUNCTION(int, Max, (int a, int b))
);
```

### 4.4 Parameterized Templates (`TEMPLATE_OBJECT` and `TEMPLATE_PACKAGE`)
Templates define parameterized blueprints instantiated on demand, adhering strictly to the `TEMPLATE_*` naming convention:

```c
MODULE(DOMAIN(Container), Container,
    TEMPLATE_OBJECT(Vector, (SUFFIX, TYPE),
        FIELDS(
            TYPE *items;
            size_t count;
            size_t capacity;
        ),
        OPERATION(Push, (TYPE item)),
        OPERATION(Pop, (TYPE *outItem)),
        CONST_FUNCTION(size_t, Count, ()),
        CONST_PREDICATE(IsEmpty, ())
    ),

    TEMPLATE_PACKAGE(BitOps, (SUFFIX, TYPE),
        OPERATION(Set, (TYPE *value, uint8_t bit)),
        PREDICATE(Check, (TYPE value, uint8_t bit))
    )
);
```

#### Template Instantiation Workflows:
1. **Split Mode (Prototypes in `.h`, bodies in `.impl`):**
   ```c
   /* In header: */
   TEMPLATE_DECLARE(Vector, uint32, uint32_t);
   /* In implementation: */
   TEMPLATE_IMPLEMENT(Vector, uint32, uint32_t);
   ```
2. **Unified Mode (Declares types, prototypes, and bodies simultaneously):**
   ```c
   TEMPLATE_INSTANTIATE(Vector, uint32, uint32_t);
   ```
3. **Table-Driven Batch Instantiation:**
   ```c
   TEMPLATE_INSTANTIATE_TABLE(BitOps, BIT_WIDTH_TABLE);
   ```

### 4.5 First-Class Callbacks (`CALLBACK` and `TCALLBACK`)
Callbacks are declared directly inside `MODULE`, `PACKAGE`, or `OBJECT` using category descriptors:

```c
MODULE(DOMAIN(IO), StreamWatcher,
    PACKAGE(Watcher,
        CALLBACK(OPERATION(OnEvent, (int eventCode, const void *payload))),
        CALLBACK(PREDICATE(Filter, (int eventCode))),

        OPERATION(Subscribe, (TCALLBACK(OnEvent) handler))
    )
);
```

Any object method returning `CMPOUT` automatically emits:
- `<CLASS>_<Method>_ComparatorAdapter(const void *left, const void *right, void *context)`
- `<CLASS>_<Method>_AsComparator()` returning a bound `TCALLBACK(Comparison)`.

### 4.6 Enums and X-Macro Reflection (`ENUM` and `MODULE_ENUM`)
Enums and X-Macros are decoupled from module internals. Enums follow the rule that **they do not carry the `T` prefix**:

```c
/* 1. X-Macro Row-Tuple Table */
#define SEEK_TABLE(X) \
    X((SET, 0, "Seek from beginning")) \
    X((CUR, 1, "Seek from current position")) \
    X((END, 2, "Seek from end"))

/* 2. Bound inside MODULE */
MODULE(DOMAIN(IO), File,
    ENUM(SeekOrigin, SEEK_TABLE),

    OBJECT(File,
        FIELDS(intptr_t handle;),
        OPERATION(Seek, (int64_t offset, MODULE_ENUM(File, SeekOrigin) origin))
    )
);
```

### 4.7 Forward Declarations
Circumvents circular dependencies without full header inclusion:
```c
FORWARD_OBJECT(Text, String);
/* Emits: typedef struct Text_TString Text_TString; */

FORWARD_TEMPLATE_OBJECT(Container, Vector, uint32);
/* Emits: typedef struct Container_TVector_uint32 Container_TVector_uint32; */

/* Note: FORWARD_ENUM is supported on C23 or compiler extensions (GNUC); standard C11 requires enum definitions */
#if defined(FORWARD_ENUM)
FORWARD_ENUM(IO, File, SeekOrigin);
#endif
```

---

## 5. Implementation Syntax (`.impl`)

To preserve native C compiler error reporting, line-number accuracy in debuggers (GDB/LLDB), and full `#ifdef` compatibility, implementation uses **Typed Lexical Scoping**.

Declarative body capsules with macro bodies (`BODY({ ... })`) are rejected because the ISO C standard (ISO/IEC 9899) strictly prohibits preprocessor directives (`#if`, `#ifdef`, `#pragma`) inside macro arguments and destroys compiler diagnostic line tracking.

### 5.1 Object Implementation Scoping (`OBJECT_SCOPE`)
`OBJECT_SCOPE` automatically binds receiver `self` (or `const self` for `CONST_*` methods):

```c
#pragma once

#include "../String.h"

OBJECT_SCOPE(Text, String)

OPERATION(Clear, ()) {
    if (!self)
        return STATUS(INVALID_ARGUMENT);

    self->size = 0;
    return STATUS(SUCCESS);
}

CONST_FUNCTION(size_t, Length, ()) {
    if (!self)
        return 0;
    return self->size;
}

CONST_COMPARISON(Compare, (const Text_TString *other)) {
    if (!self || !other)
        return COMPARISON_CONST(LOWER);
    if (self->size < other->size)
        return COMPARISON_CONST(LOWER);
    if (self->size > other->size)
        return COMPARISON_CONST(HIGHER);
    return COMPARISON_CONST(EQUAL);
}

OBJECT_SCOPE_END() /* Or: OBJECT_SCOPE(END) */
```

### 5.2 Package Implementation Scoping (`PACKAGE_SCOPE`)
`PACKAGE_SCOPE` implements static free functions without receiver `self`:

```c
#pragma once

#include "../Alloc.h"

PACKAGE_SCOPE(Memory, Alloc)

OPERATION(Bytes, (void *outPointerObject, size_t size)) {
    if (!outPointerObject)
        return STATUS(INVALID_ARGUMENT);

    if (size == 0U) {
        INS(WritePointer)(outPointerObject, NULL);
        return STATUS(SUCCESS);
    }

    void *allocated = malloc(size);
    if (!allocated)
        return STATUS(OUT_OF_MEMORY);

    INS(WritePointer)(outPointerObject, allocated);
    return STATUS(SUCCESS);
}

PACKAGE_SCOPE_END() /* Or: PACKAGE_SCOPE(END) */
```

### 5.3 Implementing `SPACE` and `VARIANT`
Sub-namespaces (`SPACE`) and overloads (`VARIANT`) are scoped cleanly inside their enclosing package or object:

```c
PACKAGE_SCOPE(Memory, Alloc)

/* Implementing VARIANT: Alloc, AllocAndFill, AllocZeroed */
VARIANT_SCOPE(Bytes)

OPERATION((void *outPointerObject, size_t size)) {
    /* Emits: Memory_Alloc_Bytes */
    return INS(AllocInternal)(outPointerObject, size, false, 0);
}

OPERATION(AndFill, (void *outPointerObject, size_t size, char ch)) {
    /* Emits: Memory_Alloc_BytesAndFill */
    return INS(AllocInternal)(outPointerObject, size, true, ch);
}

OPERATION(Zeroed, (void *outPointerObject, size_t size)) {
    /* Emits: Memory_Alloc_BytesZeroed */
    return INS(AllocInternal)(outPointerObject, size, true, 0);
}

VARIANT_SCOPE_END()

/* Implementing SPACE: Memory_Alloc_Transform_... */
SPACE_SCOPE(Transform)

PROCEDURE(AlignAddress, (void **inoutPtr, size_t alignment)) {
    /* Emits: Memory_Alloc_Transform_AlignAddress */
    uintptr_t raw = (uintptr_t)(*inoutPtr);
    *inoutPtr = (void *)((raw + alignment - 1) & ~(alignment - 1));
}

SPACE_SCOPE_END()

PACKAGE_SCOPE_END()
```

### 5.4 Contextual Identifiers inside Scope
Inside any scope, standardized contextual helpers are available:
- `INS(Name)`: Module/Package-scoped internal C identifier (`Memory_Alloc_Internal_Name`).
- `CINS(Name)`: Module/Package-scoped internal constant (`MEMORY_ALLOC_INTERNAL_NAME`).
- `CNS(Name)`: Public constant identifier (`MEMORY_ALLOC_NAME`).
- `TYPE(Name)`: Module-scoped type identifier.

---

## 6. FunctionTable Representation & Dispatch

Function tables provide uniform runtime dispatch and dynamic introspection while supporting compile-time elimination.

### 6.1 Layout of `SPACE` and `VARIANT` in `FunctionTable`
- **`VARIANT` (Flat Function Pointers):**
  Members are flattened directly into the function table struct:
  ```c
  struct Memory_Alloc_FunctionTable {
      OPSTATUS (*Bytes)(void *outPtr, size_t size);
      OPSTATUS (*BytesAndFill)(void *outPtr, size_t size, char ch);
      OPSTATUS (*BytesZeroed)(void *outPtr, size_t size);
  };
  ```
- **`SPACE` (Nested Sub-Structs):**
  Sub-namespaces are structured as nested structs within the parent table:
  ```c
  struct Memory_Alloc_FunctionTable {
      /* Base package members */
      OPSTATUS (*Bytes)(void *outPtr, size_t size);

      /* Nested SPACE sub-table */
      struct {
          void (*AlignAddress)(void **inoutPtr, size_t alignment);
      } Transform;
  };
  ```

### 6.2 Zero-Overhead Compile-Time Disabling & Granular Policies

Function table generation can be eliminated globally, per-category, or on a per-module basis:

1. **Global Configuration:** Configured via `LIBRARY_NAMESPACE_CONFIG(..., TABLE(MODE))` or `-DCOSMERON_TABLE=MODE` (`ENABLED`, `OBJECT_ONLY`, `PACKAGE_ONLY`, `DISABLED`).
2. **Per-Module Header Override:** A module can explicitly opt out of tables directly in its declaration:
   ```c
   MODULE(DOMAIN(Core), Math, TABLE(DISABLED),
       FUNCTION(int, Abs, (int x)),
       FUNCTION(int, Max, (int a, int b))
   );
   ```
3. **Per-Module Build Flag Override:** Selectively disable tables for individual modules from the build system:
   ```bash
   -DCOSMERON_TABLE_Math=DISABLED -DCOSMERON_TABLE_Bit=DISABLED
   ```
4. **Fine-Grained Macros:** Per-unit compile-time switches remain fully supported (`MODULE_DISABLE_FUNCTION_TABLE`, `PACKAGE_DISABLE_FUNCTION_TABLE`, `OBJECT_DISABLE_FUNCTION_TABLE`).

When tables are disabled for a target:
- No `FunctionTable` struct or static instance is emitted (zero code size impact).
- For `OBJECT`, the struct omits the `const FunctionTable *api` pointer completely.
- `MODULE_CALL(...)` resolves **directly to the native C function symbol**, guaranteeing zero runtime overhead and full compiler inlining.

---

## 7. Cross-Module Consumption Grammar

Code consuming other modules uses a unified, prefix-immune grammar:

```c
/* 1. Cross-Module Types (with automatic 'T' prefix for objects) */
MODULE_TYPE(Text, String) str;
MODULE_TYPED(Container, Vector, uint32) vec;

/* 2. Cross-Module Enums (strictly WITHOUT 'T' prefix) */
MODULE_ENUM(IO, File, SeekOrigin) origin = MODULE_CONST(File, SEEK_SET);

/* 3. Cross-Module Callbacks */
MODULE_CALLBACK(IO, StreamWatcher, Watcher, OnEvent) handler;

/* 4. Cross-Module Function Calls (Path is free-form, arguments enclosed in a tuple) */
OPSTATUS status = MODULE_CALL(Memory, Alloc, Bytes, (&ptr, 1024));

/* 5. Nested SPACE Function Calls (Unified: supports any depth, 2, 3, 4+ levels in one macro!) */
MODULE_CALL(Memory, Alloc, Transform, AlignAddress, (&ptr, 16));

/* 6. Universal Status Code Shortcut */
if (status != STATUS(SUCCESS)) {
    return STATUS(INVALID_ARGUMENT);
}

/* 7. Cross-Module Constants */
MODULE_CONST(Status, TIMEOUT);
```

---

## 8. Migration & Transition Checklist

1. [x] Preserve the canonical Cosmeron parameter order: `FUNCTION(RET, NAME, PARAMS)`.
2. [x] Standardize template instantiation naming to `TEMPLATE_DECLARE` and `TEMPLATE_IMPLEMENT`.
3. [x] Clarify `DOMAIN` vs `MODULE` hierarchy to guarantee zero symbol collisions under `STANDARD` mode.
4. [x] Eliminate macro bodies (`BODY({ ... })`) in favor of typed `OBJECT_SCOPE` and `PACKAGE_SCOPE`.
5. [x] Implement clean 2-arg `FUNCTION(RET, PARAMS)` overload syntax for `VARIANT`.
6. [x] Specify `.impl` scoping and `FunctionTable` nested struct layout for `SPACE` and `VARIANT`.
7. [x] Integrate first-class callbacks (`TCALLBACK`) and comparisons (`COMPARISON` / `COMPARISON_EX`).
8. [x] Add `MODULE_ENUM(...)`, `FORWARD_ENUM(...)`, and `CINS(...)`.
9. [x] Implement the core engine in `Codespace/Cosmeron/Core/Module/Module.h` and `Common/Declarative.impl`.
10. [x] Move Namespace engine to `Codespace/Cosmeron/Core/Module/Namespace.h` alongside Module infrastructure.
11. [x] Power `NS(...)` and `CNS(...)` via Binary Tree Reduction Engine (`PP_TREE_JOIN_UNDERSCORE`), retiring linear `GNS2..5` chaining.
12. [ ] Deprecate and remove all legacy `*.space` files across the codebase.
