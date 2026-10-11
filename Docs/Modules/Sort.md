# Sort

C11, header-only, zero external link libraries. Namespace, status returns,
three-way comparators, optional function table, and implementation packaging
follow [PATTERN.md](../Reference/PATTERN.md).

## Public API

```c
#include "Cosmeron/Modules/Sort/Sort.h"
int values[] = {4, 1, 3, 2};
Sort_Bubble(values, 4, sizeof values[0], compareInt);
Sort_QuickThreeWay(values, 4, sizeof values[0], compareInt);
Sort_Stable(values, 4, sizeof values[0], compareInt);
```

The generic engine is `Sort_Array` (3-way Introsort). Stable merge, heap,
insertion, and typed U32/I32/U64/I64 radix variants are retained unchanged.

### Classical algorithms

| Function | Typical complexity | Stable | Extra storage |
|---|---|---|---|
| Bubble | O(n²), O(n) best | Yes | O(1) |
| Cocktail | O(n²), O(n) best | Yes | O(1) |
| Comb | O(n²) worst | No | O(1) |
| Gnome | O(n²), O(n) best | Yes | O(1) |
| OddEven | O(n²), O(n) best | Yes | O(1) |
| Selection | O(n²) | No | O(1) |
| DoubleSelection | O(n²) | No | O(1) |
| Shell | gap-dependent | No | O(1) |
| Pancake | O(n²) | No | O(1) |
| Quick | O(n log n) average, O(n²) worst | No | O(log n) stack |
| QuickThreeWay | O(n log n) average, O(n²) worst | No | O(log n) stack |

All provide `WithContext` variants and report `OPSTATUS`.
No allocations, shared state, or OS-specific calls.

### Esoteric algorithms

```c
size_t retained = 0;
Sort_Stalin(values, 4, sizeof(int), compareInt, &retained);
Sort_Bogo(values, 4, sizeof(int), compareInt, 42, 10000);
Sort_Bozo(values, 4, sizeof(int), compareInt, 42, 10000);
Sort_Stooge(values, 4, sizeof(int), compareInt);
Sort_Slow(values, 4, sizeof(int), compareInt);
Sort_Miracle(values, 4, sizeof(int), compareInt, 100);
```

- **Stalin**: lossy stable subsequence filter. `outCount` reports retained
  prefix size; trailing slots are unspecified. It is not a permutation sort.
- **Bogo**: repeatedly shuffles with a call-local seeded PRNG. `maxSteps`
  bounds the number of shuffles. Returns `STATUS_TIMEOUT` if unsorted.
- **Bozo**: random pair swaps, with the same seed and step-budget semantics.
- **Stooge** and **Slow**: purposely expensive deterministic examples.
  Reject arrays larger than 128 and 20 elements respectively, without
  modifying the input, returning `STATUS_OUT_OF_RANGE`.
- **Miracle**: checks whether data is sorted but deliberately never mutates it;
  returns `STATUS_TIMEOUT` after bounded observations.
- PRNG is deterministic for a given seed; no cryptographic guarantees.
- All functions validate comparator, element size, pointers, and overflow.
- This module does not claim these examples are appropriate for production.

## Architecture / expansion

```text
Sort/
  Sort.h             Aggregate public header
  Sort.space         Naming and public enums
  Array.h            Introsort, Merge, Heap, Insertion
  Classical.h        Quadratic and partition exchange algorithms
  Esoteric.h         Recreational and bounded stochastic algorithms
  Integer.h          Typed LSD Radix
  Typed.h            Generic typed wrappers
  Utility.h          IsSorted, Reverse
  Impl/              Implementations split per family
  FunctionTable/     Optional table
```

No global mutable state. Add further sorting families as independent headers
and .impl files. In-place algorithms do not allocate. Sorting by comparator
requires a consistent total ordering. Random generators aren't suitable for
security purposes.

Compile the test suite:
```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Werror -I Codespace Codespace/Tests/Sort.c -o /tmp/cosmeron-sort
/tmp/cosmeron-sort
```
