# Cosmeron Next-Generation Namespace & Module Specification

## 1. Executive Summary

This document specifies the redesigned, unified namespace and declarative module architecture for the Cosmeron ecosystem. It replaces the legacy `.space` system with a zero-boilerplate, multi-tiered declarative engine supporting configurable symbol depth, custom library branding, decoupled X-Macro reflection, and unified implementation scoping.

---

## 2. Core Architectural Pillars

### 2.1 The Four Declarative Levels
The ecosystem is organized into four hierarchical levels:

```
DOMAIN (Architectural Grouping: Core, Container, Text, Graphics)
  └── MODULE (Unit of Compilation / Domain Boundary: Text, Memory, Bit)
        ├── PACKAGE (Static Collections of Free Functions: CString, Alloc)
        └── OBJECT (Stateful Instances with Receiver 'self' and FunctionTable: String)
              ├── SPACE (Nested Sub-Namespaces / Sub-Tables: Alloc.Create)
              └── VARIANT (Flat Overload Families: Alloc, AllocAndFill)
```

### 2.2 Complete Elimination of `.space` Files
- All legacy `.space` files are eliminated.
- The 11 repetitive manual macros (`*_MOD`, `*_CMOD`, `*_NS`, `*_CNS`, `*_INS`, `*_CINS`, `*_TYPE`, `*_FUNC`, `*_CONST`) previously duplicated in every header are obsolete and deleted.
- Shared structs and value objects move to formal `OBJECT(...)` declarations or dedicated `Types.h` headers.
- Enums and X-Macros are detached from namespace infrastructure and declared via `ENUM(...)`.

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
    CASE(HYBRID)        /* Constant casing: HYBRID, UPPER */
)
```

### 3.2 Compilation / CLI Configuration
Alternatively configured via build system flags:
```bash
-DCOSMERON_NAMESPACE=Mypotato -DCOSMERON_NAMESPACE_SHORT=MP -DCOSMERON_NAMESPACE_MODE=STANDARD
```

### 3.3 Default Fallbacks (Zero-Config)
If unconfigured, the engine defaults to:
- `NAME`: `Cosmeron`
- `SHORT`: `Cos`
- `MODE`: `STANDARD`
- `CASE`: `HYBRID`

### 3.4 Symbol Depth Modes

| Mode | Type Symbol Format | Function Symbol Format | Constant / Status Format | Ideal Use Case |
|---|---|---|---|---|
| `BARE` | `Text_TString` | `Text_String_Init` | `Status_SUCCESS` | Embedded, lightweight CLI tools, flat builds |
| `SHORT` | `MP_Text_TString` | `MP_Text_String_Init` | `MP_Status_SUCCESS` | Compact prefixing without namespace collision |
| `STANDARD` | `Mypotato_Text_TString` | `Mypotato_Text_String_Init` | `Mypotato_Status_SUCCESS` | Canonical production and engine integration |
| `FULL` | `Mypotato_Core_Text_TString` | `Mypotato_Core_Text_String_Init` | `Mypotato_Core_Status_SUCCESS` | Strict multi-vendor isolation, FFI bindings |

---

## 4. Header Declaration Syntax (`.h`)

### 4.1 Stateful Objects (`OBJECT` and `EXTENDS`)
- Objects represent types with state, fields, receiver `self`, and optional `FunctionTable`.
- The `T` prefix rule is applied **automatically** to the generated user-facing type (`Text_TString`), while methods and tables use the clean name (`Text_String_Init`, `Text_String_FunctionTable`).
- Inheritance is declared with `EXTENDS(BaseType)`.

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
        CONST_PREDICATE(IsEmpty, ())
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
- `SPACE`: Nested sub-namespace with nested `FunctionTable` sub-struct.
- `VARIANT`: Flat family of name-overloaded functions sharing a common prefix.

```c
MODULE(DOMAIN(Core), Text,
    PACKAGE(CString,
        FUNCTION(Length, size_t, (const char *str)),
        PREDICATE(IsEmpty, (const char *str)),

        /* VARIANT: Flat overload family (Alloc, AllocAndFill, AllocZeroed) */
        VARIANT(Alloc,
            FUNCTION(, void *, (size_t size)),
            FUNCTION(AndFill, void *, (size_t size, char ch)),
            FUNCTION(Zeroed, void *, (size_t size))
        ),

        /* SPACE: Nested sub-namespace with nested FunctionTable struct */
        SPACE(Transform,
            FUNCTION(ToUpper, void, (char *dest, const char *src)),
            FUNCTION(ToLower, void, (char *dest, const char *src))
        )
    )
);
```

### 4.3 Pure Package Modules (Direct Functions in `MODULE`)
When an entire module is a static collection of functions (e.g. `Math`, `Bit`), functions are placed directly in `MODULE(...)`, eliminating redundant `Math_Math_...` naming:

```c
MODULE(DOMAIN(Core), Math,
    FUNCTION(Abs, int, (int x)),
    FUNCTION(Sqrt, double, (double x)),
    FUNCTION(Min, int, (int a, int b)),
    FUNCTION(Max, int, (int a, int b))
);
```

### 4.4 Parameterized Templates (`TEMPLATE_OBJECT` and `TEMPLATE_PACKAGE`)
Templates define parameterized blueprints instantiated on demand:

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
   DECLARE_TEMPLATE(Vector, uint32, uint32_t);
   /* In implementation: */
   IMPLEMENT_TEMPLATE(Vector, uint32, uint32_t);
   ```
2. **Unified Mode (Declares types, prototypes, and bodies simultaneously):**
   ```c
   INSTANTIATE_TEMPLATE(Vector, uint32, uint32_t);
   ```
3. **Table-Driven Batch Instantiation:**
   ```c
   /* Instantiates all types listed in the X-Macro table in one line: */
   INSTANTIATE_TEMPLATE_TABLE(BitOps, BIT_WIDTH_TABLE);
   ```

### 4.5 Forward Declarations
Circumvents circular dependencies and avoids full header inclusion:
```c
FORWARD_OBJECT(Text, String);
/* Emits canonical: typedef struct Text_TString Text_TString; */
```

### 4.6 Enums and X-Macro Reflection
X-Macro tables live outside of modules (at the top of the file or in a dedicated `Common.h` / `DomainTable`). The module binds them using `ENUM(...)`:

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
        OPERATION(Seek, (int64_t offset, TYPE(SeekOrigin) origin))
    )
);
```

---

## 5. Implementation Syntax (`.impl`)

Developers have full flexibility between two authorized implementation patterns:

### Pattern A: Lexical Scope Model (`MODULE_SCOPE`)
Preserves native C syntax, exact compiler error line numbers, step-by-step GDB/LLDB debugging, and `#ifdef` compatibility:

```c
#pragma once

#include "../Alloc.h"

MODULE_SCOPE(Memory, Alloc)

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

OPERATION(ZeroBytes, (void *outPointerObject, size_t size)) {
    /* implementation */
}

MODULE_SCOPE_END()
```

### Pattern B: Declarative Body Capsule (`PACKAGE_IMPLEMENT`)
Visual symmetry enclosing function definitions inside a declarative block:

```c
MODULE(Memory,
    PACKAGE_IMPLEMENT(Alloc,
        OPERATION(Bytes, (void *outPointerObject, size_t size), BODY({
            if (!outPointerObject)
                return STATUS(INVALID_ARGUMENT);
            /* implementation */
            return STATUS(SUCCESS);
        })),

        OPERATION(ZeroBytes, (void *outPointerObject, size_t size), BODY({
            /* implementation */
        }))
    )
);
```

### Contextual Keywords inside Scope:
- `INS(Name)`: Internal/private C identifier (`Memory_Alloc_Internal_Name`).
- `CNS(Name)`: Module-scoped constant identifier.
- `TYPE(Name)`: Module-scoped type identifier (`Memory_TAlloc`).

---

## 6. Cross-Module Consumption Grammar

Code consuming other modules uses a unified, prefix-immune grammar:

```c
/* 1. Cross-Module Types */
MODULE_TYPE(Text, String) str;
MODULE_TYPED(Container, Vector, uint32) vec;

/* 2. Cross-Module Function Calls */
OPSTATUS status = MODULE_CALL(Memory, Alloc, Bytes, &ptr, 1024);

/* 3. Universal Status Code Shortcut */
if (status != STATUS(SUCCESS)) {
    return STATUS(INVALID_ARGUMENT);
}

/* 4. Cross-Module Constants */
MODULE_CONST(Status, TIMEOUT);
```

---

## 7. Migration & Transition Checklist

1. [ ] Create `Codespace/Cosmeron/Core/Module/Module.h` with the unified declarative engine.
2. [ ] Update `Codespace/Cosmeron/Core/Namespace.h` to support `LIBRARY_NAMESPACE_CONFIG` and variadic `NS_JOIN`.
3. [ ] Deprecate and remove all 50 `*.space` files across `Codespace/Cosmeron/Core` and `Codespace/Cosmeron/Modules`.
4. [ ] Standardize `FUNCTION` descriptor parameter order to `FUNCTION(Name, RetType, Params)`.
5. [ ] Integrate `VARIANT` and `SPACE` handling in `FunctionTable` generators.
6. [ ] Replace legacy `STATUS_CONST(...)` with the canonical `STATUS(...)` shortcut across modules.
7. [ ] Update `Docs/Reference/PATTERN.md` to reflect the next-generation specification.

