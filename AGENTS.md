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

### Namespace Architecture & Conventions
- **Complete Elimination of `.space` Files:** All legacy `.space` files are permanently deleted. Modules, packages, and objects must never include or define `.space` files.
- **Unified Namespaces (`NS_JOIN`, `MODULE_NS`, `MODULE_FUNC` in O(1)):**
  - **Prefer `MODULE_*` over `NS`**: Always prefer semantic macros (`MODULE_TYPE`, `MODULE_TYPED`, `MODULE_ENUM`, `MODULE_CONST`, `MODULE_FUNC`, `MODULE_NS`, `DOMAIN_NS`) over raw `NS(...)` for all public and consumer-facing code.
  - `NS_JOIN(...)` provides fast, flat, $O(1)$ single-substitution concatenation with `_`.
  - `MODULE_NS(Modulo, Pacote)` and `MODULE_FUNC(Modulo, Pacote, Func)` expand semantically in $O(1)$.
  - `DOMAIN_NS(Domain, ...)` expands domain-qualified namespaces in $O(1)$.
  - Numbered namespace macros (`GNS2`..`GNS5`, `CNS2`..`CNS5`) are obsolete and strictly forbidden.
- **Prohibition of Local Header Namespace Boilerplate:**
  - Never define manual header-level namespace macros such as:
    ```c
    #define FOO_NS(NAME)     ...
    #define FOO_CNS(NAME)    ...
    #define FOO_INS(NAME)    ...
    #define FOO_CINS(NAME)   ...
    #define FOO_TYPE(NAME)   ...
    #define FOO_FUNC(NAME)   ...
    #define FOO_CONST(NAME)  ...
    ```
  - Intermediate token macros (`*_MOD`, `*_CMOD`) are obsolete.
  - Use `NS(...)` and `CNS(...)` directly, or the canonical declarative `MODULE`, `MODULE_SCOPE`, `MODULE_CALL`, and `MODULE_TYPE` macros.
- **Canonical File Organization & Infrastructure Header (`Common.inc`):**
  - No synthetic `Types.h` files: types, enums, and tables belong directly in the header of their corresponding feature (e.g., `OPSTATUS` in `Outcome/Status.h`, `CMPOUT` in `Algorithm/Comparison.h`).
  - `Common.inc` is reserved strictly for internal shared infrastructure of a module (inline helpers, calculations, internal shared types like in `Memory/Common.inc`) to prevent duplicate implementations across packages. The `.inc` extension explicitly indicates it is an internal non-public include.
- Internal preprocessor macros: `COSMERON_MACRO_INTERNAL_*` (no namespace guards).
- **Strict Prohibition of Unnamespaced Internals:**
  - Absolutely no C identifier (structs, unions, typedefs, static inline helpers, internal functions, internal enums) may ever exist in the raw global namespace without namespace macros.
  - All internal structs, helpers, static inline functions, and private types **MUST** be wrapped with `NS(...)` or `MODULE_SCOPE`.
  - Callbacks and internal types must also live within the module namespace.

---

## 2. Types & `T` Prefix Rule

- Use `T` prefix **only** for types intended to be directly handled by the user (e.g., `TBit32`, `TVector_uint32`, `TText_Underline_Style`).
- `*_TYPE(SUFFIX)`: Semantic suffix (e.g., `BIT_TYPE(32)` $\to$ `TBit32`).
- `*_TYPED(TYPE)`: Specialization by another type (e.g., `VECTOR_TYPED(uint32)` $\to$ `TVector_uint32`).
- Output enums or internal types do **not** use `T` (e.g., `ComparisonResult`, aliased to `CMPOUT`).
- **Unambiguous naming policy:** Avoid overly generic names (such as `TPool`, `TNode`) that collide semantically across domains (e.g. Memory vs. Random vs. Concurrency). Always qualify the domain explicitly (e.g. `TMemoryPool`).
- **Strict alias policy:** Public aliases must be rare, deliberate, and highly specific. Do not create short ambiguous aliases.
- **Prohibition of Global Unnamespaced Typedefs and Forwarding Wrappers:**
  - Never emit global unnamespaced typedef shortcuts for types declared with `*_TYPE` (e.g. `typedef ERROR_TYPE(TError) TError;` is strictly prohibited).
  - Never generate unnamespaced inline forwarding wrappers (e.g. `static inline ... TError_Init(...) { return Error_TError_Init(...); }`).
  - Consumers must directly use the canonical namespaced types (`ERROR_TYPE(TError)` / `Error_TError`) and their emitted functions (`Error_TError_Init`) or declarative macros (`OBJECT_CALL`). Any exceptions require explicit user approval first.

---

## 3. Functions & Declarative Contracts

- Functions and methods belong to declarative modules (`PACKAGE` or `OBJECT`) and follow strict return contracts:
  - `OPERATION(NAME, PARAMS)`: Fallible operation returning `OPSTATUS`.
  - `PREDICATE(NAME, PARAMS)`: Proposition returning `bool`.
  - `FUNCTION(RET, NAME, PARAMS)`: Value-producing function returning `RET`.
  - `PROCEDURE(NAME, PARAMS)`: Infallible action returning `void`.
  - `COMPARISON(NAME, PARAMS)`: Three-way comparison returning `CMPOUT`.
  - Use `CONST_*` for const receiver methods (`const CLASS * self`).
  - Use `*_INLINE` when providing the implementation directly in the declaration block.
- **Implementations (`.impl` / `.c`):** Implement non-inline members using either the explicit or unified prototype macros:
  - **Explicit Family (Recommended - Zero Ambiguity & Zero Configuration):**
    ```c
    OBJECT_OPERATION_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* binds (Target *self, ...) */ }
    OBJECT_PREDICATE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    OBJECT_FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (parameters...)) { /* ... */ }
    OBJECT_PROCEDURE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    OBJECT_COMPARISON_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* binds (const Target *self, const Target *other) */ }

    PACKAGE_OPERATION_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* free function */ }
    PACKAGE_PREDICATE_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* ... */ }
    PACKAGE_FUNCTION_IMPLEMENT_EX(Package, ReturnType, FuncName, (parameters...)) { /* ... */ }
    PACKAGE_PROCEDURE_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* ... */ }
    PACKAGE_COMPARISON_IMPLEMENT_EX(Package, FuncName, (parameters...)) { /* ... */ }
    ```
  - **Unified Family (`*_IMPLEMENT_EX`):**
    ```c
    OPERATION_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    PREDICATE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (parameters...)) { /* ... */ }
    PROCEDURE_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    COMPARISON_IMPLEMENT_EX(Target, MethodName, (parameters...)) { /* ... */ }
    ```
    The dispatcher binds receiver `self` for objects if **any** of these traits hold:
    1. Direct type tag: `#define <Target>_IS_OBJECT 1` (e.g. `#define TCalculator_IS_OBJECT 1`)
    2. Namespaced tag: `#define COSMERON_TAG_OBJ_<Target> 1`
    3. File-wide override: `#define TARGET_IS_OBJECT 1`
    Otherwise, it automatically emits free functions for packages.
- **Deprecated aliases:** Never use legacy aliases (`METHOD`, `CONST_METHOD`, `OBJECT_PROTOTYPEEX`, `PACKAGE_PROTOTYPEEX`, `OBJECT_PROTOTYPE`) in new code; they exist solely for backward compatibility.
- **Implementation Scope:** Use `MODULE_SCOPE(MOD, Target)` and `MODULE_SCOPE_END()` to scope implementations cleanly without needing dummy finalizer calls.

---

## 4. Structs, Objects & Function Tables

- **Declarative Structs (POD Data Containers):** Use `STRUCT(NAME, FIELDS(...))` and `TEMPLATE_STRUCT(NAME, (ARGS...), FIELDS(...))`.
  - **Canonical Pure POD Rule (POD Puro Canônico é Regra Absoluta):** Structs are strictly Plain Old Data containers without methods, receiver `self`, `Create`, `Extract`, `Unpack`, `Cast_*`, or `FunctionTable` overhead.
  - Zero memory overhead: no `api` pointer, zero cache line pollution, exactly `sizeof(fields)`.
  - Fully C-ABI compliant: supports designated initializers (`.field = val`), direct register passing by value, `memcpy`, `memcmp`, and strict contiguity in arrays.
  - Generates canonical namespaced types: `MODULE_TYPE(MOD, NAME)` $\to$ `MOD_TNAME` and `MODULE_TYPED(MOD, NAME, SUFFIX)` $\to$ `MOD_TNAME_SUFFIX`.
  - Usable inside `MODULE(...)` or standalone via `STRUCT(MOD, NAME, FIELDS(...))`.
  - Forward declarations: `FORWARD_STRUCT(MOD, NAME)` and `FORWARD_TEMPLATE_STRUCT(MOD, NAME, SUFFIX)`.
  - Never generate artificial constructors, destructors, extraction helpers, or FunctionTables around POD structs. Manipulation and instantiation rely 100% on native C11 constructs (designated initializers and direct field access).
- **Declarative Internal Structs (`ISTRUCT` / `TEMPLATE_ISTRUCT`):** Use `ISTRUCT(NAME, FIELDS(...))` and `TEMPLATE_ISTRUCT(NAME, (ARGS...), FIELDS(...))`.
  - Intended for internal implementation structures (allocator nodes, radix levels, book-keeping headers) that do **not** use the user-facing `T` prefix.
  - Generates canonical namespaced internal types: `MODULE_ITYPE(MOD, NAME)` $\to$ `MOD_Internal_NAME` and `MODULE_ITYPED(MOD, NAME, SUFFIX)` $\to$ `MOD_Internal_NAME_SUFFIX`.
  - Usable inside `MODULE(...)` or standalone via `ISTRUCT(MOD, NAME, FIELDS(...))`.
  - Forward declarations: `FORWARD_ISTRUCT(MOD, NAME)` and `FORWARD_TEMPLATE_ISTRUCT(MOD, NAME, SUFFIX)`.
- **Declarative Objects:** Use `OBJECT(CLASS, FIELDS(...), ...)` and `OBJECT_EXTENDS(CLASS, BASE_CLASS, FIELDS(...), ...)`.
  - Members are declared with `OPERATION`, `PREDICATE`, `FUNCTION`, `PROCEDURE`, `COMPARISON`.
  - Comparison extensions generate specialized comparisons directly inside objects:
    - `COMPARISON_EX(TYPE, NAME, FIELD)`
    - Shortcuts: `COMPARISON_NUMERIC(NAME, FIELD)`, `COMPARISON_STRING(NAME, FIELD)`, `COMPARISON_BYTES(NAME, FIELD)`.
  - Declarative callbacks can be declared inside `OBJECT` or `PACKAGE` via the `CALLBACK(...)` capsule:
    - `CALLBACK(OPERATION(Name, (params)))`, `CALLBACK(PREDICATE(Name, (params)))`, etc.
  - Automatic `TCALLBACK(Comparison)` adapter generation for object methods returning `CMPOUT`:
    - Generates `<CLASS>_<Method>_ComparatorAdapter(const void *left, const void *right, void *context)` and `<CLASS>_<Method>_AsComparator()` which produces a ready-to-use `TCALLBACK(Comparison)`.
  - Member fields are declared cleanly within `FIELDS(...)`.
  - Casting: `OBJECT_AS(OBJ, TARGET_TYPE)`.
  - Invocation: `OBJECT_CALL(obj, MethodName, args...)` or `OBJECT_CALL_0(obj, MethodName)`.
- **Declarative Callbacks System:**
  - Standalone declarations use category descriptors:
    - `CALLBACK_FUNCTION(RET, NAME, (params))`
    - `CALLBACK_PROCEDURE(NAME, (params))`
    - `CALLBACK_OPERATION(NAME, (params))`
    - `CALLBACK_PREDICATE(NAME, (params))`
    - `CALLBACK_COMPARISON(NAME, (params))`
  - Universal invocation: `CALLBACK_CALL(callback, args...)` or `CALLBACK_INVOKE(callback, args...)` (supports arbitrary arity).
  - Validity checking: `CALLBACK_IS_VALID(callback)` or `callback.IsValid(&callback)`.
  - Binding: `callback.Bind(&callback, func, ctx)` or `CALLBACK_FUNC(NAME, Bind)(func, ctx)`.
- **Declarative Packages:** Use `PACKAGE(PKG, ...)`.
  - Invocation: `PACKAGE_CALL(PKG, FuncName, args...)` or direct calls `PKG_FuncName(...)`.
- **Declarative Internal Scopes (`INTERNAL` / `INTERNAL_EX`):** Use `INTERNAL(...)` or `INTERNAL_EX(PKG, ...)`.
  - Intended for internal helpers, non-public routines, and module infrastructure.
  - Zero FunctionTable overhead: pure `static inline` functions with no vtables or function pointers.
  - Generates canonical namespaced internal identifiers:
    - Module-level `INTERNAL(...)` $\to$ `MOD_Internal_FuncName` (invoked via `MODULE_IFUNC(MOD, FuncName)` or `MODULE_ICALL(MOD, FuncName, (args...))`).
    - Component-level `INTERNAL_EX(PKG, ...)` or `INTERNAL(PKG, ...)` $\to$ `MOD_PKG_Internal_FuncName` (invoked via `MODULE_IFUNC(MOD, PKG, FuncName)` or `MODULE_ICALL(MOD, PKG, FuncName, (args...))`).
  - Usable inside `MODULE(...)` for declarations in headers/infrastructure (`.inc`), inside `MODULE_IMPLEMENT(...)` for implementations in `.impl`, or standalone via `INTERNAL_DEF` / `INTERNAL_IMPLEMENT`.
- **Declarative Events & Reactive Mutation:**
  - Events are declared natively inside `OBJECT` or `PACKAGE` with zero heap allocation:
    - `EVENT_PROCEDURE(Name, ((Type, argName), ...))` (infallible, `void`)
    - `EVENT_OPERATION(Name, ((Type, argName), ...))` (fallible, `OPSTATUS`, short-circuits on first error)
    - Optional explicit capacity: `EVENT_PROCEDURE(Name, ((Type, argName)), 8)` (decays to 4, 8, 12, or 16 slots aligned with L1 cache line).
    - Nullary events: `EVENT_PROCEDURE(Reset, ())`.
  - API automatically emitted per event: `Subscribe`, `Unsubscribe` (FIFO shift removal by `function` + `context`), `Emit` (re-entrancy safe via stack snapshot), `Clear`, `Count`, `Capacity`.
  - Reactive mutation helper: `OBJECT_MUTATE(self, ClassName, fieldName, newValue, EventName)` evaluates `self->fieldName != (newValue)`, updates the field, and emits the event.
- **Compile-time disabling:** Every Function Table family provides compile-time disabling via `MODULE_DISABLE_FUNCTION_TABLE`, `PACKAGE_DISABLE_FUNCTION_TABLE`, or `OBJECT_DISABLE_FUNCTION_TABLE`. When defined, no struct, instance, or table is emitted.
- **Naming:** Always use the term `FunctionTable`, never `VTable`.
- **Universal Template Engine (`TEMPLATE_OBJECT` / `TEMPLATE_PACKAGE` / `TEMPLATE_STRUCT`):**
  - Declare parameterized templates directly inside `MODULE(...)` via `TEMPLATE_OBJECT(Name, (SUFFIX, TYPE), ...)`, `TEMPLATE_PACKAGE(Name, (SUFFIX, TYPE), ...)`, or `TEMPLATE_STRUCT(Name, (SUFFIX, TYPE), FIELDS(...))`.
  - **Zero Blueprint Macros & Zero Workaround Hacks:** Never create `#define TEMPLATE_<Name>(...)` macros duplicating member lists, multi-line macros with `\`, or include-file webs to simulate templates. The template is declared once, directly inside `MODULE(...)`.
  - **DSL Extension Protocol:** If a desired feature (e.g. structured packing/unpacking, batch operations, serialization) does not fit cleanly into the existing declarative DSL, **NEVER** improvise workaround hacks. **STOP IMMEDIATELY**, propose a clean extension to the declarative DSL engine (`MODULE(...)`, `Declarative.impl`), consult the user, and wait for approval.
  - Instantiate on-demand with `TEMPLATE_INSTANTIATE(Name, Suffix, Type)`.
  - Batch instantiate with X-Macros via `TEMPLATE_INSTANTIATE_TABLE(Name, TYPE_TABLE)`.
  - Type naming convention for user types: `T<Name>_<suffix>` (e.g. `TStack_int`, `TVector_uint32`).
  - Prohibit ad-hoc monolithic instantiation macros (e.g., `VECTOR_IMPLEMENT_ALL`).

---

## 5. Enums & Constants

- **Mandatory X-Macro / Table Pattern:** Every enum must be implemented via an X-Macro table (`<DOMAIN>_TABLE(X)`). Directly hardcoding raw lists inside `enum { ... }` is strictly prohibited. The table macro is the single source of truth for generating enum values, constants, descriptions, and string tables.
- **Declarative Enums (`ENUM` / `ENUM_EX`):**
  - Use `ENUM(Name, TABLE)` or `ENUM_EX(Name, TABLE, DISPATCH)` natively inside `MODULE(...)`, `PACKAGE(...)`, or `OBJECT(...)`.
  - For standalone declarations outside `MODULE(...)`, use `ENUM_DEF(TARGET, Name, TABLE)` or `ENUM_DEF_EX(TARGET, Name, TABLE, DISPATCH)`.
  - Zero FunctionTable overhead: enums declared inside `PACKAGE` or `OBJECT` do not emit fields or entries into `FunctionTable`.
  - Generates canonical namespaced enum types: `MODULE_ENUM(...)` / `NS(TARGET, Name)`.
  - **Configurable Dispatch Presets:**
    - `PLAIN` (or `NAME_VALUE`, default for `ENUM(Name, TABLE)`): emits `CNS(TARGET, NAME) = VALUE,` (or `CNS(TARGET, NAME),` if 1 column).
    - `QUALIFIED`: emits `CNS(TARGET, ENUM_NAME, NAME) = VALUE,` (prefixed by enum name to guarantee zero collisions across multiple enums in the same target).
    - `NAME_ONLY`: emits `CNS(TARGET, NAME),` without value assignment (allows C to auto-increment from 0).
    - `QUALIFIED_NAME`: emits `CNS(TARGET, ENUM_NAME, NAME),` without value assignment.
    - Custom dispatch macro: user passes `DISPATCH(TARGET, ENUM_NAME, ROW)` for specialized row transformations (e.g., bitshifts `1U << VALUE`, custom prefixes).
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

- Use when one semantic table generates multiple representations (enum, string tables, switch cases, mappings).
- The table macro (`SNAKE_CASE`) is the single source of truth; never duplicate lists.
- X-macros are code generation tools, not public runtime API.
- **Decoupled Column Selectors (Row-Tuple Pattern):**
  - Always pack rows as parenthesized tuples: `X((COL1, COL2, ...))`.
  - Define column selectors at the table definition: `#define DOMAIN_COL_<FIELD>(PARAMS...) <FIELD>`.
  - Unpack fields in consumers using `DOMAIN_GET(FIELD, ROW)` via `PP_OP_CAT2(DOMAIN_COL_, FIELD) ROW`.
  - Adding or rearranging columns must never break existing consumer macros.
- **Categorical Modular Aggregation:**
  - Multi-category tables are split into `.impl` files (e.g. `DomainTable/Generic.impl`, `DomainTable/Arithmetic.impl`) defining `DOMAIN_TABLE_<CATEGORY>(X)`.
  - A master aggregator header `DomainTable/DomainTable.h` unifies them into `DOMAIN_TABLE(X)`.

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
