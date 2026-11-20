# Numeric

The **Numeric** module provides extended numeric types in pure C11: rationals, complex numbers, fixed-point numbers, arbitrary-width big integers, and fixed-width decimals.

```c
#include "Cosmeron/Modules/Numeric/Numeric.h"
```

Individual headers are also available: `Rational.h`, `Complex.h`, `Fixed.h`, `Bigint.h`, and `Decimal.h`.

---

## Architecture & Design

1. **Dual-Mode `.def` Templates**:
   All numeric types are defined once in `.def` templates (`Rational.def`, `Complex.def`, `Fixed.def`, `Bigint.def`, `Decimal.def`) and pre-instantiated for standard widths/types.
2. **Custom Width Instantiation**:
   Custom widths (such as `Bigint192`) can be instantiated on demand via `TEMPLATE_INSTANTIATE(Numeric, Bigint, 192, uint64_t)`.
3. **Strict Contracts**:
   Division by zero and overflow return `STATUS_CONST(INVALID_ARGUMENT)` or `STATUS_CONST(OUT_OF_RANGE)` via `OPSTATUS`.

---

## Rational (`Rational.h`)

Represents an exact fraction `numerator / denominator` normalized on construction.

```c
MODULE_TYPED(Numeric, Rational, Int32) r1, r2, sum;
MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 3, &r1);
MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 6, &r2);
MODULE_FUNC(Numeric, Rational, Int32, Add)(r1, r2, &sum); /* 1/2 */
```

- **Operations**: `Make`, `Add`, `Sub`, `Mul`, `Div`.

---

## Complex (`Complex.h`)

Represents a complex number `real + imag * i`.

```c
MODULE_TYPED(Numeric, Complex, Float64) c =
    MODULE_FUNC(Numeric, Complex, Float64, Make)(3.0, 4.0);
double mag = MODULE_FUNC(Numeric, Complex, Float64, Magnitude)(c); /* 5.0 */
```

- **Functions**: `Make`, `Add`, `Sub`, `Mul`, `Magnitude`.

---

## Fixed-Point (`Fixed.h`)

Provides `Q16_16` fixed-point arithmetic without floating-point hardware requirements.

```c
MODULE_TYPED(Numeric, Fixed, Q16_16) f1 =
    MODULE_FUNC(Numeric, Fixed, Q16_16, FromFloat64)(2.5);
MODULE_TYPED(Numeric, Fixed, Q16_16) f2 =
    MODULE_FUNC(Numeric, Fixed, Q16_16, FromInt)(3);
MODULE_TYPED(Numeric, Fixed, Q16_16) prod =
    MODULE_FUNC(Numeric, Fixed, Q16_16, Mul)(f1, f2);
double result = MODULE_FUNC(Numeric, Fixed, Q16_16, ToFloat64)(prod); /* 7.5 */
```

---

## Bigint & Decimal (`Bigint.h`, `Decimal.h`)

Fixed-width multi-limb integers and decimals (`Bigint128`, `Decimal128`, etc.).

```c
MODULE_TYPE(Numeric, Bigint128) a = {0}, b = {0};
MODULE_FUNC(Numeric, Bigint128, Init)(&a);
MODULE_FUNC(Numeric, Bigint128, Init)(&b);
a.limb[0] = 40ULL;
b.limb[0] = 2ULL;
OBJECT_CALL(&a, Add, &b); /* a.limb[0] == 42 */
```

See [Examples/Modules/Numeric.c](../../../Codespace/Examples/Modules/Numeric.c).
