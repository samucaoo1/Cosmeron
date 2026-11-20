# Math

The **Math** module provides checked integer arithmetic, clamped and saturating arithmetic, value-range helpers, extrema and real-valued linear/quadratic equation solvers. Its declarations are C11 templates instantiated for the type suffixes listed in the module's type tables.

## Headers

~~~c
#include "Cosmeron/Modules/Math/Math.h"
~~~

The aggregate includes all current packages:

| Package | Public header | Purpose |
| --- | --- | --- |
| Arithmetic | `Arithmetic/Basic.h` | Checked signed and unsigned integer operations. |
| ClampedArithmetic | `Arithmetic/Clamped.h` | Bounded and saturating integer arithmetic. |
| Between | `Value/Between.h` | Interval classification and membership. |
| Clamp | `Value/Clamp.h` | Clamp a value to an interval. |
| MaxAndMin | `Value/MaxAndMin.h` | Binary minimum and maximum. |
| LinearEquation | `Equation/Linear.h` | Solve a linear equation. |
| QuadraticEquation | `Equation/Quadratic.h` | Discriminant, square root helper and quadratic solving. |

---

# Type specializations

The public suffixes are part of generated names. Use the exact suffix rather than a guessed C alias.

| Suffix | C type | Available packages |
| --- | --- | --- |
| `Bool` | `bool` | Value |
| `Int8`, `Int16`, `Int32`, `Int64` | Fixed-width signed integer | Arithmetic, ClampedArithmetic, Value |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | Fixed-width unsigned integer | Arithmetic, ClampedArithmetic, Value |
| `Char` | `char` | Value |
| `Float32` | `float` | Value, Equation |
| `Float64` | `double` | Value, Equation |
| `Float128` | `long double` | Value, Equation |
| `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr` | Architecture-sized C types | Value |

`Float128` is the suffix for C `long double`. It does not promise that every platform uses an actual 128-bit representation.

---

# API reference

## Arithmetic

Header: `Arithmetic/Basic.h`.

The signed and unsigned packages are named `MODULE_FUNC(Math, Arithmetic, <Suffix>, <Operation>)`.

| Function | Signature | Behavior |
| --- | --- | --- |
| `Add` | `(TYPE a, TYPE b, TYPE *outResult)` | Checked addition. |
| `Sub` | `(TYPE a, TYPE b, TYPE *outResult)` | Checked subtraction. |
| `Mul` | `(TYPE a, TYPE b, TYPE *outResult)` | Checked multiplication. |
| `Div` | `(TYPE a, TYPE b, TYPE *outResult)` | Checked division. |
| `Mod` | `(TYPE a, TYPE b, TYPE *outResult)` | Checked remainder. |

~~~c
int32_t sum = 0;
OPSTATUS status = MODULE_FUNC(Math, Arithmetic, Int32, Add)(
    20, 22, &sum);

if (status == STATUS_CONST(SUCCESS)) {
    /* sum is 42 */
}
~~~

Every operation requires a non-null output pointer. Overflow and invalid arithmetic are reported through `OPSTATUS`; the output is written only when the operation succeeds. Division or remainder by zero is invalid. Signed minimum divided by -1 is reported as out of range.

## ClampedArithmetic

Header: `Arithmetic/Clamped.h`.

Each signed/unsigned specialization offers two groups:

| Variant | Methods | Parameters | Result |
| --- | --- | --- | --- |
| `Clamp` | `Add`, `Sub`, `Mul` | `a, b, minimum, maximum` | Returns the arithmetic result restricted to the supplied interval. |
| `Saturating` | `Add`, `Sub`, `Mul` | `a, b` | Returns a value saturated to the type's representable bounds. |

~~~c
int32_t bounded = MODULE_FUNC(Math, ClampedArithmetic, Int32, Clamp, Add)(
    INT32_MAX, 10, -100, 100);

int32_t saturated = MODULE_FUNC(Math, ClampedArithmetic, Int32, Saturating, Add)(
    INT32_MAX, 10);
~~~

The clamp form delegates to checked arithmetic. If the intermediate operation overflows, it chooses the relevant supplied bound; otherwise it clamps the computed value. The saturating form uses the built-in minimum/maximum of the selected integer type. These functions return a value directly rather than an `OPSTATUS`.

## Between

Header: `Value/Between.h`.

| Function | Description |
| --- | --- |
| `Classify(value, minimum, maximum)` | Returns a `MODULE_ENUM(Math, RangeResult)` indicating below, inside, above or invalid range. |
| `Is(value, minimum, maximum)` | Returns true only when the value is inside the inclusive range. |

`RangeResult` values are `IN_RANGE`, `BELOW_RANGE`, `ABOVE_RANGE` and `INVALID_RANGE`. The range is inclusive at both ends. If `minimum > maximum`, `Classify` returns `INVALID_RANGE` and `Is` returns false.

~~~c
MODULE_ENUM(Math, RangeResult) position =
    MODULE_FUNC(Math, Between, Int32, Classify)(value, 10, 20);

bool accepted = MODULE_FUNC(Math, Between, Int32, Is)(value, 10, 20);
~~~

## Clamp

Header: `Value/Clamp.h`.

`Apply(value, minimum, maximum)` returns the value restricted to the inclusive interval. If the endpoints are reversed, the implementation swaps them before clamping.

~~~c
int32_t bounded = MODULE_FUNC(Math, Clamp, Int32, Apply)(42, 0, 10);
/* bounded is 10 */
~~~

This is a value-returning helper and does not report an invalid range; reversed bounds are normalized.

## MaxAndMin

Header: `Value/MaxAndMin.h`.

The package provides only binary `Max(a, b)` and `Min(a, b)` functions for each ordered type specialization. They return the selected value directly.

~~~c
int32_t maximum = MODULE_FUNC(Math, MaxAndMin, Int32, Max)(left, right);
int32_t minimum = MODULE_FUNC(Math, MaxAndMin, Int32, Min)(left, right);
~~~

There are no variadic `Smallest` or `Biggest` APIs in the current implementation.

## LinearEquation

Header: `Equation/Linear.h`.

Solves the equation:

[
a x + b = 0
]

`Solve(a, b, outResult, outSolution)` returns `OPSTATUS` and writes the solution classification to `MODULE_ENUM(Math, Solution)`.

| Condition | Solution classification | Result |
| --- | --- | --- |
| `a != 0` | `ONE` | `x = -b / a` |
| `a == 0` and `b == 0` | `INFINITE` | No unique root; output value is set to zero. |
| `a == 0` and `b != 0` | `NONE` | No root; output value is set to zero. |

~~~c
double root = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, LinearEquation, Float64, Solve)(
    2.0, -8.0, &root, &count);
~~~

Both output pointers are required. A successful call indicates that the equation was classified and does not imply that a unique root exists; inspect `count`.

## QuadraticEquation

Header: `Equation/Quadratic.h`.

The package exposes `Sqrt(x)`, `Discriminant(a, b, c)` and `Solve(a, b, c, outResult1, outResult2, outSolution)`.

For:

[
a x^2 + b x + c = 0
]

the discriminant is:

[
Delta = b^2 - 4ac
]

| Condition | Solution classification |
| --- | --- |
| `a == 0` | Delegates to the linear solver. |
| `Δ < 0` | `COMPLEX`; the real solver does not return complex roots. |
| `Δ == 0` | `ONE`; both output values receive the repeated root. |
| `Δ > 0` | `TWO`; the outputs receive the two real roots. |

~~~c
double x1 = 0.0;
double x2 = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, QuadraticEquation, Float64, Solve)(
    1.0, -3.0, 2.0, &x1, &x2, &count);
~~~

The output pointers and solution pointer are all required. The current Quadratic API classifies a negative discriminant as complex but does not expose a complex-root result object or a separate complex solver. `Sqrt` uses a bounded iterative calculation; use ordinary finite inputs and account for floating-point precision, overflow and underflow.

---

# Contracts and error handling

- `Arithmetic` operations return `OPSTATUS` and write through an output pointer only on success.
- `ClampedArithmetic`, `Clamp`, `Between.Is` and `MaxAndMin` return values or Boolean classifications directly.
- `Between.Classify` is the only interval helper that distinguishes an invalid interval from an ordinary out-of-range value.
- `LinearEquation.Solve` and `QuadraticEquation.Solve` can succeed while reporting `NONE`, `INFINITE` or `COMPLEX`. Always inspect the solution enum.
- The solution enum is `MODULE_ENUM(Math, Solution)`; range classification uses `MODULE_ENUM(Math, RangeResult)`.
- Integer operations are bounded by their selected C type. Floating-point operations inherit C floating-point precision and exceptional-value behavior.

## Implementation and portability

The templates and instantiation tables are in the individual package headers and `Impl/Common.inc`. Include the public header, not `Impl/*.impl` or `.def`, from consumer code. The aggregate `Math.h` has no dependency on a separate math library for its current equation implementation.

## Related modules

- [Numeric](Numeric.md) — fixed-point, rational and multi-limb values.
- [Algorithm](Algorithm.md) — comparison and generic repetition.
- [Types](Types.md) — primitive type names and traits.
- [Module](Module.md) — template declarations and namespaced function access.

## Source headers

- `Codespace/Cosmeron/Modules/Math/Math.h`
- `Codespace/Cosmeron/Modules/Math/Impl/Common.inc`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Basic.def`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Clamped.def`
- `Codespace/Cosmeron/Modules/Math/Value/Between.def`
- `Codespace/Cosmeron/Modules/Math/Value/Clamp.def`
- `Codespace/Cosmeron/Modules/Math/Value/MaxAndMin.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Linear.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Quadratic.def`
