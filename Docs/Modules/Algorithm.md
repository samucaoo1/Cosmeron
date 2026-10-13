# Algorithm

The **Algorithm** module supplies a consistent three-way comparison interface for numerical values, strings, byte sequences, and custom comparators. It is a foundation for searching, sorting, and ordered containers.

The currently implemented package is **Comparison**.

```c
#include "Cosmeron/Core/Algorithm/Comparison.h"
```

> **Source layout:** This API was verified against `Codespace/Congro/Core/Algorithm/Comparison.h` on the existing `network-pattern-refactor` code branch. The include above is the corresponding planned Cosmeron path; it is not yet present on Cosmeron's `main` branch.

---

# Overview

All comparison functions return `TComparisonResult`.

| Constant | Value | Meaning |
| --- | ---: | --- |
| `COMPARISON_CONST(LOWER)` | -1 | Left precedes right |
| `COMPARISON_CONST(EQUAL)` | 0 | Both compare equally |
| `COMPARISON_CONST(HIGHER)` | 1 | Left follows right |

Two equivalent call forms are supported:

### Macro form

```c
COMPARISON_FUNC(int)(3, 7);
```

### Direct form

```c
Comparison_int(3, 7);
```

The macro form automatically respects the configured library namespace. Direct names shown here assume the default namespace.

A third convenience form is also available:

```c
Comparison_Compare(int, 3, 7);
```

---

# API reference

## Function summary

| Function | Description | Modifies input |
| --- | --- | --- |
| [`int`](#int) | Compares two `int` values | No |
| [`float`](#float) | Compares two `float` values | No |
| [`double`](#double) | Compares two `double` values | No |
| [`LongDouble`](#longdouble) | Compares two `long double` values | No |
| [`CString`](#cstring) | Compares two `const char *` values | No |
| [`Bytes`](#bytes) | Lexicographic comparison of sized byte buffers | No |
| [`Invoke`](#invoke) | Invokes a custom comparison callback | No |

---

# Comparison types and constants

### Types

| Type | Description |
| --- | --- |
| `TComparisonResult` | Enumeration with LOWER, EQUAL, HIGHER |
| `TComparator` | Callback `TComparisonResult (*)(const void *, const void *)` |

Use `COMPARISON_CONST(NAME)` to access the namespaced comparison constants.

### X-macro

`COMPARISON_TABLE(X)` expands `X` once for each of the three comparison constants, in the order LOWER, EQUAL, HIGHER:

```c
#define CASE_VALUE(value) case value:
switch (result) {
    COMPARISON_TABLE(CASE_VALUE)
        break;
}
#undef CASE_VALUE
```

---

# Numeric and string comparisons

Functions in this group receive their inputs by value and return a comparison result without modifying either argument.

---

# int

Compares two `int` values.

### Syntax

#### Macro form

```c
TComparisonResult result = COMPARISON_FUNC(int)(left, right);
```

#### Direct form

```c
TComparisonResult result = Comparison_int(left, right);
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
TComparisonResult result =
    COMPARISON_FUNC(int)(1, 2);

/* result == COMPARISON_CONST(LOWER) */
```

---

# float

Compares two `float` values.

### Syntax

#### Macro form

```c
TComparisonResult result = COMPARISON_FUNC(float)(left, right);
```

#### Direct form

```c
TComparisonResult result = Comparison_float(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `float` value |
| `right` | Second `float` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when left compares lower, `COMPARISON_CONST(HIGHER)` when higher, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Exact comparison, no tolerance. NaN is incorrectly classified as EQUAL by the current implementation because both relational checks are false.

---

### Example

```c
TComparisonResult result =
    COMPARISON_FUNC(float)(1.0f, 2.0f);

/* result == COMPARISON_CONST(LOWER) */
```

---

# double

Compares two `double` values.

### Syntax

#### Macro form

```c
TComparisonResult result = COMPARISON_FUNC(double)(left, right);
```

#### Direct form

```c
TComparisonResult result = Comparison_double(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `double` value |
| `right` | Second `double` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when left compares lower, `COMPARISON_CONST(HIGHER)` when higher, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Exact comparison; NaN is classified as EQUAL, so this is not a total-order comparator.

---

### Example

```c
TComparisonResult result =
    COMPARISON_FUNC(double)(3.0, 3.0);

/* result == COMPARISON_CONST(EQUAL) */
```

---

# LongDouble

Compares two `long double` values.

### Syntax

#### Macro form

```c
TComparisonResult result = COMPARISON_FUNC(LongDouble)(left, right);
```

#### Direct form

```c
TComparisonResult result = Comparison_LongDouble(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `long double` value |
| `right` | Second `long double` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when left compares lower, `COMPARISON_CONST(HIGHER)` when higher, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

The function suffix is LongDouble. NaN has the same limitation as float and double.

---

### Example

```c
TComparisonResult result =
    COMPARISON_FUNC(LongDouble)(1.0L, 2.0L);

/* result == COMPARISON_CONST(LOWER) */
```

---

# CString

Compares two `const char *` values.

### Syntax

#### Macro form

```c
TComparisonResult result = COMPARISON_FUNC(CString)(left, right);
```

#### Direct form

```c
TComparisonResult result = Comparison_CString(left, right);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First `const char *` value |
| `right` | Second `const char *` value |

---

### Return value

Returns `COMPARISON_CONST(LOWER)` when left compares lower, `COMPARISON_CONST(HIGHER)` when higher, or `COMPARISON_CONST(EQUAL)` otherwise.

---

### Remarks

Calls strcmp. Both arguments must be valid, null-terminated strings; comparison is case-sensitive and not locale-aware.

---

### Example

```c
TComparisonResult result =
    COMPARISON_FUNC(CString)("apple", "banana");

/* result == COMPARISON_CONST(LOWER) */
```

---

# Byte comparisons

Byte comparisons operate on buffers with explicit lengths and therefore do not require null terminators.

---

# Bytes

Compares two byte sequences lexicographically, using `memcmp` for the shared prefix and comparing lengths if that prefix is identical.

### Syntax

#### Macro form

```c
TComparisonResult result =
    COMPARISON_FUNC(Bytes)(left, leftSize, right, rightSize);
```

#### Direct form

```c
TComparisonResult result =
    Comparison_Bytes(left, leftSize, right, rightSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First buffer |
| `leftSize` | Its length in bytes |
| `right` | Second buffer |
| `rightSize` | Its length in bytes |

---

### Return value

Returns LOWER or HIGHER for the first differing byte; if all shared bytes are equal, the shorter sequence is LOWER. Equal contents and equal lengths return EQUAL.

---

### Remarks

- Buffers must be readable for the bytes compared.
- It compares bytes, not logical typed elements.
- Neither buffer is modified.

---

### Example

```c
const unsigned char a[] = {1, 2};
const unsigned char b[] = {1, 3};

TComparisonResult result =
    COMPARISON_FUNC(Bytes)(a, sizeof a, b, sizeof b);

/* result == COMPARISON_CONST(LOWER) */
```

---

# Custom comparisons

Custom callbacks let caller-defined types follow the same three-way ordering convention.

---

# Invoke

Calls a `TComparator` supplied by the caller.

### Syntax

#### Macro form

```c
TComparisonResult result =
    COMPARISON_FUNC(Invoke)(left, right, comparator);
```

#### Direct form

```c
TComparisonResult result =
    Comparison_Invoke(left, right, comparator);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | First object pointer |
| `right` | Second object pointer |
| `comparator` | Valid non-null `TComparator` callback |

---

### Return value

Returns the comparator result unchanged.

---

### Remarks

- Does not inspect or validate object contents.
- Does not validate the callback pointer or normalize its result.
- The comparator must implement consistent ordering where required by the caller.

---

### Example

```c
static TComparisonResult CompareIntegers(
    const void *left, const void *right
) {
    const int a = *(const int *)left;
    const int b = *(const int *)right;
    return COMPARISON_FUNC(int)(a, b);
}

int a = 4, b = 9;
TComparisonResult result =
    COMPARISON_FUNC(Invoke)(&a, &b, CompareIntegers);

/* result == COMPARISON_CONST(LOWER) */
```

---

# Convenience macros

## Comparison_Compare

```c
Comparison_Compare(TYPE, left, right)
```

Expands to `COMPARISON_FUNC(TYPE)((left), (right))`.

Supported suffixes: `int`, `float`, `double`, `LongDouble`, `CString`. `Bytes` and `Invoke` have different argument lists and should be called via `COMPARISON_FUNC`.

```c
TComparisonResult result = Comparison_Compare(int, 5, 8);
```

---

# Notes

- The implementations are `static inline`.
- No sort or search API is currently defined by the verified Algorithm source; this document covers the existing Comparison package only.
- Floating-point comparisons are not a total ordering for NaNs, which matters for sorted containers and tree keys.
- Direct names in this document presume the default namespace; use `COMPARISON_FUNC` for namespace-independent code.
