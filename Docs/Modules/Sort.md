# Sort

C11, header-only, allocation-free introsort/heap/insertion and stable merge
and radix variants. The module depends only on the Cosmeron Core headers.

```c
#include "Cosmeron/Modules/Sort/Sort.h"

int numbers[] = { 4, 1, 3, 1 };
Sort_Array(numbers, 4, sizeof numbers[0], myComparator);
Sort_Radix_U32(ids, idCount, SORT_ASCENDING);
```

- **Array / Intro**: 3-way introsort, insertion cutoff and heapsort fallback.
  O(n log n) worst-case, O(log n) stack, nonstable.
- **Insertion**: stable, O(n²) worst-case, O(1) storage.
- **Heap**: O(n log n), O(1) storage, nonstable.
- **Stable / Merge**: stable mergesort, O(n log n), O(n) scratch allocation
  through `Memory_AllocArray`. `StableWithBuffer` uses a caller-owned buffer.
- **Radix_U32 / I32 / U64 / I64**: stable 8-bit LSD radix, numeric signed
  ordering, ascending/descending, O(n) on fixed-width keys.
  `RadixWithBuffer` accepts a disjoint array of at least `count` elements.
- **Typed**: include `Typed.h`, then use
  `SORT_IMPLEMENT_ALL(T, Suffix, typedPointerComparator)`.
  This creates specialized entry points backed by the generic engine,
  not direct value-based sorting implementations.

An empty array is accepted with a valid comparator/elementSize; no null data
with positive count. Overflow is detected before modification.
Buffer allocation occurs before stable sorting mutates the input. Caller
buffers must be disjoint from the array; providing an overlapping region
violates the precondition. A custom comparator must define a consistent
three-way order, including any special floating-point values.

`SORT_DISABLE_FUNCTION_TABLE` suppresses the optional static const table.
Define `COSMERON_NAMESPACE` / `COSMERON_NAMESPACE_CONST` before including
the headers for alternate symbol prefixes. No global mutable state is used.

Linked-list node mergesort is deliberately outside this module; an adapter
belongs in `Container` and should not copy node payloads.

Tests: `Codespace/Tests/Sort.c`.
