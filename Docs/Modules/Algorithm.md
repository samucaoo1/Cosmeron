# Algorithm

The **Algorithm** core module currently exposes a three-way comparison package. It supplies the comparison result and callback types reused by sorting and other generic routines. This document describes the API implemented in the current branch, rather than proposed algorithm packages.

```c
#include "Cosmeron/Core/Algorithm/Comparison.h"
```

---

# Overview

A comparison produces one of three results: lower, equal, or higher. The comparison package has typed scalar helpers, a C-string comparison, an unsigned byte-sequence comparison, and a callback invoker. All functions are `static inline` and require no extra linker library.

| Purpose | Macro form | Direct form (default namespace) |
| --- | --- | --- |
| Integer comparison | `COMPARISON_FUNC(int)` | `Algorithm_Comparison_int` |
| Float comparison | `COMPARISON_FUNC(float)` | `Algorithm_Comparison_float` |
| Double comparison | `COMPARISON_FUNC(double)` | `Algorithm_Comparison_double` |
| Long-double comparison | `COMPARISON_FUNC(LongDouble)` | `Algorithm_Comparison_LongDouble` |
| C-string comparison | `COMPARISON_FUNC(CString)` | `Algorithm_Comparison_CString` |
| Byte-sequence comparison | `COMPARISON_FUNC(Bytes)` | `Algorithm_Comparison_Bytes` |
| Callback invocation | `COMPARISON_FUNC(Invoke)` | `Algorithm_Comparison_Invoke` |

### Macro form

```c
CMPOUT order = COMPARISON_FUNC(int)(2, 7);
```

### Direct form

```c
CMPOUT order = Algorithm_Comparison_int(2, 7);
```

The direct form above assumes the library's default namespace. Prefer the namespace macro when building with custom prefixes.

# Types, constants, and contracts

```c
typedef enum COMPARISON_TYPE(Result) {
    COMPARISON_LOWER_CONST  = -1,
    COMPARISON_EQUAL_CONST  = 0,
    COMPARISON_HIGHER_CONST = 1
} COMPARISON_TYPE(Result);

typedef COMPARISON_TYPE(Result) CMPOUT;

typedef CMPOUT (*COMPARISON_TYPE(Comparator))(
    const void *left, const void *right);

typedef CMPOUT (*COMPARISON_TYPE(ContextComparator))(
    const void *left, const void *right, void *context);
```

The declarations above expand the result constants for readability; in the actual header they are instantiated through `COMPARISON_TABLE(X)`. The public shorthand for the result is `CMPOUT`.

A comparison returns `COMPARISON_LOWER_CONST` if the left operand precedes the right operand, `COMPARISON_EQUAL_CONST` if they compare equal, and `COMPARISON_HIGHER_CONST` if the left operand follows the right operand. These names are namespace macros; use them instead of assuming numerical values when possible.

`COMPARISON_TYPE(Comparator)` receives pointers to two elements. `COMPARISON_TYPE(ContextComparator)` additionally accepts a caller-provided context; the latter is consumed by APIs such as Sort but has no separate invoker in this package.

The functions do not allocate, retain pointers, or report `OPSTATUS`. Pointers passed to CString, Bytes, and Invoke must satisfy the underlying C operations; no runtime validation is performed.

# API reference

## Function summary

| Function | Arguments | Result |
| --- | --- | --- |
| `int` | `int left, int right` | `CMPOUT` |
| `float` | `float left, float right` | `CMPOUT` |
| `double` | `double left, double right` | `CMPOUT` |
| `LongDouble` | `long double left, long double right` | `CMPOUT` |
| `CString` | `const char *left, const char *right` | `CMPOUT` |
| `Bytes` | `const void *left, size_t leftSize, const void *right, size_t rightSize` | `CMPOUT` |
| `Invoke` | `const void *left, const void *right, COMPARISON_TYPE(Comparator) comparator` | `CMPOUT` |

# Scalar comparison

The five scalar/string specializations below are generated from the `COMPARISON_TYPED_PROTOTYPE` and `COMPARISON_STRING_PROTOTYPE` declarations.

## int

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(int)(int left, int right);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_int(int left, int right);
```

### Parameters

- `left`: first signed integer.
- `right`: second signed integer.

### Return value

The lower/equal/higher comparison of `left` against `right`.

### Remarks

Uses relational comparisons without subtraction, avoiding comparison overflow.

### Example

```c
CMPOUT result = COMPARISON_FUNC(int)(-1, 2);
/* result == COMPARISON_LOWER_CONST */
```

## float

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(float)(float left, float right);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_float(float left, float right);
```

### Parameters

- `left`: first `float`.
- `right`: second `float`.

### Return value

Lower, equal, or higher according to the implemented relational checks.

### Remarks

The implementation evaluates `left < right` and `left > right`; if both are false it returns equal. **Unordered NaN comparisons therefore return equal**, not a distinct unordered status. This helper is not a total ordering of IEEE floating-point values.

### Example

```c
CMPOUT result = COMPARISON_FUNC(float)(1.0f, 2.0f);
```

## double

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(double)(double left, double right);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_double(double left, double right);
```

### Parameters

- `left`: first `double`.
- `right`: second `double`.

### Return value

Lower, equal, or higher according to the relational checks.

### Remarks

The same unordered-NaN limitation as `float` applies.

### Example

```c
CMPOUT result = COMPARISON_FUNC(double)(3.0, 3.0);
/* result == COMPARISON_EQUAL_CONST */
```

## LongDouble

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(LongDouble)(long double left, long double right);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_LongDouble(long double left, long double right);
```

### Parameters

- `left`: first `long double`.
- `right`: second `long double`.

### Return value

Lower, equal, or higher according to the relational checks.

### Remarks

The same unordered-NaN limitation applies. The suffix is `LongDouble`, not `long double`.

### Example

```c
CMPOUT result = COMPARISON_FUNC(LongDouble)(1.0L, 0.0L);
/* result == COMPARISON_HIGHER_CONST */
```

## CString

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(CString)(const char *left, const char *right);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_CString(const char *left, const char *right);
```

### Parameters

- `left`: pointer to the first null-terminated C string.
- `right`: pointer to the second null-terminated C string.

### Return value

The sign of `strcmp(left, right)`, normalized to lower/equal/higher.

### Remarks

Both strings must be valid null-terminated byte strings. Comparison is lexicographic on their byte values, not Unicode collation, case folding, or locale-specific ordering. Does not accept NULL pointers.

### Example

```c
CMPOUT result = COMPARISON_FUNC(CString)("alpha", "beta");
```

# Byte and callback operations

## Bytes

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(Bytes)(
    const void *left, size_t leftSize,
    const void *right, size_t rightSize);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_Bytes(
    const void *left, size_t leftSize,
    const void *right, size_t rightSize);
```

### Parameters

- `left`, `right`: readable byte ranges of the declared sizes.
- `leftSize`, `rightSize`: byte counts.

### Return value

`memcmp` order on the shared prefix; if that prefix compares equally, the shorter range precedes the longer range.

### Remarks

The function compares at most `min(leftSize, rightSize)` bytes, then compares lengths. It does not allocate or append terminators. Provide valid readable pointers for the ranges, including pointers satisfying the platform C-library requirements when a length is zero.

### Example

```c
const unsigned char a[] = {1, 2};
const unsigned char b[] = {1, 2, 3};
CMPOUT result = COMPARISON_FUNC(Bytes)(a, sizeof a, b, sizeof b);
/* result == COMPARISON_LOWER_CONST */
```

## Invoke

### Syntax

#### Macro form

```c
CMPOUT COMPARISON_FUNC(Invoke)(
    const void *left, const void *right,
    COMPARISON_TYPE(Comparator) comparator);
```

#### Direct form

```c
CMPOUT Algorithm_Comparison_Invoke(
    const void *left, const void *right,
    Algorithm_Comparison_Comparator comparator);
```

### Parameters

- `left`, `right`: operands passed unchanged to the callback.
- `comparator`: non-null function pointer conforming to `COMPARISON_TYPE(Comparator)`.

### Return value

The callback's return value, unchanged.

### Remarks

Does not validate pointers or normalize the callback's result. This is a simple call-through; a context comparator cannot be supplied to `Invoke`.

### Example

```c
static CMPOUT CompareIntegers(const void *a, const void *b) {
    return COMPARISON_FUNC(int)(*(const int *)a, *(const int *)b);
}

int first = 7, second = 9;
CMPOUT result = COMPARISON_FUNC(Invoke)(
    &first, &second, CompareIntegers);
```

# Convenience macro

`Comparison_Compare(TYPE, left, right)` invokes `COMPARISON_FUNC(TYPE)(left, right)`. Its first argument is the suffix, such as `int`, `double`, `LongDouble`, or `CString`. Unlike a regular C function, this macro expands before type checking; pass arguments suitable for the selected suffix.

```c
CMPOUT result = Comparison_Compare(int, 4, 5);
```

# Naming and namespace

The module uses `ALGORITHM_MOD`, `ALGORITHM_NS`, `ALGORITHM_TYPE`, `ALGORITHM_FUNC`, `ALGORITHM_CONST`, and package-specific `COMPARISON_TYPE`/`COMPARISON_FUNC` macros. Use the exported result constants rather than hardcoding `-1`, `0`, or `1`.

# Requirements

- C11; `<stddef.h>` and `<string.h>` via the public header.
- Header-only; no extra linker flags.
- Valid inputs are the caller's responsibility.
- No global state, heap allocation, or extra initialization.

# See also

- [Sort](Sort.md): generic and integer-specific sorting built around comparison callbacks.
- [Container](Container.md): collections that may use order-based operations.
- [Bit](Bit.md): reference for the documentation style.

# C11 smoke test

The following complete example is compiled and executed by `Tools/check_doc_examples.py`. Unlike the individual API fragments above, it can be copied into one C11 translation unit.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include <assert.h>

int main(void) {
    const unsigned char a[] = {1, 2};
    const unsigned char b[] = {1, 2, 3};
    assert(COMPARISON_FUNC(int)(2, 3) == COMPARISON_LOWER_CONST);
    assert(COMPARISON_FUNC(double)(3.0, 3.0) == COMPARISON_EQUAL_CONST);
    assert(COMPARISON_FUNC(Bytes)(a, sizeof a, b, sizeof b) ==
           COMPARISON_LOWER_CONST);
    return 0;
}
```
