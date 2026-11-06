# Sort

The **Sort** module provides generic, high-performance sorting algorithms, specialized integer radix sorting, typed template wrappers, and educational classical and esoteric algorithms in C11. It is header-only and conforms strictly to the Cosmeron architecture.

```c
#include "Cosmeron/Modules/Sort/Sort.h"
```

---

## Architecture & Design

The module is built entirely on Cosmeron's canonical architectural foundations:

1. **Declarative Package (`PACKAGE(Sort, ...)`)**:
   All algorithms are registered within a unified declarative package (`LIB_PREFIX(SORT_MOD)` / `Sort`), providing direct inline/static calls, package macros (`PACKAGE_CALL`), and function tables (`Sort_FunctionTable`, `Sort_GetFunctionTable()`).
2. **First-Class Unified Callbacks (`TCALLBACK(Comparison)`)**:
   Eliminates the legacy duality of `Comparator` and `ContextComparator`. All generic algorithms receive a single, first-class `TCALLBACK(Comparison)`. Callers bind functions with or without contextual state cleanly using `CALLBACK_FUNC(Comparison_Bind)(func, context)`, eliminating all duplicate `*WithContext` variants and adapter trampolines.
3. **Decoupled X-Macro Table for Order (`SORT_ORDER_TABLE`)**:
   Defines the sorting order enum `SORT_TYPE(Order)` (`Sort_Order`) and constants `SORT_CONST(ASCENDING)` and `SORT_CONST(DESCENDING)` using parenthesized row tuples.
4. **Decoupled X-Macro Table for Radix Sorting (`SORT_RADIX_TABLE`)**:
   Single source of truth for fixed-width integer radix sorting (`U32`, `I32`, `U64`, `I64`). Supports in-place and external-buffer execution with signed integer transformation.
5. **Universal Template Engine (`TEMPLATE_SORT`)**:
   Allows instant creation of type-safe specialized sorting routines (`TEMPLATE_SORT(Int, int, CompareInt)`) without casting `void *` pointers or element sizes.
6. **Compile-Time Function Table Disabling**:
   Fully respects `SORT_DISABLE_FUNCTION_TABLE`, `PACKAGE_DISABLE_FUNCTION_TABLE`, and `MODULE_DISABLE_FUNCTION_TABLE`.
7. **Strict Function Contracts**:
   - `OPERATION`: All sorting and utility routines return `OPSTATUS` (`STATUS_CONST(SUCCESS)`, `STATUS_CONST(INVALID_ARGUMENT)`, `STATUS_CONST(ARITHMETIC_OVERFLOW)`, `STATUS_CONST(INSUFFICIENT_SPACE)`, `STATUS_CONST(TIMEOUT)`).

---

## Invocation Styles

Sort routines support four canonical calling styles:

### 1. Direct Form
```c
TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(CompareInt, NULL);
OPSTATUS status = Sort_Array(values, count, sizeof(int), cb);
```

### 2. Macro Form
```c
TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(CompareInt, NULL);
OPSTATUS status = SORT_FUNC(Array)(values, count, sizeof(int), cb);
```

### 3. Declarative Package Call
```c
OPSTATUS status = PACKAGE_CALL(Sort, Array, values, count, sizeof(int), cb);
```

### 4. Function Table Dispatch
```c
const struct Sort_FunctionTable *ft = Sort_GetFunctionTable();
OPSTATUS status = ft->Array(values, count, sizeof(int), cb);
```

---

## Comparison Callbacks

All generic algorithms accept `TCALLBACK(Comparison)`. The callback function signature is:

```c
CMPOUT CompareFunc(const void *left, const void *right, void *context);
```

Comparators must return `COMPARISON_CONST(LOWER)`, `COMPARISON_CONST(EQUAL)`, or `COMPARISON_CONST(HIGHER)`.

### Stateless Comparator Example
```c
static CMPOUT CompareInt(const void *a, const void *b, void *ctx) {
  (void)ctx;
  int x = *(const int *)a;
  int y = *(const int *)b;
  return x < y ? COMPARISON_CONST(LOWER)
               : (x > y ? COMPARISON_CONST(HIGHER) : COMPARISON_CONST(EQUAL));
}

TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(CompareInt, NULL);
SORT_FUNC(Array)(values, count, sizeof(int), cb);
```

### Context-Aware Comparator Example
```c
typedef struct SortConfig {
  bool descending;
} SortConfig;

static CMPOUT CompareWithConfig(const void *a, const void *b, void *ctx) {
  const SortConfig *cfg = (const SortConfig *)ctx;
  int x = *(const int *)a;
  int y = *(const int *)b;
  CMPOUT result = x < y ? COMPARISON_CONST(LOWER)
                        : (x > y ? COMPARISON_CONST(HIGHER) : COMPARISON_CONST(EQUAL));
  if (cfg != NULL && cfg->descending) {
    if (result == COMPARISON_CONST(LOWER)) return COMPARISON_CONST(HIGHER);
    if (result == COMPARISON_CONST(HIGHER)) return COMPARISON_CONST(LOWER);
  }
  return result;
}

SortConfig cfg = { .descending = true };
TCALLBACK(Comparison) cb = CALLBACK_FUNC(Comparison_Bind)(CompareWithConfig, &cfg);
SORT_FUNC(Array)(values, count, sizeof(int), cb);
```

---

## API Reference

### 1. General & Practical Algorithms (`Array.h`)

| Function | Signature | Description |
| --- | --- | --- |
| `Array` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Default Introsort (delegates to `Intro`). $O(n \log n)$ worst-case. |
| `Stable` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Stable Merge Sort; allocates temporary buffer dynamically. |
| `StableWithBuffer` | `(..., void *buffer, size_t bufferSize)` | Stable Merge Sort using caller-provided scratch buffer. |
| `Insertion` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | In-place Insertion Sort. Efficient for small or nearly-sorted arrays. |
| `Heap` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | In-place Heap Sort. $O(n \log n)$ worst-case, $O(1)$ memory. |
| `Intro` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Introsort (QuickSort falling back to HeapSort based on depth limit). |
| `Merge` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Stable Merge Sort. |

### 2. Classical Sorting Algorithms (`Classical.h`)

| Function | Signature | Description |
| --- | --- | --- |
| `Bubble` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Classic Bubble Sort. |
| `Cocktail` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Bidirectional Cocktail Shaker Sort. |
| `Comb` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Comb Sort using gap factor $\approx 1.3$. |
| `Gnome` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Gnome Sort. |
| `OddEven` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Odd-Even Transposition Sort. |
| `Selection` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Selection Sort finding minimum element. |
| `DoubleSelection` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Bidirectional Selection Sort finding minimum and maximum simultaneously. |
| `Shell` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Shell Sort with diminishing gap increments. |
| `Pancake` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Pancake Sort by prefix reversals. |
| `Quick` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Standard Lomuto/Hoare QuickSort. |
| `QuickThreeWay` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | 3-way partitioning QuickSort (Dijkstra Dutch National Flag). |

### 3. Esoteric & Educational Algorithms (`Esoteric.h`)

| Function | Signature | Description |
| --- | --- | --- |
| `Stalin` | `(..., size_t *outCount)` | Non-decreasing subsequence filter (drops out-of-order elements). |
| `Bogo` | `(..., uint64_t seed, size_t maxSteps)` | Random permutation sort until sorted or `maxSteps` reached. |
| `Bozo` | `(..., uint64_t seed, size_t maxSteps)` | Random pair swap sort until sorted or `maxSteps` reached. |
| `Stooge` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Recursive Stooge Sort ($O(n^{2.7})$). |
| `Slow` | `(void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp)` | Multiply-and-surrender Slowsort. |
| `Miracle` | `(..., size_t maxChecks)` | Verifies array repeatedly awaiting external mutation. |

### 4. Utility Functions (`Utility.h`)

| Function | Signature | Description |
| --- | --- | --- |
| `IsSorted` | `(const void *data, size_t count, size_t elemSize, TCALLBACK(Comparison) cmp, bool *outSorted)` | Checks if the range is sorted in non-decreasing order. |
| `Reverse` | `(void *data, size_t count, size_t elemSize)` | In-place byte-level element reversal. |

### 5. Integer Radix Sorting (`Integer.h`)

Radix sort for native fixed-width integers without comparison callbacks.

```c
OPSTATUS SORT_TYPED_FUNC(Radix, U32)(uint32_t *data, size_t count, SORT_TYPE(Order) order);
OPSTATUS SORT_TYPED_FUNC(RadixWithBuffer, U32)(uint32_t *data, size_t count, SORT_TYPE(Order) order, uint32_t *buffer, size_t bufferCount);
```

Available suffixes:
- `U32`: `uint32_t`
- `I32`: `int32_t` (handles two's complement signed ordering)
- `U64`: `uint64_t`
- `I64`: `int64_t` (handles two's complement signed ordering)

Orders:
- `SORT_CONST(ASCENDING)`
- `SORT_CONST(DESCENDING)`

---

## Typed Template Wrappers (`Typed.h`)

Generates a fully typed sorting suite for a custom type and comparator:

```c
static CMPOUT CompareFloats(const float *a, const float *b) {
  if (*a < *b) return COMPARISON_CONST(LOWER);
  if (*a > *b) return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

TEMPLATE_SORT(Float, float, CompareFloats)

/* Usage */
float numbers[] = {3.14f, 1.41f, 2.71f, 0.57f};
SORT_TYPED_FUNC(Array, Float)(numbers, 4);
SORT_TYPED_FUNC(Stable, Float)(numbers, 4);
SORT_TYPED_FUNC(Insertion, Float)(numbers, 4);
SORT_TYPED_FUNC(Heap, Float)(numbers, 4);
SORT_TYPED_FUNC(Intro, Float)(numbers, 4);
SORT_TYPED_FUNC(Merge, Float)(numbers, 4);
```

---

## Error Codes

| Status Code | Description |
| --- | --- |
| `STATUS_CONST(SUCCESS)` | Operation completed successfully. |
| `STATUS_CONST(INVALID_ARGUMENT)` | `data` is `NULL` (when `count > 0`), `elementSize == 0`, `compare` callback is invalid, or `outCount`/`outSorted` is `NULL`. |
| `STATUS_CONST(ARITHMETIC_OVERFLOW)` | Multiplication `count * elementSize` overflows `size_t`. |
| `STATUS_CONST(INSUFFICIENT_SPACE)` | Caller-supplied buffer is smaller than required. |
| `STATUS_CONST(OUT_OF_MEMORY)` | Dynamic allocation failed for scratch buffer. |
| `STATUS_CONST(TIMEOUT)` | Maximum steps/checks reached before array was sorted (`Bogo`, `Bozo`, `Miracle`). |
