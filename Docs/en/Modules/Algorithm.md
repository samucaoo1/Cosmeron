# Algorithm

The **Algorithm** module supplies fundamental algorithmic building blocks for comparison, iteration, repetition, and data arrangement.

The packages in this module are:
- [Comparison](#comparison): Consistent three-way comparison interface for numerical values, strings, byte sequences, and custom comparators.
- [Repeat](#repeat): Repetition of actions, fallible operations, predicates, memory patterns, primitive values, and typed element generation.

```c
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Algorithm/Repeat.h"
```

---

# Overview

## Comparison

All comparison functions return `CMPOUT`.

| Constant | Value | Meaning |
| --- | ---: | --- |
| `COMPARISON_CONST(LOWER)` | -1 | Left precedes right |
| `COMPARISON_CONST(EQUAL)` | 0 | Both compare equally |
| `COMPARISON_CONST(HIGHER)` | 1 | Left follows right |

```c
MODULE_FUNC(Algorithm, Comparison, Int)(3, 7);
```

## Repeat

The Repeat package provides counted repetition over typed closures (`MODULE_CLOSURE`), byte buffers, primitive arrays, and custom types via universal templates (`TEMPLATE_REPEAT`).

Two equivalent call forms are supported:

### Direct form

```c
Algorithm_Repeat_Procedure(5, action);
Algorithm_Repeat_Int(array, 42, 10);
```

### Declarative form

```c
MODULE_CALL(Algorithm, Repeat, Procedure, (5, action));
MODULE_CALL(Algorithm, Repeat, Int, (array, 42, 10));
```

---

# API reference

## Function summary

### Comparison

| Function | Description | Modifies input |
| --- | --- | --- |
| [`Int`](#int) | Compares two `int` values | No |
| [`Float`](#float) | Compares two `float` values | No |
| [`Double`](#double) | Compares two `double` values | No |
| [`LongDouble`](#longdouble) | Compares two `long double` values | No |
| [`CString`](#cstring) | Compares two `const char *` values | No |
| [`Bytes`](#bytes) | Lexicographic comparison of sized byte buffers | No |

### Repeat

| Function | Description | Modifies input |
| --- | --- | --- |
| [`Procedure`](#procedure) | Repeats a nullary procedure `count` times | No |
| [`ProcedureIndexed`](#procedureindexed) | Repeats a procedure with index `(0 .. count-1)` | No |
| [`Operation`](#operation) | Repeats a fallible operation with short-circuit on error | No |
| [`OperationIndexed`](#operationindexed) | Repeats a fallible operation with index and short-circuit | No |
| [`Predicate`](#predicate) | Repeats a predicate while returning `true` | No |
| [`PredicateIndexed`](#predicateindexed) | Repeats a predicate with index while returning `true` | No |
| [`RepeatBytes`](#repeatbytes) | Repeats a byte pattern `count` times into destination | Yes |
| [`RepeatPrimitives`](#repeatprimitives) | Prefills a primitive array with a value `count` times | Yes |
| [`RepeatCString`](#repeatcstring) | Prefills an array of C-strings with a pointer `count` times | Yes |

---

# Comparison types and constants

### Types

| Type | Description |
| --- | --- |
| `CMPOUT` | Enumeration with LOWER, EQUAL, HIGHER |
| `TCLOSURE(Comparison)` | Callback `CMPOUT (*)(const void *, const void *, void *)` |

Use `COMPARISON_CONST(NAME)` to access the namespaced comparison constants.

### X-macro

`COMPARISON_TABLE(X)` expands `X` once for each of the three comparison constants, in the order LOWER, EQUAL, HIGHER.

---

# Numeric and string comparisons

Functions in this group receive their inputs by value and return a comparison result without modifying either argument.

---

# Int

Compares two `int` values.

### Syntax

#### Macro form

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Int)(left, right);
```

#### Direct form

```c
CMPOUT result = Comparison_Int(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `int` value |
| `right` | Second `int` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when left compares lower, `COMPARISON_CONST(HIGHER)` when higher, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Integer comparisons use relational operators, avoiding subtraction overflow.

---

### Example

```c
int a = 5, b = 10;
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Int)(a, b);
/* result == COMPARISON_CONST(LOWER) */
```

---

# Float

Compares two `float` values.

### Syntax

#### Macro form

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Float)(left, right);
```

#### Direct form

```c
CMPOUT result = Comparison_Float(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `float` value |
| `right` | Second `float` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when `left < right`, `COMPARISON_CONST(HIGHER)` when `left > right`, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Does not establish total order for NaNs.

---

### Example

```c
float a = 2.5f, b = 1.0f;
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Float)(a, b);
/* result == COMPARISON_CONST(HIGHER) */
```

---

# Double

Compares two `double` values.

### Syntax

#### Macro form

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Double)(left, right);
```

#### Direct form

```c
CMPOUT result = Comparison_Double(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `double` value |
| `right` | Second `double` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when `left < right`, `COMPARISON_CONST(HIGHER)` when `left > right`, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Follows standard IEEE-754 ordering.

---

### Example

```c
double a = 3.14, b = 3.14;
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Double)(a, b);
/* result == COMPARISON_CONST(EQUAL) */
```

---

# LongDouble

Compares two `long double` values.

### Syntax

#### Macro form

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, LongDouble)(left, right);
```

#### Direct form

```c
CMPOUT result = Comparison_LongDouble(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `long double` value |
| `right` | Second `long double` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when `left < right`, `COMPARISON_CONST(HIGHER)` when `left > right`, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Operates with extended precision.

---

### Example

```c
long double a = 1.0L, b = 2.0L;
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, LongDouble)(a, b);
/* result == COMPARISON_CONST(LOWER) */
```

---

# CString

Compares two null-terminated C strings.

### Syntax

#### Macro form

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, CString)(left, right);
```

#### Direct form

```c
CMPOUT result = Comparison_CString(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First null-terminated string |
| `right` | Second null-terminated string |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when `left` precedes `right`, `COMPARISON_CONST(HIGHER)` when following, or `COMPARISON_CONST(EQUAL)` if identical.

---

### Remarks

Compares lexicographically byte-by-byte using `strcmp`.

---

### Example

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, CString)("apple", "banana");
/* result == COMPARISON_CONST(LOWER) */
```

---

# Byte comparisons

---

# Bytes

Lexicographically compares two memory buffers with explicit lengths.

### Syntax

#### Macro form

```c
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Bytes)(left, leftSize, right, rightSize);
```

#### Direct form

```c
CMPOUT result = Comparison_Bytes(left, leftSize, right, rightSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | Pointer to first buffer |
| `leftSize` | Size in bytes of first buffer |
| `right` | Pointer to second buffer |
| `rightSize` | Size in bytes of second buffer |

---

### Return value

Returns comparison outcome between the two byte sequences.

---

### Remarks

If common prefixes match, shorter lengths precede longer lengths.

---

### Example

```c
const unsigned char b1[] = {1, 2};
const unsigned char b2[] = {1, 3};
CMPOUT result = MODULE_FUNC(Algorithm, Comparison, Bytes)(b1, sizeof(b1), b2, sizeof(b2));
/* result == COMPARISON_CONST(LOWER) */
```

---

# Custom comparison closures

A custom comparison is represented by a typed closure rather than an Algorithm package function. The `CLOSURE_COMPARISON` declaration supplies the three-way `CMPOUT` return contract and the closure stores a function pointer plus a borrowed context pointer. Invoke it with `CLOSURE_INVOKE`.

~~~c
static CMPOUT compare_integer(const void *left, const void *right, void *context)
{
    (void)context;
    return MODULE_FUNC(Algorithm, Comparison, Int)(
        *(const int *)left, *(const int *)right);
}

TCLOSURE(Comparison) comparator =
    CLOSURE_FUNC(Comparison_Bind)(compare_integer, NULL);

int left = 4, right = 9;
CMPOUT result = CLOSURE_INVOKE(comparator, &left, &right);
~~~

Keep the context alive for as long as the closure may be invoked. Do not cast a callback with a different signature to the comparison closure type.

---

# Repeat execution and loops

The Repeat package provides counted loop execution over typed closures.

### Callback types

| Callback Type | Invocation Signature | Purpose |
| --- | --- | --- |
| `MODULE_CLOSURE(Algorithm, Repeat, Procedure)` | `void (*)(void *context)` | Action executed without index |
| `MODULE_CLOSURE(Algorithm, Repeat, ProcedureIndexed)` | `void (*)(size_t index, void *context)` | Action executed with index `0 .. count - 1` |
| `MODULE_CLOSURE(Algorithm, Repeat, Operation)` | `OPSTATUS (*)(void *context)` | Fallible operation; halts on first failure |
| `MODULE_CLOSURE(Algorithm, Repeat, OperationIndexed)` | `OPSTATUS (*)(size_t index, void *context)` | Fallible indexed operation; halts on first failure |
| `MODULE_CLOSURE(Algorithm, Repeat, Predicate)` | `bool (*)(void *context)` | Evaluated while returning `true` |
| `MODULE_CLOSURE(Algorithm, Repeat, PredicateIndexed)` | `bool (*)(size_t index, void *context)` | Evaluated with index while returning `true` |

---

# Procedure

Repeats an infallible action `count` times.

### Syntax

#### Direct form

```c
Algorithm_Repeat_Procedure(count, action);
```

#### Declarative form

```c
MODULE_CALL(Algorithm, Repeat, Procedure, (count, action));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Number of times to execute the action |
| `action` | `MODULE_CLOSURE(Algorithm, Repeat, Procedure)` action callback |

---

### Return value

None (`void`).

---

### Remarks

If `count == 0` or `action` is invalid, the call returns immediately as a no-op.

---

### Example

```c
static void DoWork(void *ctx) {
    int *c = (int *)ctx;
    (*c)++;
}

int count = 0;
MODULE_CLOSURE(Algorithm, Repeat, Procedure) cb =
    MODULE_CLOSURE_FUNC(Algorithm, Repeat, Procedure, Bind)(DoWork, &count);
Algorithm_Repeat_Procedure(5, cb);
/* count == 5 */
```

---

# ProcedureIndexed

Repeats an action `count` times, passing current iteration index `0 .. count - 1`.

### Syntax

#### Direct form

```c
Algorithm_Repeat_ProcedureIndexed(count, action);
```

#### Declarative form

```c
MODULE_CALL(Algorithm, Repeat, ProcedureIndexed, (count, action));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Number of times to execute the action |
| `action` | `MODULE_CLOSURE(Algorithm, Repeat, ProcedureIndexed)` indexed callback |

---

### Return value

None (`void`).

---

### Remarks

Indexes are emitted in ascending order starting at 0 up to `count - 1`.

---

### Example

```c
static void Accumulate(size_t index, void *ctx) {
    *(size_t *)ctx += index;
}

size_t sum = 0;
MODULE_CLOSURE(Algorithm, Repeat, ProcedureIndexed) cb =
    MODULE_CLOSURE_FUNC(Algorithm, Repeat, ProcedureIndexed, Bind)(Accumulate, &sum);
Algorithm_Repeat_ProcedureIndexed(4, cb);
/* sum == 0 + 1 + 2 + 3 = 6 */
```

---

# Operation

Repeats a fallible operation `count` times with short-circuit on failure.

### Syntax

#### Direct form

```c
OPSTATUS status = Algorithm_Repeat_Operation(count, operation);
```

#### Declarative form

```c
OPSTATUS status = MODULE_CALL(Algorithm, Repeat, Operation, (count, operation));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Number of times to invoke the operation |
| `operation` | `MODULE_CLOSURE(Algorithm, Repeat, Operation)` operation callback |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` if all iterations succeed or if `count == 0`. Returns the failing `OPSTATUS` on the first iteration that fails. Returns `STATUS_CONST(INVALID_ARGUMENT)` if `count > 0` and `operation` is invalid.

---

### Remarks

Stops executing immediately upon encountering any status code other than `SUCCESS`.

---

### Example

```c
static OPSTATUS Step(void *ctx) {
    int *remaining = (int *)ctx;
    if (*remaining <= 0) return STATUS_CONST(OUT_OF_RANGE);
    (*remaining)--;
    return STATUS_CONST(SUCCESS);
}

int left = 2;
MODULE_CLOSURE(Algorithm, Repeat, Operation) cb =
    MODULE_CLOSURE_FUNC(Algorithm, Repeat, Operation, Bind)(Step, &left);
OPSTATUS s = Algorithm_Repeat_Operation(2, cb);
/* s == STATUS_CONST(SUCCESS), left == 0 */
```

---

# OperationIndexed

Repeats an indexed fallible operation `count` times with short-circuit on failure.

### Syntax

#### Direct form

```c
OPSTATUS status = Algorithm_Repeat_OperationIndexed(count, operation);
```

#### Declarative form

```c
OPSTATUS status = MODULE_CALL(Algorithm, Repeat, OperationIndexed, (count, operation));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Number of times to invoke the operation |
| `operation` | `MODULE_CLOSURE(Algorithm, Repeat, OperationIndexed)` indexed operation callback |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` on full completion or if `count == 0`. Returns failing `OPSTATUS` on first error. Returns `STATUS_CONST(INVALID_ARGUMENT)` if `count > 0` and `operation` is invalid.

---

### Remarks

Short-circuits on failure; subsequent iterations are skipped.

---

### Example

```c
static OPSTATUS LimitAt(size_t index, void *ctx) {
    size_t max = *(size_t *)ctx;
    return index < max ? STATUS_CONST(SUCCESS) : STATUS_CONST(OUT_OF_RANGE);
}

size_t max = 3;
MODULE_CLOSURE(Algorithm, Repeat, OperationIndexed) cb =
    MODULE_CLOSURE_FUNC(Algorithm, Repeat, OperationIndexed, Bind)(LimitAt, &max);
OPSTATUS s = Algorithm_Repeat_OperationIndexed(5, cb);
/* s == STATUS_CONST(OUT_OF_RANGE) */
```

---

# Predicate

Repeats a predicate `count` times, stopping if it returns `false`.

### Syntax

#### Direct form

```c
bool allTrue = Algorithm_Repeat_Predicate(count, predicate);
```

#### Declarative form

```c
bool allTrue = MODULE_CALL(Algorithm, Repeat, Predicate, (count, predicate));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Number of times to evaluate the predicate |
| `predicate` | `MODULE_CLOSURE(Algorithm, Repeat, Predicate)` predicate callback |

---

### Return value

Returns `true` if all iterations evaluate to `true` or if `count == 0`. Returns `false` on the first `false` evaluation or if `count > 0` and `predicate` is invalid.

---

### Remarks

Short-circuits on the first `false`.

---

### Example

```c
static bool CheckItem(void *ctx) {
    int *n = (int *)ctx;
    return (*n)-- > 0;
}

int left = 3;
MODULE_CLOSURE(Algorithm, Repeat, Predicate) cb =
    MODULE_CLOSURE_FUNC(Algorithm, Repeat, Predicate, Bind)(CheckItem, &left);
bool ok = Algorithm_Repeat_Predicate(3, cb);
/* ok == true */
```

---

# PredicateIndexed

Repeats an indexed predicate `count` times, stopping if it returns `false`.

### Syntax

#### Direct form

```c
bool allTrue = Algorithm_Repeat_PredicateIndexed(count, predicate);
```

#### Declarative form

```c
bool allTrue = MODULE_CALL(Algorithm, Repeat, PredicateIndexed, (count, predicate));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `count` | Number of times to evaluate the predicate |
| `predicate` | `MODULE_CLOSURE(Algorithm, Repeat, PredicateIndexed)` indexed predicate callback |

---

### Return value

Returns `true` if all iterations evaluate to `true` or if `count == 0`. Returns `false` on first `false` or if `count > 0` and `predicate` is invalid.

---

### Remarks

Useful for range checking and validation sequences.

---

### Example

```c
static bool BelowThreshold(size_t index, void *ctx) {
    return index < *(size_t *)ctx;
}

size_t limit = 10;
MODULE_CLOSURE(Algorithm, Repeat, PredicateIndexed) cb =
    MODULE_CLOSURE_FUNC(Algorithm, Repeat, PredicateIndexed, Bind)(BelowThreshold, &limit);
bool ok = Algorithm_Repeat_PredicateIndexed(5, cb);
/* ok == true */
```

---

# Memory and primitive repetition

---

# RepeatBytes

Repeats a byte pattern `count` times into a destination memory buffer.

### Syntax

#### Direct form

```c
OPSTATUS status = Algorithm_Repeat_Bytes(destination, pattern, patternSize, count);
```

#### Declarative form

```c
OPSTATUS status = MODULE_CALL(Algorithm, Repeat, Bytes, (destination, pattern, patternSize, count));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Pointer to destination buffer receiving repeated pattern |
| `pattern` | Pointer to source byte pattern |
| `patternSize` | Size in bytes of pattern |
| `count` | Number of times pattern is written |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` on success or when `count == 0` or `patternSize == 0`. Returns `STATUS_CONST(INVALID_ARGUMENT)` if pointers are null or buffers overlap. Returns `STATUS_CONST(ARITHMETIC_OVERFLOW)` if total bytes overflow `size_t`.

---

### Remarks

Performs checked arithmetic and overlap verification before copying.

---

### Example

```c
const unsigned char pat[2] = {0xAA, 0x55};
unsigned char dest[6];
OPSTATUS s = Algorithm_Repeat_Bytes(dest, pat, sizeof(pat), 3);
/* dest == {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55} */
```

---

# RepeatPrimitives

Prefills an array of primitive elements by repeating a scalar value `count` times.

The following primitive operations are generated from `PRIMITIVE_TYPE_TABLE`:
`Boolean`, `Int8`, `Int16`, `Int32`, `Int64`, `UInt8`, `UInt16`, `UInt32`, `UInt64`, `Char`, `Char8`, `Char16`, `Char32`, `Float32`, `Float64`, `Float`, `Double`, `LongDouble`, `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr`.

### Syntax

#### Direct form

```c
OPSTATUS status = Algorithm_Repeat_Int(destination, value, count);
```

#### Declarative form

```c
OPSTATUS status = MODULE_CALL(Algorithm, Repeat, Int, (destination, value, count));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Pointer to target primitive array |
| `value` | Scalar value to assign to each element |
| `count` | Number of elements to fill |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` on success or when `count == 0`. Returns `STATUS_CONST(INVALID_ARGUMENT)` if `destination` is null. Returns `STATUS_CONST(ARITHMETIC_OVERFLOW)` if total array size overflows `size_t`.

---

### Remarks

Provides type-safe array filling for all native C and fixed-width integer/float types without raw casting.

---

### Example

```c
int array[5];
OPSTATUS s = Algorithm_Repeat_Int(array, 42, 5);
/* array[0..4] == 42 */
```

---

# RepeatCString

Repeats a `const char *` pointer `count` times into an array of string pointers.

### Syntax

#### Direct form

```c
OPSTATUS status = Algorithm_Repeat_CString(destination, value, count);
```

#### Declarative form

```c
OPSTATUS status = MODULE_CALL(Algorithm, Repeat, CString, (destination, value, count));
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Array of `const char *` pointers |
| `value` | C-string pointer to assign |
| `count` | Number of pointers to fill |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` on success or when `count == 0`. Returns `STATUS_CONST(INVALID_ARGUMENT)` if `destination` is null. Returns `STATUS_CONST(ARITHMETIC_OVERFLOW)` if total pointer size overflows `size_t`.

---

### Remarks

Copies the pointer reference, not string content.

---

### Example

```c
const char *names[3];
OPSTATUS s = Algorithm_Repeat_CString(names, "cosmeron", 3);
/* names[0..2] == "cosmeron" */
```

---

# Universal template engine

---

# TEMPLATE_REPEAT

Declares a type-specialized repeat subpackage for user-defined structs or custom types.

### Syntax

```c
TEMPLATE_DECLARE(REPEAT, Suffix, Type);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `Suffix` | Identifier token for the specialized subpackage (e.g. `Point`) |
| `Type` | Target C type to repeat, fill, and generate (e.g. `Point`) |

---

### Return value

Macro expansion declaring types and inline package prototypes.

---

### Remarks

Generates package `Algorithm_Repeat_<Suffix>` providing:
- `Fill(Type *destination, Type value, size_t count)`: Fills array with `value`.
- `Generate(Type *destination, size_t count, TCALLBACK(...) generator)`: Fills array by repeatedly invoking a generator callback.
- `GenerateIndexed(Type *destination, size_t count, TCALLBACK(...) generator)`: Fills array by invoking a generator callback with current index `0 .. count - 1`.

---

### Example

```c
typedef struct Point { int x; int y; } Point;
TEMPLATE_DECLARE(REPEAT, Point, Point);

Point pts[3];
Point def = { 10, 20 };
Algorithm_Repeat_Point_Fill(pts, def, 3);
```

---

# C11 smoke test

The following complete example is compiled and executed by `Tools/check_doc_examples.py`. Unlike the individual API fragments above, it can be copied into one C11 translation unit.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Algorithm/Repeat.h"
#include <assert.h>
#include <string.h>

static void AddIndex(size_t index, void *ctx) {
    *(size_t *)ctx += index;
}

int main(void) {
    /* 1. Comparison checks */
    assert(MODULE_FUNC(Algorithm, Comparison, Int)(2, 3) == COMPARISON_CONST(LOWER));
    assert(MODULE_FUNC(Algorithm, Comparison, Double)(3.0, 3.0) == COMPARISON_CONST(EQUAL));

    /* 2. Repeat Procedure */
    size_t sum = 0;
    MODULE_CLOSURE(Algorithm, Repeat, ProcedureIndexed) cb =
        MODULE_CLOSURE_FUNC(Algorithm, Repeat, ProcedureIndexed, Bind)(AddIndex, &sum);
    Algorithm_Repeat_ProcedureIndexed(4, cb);
    assert(sum == 6); /* 0 + 1 + 2 + 3 */

    /* 3. Repeat Primitive */
    int numbers[4] = {0};
    assert(Algorithm_Repeat_Int(numbers, 77, 4) == STATUS_CONST(SUCCESS));
    assert(numbers[0] == 77 && numbers[3] == 77);

    /* 4. Repeat Bytes */
    const unsigned char pat[2] = {0xAA, 0x55};
    unsigned char dest[4] = {0};
    assert(Algorithm_Repeat_Bytes(dest, pat, sizeof(pat), 2) == STATUS_CONST(SUCCESS));
    assert(dest[0] == 0xAA && dest[1] == 0x55 && dest[2] == 0xAA && dest[3] == 0x55);

    return 0;
}
```


---

# Result and callback contracts

The three-way comparison result is `CMPOUT`: `LOWER`, `EQUAL` or `HIGHER`. The built-in comparison functions return these values; do not treat them as Boolean values. Repeat procedure closures return `void`, operation closures return `OPSTATUS` and stop on the first failure, and predicate closures return `bool` and stop when the predicate becomes false. Indexed forms receive a zero-based index from `0` to `count - 1`. A count of zero performs no invocation.

Primitive and byte repetition functions fill caller-provided storage; they do not allocate the destination array or buffer.
