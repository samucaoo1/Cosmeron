# Type

The **Type** module provides fundamental C type aliases, fixed-width bit blocks, unsigned multiprecision integers, and a separate `TDecimal` type family. The package is implemented using C11 headers and generated `static inline` functions.

```c
#include "Cosmeron/Modules/Type/Fundamental.h"
#include "Cosmeron/Modules/Type/TBlock.h"
#include "Cosmeron/Modules/Type/TBigint.h"
#include "Cosmeron/Modules/Type/TDecimal.h"
```

There is no single `Type.h` aggregator in the current branch.

---

# Overview

| Package | Header | Function templates | Built-in widths |
| --- | --- | ---: | --- |
| Fundamental aliases | `Fundamental.h` | 0 | Standard signed/unsigned, floating and character aliases |
| TBlock | `TBlock.h` | 12 | 128, 256, 512, 1024 bits |
| TBigint | `TBigint.h` | 31 | 128, 256, 512, 1024 bits |
| TDecimal | `TDecimal.h` | 31 | 128, 256, 512, 1024 bits |
| **Total** | | **74** | **296 concrete generated functions** |

### Macro form

```c
TBigint(128, first)
TBigint(128, second)
first.limb[0] = 7;
second.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Add)(&first, &second);
```

### Direct form

```c
Type_TBigint128 first = {0};
Type_TBigint128 second = {0};
first.limb[0] = 7;
second.limb[0] = 3;
OPSTATUS status = Type_TBigint128_Add(&first, &second);
```

Both forms call the same function in the default namespace. If `COSMERON_NAMESPACE` changes, the macro form adapts automatically and the direct C identifier changes accordingly.

---

# Fundamental aliases

`Fundamental.h` supplies typedefs, not runtime functions. The default style is `TYPE_ALIAS_COMPLETE`; optionally define `TYPE_ALIAS_SMALL` **before header inclusion** to select short typedef names. Defining both styles is a compile-time error.

| Underlying C type | Small alias suffix | Complete alias suffix |
| --- | --- | --- |
| `int8_t`, `int16_t`, `int32_t`, `int64_t` | `TI8`, `TI16`, `TI32`, `TI64` | `TInt8`, `TInt16`, `TInt32`, `TInt64` |
| `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t` | `TU8`, `TU16`, `TU32`, `TU64` | `TUInt8`, `TUInt16`, `TUInt32`, `TUInt64` |
| `float`, `double`, `long double` | `TF32`, `TF64`, `TLD` | `TFloat32`, `TFloat64`, `TLongDouble` |
| Character code units (`uint8_t`, `uint16_t`, `uint32_t`) | `TCh8`, `TCh16`, `TCh32` | `TChar8`, `TChar16`, `TChar32` |

```c
/* Default naming: */
#include "Cosmeron/Modules/Type/Fundamental.h"
Type_TInt32 signedNumber = 42;
Type_TUInt64 unsignedNumber = 123;
```

Short naming in a separate compilation configuration:

```c
#define TYPE_ALIAS_SMALL
#include "Cosmeron/Modules/Type/Fundamental.h"
Type_TI32 signedNumber = 42;
Type_TU64 unsignedNumber = 123;
```

The aliases do not add checked arithmetic; their numerical behavior is that of the underlying C types.

---

# Representation and sizes

The three data families store their numeric content in a union of `uint64_t limb[N]` and `uint8_t byte[WIDTH/8]`, with the **least significant 64-bit word in `limb[0]`**.

| Width | Limbs | Numeric bytes | Valid bit indexes |
| --- | ---: | ---: | --- |
| `128` | 2 | 16 | `0`–`127` |
| `256` | 4 | 32 | `0`–`255` |
| `512` | 8 | 64 | `0`–`511` |
| `1024` | 16 | 128 | `0`–`1023` |

`byte[]` follows the **native host endianness**, not a guaranteed network or disk byte order. With optional function tables enabled, the C structure also has an `api` pointer and `sizeof(struct)` is larger than the numeric byte count.

### Declaration macros and optional tables

```c
TBlock(128, flags)
TBigint(256, number)
TDecimal(512, decimal)
```

These macros declare zero-initialized values and, when function tables are enabled, bind their `api` pointers. To omit function tables, define `TYPE_DISABLE_FUNCTION_TABLE` before all Type headers. This changes the structure layout, but the direct functions remain usable.

`Clear` and `Init` only reset **numeric storage**; they do not bind `api` to a plain `{0}` declaration.

---

# Arithmetic interpretation and status

**TBigint and TDecimal currently share unsigned binary integer semantics.** TDecimal has no fractional decimal scale, sign field, decimal exponent, rounding mode or implicit decimal point. Its `ToCString` function prints ordinary base-10 integer digits.

Mutating operations return `OPSTATUS`: `STATUS_CONST(SUCCESS)` on success, with `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `ARITHMETIC_OVERFLOW` and `DIVISION_BY_ZERO` as appropriate. Predicate operations return `bool` directly.

Arithmetic `Add`, `Sub`, `Mul`, `Increment`, and `Decrement` check overflow/underflow; left shifts discard high bits instead of signaling overflow. Logical shifts by at least the width clear the numeric value, while rotations wrap modulo the width.

---

# API reference

## Function summary

| Family | Function | Description |
| --- | --- | --- |
| TBlock | [`Clear`](#tblock-clear) | Zeros the fixed-width numeric data. |
| TBlock | [`And`](#tblock-and) | Applies bitwise AND across all stored limbs. |
| TBlock | [`Or`](#tblock-or) | Applies bitwise OR across all stored limbs. |
| TBlock | [`Xor`](#tblock-xor) | Applies bitwise XOR across all stored limbs. |
| TBlock | [`Not`](#tblock-not) | Inverts all bits of the numeric value. |
| TBlock | [`ShiftLeft`](#tblock-shiftleft) | Shifts the complete value left, inserting zero bits. |
| TBlock | [`ShiftRight`](#tblock-shiftright) | Shifts the complete value right, inserting zero bits. |
| TBlock | [`RotateLeft`](#tblock-rotateleft) | Rotates the complete value left. |
| TBlock | [`RotateRight`](#tblock-rotateright) | Rotates the complete value right. |
| TBlock | [`BitSet`](#tblock-bitset) | Sets a single bit by its zero-based index. |
| TBlock | [`BitClear`](#tblock-bitclear) | Clears a single bit by its zero-based index. |
| TBlock | [`BitCheck`](#tblock-bitcheck) | Reads one bit through a bool output pointer. |
| TBigint | [`Clear`](#tbigint-clear) | Zeros the fixed-width numeric data. |
| TBigint | [`Init`](#tbigint-init) | Initializes the numeric value to zero. |
| TBigint | [`Add`](#tbigint-add) | Adds source to destination. |
| TBigint | [`Sub`](#tbigint-sub) | Subtracts source from destination. |
| TBigint | [`Mul`](#tbigint-mul) | Multiplies destination by source. |
| TBigint | [`DivMod`](#tbigint-divmod) | Computes quotient and remainder. |
| TBigint | [`Div`](#tbigint-div) | Replaces the dividend with its integer quotient. |
| TBigint | [`Mod`](#tbigint-mod) | Replaces the dividend with its remainder. |
| TBigint | [`Increment`](#tbigint-increment) | Increments the value by one. |
| TBigint | [`Decrement`](#tbigint-decrement) | Decrements the value by one. |
| TBigint | [`And`](#tbigint-and) | Applies bitwise AND across all stored limbs. |
| TBigint | [`Or`](#tbigint-or) | Applies bitwise OR across all stored limbs. |
| TBigint | [`Xor`](#tbigint-xor) | Applies bitwise XOR across all stored limbs. |
| TBigint | [`Not`](#tbigint-not) | Inverts all bits of the numeric value. |
| TBigint | [`ShiftLeft`](#tbigint-shiftleft) | Shifts the complete value left, inserting zero bits. |
| TBigint | [`ShiftRight`](#tbigint-shiftright) | Shifts the complete value right, inserting zero bits. |
| TBigint | [`RotateLeft`](#tbigint-rotateleft) | Rotates the complete value left. |
| TBigint | [`RotateRight`](#tbigint-rotateright) | Rotates the complete value right. |
| TBigint | [`BitSet`](#tbigint-bitset) | Sets a single bit by its zero-based index. |
| TBigint | [`BitClear`](#tbigint-bitclear) | Clears a single bit by its zero-based index. |
| TBigint | [`BitCheck`](#tbigint-bitcheck) | Reads one bit through a bool output pointer. |
| TBigint | [`Compare`](#tbigint-compare) | Writes a three-way unsigned comparison result. |
| TBigint | [`Equal`](#tbigint-equal) | Checks numeric equality. |
| TBigint | [`NotEqual`](#tbigint-notequal) | Checks numeric inequality. |
| TBigint | [`LessThan`](#tbigint-lessthan) | Checks whether left is less than right. |
| TBigint | [`GreaterThan`](#tbigint-greaterthan) | Checks whether left is greater than right. |
| TBigint | [`LessOrEqual`](#tbigint-lessorequal) | Checks less-than-or-equal relation. |
| TBigint | [`GreaterOrEqual`](#tbigint-greaterorequal) | Checks greater-than-or-equal relation. |
| TBigint | [`ToCStringBase`](#tbigint-tocstringbase) | Formats an unsigned value as text in base 2–36. |
| TBigint | [`ToCString`](#tbigint-tocstring) | Formats an unsigned value as decimal integer text. |
| TBigint | [`IsZero`](#tbigint-iszero) | Tests whether all numeric limbs are zero. |
| TDecimal | [`Clear`](#tdecimal-clear) | Zeros the fixed-width numeric data. |
| TDecimal | [`Init`](#tdecimal-init) | Initializes the numeric value to zero. |
| TDecimal | [`Add`](#tdecimal-add) | Adds source to destination. |
| TDecimal | [`Sub`](#tdecimal-sub) | Subtracts source from destination. |
| TDecimal | [`Mul`](#tdecimal-mul) | Multiplies destination by source. |
| TDecimal | [`DivMod`](#tdecimal-divmod) | Computes quotient and remainder. |
| TDecimal | [`Div`](#tdecimal-div) | Replaces the dividend with its integer quotient. |
| TDecimal | [`Mod`](#tdecimal-mod) | Replaces the dividend with its remainder. |
| TDecimal | [`Increment`](#tdecimal-increment) | Increments the value by one. |
| TDecimal | [`Decrement`](#tdecimal-decrement) | Decrements the value by one. |
| TDecimal | [`And`](#tdecimal-and) | Applies bitwise AND across all stored limbs. |
| TDecimal | [`Or`](#tdecimal-or) | Applies bitwise OR across all stored limbs. |
| TDecimal | [`Xor`](#tdecimal-xor) | Applies bitwise XOR across all stored limbs. |
| TDecimal | [`Not`](#tdecimal-not) | Inverts all bits of the numeric value. |
| TDecimal | [`ShiftLeft`](#tdecimal-shiftleft) | Shifts the complete value left, inserting zero bits. |
| TDecimal | [`ShiftRight`](#tdecimal-shiftright) | Shifts the complete value right, inserting zero bits. |
| TDecimal | [`RotateLeft`](#tdecimal-rotateleft) | Rotates the complete value left. |
| TDecimal | [`RotateRight`](#tdecimal-rotateright) | Rotates the complete value right. |
| TDecimal | [`BitSet`](#tdecimal-bitset) | Sets a single bit by its zero-based index. |
| TDecimal | [`BitClear`](#tdecimal-bitclear) | Clears a single bit by its zero-based index. |
| TDecimal | [`BitCheck`](#tdecimal-bitcheck) | Reads one bit through a bool output pointer. |
| TDecimal | [`Compare`](#tdecimal-compare) | Writes a three-way unsigned comparison result. |
| TDecimal | [`Equal`](#tdecimal-equal) | Checks numeric equality. |
| TDecimal | [`NotEqual`](#tdecimal-notequal) | Checks numeric inequality. |
| TDecimal | [`LessThan`](#tdecimal-lessthan) | Checks whether left is less than right. |
| TDecimal | [`GreaterThan`](#tdecimal-greaterthan) | Checks whether left is greater than right. |
| TDecimal | [`LessOrEqual`](#tdecimal-lessorequal) | Checks less-than-or-equal relation. |
| TDecimal | [`GreaterOrEqual`](#tdecimal-greaterorequal) | Checks greater-than-or-equal relation. |
| TDecimal | [`ToCStringBase`](#tdecimal-tocstringbase) | Formats an unsigned value as text in base 2–36. |
| TDecimal | [`ToCString`](#tdecimal-tocstring) | Formats an unsigned value as decimal integer text. |
| TDecimal | [`IsZero`](#tdecimal-iszero) | Tests whether all numeric limbs are zero. |

---

# TBlock package

Header: `Cosmeron/Modules/Type/TBlock.h`

A fixed-width bit block offering mask operations, bit access, logical shifts and rotations. There is no TBlock arithmetic or Init function.

Built-in suffixes are `128`, `256`, `512`, and `1024`.

### Function summary

| Function | Description |
| --- | --- |
| [`Clear`](#tblock-clear) | Zeros the fixed-width numeric data. |
| [`And`](#tblock-and) | Applies bitwise AND across all stored limbs. |
| [`Or`](#tblock-or) | Applies bitwise OR across all stored limbs. |
| [`Xor`](#tblock-xor) | Applies bitwise XOR across all stored limbs. |
| [`Not`](#tblock-not) | Inverts all bits of the numeric value. |
| [`ShiftLeft`](#tblock-shiftleft) | Shifts the complete value left, inserting zero bits. |
| [`ShiftRight`](#tblock-shiftright) | Shifts the complete value right, inserting zero bits. |
| [`RotateLeft`](#tblock-rotateleft) | Rotates the complete value left. |
| [`RotateRight`](#tblock-rotateright) | Rotates the complete value right. |
| [`BitSet`](#tblock-bitset) | Sets a single bit by its zero-based index. |
| [`BitClear`](#tblock-bitclear) | Clears a single bit by its zero-based index. |
| [`BitCheck`](#tblock-bitcheck) | Reads one bit through a bool output pointer. |

---

# TBlock Clear

Zeros the fixed-width numeric data.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, Clear)(TBLOCK_TYPE(128) *target);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_Clear(Type_TBlock128 *target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) *target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires a non-NULL target. Clears the `.byte` numeric member without altering the optional `.api` pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, Clear)(&target);
```


---

# TBlock And

Applies bitwise AND across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, And)(TBLOCK_TYPE(128) * destination, const TBLOCK_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_And(Type_TBlock128 * destination, const Type_TBlock128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBLOCK_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBLOCK_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, destination)
destination.limb[0] = 17;
TBlock(128, source)
source.limb[0] = 3;
OPSTATUS status = TBLOCK_FUNC(128, And)(&destination, &source);
```


---

# TBlock Or

Applies bitwise OR across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, Or)(TBLOCK_TYPE(128) * destination, const TBLOCK_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_Or(Type_TBlock128 * destination, const Type_TBlock128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBLOCK_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBLOCK_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, destination)
destination.limb[0] = 17;
TBlock(128, source)
source.limb[0] = 3;
OPSTATUS status = TBLOCK_FUNC(128, Or)(&destination, &source);
```


---

# TBlock Xor

Applies bitwise XOR across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, Xor)(TBLOCK_TYPE(128) * destination, const TBLOCK_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_Xor(Type_TBlock128 * destination, const Type_TBlock128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBLOCK_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBLOCK_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, destination)
destination.limb[0] = 17;
TBlock(128, source)
source.limb[0] = 3;
OPSTATUS status = TBLOCK_FUNC(128, Xor)(&destination, &source);
```


---

# TBlock Not

Inverts all bits of the numeric value.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, Not)(TBLOCK_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_Not(Type_TBlock128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bitwise NOT is not signed negation; no sign field is represented.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, Not)(&target);
```


---

# TBlock ShiftLeft

Shifts the complete value left, inserting zero bits.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, ShiftLeft)(TBLOCK_TYPE(128) * target, unsigned bitCount);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_ShiftLeft(Type_TBlock128 * target, unsigned bitCount);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) * target` | Mutable numeric object. |
| `bitCount` | `unsigned bitCount` | Logical shift count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

High bits are discarded without arithmetic overflow status. `bitCount >= width` clears numeric bits and returns SUCCESS.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, ShiftLeft)(&target, 3U);
```


---

# TBlock ShiftRight

Shifts the complete value right, inserting zero bits.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, ShiftRight)(TBLOCK_TYPE(128) * target, unsigned bitCount);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_ShiftRight(Type_TBlock128 * target, unsigned bitCount);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) * target` | Mutable numeric object. |
| `bitCount` | `unsigned bitCount` | Logical shift count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Logical right shift. `bitCount >= width` clears numeric bits and returns SUCCESS.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, ShiftRight)(&target, 3U);
```


---

# TBlock RotateLeft

Rotates the complete value left.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, RotateLeft)(TBLOCK_TYPE(128) *target, unsigned shift);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_RotateLeft(Type_TBlock128 *target, unsigned shift);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) *target` | Mutable numeric object. |
| `shift` | `unsigned shift` | Rotation count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bits wrap around the numeric width. Rotation count is reduced modulo 128, 256, 512 or 1024.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, RotateLeft)(&target, 3U);
```


---

# TBlock RotateRight

Rotates the complete value right.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, RotateRight)(TBLOCK_TYPE(128) *target, unsigned shift);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_RotateRight(Type_TBlock128 *target, unsigned shift);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) *target` | Mutable numeric object. |
| `shift` | `unsigned shift` | Rotation count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bits wrap around the numeric width. Rotation count is reduced modulo the width.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, RotateRight)(&target, 3U);
```


---

# TBlock BitSet

Sets a single bit by its zero-based index.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, BitSet)(TBLOCK_TYPE(128) *target, uint32_t bit);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_BitSet(Type_TBlock128 *target, uint32_t bit);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Index 0 is the least-significant bit of `limb[0]`. Indices outside 0–127 (for width 128) return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, BitSet)(&target, 3U);
```


---

# TBlock BitClear

Clears a single bit by its zero-based index.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, BitClear)(TBLOCK_TYPE(128) *target, uint32_t bit);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_BitClear(Type_TBlock128 *target, uint32_t bit);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBLOCK_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Indices outside 0–127 (for width 128) return OUT_OF_RANGE and leave the value unchanged.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
OPSTATUS status = TBLOCK_FUNC(128, BitClear)(&target, 3U);
```


---

# TBlock BitCheck

Reads one bit through a bool output pointer.

### Syntax

#### Macro form

```c
OPSTATUS TBLOCK_FUNC(128, BitCheck)(const TBLOCK_TYPE(128) *target, uint32_t bit, bool *outResult);
```

#### Direct form

```c
OPSTATUS Type_TBlock128_BitCheck(const Type_TBlock128 *target, uint32_t bit, bool *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `const TBLOCK_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |
| `outResult` | `bool *outResult` | Required writable bool pointer. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires both target and outResult. Out-of-range indexes return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBlock(128, target)
target.limb[0] = 17;
bool checked = false;
OPSTATUS status = TBLOCK_FUNC(128, BitCheck)(&target, 3U, &checked);
```


---

# TBigint package

Header: `Cosmeron/Modules/Type/TBigint.h`

A fixed-width unsigned multiprecision binary integer with overflow-checked arithmetic, comparison and string formatting.

Built-in suffixes are `128`, `256`, `512`, and `1024`.

### Function summary

| Function | Description |
| --- | --- |
| [`Clear`](#tbigint-clear) | Zeros the fixed-width numeric data. |
| [`Init`](#tbigint-init) | Initializes the numeric value to zero. |
| [`Add`](#tbigint-add) | Adds source to destination. |
| [`Sub`](#tbigint-sub) | Subtracts source from destination. |
| [`Mul`](#tbigint-mul) | Multiplies destination by source. |
| [`DivMod`](#tbigint-divmod) | Computes quotient and remainder. |
| [`Div`](#tbigint-div) | Replaces the dividend with its integer quotient. |
| [`Mod`](#tbigint-mod) | Replaces the dividend with its remainder. |
| [`Increment`](#tbigint-increment) | Increments the value by one. |
| [`Decrement`](#tbigint-decrement) | Decrements the value by one. |
| [`And`](#tbigint-and) | Applies bitwise AND across all stored limbs. |
| [`Or`](#tbigint-or) | Applies bitwise OR across all stored limbs. |
| [`Xor`](#tbigint-xor) | Applies bitwise XOR across all stored limbs. |
| [`Not`](#tbigint-not) | Inverts all bits of the numeric value. |
| [`ShiftLeft`](#tbigint-shiftleft) | Shifts the complete value left, inserting zero bits. |
| [`ShiftRight`](#tbigint-shiftright) | Shifts the complete value right, inserting zero bits. |
| [`RotateLeft`](#tbigint-rotateleft) | Rotates the complete value left. |
| [`RotateRight`](#tbigint-rotateright) | Rotates the complete value right. |
| [`BitSet`](#tbigint-bitset) | Sets a single bit by its zero-based index. |
| [`BitClear`](#tbigint-bitclear) | Clears a single bit by its zero-based index. |
| [`BitCheck`](#tbigint-bitcheck) | Reads one bit through a bool output pointer. |
| [`Compare`](#tbigint-compare) | Writes a three-way unsigned comparison result. |
| [`Equal`](#tbigint-equal) | Checks numeric equality. |
| [`NotEqual`](#tbigint-notequal) | Checks numeric inequality. |
| [`LessThan`](#tbigint-lessthan) | Checks whether left is less than right. |
| [`GreaterThan`](#tbigint-greaterthan) | Checks whether left is greater than right. |
| [`LessOrEqual`](#tbigint-lessorequal) | Checks less-than-or-equal relation. |
| [`GreaterOrEqual`](#tbigint-greaterorequal) | Checks greater-than-or-equal relation. |
| [`ToCStringBase`](#tbigint-tocstringbase) | Formats an unsigned value as text in base 2–36. |
| [`ToCString`](#tbigint-tocstring) | Formats an unsigned value as decimal integer text. |
| [`IsZero`](#tbigint-iszero) | Tests whether all numeric limbs are zero. |

---

# TBigint Clear

Zeros the fixed-width numeric data.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Clear)(TBIGINT_TYPE(128) *target);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Clear(Type_TBigint128 *target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) *target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires a non-NULL target. Clears the `.byte` numeric member without altering the optional `.api` pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, Clear)(&target);
```


---

# TBigint Init

Initializes the numeric value to zero.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Init)(TBIGINT_TYPE(128) *target);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Init(Type_TBigint128 *target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) *target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

This delegates to Clear. It does not allocate memory or bind a function table to an object declared with plain `{0}`.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, Init)(&target);
```


---

# TBigint Add

Adds source to destination.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Add)(TBIGINT_TYPE(128) * destination, const TBIGINT_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Add(Type_TBigint128 * destination, const Type_TBigint128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBIGINT_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBIGINT_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Unsigned carry beyond the selected width returns `ARITHMETIC_OVERFLOW` and preserves destination on failure.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, destination)
destination.limb[0] = 17;
TBigint(128, source)
source.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Add)(&destination, &source);
```


---

# TBigint Sub

Subtracts source from destination.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Sub)(TBIGINT_TYPE(128) * destination, const TBIGINT_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Sub(Type_TBigint128 * destination, const Type_TBigint128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBIGINT_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBIGINT_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Unsigned underflow returns `ARITHMETIC_OVERFLOW`, preserving destination instead of wrapping.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, destination)
destination.limb[0] = 17;
TBigint(128, source)
source.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Sub)(&destination, &source);
```


---

# TBigint Mul

Multiplies destination by source.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Mul)(TBIGINT_TYPE(128) * destination, const TBIGINT_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Mul(Type_TBigint128 * destination, const Type_TBigint128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBIGINT_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBIGINT_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Returns `ARITHMETIC_OVERFLOW` if the product does not fit within the fixed width; destination is preserved on failure.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, destination)
destination.limb[0] = 17;
TBigint(128, source)
source.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Mul)(&destination, &source);
```


---

# TBigint DivMod

Computes quotient and remainder.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, DivMod)(const TBIGINT_TYPE(128) *dividend, const TBIGINT_TYPE(128) *divisor, TBIGINT_TYPE(128) *outQuotient, TBIGINT_TYPE(128) *outRemainder);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_DivMod(const Type_TBigint128 *dividend, const Type_TBigint128 *divisor, Type_TBigint128 *outQuotient, Type_TBigint128 *outRemainder);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dividend` | `const TBIGINT_TYPE(128) *dividend` | Read-only dividend. |
| `divisor` | `const TBIGINT_TYPE(128) *divisor` | Read-only divisor, must not represent zero. |
| `outQuotient` | `TBIGINT_TYPE(128) *outQuotient` | Required writable quotient, distinct from outRemainder. |
| `outRemainder` | `TBIGINT_TYPE(128) *outRemainder` | Required writable remainder, distinct from outQuotient. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

The public wrapper requires both outputs to be non-NULL and distinct. A zero divisor reports `DIVISION_BY_ZERO`. Outputs are written on success.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, dividend)
dividend.limb[0] = 17;
TBigint(128, divisor)
divisor.limb[0] = 3;
TBigint(128, outQuotient)
TBigint(128, outRemainder)
OPSTATUS status = TBIGINT_FUNC(128, DivMod)(&dividend, &divisor, &outQuotient, &outRemainder);
```


---

# TBigint Div

Replaces the dividend with its integer quotient.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Div)(TBIGINT_TYPE(128) * dividendBigint, const TBIGINT_TYPE(128) * divisorBigint);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Div(Type_TBigint128 * dividendBigint, const Type_TBigint128 * divisorBigint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dividendBigint` | `TBIGINT_TYPE(128) * dividendBigint` | Mutable Bigint dividend. |
| `divisorBigint` | `const TBIGINT_TYPE(128) * divisorBigint` | Read-only Bigint divisor. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Performs unsigned integer division in place. A zero divisor reports `DIVISION_BY_ZERO`.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, dividendBigint)
dividendBigint.limb[0] = 17;
TBigint(128, divisorBigint)
divisorBigint.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Div)(&dividendBigint, &divisorBigint);
```


---

# TBigint Mod

Replaces the dividend with its remainder.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Mod)(TBIGINT_TYPE(128) * dividendBigint, const TBIGINT_TYPE(128) * divisorBigint);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Mod(Type_TBigint128 * dividendBigint, const Type_TBigint128 * divisorBigint);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dividendBigint` | `TBIGINT_TYPE(128) * dividendBigint` | Mutable Bigint dividend. |
| `divisorBigint` | `const TBIGINT_TYPE(128) * divisorBigint` | Read-only Bigint divisor. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Remainder of unsigned division; a zero divisor reports `DIVISION_BY_ZERO`.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, dividendBigint)
dividendBigint.limb[0] = 17;
TBigint(128, divisorBigint)
divisorBigint.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Mod)(&dividendBigint, &divisorBigint);
```


---

# TBigint Increment

Increments the value by one.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Increment)(TBIGINT_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Increment(Type_TBigint128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

The maximum unsigned value returns `ARITHMETIC_OVERFLOW` and is not changed.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, Increment)(&target);
```


---

# TBigint Decrement

Decrements the value by one.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Decrement)(TBIGINT_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Decrement(Type_TBigint128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Zero underflow returns `ARITHMETIC_OVERFLOW` and is not changed.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, Decrement)(&target);
```


---

# TBigint And

Applies bitwise AND across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, And)(TBIGINT_TYPE(128) * destination, const TBIGINT_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_And(Type_TBigint128 * destination, const Type_TBigint128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBIGINT_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBIGINT_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, destination)
destination.limb[0] = 17;
TBigint(128, source)
source.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, And)(&destination, &source);
```


---

# TBigint Or

Applies bitwise OR across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Or)(TBIGINT_TYPE(128) * destination, const TBIGINT_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Or(Type_TBigint128 * destination, const Type_TBigint128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBIGINT_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBIGINT_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, destination)
destination.limb[0] = 17;
TBigint(128, source)
source.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Or)(&destination, &source);
```


---

# TBigint Xor

Applies bitwise XOR across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Xor)(TBIGINT_TYPE(128) * destination, const TBIGINT_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Xor(Type_TBigint128 * destination, const Type_TBigint128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TBIGINT_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TBIGINT_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, destination)
destination.limb[0] = 17;
TBigint(128, source)
source.limb[0] = 3;
OPSTATUS status = TBIGINT_FUNC(128, Xor)(&destination, &source);
```


---

# TBigint Not

Inverts all bits of the numeric value.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Not)(TBIGINT_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Not(Type_TBigint128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bitwise NOT is not signed negation; no sign field is represented.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, Not)(&target);
```


---

# TBigint ShiftLeft

Shifts the complete value left, inserting zero bits.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, ShiftLeft)(TBIGINT_TYPE(128) * target, unsigned bitCount);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_ShiftLeft(Type_TBigint128 * target, unsigned bitCount);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) * target` | Mutable numeric object. |
| `bitCount` | `unsigned bitCount` | Logical shift count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

High bits are discarded without arithmetic overflow status. `bitCount >= width` clears numeric bits and returns SUCCESS.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, ShiftLeft)(&target, 3U);
```


---

# TBigint ShiftRight

Shifts the complete value right, inserting zero bits.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, ShiftRight)(TBIGINT_TYPE(128) * target, unsigned bitCount);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_ShiftRight(Type_TBigint128 * target, unsigned bitCount);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) * target` | Mutable numeric object. |
| `bitCount` | `unsigned bitCount` | Logical shift count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Logical right shift. `bitCount >= width` clears numeric bits and returns SUCCESS.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, ShiftRight)(&target, 3U);
```


---

# TBigint RotateLeft

Rotates the complete value left.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, RotateLeft)(TBIGINT_TYPE(128) *target, unsigned shift);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_RotateLeft(Type_TBigint128 *target, unsigned shift);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) *target` | Mutable numeric object. |
| `shift` | `unsigned shift` | Rotation count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bits wrap around the numeric width. Rotation count is reduced modulo 128, 256, 512 or 1024.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, RotateLeft)(&target, 3U);
```


---

# TBigint RotateRight

Rotates the complete value right.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, RotateRight)(TBIGINT_TYPE(128) *target, unsigned shift);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_RotateRight(Type_TBigint128 *target, unsigned shift);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) *target` | Mutable numeric object. |
| `shift` | `unsigned shift` | Rotation count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bits wrap around the numeric width. Rotation count is reduced modulo the width.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, RotateRight)(&target, 3U);
```


---

# TBigint BitSet

Sets a single bit by its zero-based index.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, BitSet)(TBIGINT_TYPE(128) *target, uint32_t bit);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_BitSet(Type_TBigint128 *target, uint32_t bit);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Index 0 is the least-significant bit of `limb[0]`. Indices outside 0–127 (for width 128) return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, BitSet)(&target, 3U);
```


---

# TBigint BitClear

Clears a single bit by its zero-based index.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, BitClear)(TBIGINT_TYPE(128) *target, uint32_t bit);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_BitClear(Type_TBigint128 *target, uint32_t bit);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TBIGINT_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Indices outside 0–127 (for width 128) return OUT_OF_RANGE and leave the value unchanged.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
OPSTATUS status = TBIGINT_FUNC(128, BitClear)(&target, 3U);
```


---

# TBigint BitCheck

Reads one bit through a bool output pointer.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, BitCheck)(const TBIGINT_TYPE(128) *target, uint32_t bit, bool *outResult);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_BitCheck(const Type_TBigint128 *target, uint32_t bit, bool *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `const TBIGINT_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |
| `outResult` | `bool *outResult` | Required writable bool pointer. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires both target and outResult. Out-of-range indexes return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
bool checked = false;
OPSTATUS status = TBIGINT_FUNC(128, BitCheck)(&target, 3U, &checked);
```


---

# TBigint Compare

Writes a three-way unsigned comparison result.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, Compare)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right, CMPOUT *result);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_Compare(const Type_TBigint128 * left, const Type_TBigint128 * right, CMPOUT *result);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |
| `result` | `CMPOUT *result` | Required writable CMPOUT pointer. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Returns OPSTATUS, not CMPOUT. Writes `COMPARISON_LOWER_CONST`, `COMPARISON_EQUAL_CONST` or `COMPARISON_HIGHER_CONST` via result.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
CMPOUT comparison = COMPARISON_EQUAL_CONST;
OPSTATUS status = TBIGINT_FUNC(128, Compare)(&left, &right, &comparison);
```


---

# TBigint Equal

Checks numeric equality.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, Equal)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TBigint128_Equal(const Type_TBigint128 * left, const Type_TBigint128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Returns bool. NULL pointer operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
bool answer = TBIGINT_FUNC(128, Equal)(&left, &right);
```


---

# TBigint NotEqual

Checks numeric inequality.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, NotEqual)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TBigint128_NotEqual(const Type_TBigint128 * left, const Type_TBigint128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Returns bool. NULL pointer operands return false rather than true.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
bool answer = TBIGINT_FUNC(128, NotEqual)(&left, &right);
```


---

# TBigint LessThan

Checks whether left is less than right.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, LessThan)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TBigint128_LessThan(const Type_TBigint128 * left, const Type_TBigint128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Uses unsigned high-limb-first comparison; NULL operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
bool answer = TBIGINT_FUNC(128, LessThan)(&left, &right);
```


---

# TBigint GreaterThan

Checks whether left is greater than right.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, GreaterThan)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TBigint128_GreaterThan(const Type_TBigint128 * left, const Type_TBigint128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Uses unsigned high-limb-first comparison; NULL operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
bool answer = TBIGINT_FUNC(128, GreaterThan)(&left, &right);
```


---

# TBigint LessOrEqual

Checks less-than-or-equal relation.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, LessOrEqual)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TBigint128_LessOrEqual(const Type_TBigint128 * left, const Type_TBigint128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

NULL pointer operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
bool answer = TBIGINT_FUNC(128, LessOrEqual)(&left, &right);
```


---

# TBigint GreaterOrEqual

Checks greater-than-or-equal relation.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, GreaterOrEqual)(const TBIGINT_TYPE(128) * left, const TBIGINT_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TBigint128_GreaterOrEqual(const Type_TBigint128 * left, const Type_TBigint128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TBIGINT_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TBIGINT_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

NULL pointer operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, left)
left.limb[0] = 17;
TBigint(128, right)
right.limb[0] = 3;
bool answer = TBIGINT_FUNC(128, GreaterOrEqual)(&left, &right);
```


---

# TBigint ToCStringBase

Formats an unsigned value as text in base 2–36.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, ToCStringBase)(const TBIGINT_TYPE(128) *value, char *buffer, size_t bufferSize, unsigned base);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_ToCStringBase(const Type_TBigint128 *value, char *buffer, size_t bufferSize, unsigned base);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `const TBIGINT_TYPE(128) *value` | Read-only value to format. |
| `buffer` | `char *buffer` | Writable ASCII buffer for digits and NUL terminator. |
| `bufferSize` | `size_t bufferSize` | Capacity in bytes, including the NUL terminator. |
| `base` | `unsigned base` | Conversion radix in the inclusive range 2–36. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires valid value/buffer and a buffer including the NUL terminator. Radix 2–36, uppercase A–Z, short buffers return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, value)
value.limb[0] = 17;
char buffer[144];
OPSTATUS status = TBIGINT_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 16U);
```


---

# TBigint ToCString

Formats an unsigned value as decimal integer text.

### Syntax

#### Macro form

```c
OPSTATUS TBIGINT_FUNC(128, ToCString)(const TBIGINT_TYPE(128) *value, char *buffer, size_t bufferSize);
```

#### Direct form

```c
OPSTATUS Type_TBigint128_ToCString(const Type_TBigint128 *value, char *buffer, size_t bufferSize);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `const TBIGINT_TYPE(128) *value` | Read-only value to format. |
| `buffer` | `char *buffer` | Writable ASCII buffer for digits and NUL terminator. |
| `bufferSize` | `size_t bufferSize` | Capacity in bytes, including the NUL terminator. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Equivalent to ToCStringBase(...,10). Even for TDecimal, no decimal point or fraction is added.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, value)
value.limb[0] = 17;
char buffer[144];
OPSTATUS status = TBIGINT_FUNC(128, ToCString)(&value, buffer, sizeof(buffer));
```


---

# TBigint IsZero

Tests whether all numeric limbs are zero.

### Syntax

#### Macro form

```c
bool TBIGINT_FUNC(128, IsZero)(const TBIGINT_TYPE(128) * target);
```

#### Direct form

```c
bool Type_TBigint128_IsZero(const Type_TBigint128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `const TBIGINT_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Returns false for NULL; ignores any function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TBigint(128, target)
target.limb[0] = 17;
bool answer = TBIGINT_FUNC(128, IsZero)(&target);
```


---

# TDecimal package

Header: `Cosmeron/Modules/Type/TDecimal.h`

A separate type family currently implemented with the **same unsigned binary-limb arithmetic** as TBigint. It does not encode scaled decimal fractions.

Built-in suffixes are `128`, `256`, `512`, and `1024`.

### Function summary

| Function | Description |
| --- | --- |
| [`Clear`](#tdecimal-clear) | Zeros the fixed-width numeric data. |
| [`Init`](#tdecimal-init) | Initializes the numeric value to zero. |
| [`Add`](#tdecimal-add) | Adds source to destination. |
| [`Sub`](#tdecimal-sub) | Subtracts source from destination. |
| [`Mul`](#tdecimal-mul) | Multiplies destination by source. |
| [`DivMod`](#tdecimal-divmod) | Computes quotient and remainder. |
| [`Div`](#tdecimal-div) | Replaces the dividend with its integer quotient. |
| [`Mod`](#tdecimal-mod) | Replaces the dividend with its remainder. |
| [`Increment`](#tdecimal-increment) | Increments the value by one. |
| [`Decrement`](#tdecimal-decrement) | Decrements the value by one. |
| [`And`](#tdecimal-and) | Applies bitwise AND across all stored limbs. |
| [`Or`](#tdecimal-or) | Applies bitwise OR across all stored limbs. |
| [`Xor`](#tdecimal-xor) | Applies bitwise XOR across all stored limbs. |
| [`Not`](#tdecimal-not) | Inverts all bits of the numeric value. |
| [`ShiftLeft`](#tdecimal-shiftleft) | Shifts the complete value left, inserting zero bits. |
| [`ShiftRight`](#tdecimal-shiftright) | Shifts the complete value right, inserting zero bits. |
| [`RotateLeft`](#tdecimal-rotateleft) | Rotates the complete value left. |
| [`RotateRight`](#tdecimal-rotateright) | Rotates the complete value right. |
| [`BitSet`](#tdecimal-bitset) | Sets a single bit by its zero-based index. |
| [`BitClear`](#tdecimal-bitclear) | Clears a single bit by its zero-based index. |
| [`BitCheck`](#tdecimal-bitcheck) | Reads one bit through a bool output pointer. |
| [`Compare`](#tdecimal-compare) | Writes a three-way unsigned comparison result. |
| [`Equal`](#tdecimal-equal) | Checks numeric equality. |
| [`NotEqual`](#tdecimal-notequal) | Checks numeric inequality. |
| [`LessThan`](#tdecimal-lessthan) | Checks whether left is less than right. |
| [`GreaterThan`](#tdecimal-greaterthan) | Checks whether left is greater than right. |
| [`LessOrEqual`](#tdecimal-lessorequal) | Checks less-than-or-equal relation. |
| [`GreaterOrEqual`](#tdecimal-greaterorequal) | Checks greater-than-or-equal relation. |
| [`ToCStringBase`](#tdecimal-tocstringbase) | Formats an unsigned value as text in base 2–36. |
| [`ToCString`](#tdecimal-tocstring) | Formats an unsigned value as decimal integer text. |
| [`IsZero`](#tdecimal-iszero) | Tests whether all numeric limbs are zero. |

---

# TDecimal Clear

Zeros the fixed-width numeric data.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Clear)(TDECIMAL_TYPE(128) *target);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Clear(Type_TDecimal128 *target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) *target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires a non-NULL target. Clears the `.byte` numeric member without altering the optional `.api` pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, Clear)(&target);
```


---

# TDecimal Init

Initializes the numeric value to zero.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Init)(TDECIMAL_TYPE(128) *target);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Init(Type_TDecimal128 *target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) *target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

This delegates to Clear. It does not allocate memory or bind a function table to an object declared with plain `{0}`.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, Init)(&target);
```


---

# TDecimal Add

Adds source to destination.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Add)(TDECIMAL_TYPE(128) * destination, const TDECIMAL_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Add(Type_TDecimal128 * destination, const Type_TDecimal128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TDECIMAL_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TDECIMAL_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Unsigned carry beyond the selected width returns `ARITHMETIC_OVERFLOW` and preserves destination on failure.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, destination)
destination.limb[0] = 17;
TDecimal(128, source)
source.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Add)(&destination, &source);
```


---

# TDecimal Sub

Subtracts source from destination.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Sub)(TDECIMAL_TYPE(128) * destination, const TDECIMAL_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Sub(Type_TDecimal128 * destination, const Type_TDecimal128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TDECIMAL_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TDECIMAL_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Unsigned underflow returns `ARITHMETIC_OVERFLOW`, preserving destination instead of wrapping.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, destination)
destination.limb[0] = 17;
TDecimal(128, source)
source.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Sub)(&destination, &source);
```


---

# TDecimal Mul

Multiplies destination by source.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Mul)(TDECIMAL_TYPE(128) * destination, const TDECIMAL_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Mul(Type_TDecimal128 * destination, const Type_TDecimal128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TDECIMAL_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TDECIMAL_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Returns `ARITHMETIC_OVERFLOW` if the product does not fit within the fixed width; destination is preserved on failure.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, destination)
destination.limb[0] = 17;
TDecimal(128, source)
source.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Mul)(&destination, &source);
```


---

# TDecimal DivMod

Computes quotient and remainder.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, DivMod)(const TDECIMAL_TYPE(128) *dividend, const TDECIMAL_TYPE(128) *divisor, TDECIMAL_TYPE(128) *outQuotient, TDECIMAL_TYPE(128) *outRemainder);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_DivMod(const Type_TDecimal128 *dividend, const Type_TDecimal128 *divisor, Type_TDecimal128 *outQuotient, Type_TDecimal128 *outRemainder);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dividend` | `const TDECIMAL_TYPE(128) *dividend` | Read-only dividend. |
| `divisor` | `const TDECIMAL_TYPE(128) *divisor` | Read-only divisor, must not represent zero. |
| `outQuotient` | `TDECIMAL_TYPE(128) *outQuotient` | Required writable quotient, distinct from outRemainder. |
| `outRemainder` | `TDECIMAL_TYPE(128) *outRemainder` | Required writable remainder, distinct from outQuotient. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

The public wrapper requires both outputs to be non-NULL and distinct. A zero divisor reports `DIVISION_BY_ZERO`. Outputs are written on success.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, dividend)
dividend.limb[0] = 17;
TDecimal(128, divisor)
divisor.limb[0] = 3;
TDecimal(128, outQuotient)
TDecimal(128, outRemainder)
OPSTATUS status = TDECIMAL_FUNC(128, DivMod)(&dividend, &divisor, &outQuotient, &outRemainder);
```


---

# TDecimal Div

Replaces the dividend with its integer quotient.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Div)(TDECIMAL_TYPE(128) * dividendDecimal, const TDECIMAL_TYPE(128) * divisorDecimal);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Div(Type_TDecimal128 * dividendDecimal, const Type_TDecimal128 * divisorDecimal);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dividendDecimal` | `TDECIMAL_TYPE(128) * dividendDecimal` | Mutable TDecimal dividend. |
| `divisorDecimal` | `const TDECIMAL_TYPE(128) * divisorDecimal` | Read-only TDecimal divisor. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Performs unsigned integer division in place. A zero divisor reports `DIVISION_BY_ZERO`.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, dividendDecimal)
dividendDecimal.limb[0] = 17;
TDecimal(128, divisorDecimal)
divisorDecimal.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Div)(&dividendDecimal, &divisorDecimal);
```


---

# TDecimal Mod

Replaces the dividend with its remainder.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Mod)(TDECIMAL_TYPE(128) * dividendDecimal, const TDECIMAL_TYPE(128) * divisorDecimal);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Mod(Type_TDecimal128 * dividendDecimal, const Type_TDecimal128 * divisorDecimal);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dividendDecimal` | `TDECIMAL_TYPE(128) * dividendDecimal` | Mutable TDecimal dividend. |
| `divisorDecimal` | `const TDECIMAL_TYPE(128) * divisorDecimal` | Read-only TDecimal divisor. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Remainder of unsigned division; a zero divisor reports `DIVISION_BY_ZERO`.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, dividendDecimal)
dividendDecimal.limb[0] = 17;
TDecimal(128, divisorDecimal)
divisorDecimal.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Mod)(&dividendDecimal, &divisorDecimal);
```


---

# TDecimal Increment

Increments the value by one.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Increment)(TDECIMAL_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Increment(Type_TDecimal128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

The maximum unsigned value returns `ARITHMETIC_OVERFLOW` and is not changed.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, Increment)(&target);
```


---

# TDecimal Decrement

Decrements the value by one.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Decrement)(TDECIMAL_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Decrement(Type_TDecimal128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Zero underflow returns `ARITHMETIC_OVERFLOW` and is not changed.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, Decrement)(&target);
```


---

# TDecimal And

Applies bitwise AND across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, And)(TDECIMAL_TYPE(128) * destination, const TDECIMAL_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_And(Type_TDecimal128 * destination, const Type_TDecimal128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TDECIMAL_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TDECIMAL_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, destination)
destination.limb[0] = 17;
TDecimal(128, source)
source.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, And)(&destination, &source);
```


---

# TDecimal Or

Applies bitwise OR across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Or)(TDECIMAL_TYPE(128) * destination, const TDECIMAL_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Or(Type_TDecimal128 * destination, const Type_TDecimal128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TDECIMAL_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TDECIMAL_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, destination)
destination.limb[0] = 17;
TDecimal(128, source)
source.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Or)(&destination, &source);
```


---

# TDecimal Xor

Applies bitwise XOR across all stored limbs.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Xor)(TDECIMAL_TYPE(128) * destination, const TDECIMAL_TYPE(128) * source);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Xor(Type_TDecimal128 * destination, const Type_TDecimal128 * source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `TDECIMAL_TYPE(128) * destination` | Mutable destination object. |
| `source` | `const TDECIMAL_TYPE(128) * source` | Read-only source value. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Only numeric bits are changed, not the function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, destination)
destination.limb[0] = 17;
TDecimal(128, source)
source.limb[0] = 3;
OPSTATUS status = TDECIMAL_FUNC(128, Xor)(&destination, &source);
```


---

# TDecimal Not

Inverts all bits of the numeric value.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Not)(TDECIMAL_TYPE(128) * target);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Not(Type_TDecimal128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bitwise NOT is not signed negation; no sign field is represented.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, Not)(&target);
```


---

# TDecimal ShiftLeft

Shifts the complete value left, inserting zero bits.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, ShiftLeft)(TDECIMAL_TYPE(128) * target, unsigned bitCount);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_ShiftLeft(Type_TDecimal128 * target, unsigned bitCount);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) * target` | Mutable numeric object. |
| `bitCount` | `unsigned bitCount` | Logical shift count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

High bits are discarded without arithmetic overflow status. `bitCount >= width` clears numeric bits and returns SUCCESS.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, ShiftLeft)(&target, 3U);
```


---

# TDecimal ShiftRight

Shifts the complete value right, inserting zero bits.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, ShiftRight)(TDECIMAL_TYPE(128) * target, unsigned bitCount);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_ShiftRight(Type_TDecimal128 * target, unsigned bitCount);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) * target` | Mutable numeric object. |
| `bitCount` | `unsigned bitCount` | Logical shift count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Logical right shift. `bitCount >= width` clears numeric bits and returns SUCCESS.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, ShiftRight)(&target, 3U);
```


---

# TDecimal RotateLeft

Rotates the complete value left.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, RotateLeft)(TDECIMAL_TYPE(128) *target, unsigned shift);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_RotateLeft(Type_TDecimal128 *target, unsigned shift);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) *target` | Mutable numeric object. |
| `shift` | `unsigned shift` | Rotation count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bits wrap around the numeric width. Rotation count is reduced modulo 128, 256, 512 or 1024.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, RotateLeft)(&target, 3U);
```


---

# TDecimal RotateRight

Rotates the complete value right.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, RotateRight)(TDECIMAL_TYPE(128) *target, unsigned shift);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_RotateRight(Type_TDecimal128 *target, unsigned shift);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) *target` | Mutable numeric object. |
| `shift` | `unsigned shift` | Rotation count in bits. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Bits wrap around the numeric width. Rotation count is reduced modulo the width.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, RotateRight)(&target, 3U);
```


---

# TDecimal BitSet

Sets a single bit by its zero-based index.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, BitSet)(TDECIMAL_TYPE(128) *target, uint32_t bit);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_BitSet(Type_TDecimal128 *target, uint32_t bit);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Index 0 is the least-significant bit of `limb[0]`. Indices outside 0–127 (for width 128) return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, BitSet)(&target, 3U);
```


---

# TDecimal BitClear

Clears a single bit by its zero-based index.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, BitClear)(TDECIMAL_TYPE(128) *target, uint32_t bit);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_BitClear(Type_TDecimal128 *target, uint32_t bit);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `TDECIMAL_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Indices outside 0–127 (for width 128) return OUT_OF_RANGE and leave the value unchanged.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
OPSTATUS status = TDECIMAL_FUNC(128, BitClear)(&target, 3U);
```


---

# TDecimal BitCheck

Reads one bit through a bool output pointer.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, BitCheck)(const TDECIMAL_TYPE(128) *target, uint32_t bit, bool *outResult);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_BitCheck(const Type_TDecimal128 *target, uint32_t bit, bool *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `const TDECIMAL_TYPE(128) *target` | Mutable numeric object. |
| `bit` | `uint32_t bit` | Zero-based bit position. |
| `outResult` | `bool *outResult` | Required writable bool pointer. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires both target and outResult. Out-of-range indexes return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
bool checked = false;
OPSTATUS status = TDECIMAL_FUNC(128, BitCheck)(&target, 3U, &checked);
```


---

# TDecimal Compare

Writes a three-way unsigned comparison result.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, Compare)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right, CMPOUT *result);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_Compare(const Type_TDecimal128 * left, const Type_TDecimal128 * right, CMPOUT *result);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |
| `result` | `CMPOUT *result` | Required writable CMPOUT pointer. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Returns OPSTATUS, not CMPOUT. Writes `COMPARISON_LOWER_CONST`, `COMPARISON_EQUAL_CONST` or `COMPARISON_HIGHER_CONST` via result.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
CMPOUT comparison = COMPARISON_EQUAL_CONST;
OPSTATUS status = TDECIMAL_FUNC(128, Compare)(&left, &right, &comparison);
```


---

# TDecimal Equal

Checks numeric equality.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, Equal)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TDecimal128_Equal(const Type_TDecimal128 * left, const Type_TDecimal128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Returns bool. NULL pointer operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
bool answer = TDECIMAL_FUNC(128, Equal)(&left, &right);
```


---

# TDecimal NotEqual

Checks numeric inequality.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, NotEqual)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TDecimal128_NotEqual(const Type_TDecimal128 * left, const Type_TDecimal128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Returns bool. NULL pointer operands return false rather than true.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
bool answer = TDECIMAL_FUNC(128, NotEqual)(&left, &right);
```


---

# TDecimal LessThan

Checks whether left is less than right.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, LessThan)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TDecimal128_LessThan(const Type_TDecimal128 * left, const Type_TDecimal128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Uses unsigned high-limb-first comparison; NULL operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
bool answer = TDECIMAL_FUNC(128, LessThan)(&left, &right);
```


---

# TDecimal GreaterThan

Checks whether left is greater than right.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, GreaterThan)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TDecimal128_GreaterThan(const Type_TDecimal128 * left, const Type_TDecimal128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Uses unsigned high-limb-first comparison; NULL operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
bool answer = TDECIMAL_FUNC(128, GreaterThan)(&left, &right);
```


---

# TDecimal LessOrEqual

Checks less-than-or-equal relation.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, LessOrEqual)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TDecimal128_LessOrEqual(const Type_TDecimal128 * left, const Type_TDecimal128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

NULL pointer operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
bool answer = TDECIMAL_FUNC(128, LessOrEqual)(&left, &right);
```


---

# TDecimal GreaterOrEqual

Checks greater-than-or-equal relation.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, GreaterOrEqual)(const TDECIMAL_TYPE(128) * left, const TDECIMAL_TYPE(128) * right);
```

#### Direct form

```c
bool Type_TDecimal128_GreaterOrEqual(const Type_TDecimal128 * left, const Type_TDecimal128 * right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `const TDECIMAL_TYPE(128) * left` | Read-only comparison operand. |
| `right` | `const TDECIMAL_TYPE(128) * right` | Read-only comparison operand. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

NULL pointer operands return false.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, left)
left.limb[0] = 17;
TDecimal(128, right)
right.limb[0] = 3;
bool answer = TDECIMAL_FUNC(128, GreaterOrEqual)(&left, &right);
```


---

# TDecimal ToCStringBase

Formats an unsigned value as text in base 2–36.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, ToCStringBase)(const TDECIMAL_TYPE(128) *value, char *buffer, size_t bufferSize, unsigned base);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_ToCStringBase(const Type_TDecimal128 *value, char *buffer, size_t bufferSize, unsigned base);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `const TDECIMAL_TYPE(128) *value` | Read-only value to format. |
| `buffer` | `char *buffer` | Writable ASCII buffer for digits and NUL terminator. |
| `bufferSize` | `size_t bufferSize` | Capacity in bytes, including the NUL terminator. |
| `base` | `unsigned base` | Conversion radix in the inclusive range 2–36. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Requires valid value/buffer and a buffer including the NUL terminator. Radix 2–36, uppercase A–Z, short buffers return OUT_OF_RANGE.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, value)
value.limb[0] = 17;
char buffer[144];
OPSTATUS status = TDECIMAL_FUNC(128, ToCStringBase)(&value, buffer, sizeof(buffer), 16U);
```


---

# TDecimal ToCString

Formats an unsigned value as decimal integer text.

### Syntax

#### Macro form

```c
OPSTATUS TDECIMAL_FUNC(128, ToCString)(const TDECIMAL_TYPE(128) *value, char *buffer, size_t bufferSize);
```

#### Direct form

```c
OPSTATUS Type_TDecimal128_ToCString(const Type_TDecimal128 *value, char *buffer, size_t bufferSize);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `const TDECIMAL_TYPE(128) *value` | Read-only value to format. |
| `buffer` | `char *buffer` | Writable ASCII buffer for digits and NUL terminator. |
| `bufferSize` | `size_t bufferSize` | Capacity in bytes, including the NUL terminator. |

---

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` or an operation-specific failure. For BitCheck and Compare, the result is supplied through an output pointer.

---

### Remarks

Equivalent to ToCStringBase(...,10). Even for TDecimal, no decimal point or fraction is added.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, value)
value.limb[0] = 17;
char buffer[144];
OPSTATUS status = TDECIMAL_FUNC(128, ToCString)(&value, buffer, sizeof(buffer));
```


---

# TDecimal IsZero

Tests whether all numeric limbs are zero.

### Syntax

#### Macro form

```c
bool TDECIMAL_FUNC(128, IsZero)(const TDECIMAL_TYPE(128) * target);
```

#### Direct form

```c
bool Type_TDecimal128_IsZero(const Type_TDecimal128 * target);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `const TDECIMAL_TYPE(128) * target` | Mutable numeric object. |

---

### Return value

`bool`: the predicate result. Invalid pointer arguments generally return false.

---

### Remarks

Returns false for NULL; ignores any function-table pointer.

The declaration above uses the `128` suffix. The same operation is generated for `256`, `512`, and `1024` bits.

---

### Example

```c
TDecimal(128, target)
target.limb[0] = 17;
bool answer = TDECIMAL_FUNC(128, IsZero)(&target);
```

---

# Complete examples

## TBlock: 256-bit flags

```c
#include "Cosmeron/Modules/Type/TBlock.h"

int main(void) {
    TBlock(256, flags)
    bool enabled = false;

    OPSTATUS status = TBLOCK_FUNC(256, BitSet)(&flags, 200U);
    if (status == STATUS_CONST(SUCCESS))
        status = TBLOCK_FUNC(256, BitCheck)(&flags, 200U, &enabled);
    if (status == STATUS_CONST(SUCCESS))
        status = TBLOCK_FUNC(256, RotateLeft)(&flags, 1U);

    return status == STATUS_CONST(SUCCESS) && enabled ? 0 : 1;
}
```

The field `limb[0]` contains the low 64 bits. For example, bit 200 belongs to the fourth 64-bit limb (index 3), bit position 8 in that limb.

## TBigint: checked addition and base-10 conversion

```c
#include <string.h>
#include "Cosmeron/Modules/Type/TBigint.h"

int main(void) {
    TBigint(128, amount)
    TBigint(128, increment)

    amount.limb[0] = 123;
    increment.limb[0] = 456;

    OPSTATUS status = TBIGINT_FUNC(128, Add)(&amount, &increment);
    if (status != STATUS_CONST(SUCCESS))
        return 1;

    char buffer[48];
    status = TBIGINT_FUNC(128, ToCString)(
        &amount, buffer, sizeof buffer);

    return status == STATUS_CONST(SUCCESS) &&
           strcmp(buffer, "579") == 0 ? 0 : 2;
}
```

The destination's numeric storage is preserved when checked addition reports overflow, making explicit error handling possible without an extra copy.

## TBigint: quotient and remainder

```c
#include "Cosmeron/Modules/Type/TBigint.h"

int main(void) {
    TBigint(128, dividend)
    TBigint(128, divisor)
    TBigint(128, quotient)
    TBigint(128, remainder)
    dividend.limb[0] = 17;
    divisor.limb[0] = 3;

    OPSTATUS status = TBIGINT_FUNC(128, DivMod)(
        &dividend, &divisor, &quotient, &remainder);

    return status == STATUS_CONST(SUCCESS) &&
           quotient.limb[0] == 5 &&
           remainder.limb[0] == 2 ? 0 : 1;
}
```

The public `DivMod` operation requires **both** quotient and remainder outputs, even though the internal division algorithm also supports computing only one output. For single-result division, call the public `Div` or `Mod` operation instead.

## TDecimal: current integer-only representation

```c
#include <string.h>
#include "Cosmeron/Modules/Type/TDecimal.h"

int main(void) {
    TDecimal(128, value)
    value.limb[0] = 1250; /* Integer 1250, NOT 12.50 */

    char buffer[48];
    OPSTATUS status = TDECIMAL_FUNC(128, ToCString)(
        &value, buffer, sizeof buffer);

    return status == STATUS_CONST(SUCCESS) &&
           strcmp(buffer, "1250") == 0 ? 0 : 1;
}
```

The current `TDecimal` code does not store currency-scale or decimal fractions. For example, if an application stores values in cents, the unit scale must be **handled by the application**.

---

# Function tables

Each package can expose an optional `api` pointer to a static per-width function table:

| Family | Macro type | Example member |
| --- | --- | --- |
| TBlock | `TBLOCK_FUNCTION_TABLE_TYPE(128)` | `bits.api->bitSet(&bits, 3U)` |
| TBigint | `TBIGINT_FUNCTION_TABLE_TYPE(128)` | `bigint.api->add(&bigint, &other)` |
| TDecimal | `TDECIMAL_FUNCTION_TABLE_TYPE(128)` | `decimal.api->toCString(&decimal, text, sizeof text)` |

The members are lowerCamelCase forms of the operations. `TBlock` includes `clear`, logical/rotate methods and bit access. `TBigint` and `TDecimal` additionally include `init`, arithmetic, comparisons, `isZero`, `toCString` and `toCStringBase`.

```c
#include "Cosmeron/Modules/Type/TBigint.h"

int main(void) {
    TBigint(128, value)
    TBigint(128, other)
    value.limb[0] = 7;
    other.limb[0] = 8;

    OPSTATUS status = value.api->add(&value, &other);
    return status == STATUS_CONST(SUCCESS) &&
           value.limb[0] == 15 ? 0 : 1;
}
```

The example above assumes function tables are enabled. With `TYPE_DISABLE_FUNCTION_TABLE` defined before inclusion, the `api` member is omitted, and `TBIGINT_FUNC`/direct calls must be used instead.

An object declared as `TBIGINT_TYPE(128) value = {0};` does **not** automatically bind a function table even when the table type is enabled. Use the `TBigint(128, value)` declaration macro to bind it.

---

# Important limitations

- **Unsigned semantics:** TBigint and TDecimal use unsigned limbs. Subtracting a larger number returns `ARITHMETIC_OVERFLOW`, rather than producing a signed negative value.
- **Fixed precision:** arithmetic does not grow beyond its declared width. To store a larger number, choose a wider specialization in advance.
- **Binary representation:** TDecimal has no base-10 limbs, implicit decimal scale, exponent or rounding. Its current implementation shares the binary algorithms of TBigint.
- **Formatting only:** `ToCString` and `ToCStringBase` write integer strings; no corresponding string parser or decimal-fraction formatter is exposed.
- **Logical shifts:** shifts do not report lost high bits; they discard them. Rotation wraps instead of discarding.
- **Endian portability:** the numeric limb order is specified by the implementation, but raw bytes overlap native `uint64_t` storage and are not a portable serialization format.
- **Structure layout:** function-table-enabled structures include an extra pointer, changing `sizeof` and ABI. Keep compile-time settings consistent across translation units.
- **Performance:** the multi-limb algorithms are intentionally simple; no claim is made that their performance matches specialized optimized big-number libraries.
- **No additional linker libraries:** the implementation is included by the headers and uses the other Cosmeron core/Bit facilities.

---

# Tests

The existing test suite exercises all four widths, arithmetic and overflow boundaries, bit operations, quotient/remainder, comparisons, formatting, function tables, no-table layout, header aggregation, and fundamental aliases.

```sh
make -C Codespace/Tests/Type run
```

The Type test Makefile uses C11 with `-Wall -Wextra -Wpedantic -Werror`, and has no additional default link libraries. This is the available test command; a new compilation has **not** been performed during this documentation update.

---

# Notes

- The public headers declare **74 operation templates**, yielding **296 concrete functions** across four widths.
- `Fundamental.h` contains typedefs only; it does not add runtime functions.
- `*_PROTOTYPE`, `*_IMPLEMENT`, `*_DECLARE` and `TYPE_FUNC` are code-generation or naming macros rather than separate independently callable API operations.
- No source-code files were changed by this documentation.
