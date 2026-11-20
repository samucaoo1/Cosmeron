# Numeric

The **Numeric** module contains numeric representations that are not ordinary C scalar types: fixed-width multi-limb integers, fixed-point values, rational numbers and complex numbers. It belongs to **Modules** and is exposed through one aggregate header or individual type headers.

```c
#include "Cosmeron/Modules/Numeric/Numeric.h"
```

The aggregate header includes `Bigint.h`, `Complex.h`, `Decimal.h`, `Fixed.h` and `Rational.h`.

---

# Overview

| Type family | Built-in specializations | Best suited to |
| --- | --- | --- |
| `Bigint` | 128, 256, 512, 1024 bits | Fixed-width multi-limb integer arithmetic |
| `Decimal` | 128, 256, 512, 1024 bits | Multi-limb integer arithmetic with the same bit/limb operation model |
| `Complex` | `Float32`, `Float64`, `Float128` | Complex arithmetic |
| `Fixed` | `Q8_8`, `Q16_16` | Deterministic fixed-point arithmetic |
| `Rational` | `Int32`, `Int64` | Fractions represented by numerator and denominator |

Built-in variants are instantiated by the public headers. `MODULE_TYPED` names a specialization's type, while `MODULE_FUNC` addresses its generated package functions.

---

# Bigint

`Bigint` stores a fixed number of bits in 64-bit limbs. The declared widths are 128, 256, 512 and 1024 bits. It is a fixed-capacity integer representation, not an arbitrary-precision value that grows automatically.

The object operations include:

| Group | Operations |
| --- | --- |
| Arithmetic | `Clear`, `Add`, `Sub`, `Mul`, `Div`, `Mod`, `Increment`, `Decrement` |
| Bitwise | `And`, `Or`, `Xor`, `Not` |
| Shifts and rotations | `Shift.Left`, `Shift.Right`, `Rotate.Left`, `Rotate.Right` |
| Bit access | `Bit.Set`, `Bit.Clear`, `Bit.Check` |
| Comparison | `Compare`, `Equal`, `NotEqual`, `LessThan`, `GreaterThan`, `LessOrEqual`, `GreaterOrEqual`, `IsZero` |
| Text conversion | `ToCString.Base` and default-base `ToCString` |

Arithmetic methods mutate the receiver and report `OPSTATUS`. Division checks for an invalid divisor; the `Div.Mod` overload can write quotient and remainder to separate output objects. The type is stored as unsigned limbs, so do not assume signed two's-complement semantics.

```c
MODULE_TYPED(Numeric, Bigint, 128) a = {0};
MODULE_TYPED(Numeric, Bigint, 128) b = {0};

/* Values use the fixed limb/byte representation. */
OPSTATUS status = API(&a, Add, &b);
```

For bit-oriented tasks, use the direct bit and shift operations rather than converting through a native integer that may be narrower than the selected specialization.

---

# Decimal

`Decimal` exposes the same fixed-width limb layout and arithmetic/bit operation families as `Bigint`, with 128-, 256-, 512- and 1024-bit specializations. Its current implementation is limb-based: it does **not** provide a decimal floating-point scale, exponent, rounding mode or fractional-digit model.

The available operations include `Clear`, `Add`, `Sub`, `Mul`, `Div`, `Mod`, `Increment`, `Decrement`, `And`, `Or`, `Xor`, `Not`, `Shift`, `Rotate`, `Bit`, comparisons and `ToCString`.

Use `Decimal` where this fixed-width integer representation is intended by the API. Do not treat it as an IEEE decimal floating-point type or as a substitute for `Fixed`.

---

# Fixed-point

A `Fixed` specialization stores one signed raw integer plus a compile-time fractional-bit count. The built-in layouts are:

| Specialization | Raw type | Wide intermediate | Fraction bits |
| --- | --- | --- | ---: |
| `Q8_8` | `int16_t` | `int32_t` | 8 |
| `Q16_16` | `int32_t` | `int64_t` | 16 |

The package supplies `From.Raw(raw)`, `From.Int(value)`, `From.Float64(value)`, `To.Int(value)`, `To.Float64(value)`, `Add(a, b)`, `Sub(a, b)`, `Mul(a, b)`, `Div(a, b, outValue)` and `Compare(a, b)`.

`Add`, `Sub` and `Mul` return the fixed-point value directly. `Div` is fallible and therefore uses an output parameter plus `OPSTATUS`.

```c
MODULE_TYPED(Numeric, Fixed, Q16_16) price =
    MODULE_FUNC(Numeric, Fixed, Q16_16, FromInt)(125);

double asDouble =
    MODULE_FUNC(Numeric, Fixed, Q16_16, ToFloat64)(price);
```

The arithmetic uses the selected raw and wide C integer types. It is not saturating: account for the representable range and intermediate arithmetic limits in the calling code.

---

# Rational

A `Rational` specialization stores `numerator` and `denominator` of either `int32_t` or `int64_t`. The package functions are `Gcd`, `Make`, `Add`, `Sub`, `Mul`, `Div`, `Compare` and `ToFloat64`.

`Make(numerator, denominator, outValue)` rejects a zero denominator and creates the rational using the implementation's normalization rules. Arithmetic functions return `OPSTATUS` and require a valid output pointer. The implementation uses native signed integer storage; it does not provide arbitrary-precision overflow protection for numerator/denominator intermediates.

```c
MODULE_TYPED(Numeric, Rational, Int64) half;
OPSTATUS status = MODULE_FUNC(Numeric, Rational, Int64, Make)(1, 2, &half);

double value = MODULE_FUNC(Numeric, Rational, Int64, ToFloat64)(half);
```

---

# Complex numbers

`Complex` is a pair of values with `real` and `imag` fields. The floating-point specializations use `float`, `double` and `long double`.

The package provides `Make(real, imag)`, `Add(a, b)`, `Sub(a, b)`, `Mul(a, b)`, `Magnitude.Squared(z)`, `Magnitude(z)`, `Div(a, b, outValue)`, `Conjugate(z)` and `Equals(a, b)`.

```c
MODULE_TYPED(Numeric, Complex, Float64) x =
    MODULE_FUNC(Numeric, Complex, Float64, Make)(3.0, 4.0);

double magnitude =
    MODULE_FUNC(Numeric, Complex, Float64, Magnitude)(x);
```

`Equals` is exact component equality, not an epsilon-based approximate comparison. Use an application-defined tolerance where floating-point approximation is expected.

---

# Choosing a representation

| Requirement | Recommended family |
| --- | --- |
| Large fixed-width integer arithmetic and bit operations | `Bigint` |
| Fixed-width limb arithmetic through the Decimal family | `Decimal` |
| Fractions without immediate conversion to floating point | `Rational` |
| Fractional values with a fixed binary scale | `Fixed` |
| Two-component floating-point values | `Complex` |

# Notes

- All built-in variants are generated in headers; no separate Numeric library needs to be linked.
- Multi-limb types have fixed widths and do not reallocate when a value grows.
- `Rational` uses native signed integer storage and can overflow during arithmetic.
- `Fixed` is binary fixed-point, not decimal fixed-point.
