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
