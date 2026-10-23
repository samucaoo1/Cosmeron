# Cosmeron Architectural & Coding Guidelines

Based on [Docs/Reference/PATTERN.md](Docs/Reference/PATTERN.md).

> **Core Philosophy:** Prefer native C constructs over preprocessor macros. Use macros only for naming, code generation, internal preprocessing, or useful syntactic sugar. Magic numbers are strictly prohibited.

---

## 1. Naming & Namespaces

Everything must live inside the library/module namespace whenever C allows it. Exceptions exist only for public sugar macros (which are strictly exceptional; consult user first), `OPSTATUS`, and `*OUT` aliases.

### Case Conventions
| Element | Case | Example |
|---|---|---|
| Module / namespace token | `PascalCase` | `Memory`, `Comparison` |
| Type | `PascalCase` | `TVector`, `ComparisonResult` |
| Function | `PascalCase` | `Memory_Copy`, `Vector_Push` |
| Constant / enum value | `SNAKE_CASE` | `SUCCESS`, `OUT_OF_RANGE` |
| Preprocessor macro | `SNAKE_CASE` | `VECTOR_IMPLEMENT_ALL` |
| Internal namespace token | `PascalCase` / `SNAKE_CASE` | `Internal` / `INTERNAL` |
| Parameter / local / field | `camelCase` | `outValue`, `leftSize`, `bufferCapacity` |

### Namespace Macro Conventions
- `*_MOD` (`PascalCase`) names the module namespace.
- `*_CMOD` (`SNAKE_CASE`) names the constant namespace.
- `*_NS(NAME)`: Public C identifiers.
- `*_CNS(NAME)`: Public constants.
- `*_INS(NAME)`: Internal C identifiers.
- `*_CINS(NAME)`: Internal constants.
- Internal preprocessor macros: `COSMERON_MACRO_INTERNAL_*` (no namespace guards).

---

## 2. Types & `T` Prefix Rule

- Use `T` prefix **only** for types intended to be directly handled by the user (e.g., `TBit32`, `TVector_uint32`, `TText_Underline_Style`).
- `*_TYPE(SUFFIX)`: Semantic suffix (e.g., `BIT_TYPE(32)` $\to$ `TBit32`).
- `*_TYPED(TYPE)`: Specialization by another type (e.g., `VECTOR_TYPED(uint32)` $\to$ `TVector_uint32`).
- Output enums or internal types do **not** use `T` (e.g., `ComparisonResult`, aliased to `CMPOUT`).
- **Unambiguous naming policy:** Avoid overly generic names (such as `TPool`, `TNode`) that collide semantically across domains (e.g. Memory vs. Random vs. Concurrency). Always qualify the domain explicitly (e.g. `TMemoryPool`).
- **Strict alias policy:** Public aliases must be rare, deliberate, and highly specific. Do not create short ambiguous aliases.

---

## 3. Functions & Declarative Contracts

- Functions and methods belong to declarative modules (`PACKAGE` or `OBJECT`) and follow strict return contracts:
  - `OPERATION(NAME, PARAMS)`: Fallible operation returning `OPSTATUS`.
  - `PREDICATE(NAME, PARAMS)`: Proposition returning `bool`.
  - `FUNCTION(RET, NAME, PARAMS)`: Value-producing function returning `RET`.
  - `PROCEDURE(NAME, PARAMS)`: Infallible action returning `void`.
  - Use `CONST_*` for const receiver methods (`const CLASS * self`).
  - Use `*_INLINE` when providing the implementation directly in the declaration block.
- **Implementations (`.impl` / `.c`):** Implement non-inline members using either the explicit or unified prototype macros:
  - **Explicit Family (Recommended - Zero Ambiguity & Zero Configuration):**
    ```c
    OBJECT_OPERATION_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* binds (Target *self, ...) */ }
    OBJECT_PREDICATE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    OBJECT_FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (parameters...)) { /* ... */ }
    OBJECT_PROCEDURE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }

    PACKAGE_OPERATION_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* free function */ }
    PACKAGE_PREDICATE_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* ... */ }
    PACKAGE_FUNCTION_IMPLEMENT_EX(Package, ReturnType, FuncName, (parameters...)) { /* ... */ }
    PACKAGE_PROCEDURE_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* ... */ }
    ```
  - **Unified Family (`*_IMPLEMENT_EX`):**
    ```c
    OPERATION_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    PREDICATE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (parameters...)) { /* ... */ }
    PROCEDURE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    ```
    The dispatcher binds receiver `self` for objects if **any** of these traits hold:
    1. Direct type tag: `#define <Target>_IS_OBJECT 1` (e.g. `#define TCalculator_IS_OBJECT 1`)
    2. Namespaced tag: `#define COSMERON_TAG_OBJ_<Target> 1`
    3. File-wide override: `#define TARGET_IS_OBJECT 1`
    Otherwise, it automatically emits free functions for packages.
- **Deprecated aliases:** Never use legacy aliases (`METHOD`, `CONST_METHOD`, `OBJECT_PROTOTYPEEX`, `PACKAGE_PROTOTYPEEX`, `OBJECT_PROTOTYPE`) in new code; they exist solely for backward compatibility.
- **Finalizer call:** Always close `.impl` files with `PACKAGE_IMPLEMENT(PackageName);` or `OBJECT_IMPLEMENT(ObjectName);`.

---

## 4. Structs, Objects & Function Tables

- **Declarative Objects:** Use `OBJECT(CLASS, FIELDS(...), ...)` and `OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS(...), ...)`.
  - Members are declared with `OPERATION`, `PREDICATE`, `FUNCTION`, `PROCEDURE`.
  - Member fields are declared cleanly within `FIELDS(...)`.
  - Casting: `OBJECT_AS(OBJ, TARGET_TYPE)`.
  - Invocation: `OBJECT_CALL(obj, MethodName, args...)` or `OBJECT_CALL_0(obj, MethodName)`.
- **Declarative Packages:** Use `PACKAGE(PKG, ...)`.
  - Invocation: `PACKAGE_CALL(PKG, FuncName, args...)` or direct calls `PKG_FuncName(...)`.
- **Compile-time disabling:** Every Function Table family provides compile-time disabling via `MODULE_DISABLE_FUNCTION_TABLE`, `PACKAGE_DISABLE_FUNCTION_TABLE`, or `OBJECT_DISABLE_FUNCTION_TABLE`. When defined, no struct, instance, or table is emitted.
- **Naming:** Always use the term `FunctionTable`, never `VTable`.

---

## 5. Enums & Constants

- **Mandatory X-Macro / Table Pattern:** Every enum must be implemented via an X-Macro table (`<DOMAIN>_TABLE(X)`). Directly hardcoding raw lists inside `enum { ... }` is strictly prohibited. The table macro is the single source of truth for generating enum values, constants, descriptions, and string tables.
- Enums exist for named constant domains and function signatures. Never use raw integers for categorical states.
- Output enums: use short `*OUT` alias in signatures (e.g., `CMPOUT`).
- Public constants: `*_CONST(NAME)` (e.g., `STATUS_CONST(SUCCESS)`).
- **Prohibit magic numbers**: Always use named constants unless defining raw representations (bit flags, protocols, ABI, file format definitions).

---

## 6. Preprocessor Policy

- `#define` must be rare. Avoid macros that merely replace normal C syntax.
- **Sugar macros are an exception, not the rule:** Never proactively generate public sugar macros for functions or types. Always ask the user first if creating them is desired.
- Preprocessor operations must use helpers from `Core/Preprocessor` (e.g. `PP_OP_CAT2`, `PP_OP_CAT3`).
- Raw `##` operator is forbidden outside `Core/Preprocessor`.

---

## 7. X-Macros

- Use when one semantic table generates multiple representations (enum, string tables, switch cases).
- The table macro (`SNAKE_CASE`) is the single source of truth; never duplicate lists.
- X-macros are code generation tools, not public runtime API.

---

## 8. Function Contracts & Error Handling

Function contracts follow their result:
| Function kind | Return | Output |
|---|---|---|
| Infallible natural result | value directly | — |
| Predicate / proposition | `bool` | — |
| Fallible operation without primary result | `OPSTATUS` | — |
| Fallible operation with primary result | `OPSTATUS` | `out` parameter |
| Infallible mutation with no result | `void` | mutates target |

- `bool` answers a proposition; `OPSTATUS` reports operational outcome.
- `out` parameters:
  - Required `out == NULL` $\to$ return `INVALID_ARGUMENT`.
  - Modified **only** on `SUCCESS`, unless explicitly documented otherwise.
- Recoverable failures return `OPSTATUS`. Contract violations / impossible invariants may `PANIC`.
- Pointers are borrowed by default; use `const` whenever target data is not logically modified.
- Never hide failures in sentinel values or implicit `LastError` state.

---

## 9. Platform & OS Detection

- Any OS-conditional logic or platform checks must include and use `Codespace/Cosmeron/Core/Preprocessor/Detect/OperationSystem.h`.
- Use normalized platform macros (`OS_WINDOWS`, `OS_LINUX`, `OS_MAC`, `OS_BSD`, `OS_UNIX`, `OS_POSIX`, `OS_OHOS`, etc.).
- Do not perform ad-hoc or duplicated checks directly against native compiler macros (e.g. `#ifdef _WIN32`, `__linux__`) outside of Core detection infrastructure.

---

## 10. Privacy, Security & Public Repository Invariants

- The repository is **public**:
  - Never expose local absolute machine paths (e.g., disk mounts, home directories, or usernames).
  - Never include direct `file://` links containing local paths in versioned files; always use relative repository paths.
  - Never commit credentials, personal data, or developer-specific local environment identifiers.

---

## 11. Language

- The standard language of the library is **English**.
- All code identifiers, comments, commit messages, documentation, and technical guidelines must be written in English.
