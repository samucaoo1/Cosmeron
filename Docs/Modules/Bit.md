# Bit

The **Bit** module provides a compact set of declarative utilities for manipulating and inspecting individual bits, masks, rotations, and contiguous bit fields across unsigned integer widths (8, 16, 32, and 64 bits), as well as custom user types via the Universal Template Engine.

It is designed for code where an integer represents flags, permissions, packed hardware-style bitfields, state masks, binary protocols, or numeric bitwise algorithms.

```c
#include "Cosmeron/Modules/Bit/Bit.h"
```

---

# Architecture & Design

The module is built entirely on Cosmeron's canonical architectural foundations:

1. **Declarative Packages (`PACKAGE`)**:
   Standard integer widths are declared as packages (`Bit8`, `Bit16`, `Bit32`, `Bit64`), exposing both `static inline` functions and function tables (`Bit32_FunctionTable`).
2. **Universal Template Engine (`TEMPLATE_BIT` / `TEMPLATE_BIT_IMPLEMENT`)**:
   Allows instant instantiation for any unsigned integer or limb type (`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, `size_t`, custom typedefs) using standard `TEMPLATE_IMPLEMENT(BIT, Suffix, Type)`.
3. **Decoupled X-Macro Table (`BIT_WIDTH_TABLE`)**:
   Single source of truth for standard widths, types, and capacities using parenthesized row tuples and decoupled column selectors.
4. **Strict Function Contracts**:
   - `OPERATION`: Fallible mutations and value-producing operations returning `OPSTATUS` (`Set`, `Clear`, `Flip`, `IndexLSB`, `IndexMSB`, `Extract`, `Insert`).
   - `PREDICATE`: Proposition tests returning `bool` (`Check`, `MaskCheckAll`, `MaskCheckAny`, `IsPowerOfTwo`).
   - `FUNCTION`: Infallible value-producing computations (`Popcount`, `RotateLeft`, `RotateRight`).
   - `PROCEDURE`: Infallible in-place state mutations returning `void` (`MaskSet`, `MaskClear`, `MaskFlip`).
5. **Zero Retrocompatibility & Zero Ambiguity**:
   No legacy monolithic macros or unnamespaced wrappers. Fully compatible with `PACKAGE_DISABLE_FUNCTION_TABLE`.

---

# Invocation Styles

Bit operations support three equivalent calling styles:

### 1. Direct Form
Call the namespaced package function directly:

```c
uint32_t value = 0;
OPSTATUS status = Bit32_Set(&value, 5);
bool isSet = Bit32_Check(value, 5);
```

### 2. Macro Form
Uses the canonical namespace generator `BIT_FUNC(SUFFIX, NAME)`:

```c
uint32_t value = 0;
OPSTATUS status = BIT_FUNC(32, Set)(&value, 5);
bool isSet = BIT_FUNC(32, Check)(value, 5);
```

### 3. Declarative Package Call
Dispatches through the declarative package interface or function table:

```c
uint32_t value = 0;
OPSTATUS status = PACKAGE_CALL(Bit32, Set, &value, 5);
const Bit32_FunctionTable *ft = Bit32_GetFunctionTable();
uint64_t count = ft->Popcount(value);
```

---

# Standard Widths & X-Macro Table

The standard integer widths are defined by the X-macro table `BIT_WIDTH_TABLE(X)`:

```c
#define BIT_WIDTH_COL_SUFFIX(SUFFIX, TYPE, BITS, MAX) SUFFIX
#define BIT_WIDTH_COL_TYPE(SUFFIX, TYPE, BITS, MAX) TYPE
#define BIT_WIDTH_COL_BITS(SUFFIX, TYPE, BITS, MAX) BITS
#define BIT_WIDTH_COL_MAX(SUFFIX, TYPE, BITS, MAX) MAX
#define BIT_WIDTH_GET(FIELD, ROW) PP_OP_CAT2(BIT_WIDTH_COL_, FIELD) ROW

#define BIT_WIDTH_TABLE(X)                                                     \
  X((8, uint8_t, 8U, UINT8_MAX))                                               \
  X((16, uint16_t, 16U, UINT16_MAX))                                           \
  X((32, uint32_t, 32U, UINT32_MAX))                                           \
  X((64, uint64_t, 64U, UINT64_MAX))
```

| Width | C Type | Package Identifier | Direct Function Prefix |
|---|---|---|---|
| 8-bit | `uint8_t` | `Bit8` | `Bit8_...` |
| 16-bit | `uint16_t` | `Bit16` | `Bit16_...` |
| 32-bit | `uint32_t` | `Bit32` | `Bit32_...` |
| 64-bit | `uint64_t` | `Bit64` | `Bit64_...` |

---

# API Summary

| Category | Function | Contract | Signature | Description |
|---|---|---|---|---|
| **Single-Bit** | `Set` | `OPERATION` | `OPSTATUS (TYPE *value, uint8_t bit)` | Sets bit at position to `1` |
| | `Clear` | `OPERATION` | `OPSTATUS (TYPE *value, uint8_t bit)` | Clears bit at position to `0` |
| | `Flip` | `OPERATION` | `OPSTATUS (TYPE *value, uint8_t bit)` | Inverts bit at position |
| | `Check` | `PREDICATE` | `bool (TYPE value, uint8_t bit)` | Tests if bit at position is `1` |
| **Mask** | `MaskSet` | `PROCEDURE` | `void (TYPE *value, TYPE mask)` | In-place bitwise OR with mask |
| | `MaskClear` | `PROCEDURE` | `void (TYPE *value, TYPE mask)` | In-place bitwise AND with `~mask` |
| | `MaskFlip` | `PROCEDURE` | `void (TYPE *value, TYPE mask)` | In-place bitwise XOR with mask |
| | `MaskCheckAll` | `PREDICATE` | `bool (TYPE value, TYPE mask)` | Tests if all mask bits are set |
| | `MaskCheckAny` | `PREDICATE` | `bool (TYPE value, TYPE mask)` | Tests if any mask bit is set |
| **Inspection** | `Popcount` | `FUNCTION` | `uint64_t (TYPE value)` | Counts set bits (Hamming weight) |
| | `IsPowerOfTwo` | `PREDICATE` | `bool (TYPE value)` | Checks if value is power of two |
| | `IndexLSB` | `OPERATION` | `OPSTATUS (TYPE value, TYPE *outIndex)` | Finds position of least significant set bit |
| | `IndexMSB` | `OPERATION` | `OPSTATUS (TYPE value, TYPE *outIndex)` | Finds position of most significant set bit |
| **Rotation** | `RotateLeft` | `FUNCTION` | `TYPE (TYPE value, TYPE shift)` | Bitwise circular rotation to the left |
| | `RotateRight` | `FUNCTION` | `TYPE (TYPE value, TYPE shift)` | Bitwise circular rotation to the right |
| **Field** | `Extract` | `OPERATION` | `OPSTATUS (TYPE value, uint8_t high, uint8_t low, TYPE *outField)` | Extracts contiguous field `[low, high]` |
| | `Insert` | `OPERATION` | `OPSTATUS (TYPE *value, uint8_t high, uint8_t low, TYPE fieldValue)` | Replaces contiguous field `[low, high]` |

---

# Single-Bit Operations

Single-bit operations operate on a zero-based bit index (0 is least significant bit):

```c
/* Bit index:  7 6 5 4 3 2 1 0
 * Value:      0 0 1 0 0 0 0 0 -> Bit 5 is set
 */
```

### Set
```c
OPSTATUS Bit32_Set(uint32_t *value, uint8_t bit);
```
- Returns `STATUS_CONST(INVALID_ARGUMENT)` if `value == NULL`.
- Returns `STATUS_CONST(OUT_OF_RANGE)` if `bit >= width`.
- Returns `STATUS_CONST(SUCCESS)` on success.

### Clear
```c
OPSTATUS Bit32_Clear(uint32_t *value, uint8_t bit);
```
- Clears the specified bit to 0. Validates null pointer and bit index range.

### Flip
```c
OPSTATUS Bit32_Flip(uint32_t *value, uint8_t bit);
```
- Toggles the specified bit (0 $\to$ 1, 1 $\to$ 0). Validates null pointer and bit index range.

### Check
```c
bool Bit32_Check(uint32_t value, uint8_t bit);
```
- Returns `true` if `bit < width` and the bit at position `bit` is 1; otherwise returns `false`.

---

# Mask Operations

Mask operations operate on composite bitmasks selecting multiple bit positions simultaneously:

### MaskSet
```c
void Bit32_MaskSet(uint32_t *value, uint32_t mask);
```
- In-place mutation: `*value |= mask`. Safely does nothing if `value == NULL`.

### MaskClear
```c
void Bit32_MaskClear(uint32_t *value, uint32_t mask);
```
- In-place mutation: `*value &= ~mask`. Safely does nothing if `value == NULL`.

### MaskFlip
```c
void Bit32_MaskFlip(uint32_t *value, uint32_t mask);
```
- In-place mutation: `*value ^= mask`. Safely does nothing if `value == NULL`.

### MaskCheckAll
```c
bool Bit32_MaskCheckAll(uint32_t value, uint32_t mask);
```
- Returns `true` if every bit set in `mask` is set in `value`: `(value & mask) == mask`.

### MaskCheckAny
```c
bool Bit32_MaskCheckAny(uint32_t value, uint32_t mask);
```
- Returns `true` if at least one bit set in `mask` is set in `value`: `(value & mask) != 0`.

---

# Counting & Search Operations

### Popcount
```c
uint64_t Bit32_Popcount(uint32_t value);
```
- Returns the number of set bits (Hamming weight) using a branch-free SWAR algorithm.

### IsPowerOfTwo
```c
bool Bit32_IsPowerOfTwo(uint32_t value);
```
- Returns `true` if `value` is non-zero and has exactly one bit set. Returns `false` for `0`.

### IndexLSB
```c
OPSTATUS Bit32_IndexLSB(uint32_t value, uint32_t *outIndex);
```
- Finds the position of the least significant set bit (trailing zero count).
- Returns `STATUS_CONST(INVALID_ARGUMENT)` if `outIndex == NULL`.
- Returns `STATUS_CONST(NOT_FOUND)` if `value == 0`.
- On success, writes the index to `*outIndex` and returns `STATUS_CONST(SUCCESS)`.

### IndexMSB
```c
OPSTATUS Bit32_IndexMSB(uint32_t value, uint32_t *outIndex);
```
- Finds the position of the most significant set bit.
- Returns `STATUS_CONST(INVALID_ARGUMENT)` if `outIndex == NULL`.
- Returns `STATUS_CONST(NOT_FOUND)` if `value == 0`.
- On success, writes the index to `*outIndex` and returns `STATUS_CONST(SUCCESS)`.

---

# Rotation Operations

### RotateLeft
```c
uint32_t Bit32_RotateLeft(uint32_t value, uint32_t shift);
```
- Performs circular bitwise rotation left by `shift % width` positions.

### RotateRight
```c
uint32_t Bit32_RotateRight(uint32_t value, uint32_t shift);
```
- Performs circular bitwise rotation right by `shift % width` positions.

---

# Contiguous Field Operations

Bit fields allow extracting and inserting multi-bit integer values packed into specific ranges `[low, high]`.

### Extract
```c
OPSTATUS Bit32_Extract(uint32_t value, uint8_t high, uint8_t low, uint32_t *outField);
```
- Validates:
  - `outField != NULL` $\to$ `STATUS_CONST(INVALID_ARGUMENT)`
  - `high >= low && low < width && high < width` $\to$ `STATUS_CONST(OUT_OF_RANGE)`
- Extracts bits in range `[low, high]`, right-aligns them to bit 0, writes to `*outField`, and returns `STATUS_CONST(SUCCESS)`.

### Insert
```c
OPSTATUS Bit32_Insert(uint32_t *value, uint8_t high, uint8_t low, uint32_t fieldValue);
```
- Validates:
  - `value != NULL` $\to$ `STATUS_CONST(INVALID_ARGUMENT)`
  - `high >= low && low < width && high < width` $\to$ `STATUS_CONST(OUT_OF_RANGE)`
- Masks `fieldValue` to the width `high - low + 1`, clears range `[low, high]` in `*value`, and writes the shifted field into place.

---

# Universal Template Engine

Custom unsigned integer types or limb buffers can be specialized using the Universal Template Engine:

### Instantiation
```c
/* In header or translation unit */
TEMPLATE_IMPLEMENT(BIT, usize, size_t);
```

This generates:
- Package: `Bitusize`
- Functions: `Bitusize_Set`, `Bitusize_Check`, `Bitusize_Popcount`, `Bitusize_Extract`, etc.
- Function Table: `Bitusize_FunctionTable` and `Bitusize_GetFunctionTable()`

```c
size_t buffer = 0;
Bitusize_Set(&buffer, 12);
bool active = Bitusize_Check(buffer, 12);
```

---

# Function Table & Compile-Time Disabling

Each instantiated package emits a function table struct:

```c
const Bit32_FunctionTable *table = Bit32_GetFunctionTable();
table->Set(&flags, 3);
```

To eliminate function tables and retain strictly direct inline functions for constrained or embedded targets, define:

```c
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#include "Cosmeron/Modules/Bit/Bit.h"
```
