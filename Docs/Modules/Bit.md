# Bit

The **Bit** module provides a compact set of utilities for manipulating and inspecting individual bits, masks, rotations, and contiguous bit fields.

It is intended for code where an integer represents more than a numeric value: flags, permissions, packed data, hardware-style fields, state masks, binary protocols, or any other structure in which individual bits carry meaning.

The API is available for unsigned integers of 8, 16, 32, and 64 bits.

```c
#include "Cosmeron/Modules/Bit/Bit.h"
```

---

# Overview

The Bit API is instantiated for the standard unsigned integer widths.

| Width | C type | Macro form | Direct form |
| --- | --- | --- | --- |
| 8-bit | `uint8_t` | `BIT_FUNC(8, ...)` | `Bit_8_...` |
| 16-bit | `uint16_t` | `BIT_FUNC(16, ...)` | `Bit_16_...` |
| 32-bit | `uint32_t` | `BIT_FUNC(32, ...)` | `Bit_32_...` |
| 64-bit | `uint64_t` | `BIT_FUNC(64, ...)` | `Bit_64_...` |

For example, the following calls refer to the same function.

### Macro form

```c
BIT_FUNC(32, Popcount)(value);
```

### Direct form

```c
Bit_32_Popcount(value);
```

The macro form follows the configured Cosmeron namespace automatically and is therefore especially useful in reusable code.

If a custom namespace is configured:

```c
#define COSMERON_NAMESPACE MyProject

#include "Cosmeron/Modules/Bit/Bit.h"
```

the direct symbol becomes:

```c
MyProject_Bit_32_Popcount(value);
```

while the macro form remains unchanged:

```c
BIT_FUNC(32, Popcount)(value);
```

---

# API reference

## Function summary

| Function | Description | Modifies input |
| --- | --- | --- |
| [`Set`](#set) | Sets one bit | Yes |
| [`Clear`](#clear) | Clears one bit | Yes |
| [`Flip`](#flip) | Inverts one bit | Yes |
| [`Check`](#check) | Tests one bit | No |
| [`MaskSet`](#maskset) | Sets all bits selected by a mask | Yes |
| [`MaskClear`](#maskclear) | Clears all bits selected by a mask | Yes |
| [`MaskFlip`](#maskflip) | Inverts all bits selected by a mask | Yes |
| [`MaskCheckAll`](#maskcheckall) | Tests whether all selected bits are set | No |
| [`MaskCheckAny`](#maskcheckany) | Tests whether any selected bit is set | No |
| [`Popcount`](#popcount) | Counts set bits | No |
| [`IsPowerOfTwo`](#ispoweroftwo) | Tests whether a value is a power of two | No |
| [`IndexLSB`](#indexlsb) | Finds the least significant set bit | No |
| [`IndexMSB`](#indexmsb) | Finds the most significant set bit | No |
| [`RotateLeft`](#rotateleft) | Rotates bits to the left | No |
| [`RotateRight`](#rotateright) | Rotates bits to the right | No |
| [`Extract`](#extract) | Extracts a contiguous field | No |
| [`Insert`](#insert) | Replaces a contiguous field | Yes |

All functions are instantiated for 8-, 16-, 32-, and 64-bit unsigned integer types.


---

# Single-bit operations

Single-bit operations work with one bit identified by its **zero-based position**.

Bit `0` is always the least significant bit.

```c
/* Bit index:  7 6 5 4 3 2 1 0
 * Value:      1 0 1 1 0 1 0 0
 */
```

This makes these functions useful for compact flag sets in which each bit represents an independent state or property.

---

# Set

Sets one bit to `1`.

`Set` is used to enable a binary state without disturbing any of the other states stored in the same integer.

### Syntax

#### Macro form

```c
BIT_FUNC(width, Set)(&value, bit);
```

#### Direct form

```c
Bit_width_Set(&value, bit);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to modify. |
| `bit` | Zero-based index of the bit to set. |

Valid indexes depend on the selected width:

| Width | Valid indexes |
| --- | --- |
| 8-bit | `0`–`7` |
| 16-bit | `0`–`15` |
| 32-bit | `0`–`31` |
| 64-bit | `0`–`63` |

---

### Return value

None.

---

### Remarks

- Only the selected bit is modified. All other bits retain their previous state.
- If `bit` is outside the width of the selected integer type, the operation is ignored.
- `value` must point to a valid object.

---

### Example

```c
uint8_t flags = 0;

BIT_FUNC(8, Set)(&flags, 2);

/* flags == 0b00000100 */
```

A common use is enabling named flags:

```c
enum {
    FLAG_VISIBLE  = 0,
    FLAG_SELECTED = 1,
    FLAG_DISABLED = 2
};

uint32_t flags = 0;

BIT_FUNC(32, Set)(&flags, FLAG_VISIBLE);
BIT_FUNC(32, Set)(&flags, FLAG_SELECTED);
```

---

# Clear

Clears one bit to `0`.

`Clear` is the counterpart of `Set`: it disables one selected state while leaving every other bit untouched.

### Syntax

#### Macro form

```c
BIT_FUNC(width, Clear)(&value, bit);
```

#### Direct form

```c
Bit_width_Clear(&value, bit);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to modify. |
| `bit` | Zero-based index of the bit to clear. |

---

### Return value

None.

---

### Remarks

- Only the selected bit is modified.
- If the selected bit is already clear, the value remains unchanged.
- An out-of-range bit index is ignored.
- `value` must point to a valid object.

---

### Example

```c
uint8_t flags = 0xFF;

BIT_FUNC(8, Clear)(&flags, 3);

/* flags == 0b11110111 */
```

---

# Flip

Inverts one bit.

A bit containing `1` becomes `0`, while a bit containing `0` becomes `1`. This makes `Flip` convenient for toggle-style states.

### Syntax

#### Macro form

```c
BIT_FUNC(width, Flip)(&value, bit);
```

#### Direct form

```c
Bit_width_Flip(&value, bit);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to modify. |
| `bit` | Zero-based index of the bit to invert. |

---

### Return value

None.

---

### Remarks

- Only the selected bit is inverted.
- An out-of-range bit index is ignored.
- Applying `Flip` twice to the same bit restores its original state.

---

### Example

```c
uint8_t flags = 0b00000010;

BIT_FUNC(8, Flip)(&flags, 1);

/* flags == 0b00000000 */

BIT_FUNC(8, Flip)(&flags, 1);

/* flags == 0b00000010 */
```

---

# Check

Tests whether a single bit is set.

`Check` is the basic inspection operation for integers being used as compact collections of flags.

### Syntax

#### Macro form

```c
bool result = BIT_FUNC(width, Check)(&value, bit);
```

#### Direct form

```c
bool result = Bit_width_Check(&value, bit);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to inspect. |
| `bit` | Zero-based index of the bit to test. |

---

### Return value

Returns `true` if the selected bit is set.

Returns `false` if the bit is clear or if `bit` is outside the selected integer width.

---

### Remarks

- The input value is not modified.
- Bit positions are counted from the least significant bit, beginning at zero.

---

### Example

```c
uint32_t flags = 0x20;

if (BIT_FUNC(32, Check)(&flags, 5)) {
    /* Bit 5 is set. */
}
```

---

# Mask operations

A bit mask describes several bit positions at once.

Each `1` in the mask selects the corresponding position:

```c
/* value = 10110100
 * mask  = 00001111
 *
 * The mask selects bits 0, 1, 2 and 3.
 */
```

Masks are useful whenever multiple flags form a logical group and should be modified or inspected together.

---

# MaskSet

Sets every bit selected by a mask.

### Syntax

#### Macro form

```c
BIT_FUNC(width, MaskSet)(&value, mask);
```

#### Direct form

```c
Bit_width_MaskSet(&value, mask);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to modify. |
| `mask` | Mask containing the bits to set. |

---

### Return value

None.

---

### Remarks

- Bits corresponding to `1` values in `mask` are set.
- Bits corresponding to `0` values in `mask` remain unchanged.
- The operation is equivalent to `value |= mask`.

---

### Example

```c
uint8_t flags = 0b10000000;

BIT_FUNC(8, MaskSet)(&flags, 0b00000111);

/* flags == 0b10000111 */
```

Named flags can be combined into a mask:

```c
const uint32_t READ    = 1u << 0;
const uint32_t WRITE   = 1u << 1;
const uint32_t EXECUTE = 1u << 2;

uint32_t permissions = 0;

BIT_FUNC(32, MaskSet)(
    &permissions,
    READ | WRITE
);
```

---

# MaskClear

Clears every bit selected by a mask.

### Syntax

#### Macro form

```c
BIT_FUNC(width, MaskClear)(&value, mask);
```

#### Direct form

```c
Bit_width_MaskClear(&value, mask);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to modify. |
| `mask` | Mask containing the bits to clear. |

---

### Return value

None.

---

### Remarks

- Bits selected by `mask` are cleared.
- Bits outside the mask remain unchanged.
- The operation is equivalent to `value &= ~mask`.

---

### Example

```c
uint8_t flags = 0b11111111;

BIT_FUNC(8, MaskClear)(&flags, 0b00001111);

/* flags == 0b11110000 */
```

---

# MaskFlip

Inverts every bit selected by a mask.

This provides the mask equivalent of `Flip`, allowing several binary states to be toggled in one operation.

### Syntax

#### Macro form

```c
BIT_FUNC(width, MaskFlip)(&value, mask);
```

#### Direct form

```c
Bit_width_MaskFlip(&value, mask);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the unsigned integer to modify. |
| `mask` | Mask containing the bits to invert. |

---

### Return value

None.

---

### Remarks

- Bits selected by the mask are inverted.
- Unselected bits remain unchanged.
- The operation is equivalent to `value ^= mask`.

---

### Example

```c
uint8_t flags = 0b10100000;

BIT_FUNC(8, MaskFlip)(&flags, 0b11110000);

/* flags == 0b01010000 */
```

---

# MaskCheckAll

Tests whether **all** bits selected by a mask are set.

This answers questions such as: does this value contain every flag required by this operation?

### Syntax

#### Macro form

```c
bool result = BIT_FUNC(width, MaskCheckAll)(&value, mask);
```

#### Direct form

```c
bool result = Bit_width_MaskCheckAll(&value, mask);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the value to inspect. |
| `mask` | Set of bits that must all be present. |

---

### Return value

Returns `true` if every bit selected by `mask` is also set in `value`.

Otherwise, returns `false`.

---

### Remarks

- The value is not modified.
- The operation is equivalent to `(value & mask) == mask`.
- This test requires every selected bit to match.

---

### Example

```c
uint8_t permissions = 0b00000111;
uint8_t required    = 0b00000101;

bool allowed =
    BIT_FUNC(8, MaskCheckAll)(
        &permissions,
        required
    );

/* allowed == true */
```

---

# MaskCheckAny

Tests whether **at least one** bit selected by a mask is set.

Unlike `MaskCheckAll`, a complete match is not required. One matching bit is sufficient.

### Syntax

#### Macro form

```c
bool result = BIT_FUNC(width, MaskCheckAny)(&value, mask);
```

#### Direct form

```c
bool result = Bit_width_MaskCheckAny(&value, mask);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the value to inspect. |
| `mask` | Set of candidate bits. |

---

### Return value

Returns `true` if at least one bit selected by `mask` is set in `value`.

Otherwise, returns `false`.

---

### Remarks

- The value is not modified.
- The operation is equivalent to `(value & mask) != 0`.
- This test is useful when any one of several states satisfies a condition.

---

### Example

```c
uint8_t status = 0b00100000;
uint8_t mask   = 0b00110000;

bool active =
    BIT_FUNC(8, MaskCheckAny)(
        &status,
        mask
    );

/* active == true */
```

---

# Bit analysis

The analysis functions inspect the binary structure of a value without modifying it.

They provide information such as how many bits are active, whether a value has a particular binary form, and where significant set bits occur.

---

# Popcount

Counts how many bits in a value are set to `1`.

The operation is commonly known as **population count**, **popcount**, or **Hamming weight**.

It is useful whenever an integer behaves as a compact set of states and the number of active states matters.

### Syntax

#### Macro form

```c
uint64_t count = BIT_FUNC(width, Popcount)(value);
```

#### Direct form

```c
uint64_t count = Bit_width_Popcount(value);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Unsigned integer whose set bits will be counted. |

---

### Return value

Returns the number of bits equal to `1`.

The return type is always `uint64_t`, regardless of the input width.

---

### Remarks

- The input value is not modified.
- The implementation uses a SWAR-style population-count algorithm.
- SWAR means **SIMD Within A Register**, allowing several partial bit counts to be performed inside a single integer.
- No compiler-specific popcount intrinsic is required.

---

### Example

```c
uint32_t value = 0b10110100;

uint64_t count =
    BIT_FUNC(32, Popcount)(value);

/* count == 4 */
```

---

# IsPowerOfTwo

Tests whether a value is an exact power of two.

Every positive power of two has a useful binary property: it contains exactly one set bit.

```c
/* 1   == 00000001
 * 2   == 00000010
 * 4   == 00000100
 * 8   == 00001000
 * 16  == 00010000
 * 32  == 00100000
 */
```

This makes the function useful for alignment checks, capacities, binary trees, masks, and algorithms based on powers of two.

### Syntax

#### Macro form

```c
bool result = BIT_FUNC(width, IsPowerOfTwo)(value);
```

#### Direct form

```c
bool result = Bit_width_IsPowerOfTwo(value);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Unsigned integer to inspect. |

---

### Return value

Returns `true` if `value` is an exact power of two.

Returns `false` otherwise.

Zero always returns `false`.

---

### Remarks

- The input value is not modified.
- The implementation uses the identity `value && !(value & (value - 1))`.
- Subtracting one from a power of two clears its single set bit and sets every lower bit.

```c
/* 8      = 1000
 * 8 - 1  = 0111
 *
 * 1000 & 0111 == 0000
 */
```

---

### Example

```c
bool a = BIT_FUNC(32, IsPowerOfTwo)(64);
bool b = BIT_FUNC(32, IsPowerOfTwo)(70);

/* a == true
 * b == false
 */
```

---

# IndexLSB

Returns the index of the least significant set bit.

**LSB** means **Least Significant Bit**.

Conceptually, the function finds the first `1` when the value is examined from right to left.

### Syntax

#### Macro form

```c
TYPE index = BIT_FUNC(width, IndexLSB)(value);
```

#### Direct form

```c
TYPE index = Bit_width_IndexLSB(value);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Unsigned integer to inspect. |

---

### Return value

Returns the zero-based index of the least significant set bit.

If `value` is zero, no set bit exists. The maximum value representable by the selected unsigned type is returned instead.

| Width | Return type | Not-found value |
| --- | --- | --- |
| 8-bit | `uint8_t` | `UINT8_MAX` |
| 16-bit | `uint16_t` | `UINT16_MAX` |
| 32-bit | `uint32_t` | `UINT32_MAX` |
| 64-bit | `uint64_t` | `UINT64_MAX` |

---

### Remarks

- Bit indexes begin at zero.
- The input value is not modified.
- The maximum representable value acts as the not-found sentinel.

---

### Example

```c
uint32_t value = 0b00101000;

uint32_t index =
    BIT_FUNC(32, IndexLSB)(value);

/* value = 0 0 1 0 1 0 0 0
 * index = 7 6 5 4 3 2 1 0
 *                 ^
 *
 * index == 3
 */
```

---

# IndexMSB

Returns the index of the most significant set bit.

**MSB** means **Most Significant Bit**.

The function locates the highest-positioned `1` in the value and can therefore be useful when determining its effective binary magnitude.

### Syntax

#### Macro form

```c
TYPE index = BIT_FUNC(width, IndexMSB)(value);
```

#### Direct form

```c
TYPE index = Bit_width_IndexMSB(value);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Unsigned integer to inspect. |

---

### Return value

Returns the zero-based index of the most significant set bit.

If `value` is zero, the maximum value representable by the selected unsigned type is returned.

---

### Remarks

- Bit indexes begin at zero.
- The input value is not modified.
- Zero uses the same not-found convention as `IndexLSB`.

---

### Example

```c
uint32_t value = 0b00101000;

uint32_t index =
    BIT_FUNC(32, IndexMSB)(value);

/* value = 0 0 1 0 1 0 0 0
 * index = 7 6 5 4 3 2 1 0
 *             ^
 *
 * index == 5
 */
```

---

# Bit rotation

Rotation moves bits across the boundaries of an integer without discarding them.

Unlike an ordinary shift, bits that leave one side of the integer are reintroduced at the opposite side.

This behavior is useful in hashing, pseudo-random generators, checksums, binary transforms, and algorithms that intentionally mix bit positions.

---

# RotateLeft

Rotates all bits in a value to the left.

Bits leaving the most significant side re-enter through the least significant side.

### Syntax

#### Macro form

```c
TYPE result = BIT_FUNC(width, RotateLeft)(value, shift);
```

#### Direct form

```c
TYPE result = Bit_width_RotateLeft(value, shift);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Unsigned integer to rotate. |
| `shift` | Number of bit positions to rotate. |

---

### Return value

Returns the rotated value.

The original value is not modified.

---

### Remarks

- The shift amount is reduced modulo the selected integer width.
- Rotating by exactly the integer width returns the original value.
- Rotating a 32-bit value by `33` positions is equivalent to rotating it by `1`.
- No bits are discarded.

---

### Example

```c
uint8_t value = 0b10000001;

uint8_t result =
    BIT_FUNC(8, RotateLeft)(
        value,
        1
    );

/* Before: 10000001
 * After:  00000011
 */
```

---

# RotateRight

Rotates all bits in a value to the right.

Bits leaving the least significant side re-enter through the most significant side.

### Syntax

#### Macro form

```c
TYPE result = BIT_FUNC(width, RotateRight)(value, shift);
```

#### Direct form

```c
TYPE result = Bit_width_RotateRight(value, shift);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Unsigned integer to rotate. |
| `shift` | Number of bit positions to rotate. |

---

### Return value

Returns the rotated value.

The original value is not modified.

---

### Remarks

- The shift amount is reduced modulo the selected integer width.
- Rotation preserves every bit in the value.
- `RotateRight` mirrors the behavior of `RotateLeft` in the opposite direction.

---

### Example

```c
uint8_t value = 0b10000001;

uint8_t result =
    BIT_FUNC(8, RotateRight)(
        value,
        1
    );

/* Before: 10000001
 * After:  11000000
 */
```

---

# Bit fields

Bit fields allow a contiguous region inside an integer to be treated as a smaller independent value.

This is particularly useful when several logical values are packed inside the same integer, as commonly occurs in compact data structures, binary formats, registers, and protocols.

A field is described by two **inclusive** indexes:

```c
/* high ... low */
```

For example:

```c
/* high = 6
 * low  = 3
 *
 * Selected indexes:
 *
 * 6 5 4 3
 */
```

The number of selected bits is:

```c
high - low + 1
```

---

# Extract

Extracts a contiguous range of bits from an integer.

The selected field is shifted down so that its lowest bit becomes bit `0` in the result. The extracted field can therefore be used as an ordinary integer.

### Syntax

#### Macro form

```c
TYPE result = BIT_FUNC(width, Extract)(
    &value,
    high,
    low
);
```

#### Direct form

```c
TYPE result = Bit_width_Extract(
    &value,
    high,
    low
);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the source unsigned integer. |
| `high` | Index of the most significant bit in the field. |
| `low` | Index of the least significant bit in the field. |

---

### Return value

Returns the extracted field aligned to bit `0`.

Returns `0` for an invalid field description.

---

### Remarks

- `high` and `low` are both inclusive.
- `value == NULL` returns `0`.
- `high < low` returns `0`.
- An out-of-range `high` or `low` returns `0`.
- Extracting the complete integer width returns the original value.
- The source integer is not modified.

---

### Example

```c
uint8_t value = 0b11010110;

uint8_t field =
    BIT_FUNC(8, Extract)(
        &value,
        5,
        2
    );

/* value:
 *
 * 1 1 0 1 0 1 1 0
 *     ^ ^ ^ ^
 *     5 4 3 2
 *
 * Selected field: 0101
 *
 * field == 0b00000101
 */
```

---

# Insert

Writes a value into a contiguous range of bits.

Only the selected field is replaced. Bits outside the field retain their original values.

`Insert` is the natural counterpart of `Extract` and provides a simple way to construct packed integers without manually building masks and shifts.

### Syntax

#### Macro form

```c
BIT_FUNC(width, Insert)(
    &value,
    high,
    low,
    field_value
);
```

#### Direct form

```c
Bit_width_Insert(
    &value,
    high,
    low,
    field_value
);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `value` | Pointer to the destination unsigned integer. |
| `high` | Index of the most significant bit in the destination field. |
| `low` | Index of the least significant bit in the destination field. |
| `field_value` | Value to write into the selected field. |

---

### Return value

None.

---

### Remarks

- `high` and `low` are inclusive.
- Only as many low bits of `field_value` as fit in the selected field are inserted.
- Higher bits of `field_value` are discarded automatically.
- Bits outside the selected field remain unchanged.
- `value == NULL` causes the operation to be ignored.
- `high < low` causes the operation to be ignored.
- Out-of-range indexes cause the operation to be ignored.
- Selecting the complete integer width replaces the entire value.

---

### Example

```c
uint8_t value = 0b11000011;

BIT_FUNC(8, Insert)(
    &value,
    5,
    2,
    0b1010
);

/* Before:
 *
 * 1 1 0 0 0 0 1 1
 *
 * Field 5..2:
 *
 *     5 4 3 2
 *     1 0 1 0
 *
 * After:
 *
 * 1 1 1 0 1 0 1 1
 *
 * value == 0b11101011
 */
```

---

# Width variants

Every Bit operation follows the same width-based naming model.

For example, `Popcount` is available as:

| Width | Macro form | Direct form |
| --- | --- | --- |
| 8-bit | `BIT_FUNC(8, Popcount)` | `Bit_8_Popcount` |
| 16-bit | `BIT_FUNC(16, Popcount)` | `Bit_16_Popcount` |
| 32-bit | `BIT_FUNC(32, Popcount)` | `Bit_32_Popcount` |
| 64-bit | `BIT_FUNC(64, Popcount)` | `Bit_64_Popcount` |

The same structure applies to every operation in the module.

---

# Examples

## Using bits as flags

Individual bits provide a compact way to store several boolean states inside a single integer.

```c
#include "Cosmeron/Modules/Bit/Bit.h"

enum {
    FLAG_VISIBLE  = 0,
    FLAG_SELECTED = 1,
    FLAG_DISABLED = 2
};

int main(void)
{
    uint32_t flags = 0;

    BIT_FUNC(32, Set)(&flags, FLAG_VISIBLE);
    BIT_FUNC(32, Set)(&flags, FLAG_SELECTED);

    if (BIT_FUNC(32, Check)(&flags, FLAG_VISIBLE)) {
        /* The object is visible. */
    }

    BIT_FUNC(32, Clear)(&flags, FLAG_SELECTED);

    return 0;
}
```

---

## Grouping permissions with masks

Masks allow several independent flags to be treated as one logical group.

```c
#include "Cosmeron/Modules/Bit/Bit.h"

int main(void)
{
    const uint32_t READ    = 1u << 0;
    const uint32_t WRITE   = 1u << 1;
    const uint32_t EXECUTE = 1u << 2;

    uint32_t permissions = 0;

    BIT_FUNC(32, MaskSet)(
        &permissions,
        READ | WRITE
    );

    if (BIT_FUNC(32, MaskCheckAll)(
            &permissions,
            READ | WRITE
        )) {
        /* Both permissions are enabled. */
    }

    BIT_FUNC(32, MaskClear)(
        &permissions,
        WRITE
    );

    return 0;
}
```

---

## Packing multiple values

`Insert` and `Extract` can be combined to store and retrieve logical values inside selected regions of the same integer.

```c
#include "Cosmeron/Modules/Bit/Bit.h"

int main(void)
{
    uint32_t packet = 0;

    BIT_FUNC(32, Insert)(
        &packet,
        7,
        4,
        0xA
    );

    uint32_t field =
        BIT_FUNC(32, Extract)(
            &packet,
            7,
            4
        );

    /* field == 0xA */

    return 0;
}
```

---

## Using the direct form

Generated symbols may also be called directly.

```c
#include "Cosmeron/Modules/Bit/Bit.h"

int main(void)
{
    uint32_t flags = 0;

    Bit_32_Set(&flags, 4);

    uint64_t count =
        Bit_32_Popcount(flags);

    return count == 1 ? 0 : 1;
}
```

The equivalent macro form is:

```c
#include "Cosmeron/Modules/Bit/Bit.h"

int main(void)
{
    uint32_t flags = 0;

    BIT_FUNC(32, Set)(&flags, 4);

    uint64_t count =
        BIT_FUNC(32, Popcount)(flags);

    return count == 1 ? 0 : 1;
}
```

---

# Naming and namespace

Bit functions are expressed through the module macro:

```c
BIT_FUNC(width, Function)
```

Without a custom namespace:

```c
BIT_FUNC(32, Popcount)
```

corresponds to the direct symbol:

```c
Bit_32_Popcount
```

If a namespace is configured:

```c
#define COSMERON_NAMESPACE MyProject
```

the direct symbol becomes:

```c
MyProject_Bit_32_Popcount
```

while the macro form remains:

```c
BIT_FUNC(32, Popcount)
```

This allows source code using the macro API to remain independent of the global namespace selected by the project.

---

# Requirements

| Requirement | Value |
| --- | --- |
| Language | C |
| Minimum target | C11 |
| Integer widths | 8, 16, 32 and 64 bits |
| External libraries | None |
| Additional linker flags | None |
| Dynamic allocation | None |

The module uses the standard C headers:

```c
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
```

The Bit module follows Cosmeron's header-based architecture and does not require a separately compiled library.

---

# See also

**Type**

Provides numeric types and utilities that complement low-level bit operations.

**Math**

Provides general arithmetic and numeric operations.

**Core / Namespace**

Defines the namespace machinery used to generate Cosmeron symbols such as:

```c
Bit_32_Popcount
```

and custom-prefixed variants such as:

```c
MyProject_Bit_32_Popcount
```
