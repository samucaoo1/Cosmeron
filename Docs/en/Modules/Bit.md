# Bit

The **Bit** module provides unsigned-integer bit operations and fixed-capacity bitsets. `Bit` works directly on scalar integer values; `Bitset` stores a compile-time-selected number of bits in 64-bit limbs.

## Headers

~~~c
#include "Cosmeron/Core/Bit/Bit.h"
#include "Cosmeron/Core/Bit/Bitset.h"
~~~

---

# Overview

## Scalar bit operations

The `Bit` template generates a package for an unsigned integer type. Standard packages are instantiated for the widths declared by `BIT_WIDTH_TYPE_TABLE`. The API covers individual bits, masks, population counts, zero counts, power-of-two helpers, rotations, field extraction/insertion, bit reversal and byte swapping.

## Fixed-size bitsets

`Bitset` generates an object whose capacity is fixed at compile time. Its storage is an array of `uint64_t` limbs; the last limb is masked so bits outside the declared capacity do not become observable. A bitset is useful for flags, feature sets, compact membership maps and bitwise set algebra when the number of positions is known in advance.

The standard specializations are `Bitset128`, `Bitset256`, `Bitset512` and `Bitset1024`.

---

# API reference

## Scalar Bit package

Header: `Cosmeron/Core/Bit/Bit.h`.

For a generated scalar package with suffix `<Width>`, the direct function prefix is `MODULE_FUNC(Bit, <Width>, ...)`. Typical suffixes include `8`, `16`, `32` and `64`.

| Group | Functions | Contract |
| --- | --- | --- |
| Individual bit | `Set`, `Clear`, `Flip`, `Check` | Set, clear, toggle or test a zero-based bit index. |
| Mask | `Mask.Set`, `Mask.Clear`, `Mask.Flip`, `Mask.CheckAll`, `Mask.CheckAny`, `Mask.Range` | Apply or test a bit mask, or construct an inclusive high/low range mask. |
| Counts | `Popcount`, `Count.LeadingZeros`, `Count.TrailingZeros` | Count set or zero bits. |
| Properties | `Parity`, `IsPowerOfTwo` | Test parity or whether a nonzero value has exactly one set bit. |
| Powers and logarithms | `FloorPowerOfTwo`, `NextPowerOfTwo`, `Log2.Floor`, `Log2.Ceil` | Round to powers of two or compute integer base-two logarithms. |
| Bit indexes | `Index.LSB`, `Index.MSB` | Find the least- or most-significant set-bit index. |
| Lowest set bit | `IsolateLowestSet`, `ClearLowestSet` | Isolate or clear the lowest set bit. |
| Rotation | `Rotate.Left`, `Rotate.Right` | Rotate within the selected integer width. |
| Representation | `ReverseBits`, `ByteSwap` | Reverse bit order or byte order. |
| Bit fields | `Extract`, `Insert` | Read or replace an inclusive bit field. |

### Individual bits

~~~c
uint32_t flags = 0U;

OPSTATUS status = MODULE_FUNC(Bit, 32, Set)(&flags, 5U);
bool enabled = MODULE_FUNC(Bit, 32, Check)(flags, 5U);
~~~

Mutation operations receive a pointer to the value and return `OPSTATUS`. `Check` receives the value by value and returns `bool`. Bit indexes are zero-based and must be less than the type width.

### Masks

`Mask.Set`, `Mask.Clear` and `Mask.Flip` are procedures: they modify all bits selected by the mask and silently do nothing when the value pointer is null. `Mask.CheckAll` tests whether every requested bit is set; `Mask.CheckAny` tests whether at least one is set. `Mask.Range` is fallible and reports an invalid output pointer or out-of-width/inverted endpoints through `OPSTATUS`.

`Mask.Range(high, low, outMask)` creates a mask covering both endpoints. It rejects an inverted range and indexes outside the scalar width.

~~~c
uint32_t fieldMask = 0U;
OPSTATUS status = MODULE_FUNC(Bit, 32, Mask, Range)(15U, 8U, &fieldMask);
~~~

### Counts, indexes and powers

`Popcount` returns the number of set bits. Leading/trailing zero counts are measured within the selected type width. `Index.LSB` and `Index.MSB` use an output parameter and return `NOT_FOUND` when the input is zero.

`NextPowerOfTwo`, `Log2.Floor`, `Log2.Ceil` and the index operations are fallible because some inputs have no valid result or would overflow. Check their `OPSTATUS` before reading the output parameter.

### Rotations and bit fields

Rotation counts wrap modulo the integer width. `Extract(value, high, low, outField)` and `Insert(value, high, low, fieldValue)` use inclusive endpoints. They validate the output pointer or mutable input pointer and reject invalid bounds.

---

## Bitset package

Header: `Cosmeron/Core/Bit/Bitset.h`.

The generated object types are `MODULE_TYPE(Bitset128)`, `MODULE_TYPE(Bitset256)`, `MODULE_TYPE(Bitset512)` and `MODULE_TYPE(Bitset1024)`.

| Group | Methods |
| --- | --- |
| Initialization and base state | `Clear`, `SetAll` |
| Set algebra | `And`, `Or`, `Xor`, `Not` |
| Shifts | `ShiftLeft`, `ShiftRight` |
| Single bit | `Bit.Set`, `Bit.Clear`, `Bit.Flip`, `Bit.Check`, `Bit.Test` |
| Ranges | `SetRange`, `ClearRange`, `FlipRange` |
| Queries | `Count`, `Capacity`, `Any`, `All`, `None`, `Equal`, `Intersects` |
| Relations | `Is.Subset`, `Is.Disjoint` |
| Search | `Find.FirstSet`, `Find.FirstZero`, `Find.NextSet` |

### Example

~~~c
MODULE_TYPE(Bitset128) permissions = {0};

OPSTATUS status = MODULE_FUNC(Bitset128, Clear)(&permissions);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Bitset128, Bit, Set)(&permissions, 7U);
}

bool hasPermission = MODULE_FUNC(Bitset128, Bit, Test)(&permissions, 7U);
size_t setCount = MODULE_FUNC(Bitset128, Count)(&permissions);
~~~

`Bitset.h` deliberately disables its object function table, so call its generated methods directly with `MODULE_FUNC(Bitset128, ...)`; `OBJECT_CALL` is not available for these types. Bitset operations that mutate state return `OPSTATUS`. The checked `Bit.Check` form writes a Boolean to an output parameter; the convenience `Bit.Test` form returns `bool` directly and evaluates to false for an invalid object or out-of-range index. `Find` methods return `NOT_FOUND` when no matching bit exists.

Range operations use a half-open interval `[startBit, endBit)`: the start is included and the end is excluded. The end may equal the bitset capacity.

---

# Contracts and configuration

- The scalar `Bit` functions operate on unsigned integer types; do not pass a signed type and assume identical shift semantics.
- All bit indexes are zero-based.
- Range endpoints for scalar mask extraction/insertion are inclusive; Bitset range methods use half-open intervals.
- `Bitset` capacity is fixed by the type suffix; it does not grow at runtime.
- Invalid pointers and out-of-range bit indexes are reported by status-returning operations.
- `Bit.h` supports function-table configuration through its global and package-specific switches. `Bitset.h` unconditionally disables its function table in the current implementation, so its methods always use direct `MODULE_FUNC` calls.

~~~c
#define BIT_DISABLE_FUNCTION_TABLE 1
#include "Cosmeron/Core/Bit/Bit.h"
#include "Cosmeron/Core/Bit/Bitset.h"
~~~

## Related modules

- [Algorithm](Algorithm.md) — comparison and repetition helpers.
- [Memory](Memory.md) — runtime allocation and related bitset facilities.
- [Types](Types.md) — primitive type catalog and limb operations.

## Source headers

- `Codespace/Cosmeron/Core/Bit/Bit.def`
- `Codespace/Cosmeron/Core/Bit/Bitset.def`
- `Codespace/Cosmeron/Core/Types/Impl/Common.inc`
