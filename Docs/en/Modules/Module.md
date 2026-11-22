# Module

The **Module** core provides Cosmeron's declarative C API system. It generates namespaced types, enums, constants, structs, objects, packages, callables, delegates, templates and optional function tables from declarations in the headers.

## Headers

~~~c
#include "Cosmeron/Core/Module/Module.h"
~~~

The aggregate header includes the namespace, declaration, object, package, callable, delegate, dispatch, function-table, template and implementation-scope machinery used by the public module headers.

---

# Overview

## Declaration model

The declaration model is organized as:

| Construct | Purpose |
| --- | --- |
| `DOMAIN` | Separates broad library areas, such as `Core` and `Modules`. |
| `MODULE` | Owns a related family of declarations under a public namespace. |
| `PACKAGE` | Groups free functions that share a package name. |
| `OBJECT` | Declares a stateful type with methods that receive an implicit object receiver. |
| `STRUCT` | Declares a plain C data type with no implicit method receiver. |
| `ISTRUCT` | Declares a module-internal plain structure. |
| `VARIANT` | Groups related operations under a nested method/function path. |
| `CALLABLE` | Declares a stateless function-pointer callable signature. |
| `DELEGATE` | Declares a dual-mode callable signature that optionally carries a context pointer. |

A declaration is not a runtime class system. Objects and packages are generated C structs, functions and macros. There is no dynamic type discovery or automatic garbage collection supplied by the Module layer.

---

# API reference

## Namespace and public naming

### Namespace access

Header: `Namespace.h` (included by `Module.h`).

Use the semantic naming macros rather than constructing public identifiers manually:

| Macro | Purpose |
| --- | --- |
| `MODULE_TYPE(Module, Type)` | Names a non-templated public type. |
| `MODULE_TYPED(Module, Type, Suffix)` | Names a generated typed specialization. |
| `MODULE_ENUM(Module, Enum)` | Names a module enum. |
| `MODULE_CONST(Module, Constant)` | Names a module constant. |
| `MODULE_FUNC(Module, Function)` | Names a package or object function. |
| `MODULE_FUNC(Module, Package, Function)` | Names a function in a package. |
| `MODULE_DELEGATE(Module, Package, Name)` | Names a delegate type generated in a package. |
| `MODULE_CALLABLE(Module, Name)` | Names a callable type declared by a module. |
| `MODULE_NS(...)` | Forms a module namespace token sequence. |
| `NS(...)` / `CNS(...)` | Forms ordinary or constant namespace tokens. |

For example:

~~~c
MODULE_TYPE(Type, Traits) descriptor;
MODULE_TYPED(Numeric, Rational, Int32) fraction;
MODULE_ENUM(Algorithm, Comparison, Result) order;
~~~

The actual identifier is produced by the configured library prefix and namespace rules. Application code should prefer these access macros to hard-coded expanded names.

### Configuration

The namespace header defines the prefix and namespace policy macros used by the project. Configure them before including public module headers. Function-table policy is also a compile-time choice; a module, package or object may define a disable switch before its header is included.

Do not change namespace or function-table configuration between translation units that exchange the affected types. Doing so can produce incompatible names or object layouts.

Namespace configuration and public access macros are provided by `Core/Module/Namespace.h`.

---

## Packages

A package is the right construct for operations that do not need an object receiver.

### Declaration

~~~c
MODULE(DOMAIN(Modules), Example,
    PACKAGE(Math,
        FUNCTION(int, Add, (int left, int right)),
        OPERATION(Divide, (int left, int right, int *outValue))
    )
);
~~~

The package declaration generates namespaced functions and, when enabled, a typed function table. The function category determines its contract:

| Declaration | Return contract |
| --- | --- |
| `PROCEDURE` | No return value; used for side effects. |
| `FUNCTION` | Returns a value directly. |
| `PREDICATE` | Returns `bool`. |
| `OPERATION` | Returns `OPSTATUS` and may write output parameters. |
| `COMPARISON` | Returns the library's three-way comparison result. |

Use the narrowest applicable category and keep fallible work under `OPERATION`.

### Invocation

The canonical package function-table invocation macro is `PACKAGE_API`:

~~~c
int sum = PACKAGE_API(Example_Math, Add, 10, 20);
~~~

For a direct namespaced call that avoids the function table, use `MODULE_FUNC`:

~~~c
int sum = MODULE_FUNC(Example, Math, Add)(10, 20);
~~~

Package function tables are optional. If function tables are disabled, use the direct form rather than attempting package dispatch.

---

## Objects

An object combines a C struct with methods that receive a pointer to the instance as their first hidden receiver parameter.

### Declaration

~~~c
MODULE(DOMAIN(Modules), Counter,
    OBJECT(Counter,
        FIELDS(
            int value;
        ),
        PROCEDURE(Reset, ()),
        OPERATION(Add, (int amount)),
        CONST_FUNCTION(int, Value, ())
    )
);
~~~

The receiver is supplied by the generated object API. A method declared with `CONST_` is read-only with respect to its receiver contract.

### Invocation

The canonical object dispatch macro is `API`; `OBJECT_CALL` and `OBJECT_CALL_0` are compatibility aliases:

~~~c
MODULE_TYPE(Counter, Counter) counter = {0};

API(&counter, Reset);
OPSTATUS status = API(&counter, Add, 5);
int value = API(&counter, Value);
~~~

For a direct call without object function-table dispatch, use the generated namespaced method:

~~~c
MODULE_FUNC(Counter, Counter, Add)(&counter, 5);
~~~

Use `API` when the object has a valid function table and runtime dispatch is intended. Use direct `MODULE_FUNC` calls when the configuration removes function tables or when direct dispatch is desired. Never call a method on an uninitialized or invalid object.

### Inheritance and extensions

The current Object declaration layer includes `OBJECT_EXTENDS` and typed casts such as `OBJECT_AS`. These are compile-time C layout/declaration facilities; they do not provide virtual inheritance or automatic lifetime management. Use them only when the base/derived layout and receiver contract are explicit.

---

## Structs and internal data

Use `STRUCT` for a public POD data type and `ISTRUCT` for an implementation detail. A struct has no generated receiver and does not gain method dispatch merely by being declared in a module.

For fixed-layout value types, prefer direct field initialization and ordinary assignment when the type's ownership contract permits it. Types requiring custom initialization, cloning, movement or disposal should use the dedicated object or traits facilities rather than treating the value as trivially copyable.

---

## Callables and delegates

### Callable

A callable is a stateless function pointer with a declared signature. It does not inherently carry state.

`Callable.h` provides `TCALLABLE(Name)` for the generated type, `CALLABLE_FUNC(Name)` for helper names, `CALLABLE_CALL`/`CALLABLE_INVOKE` for invocation, `CALLABLE_IS_VALID` for checks, and `CALLABLE_EMPTY` (`NULL`) as the null callable value.

### Delegate

A delegate pairs a function pointer with an optional context pointer in a dual-mode POD structure. It supports both stateless callables (`FromCallable`) and stateful callbacks bound to caller-owned context (`Bind`) without a global variable.

`Delegate.h` provides `TDELEGATE(Name)` and `DELEGATE_FUNC(Name)`. The common call/validation macros are:

| Macro | Behavior |
| --- | --- |
| `DELEGATE_IS_VALID(delegate)` | Tests whether the delegate has a function. |
| `DELEGATE_CALL(delegate, ...)` | Invokes the delegate. |
| `DELEGATE_EMPTY` | Empty delegate value. |

Delegate declarations specify the return contract: `DELEGATE_FUNCTION`, `DELEGATE_PROCEDURE`, `DELEGATE_OPERATION`, `DELEGATE_PREDICATE` or `DELEGATE_COMPARISON`. A delegate's context pointer is borrowed; the Module layer does not manage the pointed-to object's lifetime.

---

## Templates

The canonical module template implementation uses a dual-mode `.def` file. A template can generate a struct, object or package specialization. It should be included from its public header and may be instantiated explicitly or from an X-macro type table.

### Instantiating a specialization

The common template helpers are:

| Macro | Purpose |
| --- | --- |
| `TEMPLATE_INSTANTIATE(Name, args...)` | Instantiates one named template. |
| `TEMPLATE_INSTANTIATE_TABLE(Name, Table)` | Instantiates a template for every row returned by the table. |
| `TEMPLATE_INSTANTIATE_TABLE(Module, Name, Table)` | Instantiates a module-qualified template for every row. |

~~~c
TEMPLATE_INSTANTIATE_TABLE(Numeric, Rational, NUMERIC_RATIONAL_TYPE_TABLE);
~~~

A table row is an argument tuple. Keep its row order identical to the corresponding template macro's parameter order.

The template engine also exposes lower-level declaration/implementation helpers, but public module headers should encapsulate those details. Consumers should use the module's documented template instantiation entry points rather than invoking internal implementation macros.

### Dual-mode .def convention

A template's `.def` file defines the template macro and supports direct instantiation when the expected configuration macros are defined. The public header normally includes shared internal definitions, declares its package/object and includes the template definition. Do not include internal `Impl` or `Common.inc` files directly from application code.

---

## Implementation and scopes

Non-inline implementations are emitted through explicit module implementation declarations. The current declaration system provides `MODULE_IMPLEMENT`, `OBJECT_IMPLEMENT_EX` and `PACKAGE_IMPLEMENT_EX`. Scope helpers such as `MODULE_SCOPE`, `OBJECT_SCOPE` and `PACKAGE_SCOPE` organize related implementations and end with their matching `*_SCOPE_END` macro.

A public header generally owns declarations and includes its implementation unit. Keep function bodies in the implementation files supplied by the module; consumers should include the public header only.

---

## Function tables

Function tables provide a typed dispatch path for packages and objects. They are optional compile-time output, not an independent virtual-method runtime.

- Use `PACKAGE_API` to dispatch package functions through a package table.
- Use `API` to dispatch object methods through the object's table (`OBJECT_CALL` is a compatibility alias).
- Use `MODULE_FUNC` for direct symbol invocation.
- Configure the relevant disable switches before including headers.
- Keep the same configuration across translation units sharing the declarations.

Function-table disabling may change generated object layout. Do not mix objects built with different table policies.

---

## Practical guidance

- Use `PACKAGE` for stateless operations and `OBJECT` only when state belongs to the instance.
- Use `STRUCT` for POD values; it has no receiver or object function table.
- Use `OPERATION` whenever a routine can fail and return an `OPSTATUS`.
- Use `FUNCTION` for direct value results and `PREDICATE` for Boolean questions.
- Use `CALLABLE` for plain stateless function pointers; use `DELEGATE` when optional context travels with the callable.
- Keep template details in `.def` files and expose a clean public header.
- Treat all pointers passed as context, delegate data or fields according to their documented ownership; the Module system does not automatically own them.

## Related modules

- [Algorithm](Algorithm.md) — practical delegate usage in repetition and comparison.
- [Condition](Condition.md) — statuses and fallible operation results.
- [Types](Types.md) — primitive and traits descriptors.
- [Preprocessor](Preprocessor.md) — token and table helpers used by the declarative layer.

## Source headers

- `Codespace/Cosmeron/Core/Module/Module.h`
- `Codespace/Cosmeron/Core/Module/Namespace.h`
- `Codespace/Cosmeron/Core/Module/Object.h`
- `Codespace/Cosmeron/Core/Module/Package.h`
- `Codespace/Cosmeron/Core/Module/Callable/Callable.h`
- `Codespace/Cosmeron/Core/Module/Delegate/Delegate.h`
- `Codespace/Cosmeron/Core/Module/Common/Template.impl`
- `Codespace/Cosmeron/Core/Module/Common/Declarative.impl`
