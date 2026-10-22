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

## 3. Functions & Prototype Macros

- Every public or normal function has a prototype macro: `*_OPERATION_PROTOTYPE(...)`.
- The prototype macro is the **single source of truth** for the function signature.
- Re-use the prototype macro directly for definition and implementation:
  ```c
  ADDRESS_EQUAL_PROTOTYPE;
  ADDRESS_EQUAL_PROTOTYPE { /* ... */ }
  ```
- Use implementation macros (`*_OPERATION_IMPLEMENT`, `*_IMPLEMENT_ALL`) **only** when generating or specializing generic code across types. Never create trivial alias macros.
- Prototypes must remain readable: keep declaration shape, break long parameter lists, never collapse into unreadable one-line macros.
- Internal functions do not need separate prototype macros (`*_IMPLEMENT_INTERNAL(...)`).
- Function names: `*_FUNC(NAME)` or `*_TYPED_FUNC(...)`.

---

## 4. Structs & Function Tables

- Generated structures: `*_STRUCT(...)`.
- Function tables: `*_FUNCTION_TABLE_STRUCT(...)`, `*_FUNCTION_TABLE_INSTANCE(...)`.
- Use the term `FunctionTable`, never `VTable`.
- Every Function Table family must provide compile-time disabling: `*_DISABLE_FUNCTION_TABLE`. When defined, no struct, instance, or bindings may be emitted.

### Package & Object Implementation Convention
- **Prefer non-inline methods:** Do not define methods inline within `PACKAGE(...)` or `OBJECT(...)` macro invocations unless explicitly instructed.
- **Headers (`.h`):** Declare members using `FUNCTION(...)` for packages or `METHOD(...)` for objects. Keep the header declarative and clean.
- **Implementations (`.impl`):** Implement each method using the prototype macro:
  ```c
  PACKAGE_PROTOTYPEEX(PackageName, MethodName, (parameters...)) {
      /* implementation */
  }
  ```
  or for objects:
  ```c
  OBJECT_PROTOTYPEEX(ObjectName, MethodName, (parameters...)) {
      /* implementation */
  }
  ```
- **Finalizer call:** Always close `.impl` files with `PACKAGE_IMPLEMENT(PackageName);` or `OBJECT_IMPLEMENT(ObjectName);` to bind function tables and singleton accessors.

---

## 5. Enums & Constants

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
