# Preprocessor

The **Preprocessor** module supplies utilities for C macro metaprogramming: argument counting, token concatenation, Boolean selection, repeated expansion, lists, token generation, and target detection.

It is **compile-time-only**. Unlike ordinary Cosmeron modules, these APIs have no callable C functions; the *Direct form* entries below therefore show literal expansions or clearly state that no direct C equivalent is defined.

```c
#include "Cosmeron/Core/Preprocessor/Arguments.h"
#include "Cosmeron/Core/Preprocessor/Boolean.h"
#include "Cosmeron/Core/Preprocessor/Operations.h"
#include "Cosmeron/Core/Preprocessor/Foreach.h"
```

---

# Overview

| Package | Header | Purpose |
| --- | --- | --- |
| [Operations](#token-operations) | `Operations.h` | Token expansion and concatenation |
| [Arguments](#argument-counting) | `Arguments.h` | Variadic argument counts |
| [Boolean](#boolean-operations) | `Boolean.h` | Conditional token selection |
| [Eval](#expansion-control) | `Eval.h` | Deferred recursive expansions |
| [Foreach](#foreach-and-map) | `Foreach.h` | Apply a macro to each argument |
| [Map](#pp_map) | `Map.h` | Foreach alias |
| [Repeat](#repetition) | `Repeat.h` | Index-based repetition and decrement |
| [Sequence](#sequence-selection) | `Sequence.h` | Select comma-separated arguments |
| [Tokens](#token-helpers) | `Tokens.h` | Emit punctuation tokens |
| [While](#conditional-repetition) | `While.h` | Repeated state transformation |
| [Detect](#platform-detection) | `Detect/*.h` | Compiler, OS and processor flags |
| [Compiling](#inclusion-diagnostics) | `Compiling.inc` | Optional include diagnostic messages |

### Macro form

```c
#define MODULE Network
PP_OP_CAT2(MODULE, _Init) /* Network_Init */
```

### Direct form

```c
Network_Init /* The literal resulting identifier. */
```

These are equivalent **as tokens**. They do not represent two public runtime functions.

### Implemented limits

| Family | Supported range |
| --- | --- |
| `PP_ARG_COUNT` | 1–100 nonempty arguments |
| `PP_FOREACH` / `PP_MAP` | 1–100 nonempty arguments |
| `PP_BOOL` | Literal integer tokens 0–100 |
| `PP_OP_CAT` and `PP_OP_CAT2`–`PP_OP_CAT10` | 2–10 pasted operands |
| `PP_REPEAT` | Counts 0–5 |
| `PP_DEC` | Integer inputs 0–5 |
| `PP_SEQ_GET_1`–`PP_SEQ_GET_10` | Indexes 1–10 |
| `PP_WHILE` | Constrained by fixed expansion depth and compiler limits |

---

# API reference

## Macro summary

| Macro or family | Description |
| --- | --- |
| [`PP_OP_EXPAND`](#pp_op_expand) | Normal argument expansion |
| [`PP_OP_CAT`](#pp_op_cat) | Primitive token paste |
| [`PP_OP_CAT2`](#pp_op_cat2) | Expanded two-token paste |
| [`PP_OP_CAT3`–`PP_OP_CAT10`](#pp_op_cat3-to-pp_op_cat10) | Multitoken concatenation |
| [`PP_ARG_COUNT`](#pp_arg_count) | Counts arguments |
| [`PP_BOOL`](#pp_bool) | Converts supported integer tokens to 0/1 |
| [`PP_BOOL_NOT`](#pp_bool_not) | Negates |
| [`PP_BOOL_IF`](#pp_bool_if) | Selects tokens conditionally |
| [`PP_BOOL_IF_ELSE`](#pp_bool_if_else) | Chooses one of two expansions |
| [`PP_BOOL_AND`](#pp_bool_and) | Logical AND |
| [`PP_BOOL_OR`](#pp_bool_or) | Logical OR |
| [`PP_BOOL_XOR`](#pp_bool_xor) | Logical XOR |
| [`PP_EMPTY`](#pp_empty) | Empty expansion |
| [`PP_DEFER`](#pp_defer) | Delays macro invocation |
| [`PP_OBSTRUCT`](#pp_obstruct) | Deferral barrier |
| [`PP_EVAL`](#pp_eval) | Repeated evaluation |
| [`PP_FOREACH`](#pp_foreach) | Applies macro to each item |
| [`PP_MAP`](#pp_map) | Foreach alias |
| [`PP_REPEAT`](#pp_repeat) | Repeats macro by index |
| [`PP_DEC`](#pp_dec) | Decrements limited integer token |
| [`PP_SEQ_FIRST`](#pp_seq_first) | Selects first |
| [`PP_SEQ_REST`](#pp_seq_rest) | Selects tail |
| [`PP_SEQ_GET_1`–`PP_SEQ_GET_10`](#pp_seq_get_1-to-pp_seq_get_10) | Indexes argument lists |
| [`PP_TOKEN_*`](#token-helpers) | Punctuation |
| [`PP_WHILE`](#pp_while) | Recursive conditional transform |
| [Compiler detection](#compiler-detection) | Compiler family |
| [Operating-system detection](#operating-system-detection) | Target OS |
| [Processor detection](#processor-detection) | Target architecture |
| [Include-message flag](#show_include_build_message) | Build messages |

---

# Token operations

Include `Cosmeron/Core/Preprocessor/Operations.h`. Token operations combine or expand **preprocessing tokens**, not C strings.

---

# PP_OP_EXPAND

Forwards a variadic token sequence through normal macro argument expansion.

### Syntax

#### Macro form

```c
PP_OP_EXPAND(...)
```

#### Direct form

The expanded argument tokens.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `...` | Any tokens to expand. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Defined as `#define PP_OP_EXPAND(...) __VA_ARGS__`.
- Does not act as an unlimited recursive expansion engine; use `PP_EVAL` for deferred patterns.

---

### Example

```c
#define VALUE 42
int value = PP_OP_EXPAND(VALUE); /* int value = 42; */
```

---

# PP_OP_CAT

Primitive two-token pasting using `##`, without pre-expansion of the adjacent arguments.

### Syntax

#### Macro form

```c
PP_OP_CAT(a, b)
```

#### Direct form

The literal pasted token, before subsequent rescanning.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `a` | First preprocessing token. |
| `b` | Second preprocessing token. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Arguments next to `##` are not expanded first.
- Use only when argument expansion must intentionally be suppressed.
- Pasting must produce a valid preprocessing token; otherwise the preprocessing behavior is invalid.

---

### Example

```c
#define ONE 1
#define NAME_1 7
/* ONE is pasted literally: */
int PP_OP_CAT(var_, ONE) = 3; /* int var_ONE = 3; */
```

---

# PP_OP_CAT2

Concatenates two operands after an additional macro-expansion layer.

### Syntax

#### Macro form

```c
PP_OP_CAT2(a, b)
```

#### Direct form

The pasted token after expanded arguments, subject to further normal rescanning.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `a` | First operand. |
| `b` | Second operand. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Calls `PP_OP_CAT` through a wrapper, so argument macros can expand first.
- Preferred for normal reusable namespace generation.
- Pasting still requires a valid token.

---

### Example

```c
#define PART Widget
int PP_OP_CAT2(PART, _Count) = 10;
/* int Widget_Count = 10; */
```

---

# PP_OP_CAT3 to PP_OP_CAT10

Concatenates between three and ten operands by composing the expanding concatenation primitives.

### Syntax

#### Macro form

```c
PP_OP_CAT3(a, b, c)
PP_OP_CAT4(a, b, c, d)
PP_OP_CAT5(a, b, c, d, e)
PP_OP_CAT6(a, b, c, d, e, f)
PP_OP_CAT7(a, b, c, d, e, f, g)
PP_OP_CAT8(a, b, c, d, e, f, g, h)
PP_OP_CAT9(a, b, c, d, e, f, g, h, i)
PP_OP_CAT10(a, b, c, d, e, f, g, h, i, j)
```

#### Direct form

The resulting literal preprocessing token (there is no corresponding exported C function).

---

### Parameters

Each argument must expand to a token that can validly be pasted to its neighbors; the number of required arguments is given by the macro suffix.

---

### Return value

One concatenated preprocessing token, possibly subsequently expanded.

---

### Remarks

- The implementation defines forms for exactly 3, 4, 5, 6, 7, 8, 9, and 10 operands.
- Module code should use these utilities rather than applying raw `##` outside the preprocessing layer.

---

### Example

```c
#define PREFIX Example
int PP_OP_CAT3(PREFIX, _, Field) = 1;
/* int Example_Field = 1; */
```

---

# Argument counting

Include `Cosmeron/Core/Preprocessor/Arguments.h`. The public interface is `PP_ARG_COUNT`; its `PP_ARG_RSEQ`, `PP_ARG_COUNT_IMPL`, and `PP_ARG_COUNT_SELECT` machinery implements the lookup.

---

# PP_ARG_COUNT

Counts a nonempty list of preprocessor macro arguments.

### Syntax

#### Macro form

```c
PP_ARG_COUNT(item1, item2, ...)
```

#### Direct form

An integer literal representing the number of items.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `...` | A comma-separated list of 1–100 items. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Supports up to 100 arguments.
- Empty input is **not** guaranteed to produce zero.
- Preprocessor comma and parenthesis grouping rules apply.

---

### Example

```c
enum { count = PP_ARG_COUNT(alpha, beta, gamma) };
/* count == 3 */
```

---

# Boolean operations

Include `Cosmeron/Core/Preprocessor/Boolean.h`. The Boolean domain is **integer tokens 0–100**: 0 is false and 1–100 are true. These macros do not evaluate arbitrary C expressions, negative values, or unbounded integer literals.

---

# PP_BOOL

Converts a supported integer token to 0 or 1.

### Syntax

#### Macro form

```c
PP_BOOL(x)
```

#### Direct form

0 if x is 0; 1 if x is 1–100.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `x` | Integer token 0–100. |

---

### Return value

The literal Boolean token 0 if x is 0; 1 if x is 1–100.

---

### Remarks

- Converts through a generated internal lookup table.

---

### Example

```c
int value = PP_BOOL(7); /* 1 */
```

---

# PP_BOOL_NOT

Negates a Boolean-convertible integer.

### Syntax

#### Macro form

```c
PP_BOOL_NOT(x)
```

#### Direct form

1 when x is 0, otherwise 0.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `x` | Integer token 0–100. |

---

### Return value

The literal Boolean token 1 when x is 0, otherwise 0.

---

### Remarks

- Normalizes with `PP_BOOL` before negating.

---

### Example

```c
enum { yes = PP_BOOL_NOT(0), no = PP_BOOL_NOT(2) };
```

---

# PP_BOOL_IF

Emits tokens only if its condition is true.

### Syntax

#### Macro form

```c
PP_BOOL_IF(condition)(tokens)
```

#### Direct form

Either the supplied token sequence or nothing.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `condition` | Integer token 0–100. |
| `tokens` | Tokens supplied by the second parenthesized invocation. |

---

### Return value

The literal Boolean token Either the supplied token sequence or nothing.

---

### Remarks

- This is a compile-time selection, not a C `if`.

---

### Example

```c
PP_BOOL_IF(1)(int enabled = 1;)
PP_BOOL_IF(0)(int disabled = 1;)
```

---

# PP_BOOL_IF_ELSE

Chooses one of two parenthesized token sequences.

### Syntax

#### Macro form

```c
PP_BOOL_IF_ELSE(condition)(trueTokens)(falseTokens)
```

#### Direct form

The chosen token sequence.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `condition` | Integer token 0–100. |
| `trueTokens` | Expansion used for true. |
| `falseTokens` | Expansion used for false. |

---

### Return value

The literal Boolean token The chosen token sequence.

---

### Remarks

- Both parenthesized branches must be supplied.

---

### Example

```c
enum { value = PP_BOOL_IF_ELSE(7)(11)(22) };
/* value == 11 */
```

---

# PP_BOOL_AND

Boolean conjunction.

### Syntax

#### Macro form

```c
PP_BOOL_AND(a, b)
```

#### Direct form

1 if both are nonzero, otherwise 0.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `a` | First supported integer token. |
| `b` | Second supported integer token. |

---

### Return value

The literal Boolean token 1 if both are nonzero, otherwise 0.

---

### Remarks

- Both operands are normalized to 0 or 1.

---

### Example

```c
enum { value = PP_BOOL_AND(2, 4) }; /* 1 */
```

---

# PP_BOOL_OR

Boolean inclusive disjunction.

### Syntax

#### Macro form

```c
PP_BOOL_OR(a, b)
```

#### Direct form

1 if either is nonzero, otherwise 0.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `a` | First supported integer token. |
| `b` | Second supported integer token. |

---

### Return value

The literal Boolean token 1 if either is nonzero, otherwise 0.

---

### Remarks

- Both operands are normalized.

---

### Example

```c
enum { value = PP_BOOL_OR(0, 9) }; /* 1 */
```

---

# PP_BOOL_XOR

Boolean exclusive disjunction.

### Syntax

#### Macro form

```c
PP_BOOL_XOR(a, b)
```

#### Direct form

1 if exactly one is nonzero, otherwise 0.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `a` | First supported integer token. |
| `b` | Second supported integer token. |

---

### Return value

The literal Boolean token 1 if exactly one is nonzero, otherwise 0.

---

### Remarks

- Both operands are normalized.

---

### Example

```c
enum { value = PP_BOOL_XOR(0, 3) }; /* 1 */
```

---

# Expansion control

Include `Cosmeron/Core/Preprocessor/Eval.h`. These helpers delay and rescan macro expansion. They implement **preprocessor recursion**, not loops or functions at runtime.

---

# PP_EMPTY

Emits no tokens.

### Syntax

#### Macro form

```c
PP_EMPTY()
```

#### Direct form

An empty token sequence.

---

### Parameters

None.

---

### Return value

No tokens.

---

### Remarks

- Used internally as a spacer for deferral.

---

### Example

```c
#define VALUE() 7
int number = PP_EMPTY() VALUE(); /* int number = 7; */
```

---

# PP_DEFER

Delays invocation of a function-like macro by inserting an empty-macro barrier.

### Syntax

#### Macro form

```c
PP_DEFER(id)(arguments)
```

#### Direct form

A deferred token sequence rather than an immediate function call.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `id` | Function-like macro identifier. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Defined as `id PP_EMPTY()`.
- Useful only with deliberate macro rescanning; its direct-looking syntax is not an exported C function.
- The number of rescans before invocation is context-sensitive.

---

### Example

```c
#define LATER() 5
#define REQUEST() PP_DEFER(LATER)()
/* REQUEST() uses an expansion barrier around LATER. */
```

---

# PP_OBSTRUCT

Adds a stronger delay that makes recursive indirect macro expansion possible.

### Syntax

#### Macro form

```c
PP_OBSTRUCT(tokens...)
```

#### Direct form

Supplied tokens with an additional deferred empty-macro barrier.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `tokens...` | Preprocessing tokens to obstruct. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Defined using `PP_DEFER(PP_EMPTY)()`.
- Usually combined with `PP_EVAL` and indirect recursion.
- Used by `PP_WHILE`.

---

### Example

```c
#define INDIRECT() PP_EMPTY
PP_OBSTRUCT(INDIRECT)() /* Example expansion barrier. */
```

---

# PP_EVAL

Rescans expanded input through a fixed cascade of internal expansion helpers.

### Syntax

#### Macro form

```c
PP_EVAL(tokens...)
```

#### Direct form

A rescanned token sequence after the fixed expansion cascade.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `tokens...` | Input tokens, potentially including deferred calls. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Implementation uses `PP_EVAL1`, `PP_EVAL2`, and `PP_EVAL3`.
- Expansion depth is finite and compiler-dependent.
- `PP_WHILE` already applies `PP_EVAL` internally.

---

### Example

```c
#define NUMBER 42
int result = PP_EVAL(NUMBER); /* 42 */
```

---

# Foreach and Map

`Foreach.h` generates a call to a unary macro for every argument. Its helpers cover `PP_FOREACH_1` through `PP_FOREACH_100`; they should usually be invoked via `PP_FOREACH` rather than directly.

---

# PP_FOREACH

Applies a unary macro once to each supplied item in order.

### Syntax

#### Macro form

```c
PP_FOREACH(macro, a, b, c, ...)
```

#### Direct form

The juxtaposed expansions `macro(a) macro(b) macro(c) ...`.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `macro` | A unary function-like macro. |
| `...` | A list of 1–100 items. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- No commas or separators are inserted automatically; the item macro must emit its own syntactic punctuation.
- Empty lists are not supported.
- Can generate declarations, expressions, or entries in X-macro-like code.

---

### Example

```c
#define DECL_FIELD(name) int name;
struct Sample { PP_FOREACH(DECL_FIELD, width, height, depth) };
#undef DECL_FIELD
```

---

# PP_MAP

Alias to `PP_FOREACH`, providing identical argument mapping.

### Syntax

#### Macro form

```c
PP_MAP(macro, a, b, ...)
```

#### Direct form

Exactly the tokens emitted by `PP_FOREACH(macro, a, b, ...)`.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `macro` | Unary macro applied to each item. |
| `...` | 1–100 list items. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Does not construct a collection or insert separators.
- Defined in `Map.h`, which includes `Foreach.h`.

---

### Example

```c
#define ADD(item) +(item)
enum { sum = 0 PP_MAP(ADD, 1, 2, 3) }; /* 6 */
#undef ADD
```

---

# Repetition

Include `Cosmeron/Core/Preprocessor/Repeat.h`. Repetition differs from foreach: it generates numerical indexes in **descending order** rather than using an explicit item list.

---

# PP_REPEAT

Invokes a unary macro for each integer index from `count` down through 1.

### Syntax

#### Macro form

```c
PP_REPEAT(count, macro)
```

#### Direct form

For count 3, `macro(3) macro(2) macro(1)`.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Integer token from 0 to 5. |
| `macro` | Macro receiving one generated index. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Count 0 expands to nothing.
- Indexes run downward: 3, 2, 1.
- No separators are inserted automatically.
- Counts beyond 5 are not defined.

---

### Example

```c
#define ADD_INDEX(index) +(index)
enum { sum = 0 PP_REPEAT(3, ADD_INDEX) }; /* 6 */
#undef ADD_INDEX
```

---

# PP_DEC

Decrements a small integer via fixed token mapping, saturating at zero.

### Syntax

#### Macro form

```c
PP_DEC(x)
```

#### Direct form

The literal result: 0→0, 1→0, 2→1, 3→2, 4→3, 5→4.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `x` | Integer token from 0 to 5. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Not a general arithmetic macro.
- Useful as the state transition for limited `PP_WHILE` examples.

---

### Example

```c
enum { n = PP_DEC(3), z = PP_DEC(0) }; /* n == 2; z == 0 */
```

---

# Sequence selection

`Sequence.h` handles ordinary **comma-separated preprocessor arguments**, not parenthesized Boost-style sequences.

---

# PP_SEQ_FIRST

Selects the first item from a macro argument list.

### Syntax

#### Macro form

```c
PP_SEQ_FIRST(first, ...)
```

#### Direct form

The first argument token sequence.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `first` | Selected argument. |
| `...` | Remaining arguments, discarded. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- Indexes are purely syntactic, with standard macro argument parsing.

---

### Example

```c
enum { n = PP_SEQ_FIRST(10, 20, 30) }; /* n == 10 */
```

---

# PP_SEQ_REST

Emits all arguments after the first, preserving their comma-separated form.

### Syntax

#### Macro form

```c
PP_SEQ_REST(first, ...)
```

#### Direct form

The remaining comma-separated arguments.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `first` | Discarded argument. |
| `...` | Remaining token arguments. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- The output must appear in a grammatical context that accepts a comma-separated list.

---

### Example

```c
int values[] = { PP_SEQ_REST(0, 10, 20, 30) };
/* values contains 10, 20, 30 */
```

---

# PP_SEQ_GET_1 to PP_SEQ_GET_10

Selects the indicated **one-based** position in a comma-separated argument list.

### Syntax

#### Macro form

```c
PP_SEQ_GET_1(a, ...)
PP_SEQ_GET_2(a, b, ...)
PP_SEQ_GET_3(a, b, c, ...)
/* ... */
PP_SEQ_GET_10(a, b, c, d, e, f, g, h, i, j, ...)
```

#### Direct form

The selected literal token sequence. These are macros, not C functions.

---

### Parameters

A comma-separated list containing at least as many arguments as the index. Unused later arguments are ignored.

---

### Return value

Emits the token sequence at the selected position.

---

### Remarks

- Supports selectors 1 through 10 only.
- Does not dynamically index a runtime array.
- A required missing argument is not handled by a runtime status check.

---

### Example

```c
enum { second = PP_SEQ_GET_2(10, 20, 30) };
enum { fourth = PP_SEQ_GET_4(10, 20, 30, 40) };
/* second == 20; fourth == 40 */
```

---

# Token helpers

`Tokens.h` offers the following function-like macros for emitting simple tokens:

| Macro form | Direct expansion |
| --- | --- |
| `PP_TOKEN_EMPTY()` | No tokens |
| `PP_TOKEN_COMMA()` | `,` |
| `PP_TOKEN_LPAREN()` | `(` |
| `PP_TOKEN_RPAREN()` | `)` |
| `PP_TOKEN_TILDE()` | `~` |

### Syntax

```c
PP_TOKEN_EMPTY()
PP_TOKEN_COMMA()
PP_TOKEN_LPAREN()
PP_TOKEN_RPAREN()
PP_TOKEN_TILDE()
```

### Parameters

None.

### Return value

The associated punctuation or empty token sequence. No runtime result.

### Remarks

- Useful when punctuation needs to appear after another macro-expansion phase.
- `PP_TOKEN_EMPTY` and `PP_EMPTY` are similar empty-expansion helpers from different headers.
- Delayed parentheses are sensitive to macro argument parsing; use `PP_TOKEN_LPAREN` and `PP_TOKEN_RPAREN` deliberately.

### Example

```c
#include "Cosmeron/Core/Preprocessor/Tokens.h"
int values[] = { 1 PP_TOKEN_COMMA() 2 PP_TOKEN_COMMA() 3 };
/* Equivalent: int values[] = { 1, 2, 3 }; */
```

---

# Conditional repetition

`While.h` combines `Boolean.h` and `Eval.h` to repeatedly transform an expandable state token until a predicate becomes false.

---

# PP_WHILE

Applies an operation while a supplied predicate evaluates to true.

### Syntax

#### Macro form

```c
PP_WHILE(pred, op, state)
```

#### Direct form

The final state token, reached when the predicate becomes false.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `pred` | Unary macro that converts state to a supported Boolean token. |
| `op` | Unary macro computing the next state. |
| `state` | Initial state token. |

---

### Return value

Not a C runtime value. The macro expands to preprocessing tokens.

---

### Remarks

- The operation must eventually cause the predicate to become false.
- Expansion is limited by `PP_EVAL` depth and compiler limits; this is not an unbounded runtime loop.
- The current `PP_DEC` accepts states only from 0 to 5.
- Internal helper `PP_WHILE_IMPL` and `PP_WHILE_INDIRECT` implement recursion.

---

### Example

```c
#define TEST(s) PP_BOOL(s)
#define STEP(s) PP_DEC(s)
enum { result = PP_WHILE(TEST, STEP, 3) };
/* result == 0 */
#undef TEST
#undef STEP
```

---

# Platform detection

The `Detect` headers expose 0/1 compile-time flags. An aggregate `*_KNOWN` expression is true when at least one implemented family is recognized. This is **target detection**, not runtime capability probing.

---

# Compiler detection

Header: `Cosmeron/Core/Preprocessor/Detect/Compiler.h`

| Flag | Recognized family |
| --- | --- |
| `COMPILER_INTEL` | Intel compiler macros |
| `COMPILER_CLANG` | Clang (unless Intel) |
| `COMPILER_MSVC` | MSVC (unless Intel or Clang) |
| `COMPILER_GCC` | GCC (unless preceding) |
| `COMPILER_KNOWN` | Boolean OR of the four families |

### Syntax

#### Macro form

```c
#if COMPILER_CLANG
/* Clang-specific implementation. */
#endif
```

#### Direct form

Compiler built-ins such as `__clang__` or `_MSC_VER` can be used directly, **but they are not interchangeable** with these normalized, exclusive family flags.

---

### Parameters

None.

### Return value

Numerical 0 or 1 for the family flags; a Boolean preprocessor expression for `COMPILER_KNOWN`.

### Remarks

- Intel is checked first, then Clang, MSVC and GCC; recognized families are mutually exclusive.
- clang-cl can define multiple compiler vendor macros, but is classified as Clang here.
- Other compilers may leave all four family flags false without making the build invalid.

### Example

```c
#include "Cosmeron/Core/Preprocessor/Detect/Compiler.h"
#if COMPILER_MSVC
/* MSVC branch */
#elif COMPILER_GCC
/* GCC branch */
#endif
```

---

# Operating-system detection

Header: `Cosmeron/Core/Preprocessor/Detect/OperationSystem.h`

| Flag | Condition |
| --- | --- |
| `OS_WINDOWS` | `_WIN32` or `_WIN64` |
| `OS_MAC` | Apple and Mach macros |
| `OS_LINUX` | Linux compiler macros |
| `OS_FREEBSD` | FreeBSD compiler macros |
| `OS_UNIX` | Linux, macOS, or FreeBSD |
| `OS_POSIX` | Linux, macOS, or FreeBSD |
| `OS_KNOWN` | Any of the four recognized OS families |

### Syntax

#### Macro form

```c
#if OS_WINDOWS
/* Windows-specific code */
#elif OS_POSIX
/* Recognized POSIX-style target */
#endif
```

#### Direct form

Platform vendor macros such as `_WIN32` or `__linux__` may be tested manually, but are not guaranteed to mean exactly the same as Cosmeron's normalized flags.

---

### Parameters

None.

### Return value

Numerical 0/1 flags and a Boolean aggregate expression.

### Remarks

- `OS_UNIX` and `OS_POSIX` currently recognize **only Linux, macOS and FreeBSD**, not every Unix-like system.
- On Windows the header defines `WIN32_LEAN_AND_MEAN` if it was not already defined.
- These macros do not discover the operating system at runtime.

### Example

```c
#include "Cosmeron/Core/Preprocessor/Detect/OperationSystem.h"
#if OS_LINUX
/* Linux */
#elif OS_MAC
/* macOS */
#elif OS_FREEBSD
/* FreeBSD */
#elif OS_WINDOWS
/* Windows */
#endif
```

---

# Processor detection

Header: `Cosmeron/Core/Preprocessor/Detect/Processor.h`

| Flag | Recognized target architecture |
| --- | --- |
| `PROCESSOR_X86` | 32-bit x86 |
| `PROCESSOR_X64` | x86-64 |
| `PROCESSOR_ARM` | 32-bit ARM |
| `PROCESSOR_ARM64` | AArch64 |
| `PROCESSOR_RISCV` | RISC-V |
| `PROCESSOR_KNOWN` | Boolean OR of those architectures |

### Syntax

#### Macro form

```c
#if PROCESSOR_ARM64
/* AArch64 branch */
#endif
```

#### Direct form

Compiler-supplied architecture macros can be queried, but the `PROCESSOR_*` form is the stable interface provided by the library.

---

### Parameters

None.

### Return value

0/1 numeric flags or an aggregate Boolean expression.

### Remarks

- Architecture detection concerns the compile target, not the actual machine running the binary.
- RISC-V is detected as a family without distinguishing 32-bit from 64-bit.
- These flags do not test architecture extensions such as AVX or NEON.

### Example

```c
#include "Cosmeron/Core/Preprocessor/Detect/Processor.h"
#if PROCESSOR_X64
/* x86-64 target */
#elif PROCESSOR_ARM64
/* ARM64 target */
#endif
```

---

# Inclusion diagnostics

The `Compiling.inc` file can emit an inclusion-time compiler message when `SHOW_INCLUDE_BUILD_MESSAGE` is defined.

---

# SHOW_INCLUDE_BUILD_MESSAGE

Enables a `#pragma message` diagnostic inside `Compiling.inc`.

### Syntax

#### Macro form

```c
#define SHOW_INCLUDE_BUILD_MESSAGE
#include "Cosmeron/Core/Preprocessor/Compiling.inc"
```

#### Direct form

There is no function equivalent; a compiler-specific `#pragma message` can be written manually.

---

### Parameters

None.

---

### Return value

No runtime return. Emits an optional compiler diagnostic when the compiler supports the pragma.

---

### Remarks

- The emitted message uses `__FILE__` where the pragma appears, i.e. the `Compiling.inc` location, not automatically the including header.
- Many headers include `Compiling.inc`, so output can repeat.
- The switch is tested with `#ifdef`, so defining it to 0 still activates it.
- Portability of `#pragma message` depends on the compiler.

---

### Example

```c
#define SHOW_INCLUDE_BUILD_MESSAGE
#include "Cosmeron/Core/Preprocessor/Operations.h"
/* May print [Compiling: ...Compiling.inc]. */
```

---

# Complete example

The example below combines concatenation, Boolean logic, and foreach-generated declarations:

```c
#include "Cosmeron/Core/Preprocessor/Operations.h"
#include "Cosmeron/Core/Preprocessor/Boolean.h"
#include "Cosmeron/Core/Preprocessor/Foreach.h"

#define PREFIX Example
#define FIELD(name) int name;

typedef struct {
    PP_FOREACH(FIELD, left, top, width, height)
} PP_OP_CAT2(PREFIX, _Rectangle);

enum {
    Example_Enabled = PP_BOOL_AND(3, 4)
};

#undef FIELD
#undef PREFIX
```

---

# Notes

- All operations execute during preprocessing; **no linking flags** or additional runtime library are required by these utilities.
- Macro token concatenation is not string concatenation. Invalid pastes generate preprocessing errors.
- `PP_BOOL` and related selection helpers are lookup-based; they cannot evaluate arbitrary arithmetic or logical expressions.
- `PP_FOREACH` processes a nonempty list; `PP_REPEAT` generates descending numeric indexes.
- Recursive expansion can be limited by the compiler and finite `PP_EVAL` implementation.
- The detection flags represent the families currently implemented, not every platform supported by C11.
- Prefer ordinary C constructs when metaprogramming has no concrete benefit. Follow `Docs/Reference/PATTERN.md` by using `PP_OP_CAT2` and related helpers instead of raw `##` in module code.
- There is **no** public `Preprocessor.h` aggregate header or runtime “direct form” API in this branch.
