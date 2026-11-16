# Math

The **Math** module provides checked integer arithmetic, clamped and saturating operations, scalar range utilities, and linear/quadratic equation solvers. The C11 API is defined by `static inline` functions generated for integer and floating-point specializations. This reference follows the format of `Bit.md` and describes only the operations currently implemented on the `audio-module` branch.

There is **no `Math.h` umbrella header** in the current tree. Include only the package headers required by your program:

```c
#include "Cosmeron/Modules/Math/Arithmetic/Basic.h"
#include "Cosmeron/Modules/Math/Arithmetic/Clamp.h"
#include "Cosmeron/Modules/Math/Value/Between.h"
#include "Cosmeron/Modules/Math/Value/Clamp.h"
#include "Cosmeron/Modules/Math/Value/MaxAndMin.h"
#include "Cosmeron/Modules/Math/Equation/Linear.h"
#include "Cosmeron/Modules/Math/Equation/Quadratic.h"
```

---

# Overview

| Package | Header | Purpose |
| --- | --- | --- |
| [Checked arithmetic](#arithmetic-basic-package) | `Arithmetic/Basic.h` | Checked integer arithmetic, including divide-by-zero and overflow detection. |
| [Clamped and saturating arithmetic](#arithmetic-clamp-package) | `Arithmetic/Clamp.h` | Bounded and saturating integer arithmetic, returning values directly. |
| [Intervals and comparisons](#value-between-package) | `Value/Between.h` | Strict interval membership and five-state position classification. |
| [Value clamping](#value-clamp-package) | `Value/Clamp.h` | Direct value restriction to an explicit numeric interval. |
| [Minimum and maximum](#value-maxandmin-package) | `Value/MaxAndMin.h` | Binary extrema and variadic extrema with status/out-parameter conventions. |
| [Linear equations](#equation-linear-package) | `Equation/Linear.h` | Solve a·x + b = 0 with explicit solution cardinality. |
| [Quadratic equations](#equation-quadratic-package) | `Equation/Quadratic.h` | Discriminant, real roots and (when supported) complex roots of quadratic equations. |

The public API contains **22 operation templates**, instantiated for the widths supported by each header. Operations are not all available for every type.

### Type suffixes

| Suffix | C type | Availability |
| --- | --- | --- |
| `I8` | `int8_t` | Arithmetic + Value |
| `I16` | `int16_t` | Arithmetic + Value |
| `I32` | `int32_t` | Arithmetic + Value |
| `I64` | `int64_t` | Arithmetic + Value |
| `U8` | `uint8_t` | Arithmetic + Value |
| `U16` | `uint16_t` | Arithmetic + Value |
| `U32` | `uint32_t` | Arithmetic + Value |
| `U64` | `uint64_t` | Arithmetic + Value |
| `F32` | `float` | Value + Equation |
| `F64` | `double` | Value + Equation |
| `F128` | `long double` | Value + Equation |

**F128** is the library's suffix for C `long double`; it does **not** guarantee that the platform provides 128 bits of floating-point precision or storage.

### Macro form

```c
int32_t result;
OPSTATUS status = ARITHMETIC_TYPED_FUNC(Add, I32)(2, 3, &result);
```

### Direct form

```c
int32_t result;
OPSTATUS status = Math_Arithmetic_Add_I32(2, 3, &result);
```

Both forms call the same function with the default namespace. If `COSMERON_NAMESPACE` is configured before the header is included, direct names gain that namespace prefix, while the macro spelling remains unchanged. The module exposes `MATH_FUNC`, `MATH_TYPED_FUNC`, `ARITHMETIC_TYPED_FUNC`, `VALUE_TYPED_FUNC`, and `EQUATION_TYPED_FUNC` for namespaced access.

---

# API reference

## Function summary

| Package | Operation | Type suffixes | Description |
| --- | --- | --- | --- |
| Checked arithmetic | [`Add`](#arithmetic-add) | `I8, I16, I32, I64, U8, U16, U32, U64` | Adds two integers without allowing overflow. |
| Checked arithmetic | [`Sub`](#arithmetic-sub) | `I8, I16, I32, I64, U8, U16, U32, U64` | Subtracts the right integer from the left. |
| Checked arithmetic | [`Mul`](#arithmetic-mul) | `I8, I16, I32, I64, U8, U16, U32, U64` | Multiplies two integers with checked overflow. |
| Checked arithmetic | [`Div`](#arithmetic-div) | `I8, I16, I32, I64, U8, U16, U32, U64` | Computes integer quotient with error detection. |
| Checked arithmetic | [`Mod`](#arithmetic-mod) | `I8, I16, I32, I64, U8, U16, U32, U64` | Computes the C integer remainder. |
| Clamped and saturating arithmetic | [`ClampAdd`](#arithmetic-clampadd) | `I8, I16, I32, I64, U8, U16, U32, U64` | Adds integers then limits the result to a supplied interval. |
| Clamped and saturating arithmetic | [`ClampSub`](#arithmetic-clampsub) | `I8, I16, I32, I64, U8, U16, U32, U64` | Subtracts integers then limits the result to a supplied interval. |
| Clamped and saturating arithmetic | [`ClampMul`](#arithmetic-clampmul) | `I8, I16, I32, I64, U8, U16, U32, U64` | Multiplies integers then limits the result to a supplied interval. |
| Clamped and saturating arithmetic | [`SaturatingAdd`](#arithmetic-saturatingadd) | `I8, I16, I32, I64, U8, U16, U32, U64` | Adds integers with saturation at the type limits. |
| Clamped and saturating arithmetic | [`SaturatingSub`](#arithmetic-saturatingsub) | `I8, I16, I32, I64, U8, U16, U32, U64` | Subtracts integers with saturation at the type limits. |
| Clamped and saturating arithmetic | [`SaturatingMul`](#arithmetic-saturatingmul) | `I8, I16, I32, I64, U8, U16, U32, U64` | Multiplies integers with saturation at the type limits. |
| Intervals and comparisons | [`IsBetween`](#value-isbetween) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Tests strict interval membership. |
| Intervals and comparisons | [`Between`](#value-between) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Classifies a value relative to two endpoints. |
| Value clamping | [`Clamp`](#value-clamp) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Clamps a scalar value to the indicated range. |
| Minimum and maximum | [`Max`](#value-max) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Returns the larger of two values. |
| Minimum and maximum | [`Min`](#value-min) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Returns the smaller of two values. |
| Minimum and maximum | [`Smallest`](#value-smallest) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Finds the smallest value among variadic arguments. |
| Minimum and maximum | [`Biggest`](#value-biggest) | `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128` | Finds the largest value among variadic arguments. |
| Linear equations | [`Linear`](#equation-linear) | `F32, F64, F128` | Solves `a*x + b = 0` over floating-point types. |
| Quadratic equations | [`QuadraticDiscriminant`](#equation-quadraticdiscriminant) | `F32, F64, F128` | Evaluates the discriminant of `a*x² + b*x + c`. |
| Quadratic equations | [`Quadratic`](#equation-quadratic) | `F32, F64, F128` | Solves a quadratic equation for real roots. |
| Quadratic equations | [`QuadraticComplex`](#equation-quadraticcomplex) | `F32, F64, F128` | Solves a quadratic equation allowing complex roots. |

---

# Shared types and constants

## Solution count

`MATH_TYPE(Solution)` (direct: `Math_Solution`) distinguishes the number of solutions. Constants are accessed using `MATH_CONST(...)`.

| Constant | Meaning |
| --- | --- |
| `MATH_CONST(SOLUTION_NONE)` | No solutions in the solver's result domain |
| `MATH_CONST(SOLUTION_ONE)` | One distinct solution |
| `MATH_CONST(SOLUTION_MULTIPLE)` | Two distinct quadratic roots |
| `MATH_CONST(SOLUTION_INFINITE)` | Every value satisfies the degenerate equation |

For real quadratic solving, a negative discriminant yields `SOLUTION_NONE`; the complex solver can instead return two complex solutions.

## Range result

`MATH_TYPE(RangeResult)` (direct: `Math_RangeResult`) is returned by `Between`:

| Constant | Condition (ordered bounds) |
| --- | --- |
| `MATH_RANGE_CONST(LOWER)` | value below minimum |
| `MATH_RANGE_CONST(EQUAL_MINIMUM)` | value equals minimum |
| `MATH_RANGE_CONST(BETWEEN)` | strictly inside range |
| `MATH_RANGE_CONST(EQUAL_MAXIMUM)` | value equals maximum |
| `MATH_RANGE_CONST(HIGHER)` | value above maximum |

These are discrete result categories, not numeric values to be compared as distances.

---

# Shared contracts

- **Checked integer arithmetic** returns `OPSTATUS` and writes an output pointer only on success. Errors include `STATUS_CONST(INVALID_ARGUMENT)` for NULL outputs, `STATUS_CONST(ARITHMETIC_OVERFLOW)` and `STATUS_CONST(DIVISION_BY_ZERO)`.
- **Saturating/clamped arithmetic and scalar value helpers** return numeric values directly. They do not report errors through `OPSTATUS`.
- **Equation solving** returns `OPSTATUS` plus `MATH_TYPE(Solution)`. All provided output pointers are required, even when no root exists. In degenerate/no-root cases, the root outputs can remain unchanged.
- **Unsigned subtraction** reports underflow rather than applying modulo wrapping in the checked Arithmetic API.
- **Floating-point operations** use ordinary C comparisons. They do not implement a general NaN/Infinity validation policy.
- **Range bounds:** `IsBetween`, `Between`, and scalar `Value_Clamp` do not reorder reversed endpoints. `Arithmetic_ClampAdd/Sub/Mul` do swap reversed bounds.
- Variadic `Smallest` and `Biggest` require the exact number and correctly promoted types of their trailing arguments.

---

# Arithmetic Basic package

Header: `Cosmeron/Modules/Math/Arithmetic/Basic.h`

Checked integer arithmetic, including divide-by-zero and overflow detection.

Specializations: `I8, I16, I32, I64, U8, U16, U32, U64`. The function prototypes below use `I32` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`Add`](#arithmetic-add) | Adds two integers without allowing overflow. |
| [`Sub`](#arithmetic-sub) | Subtracts the right integer from the left. |
| [`Mul`](#arithmetic-mul) | Multiplies two integers with checked overflow. |
| [`Div`](#arithmetic-div) | Computes integer quotient with error detection. |
| [`Mod`](#arithmetic-mod) | Computes the C integer remainder. |

---

# Arithmetic Add

Adds two integers without allowing overflow.

### Syntax

#### Macro form

```c
OPSTATUS ARITHMETIC_TYPED_FUNC(Add, I32)(int32_t a, int32_t b, int32_t *outResult);
```

#### Direct form

```c
OPSTATUS Math_Arithmetic_Add_I32(int32_t a, int32_t b, int32_t *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `outResult` | `int32_t *outResult` | Writable result pointer; required. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

On signed or unsigned overflow returns `STATUS_CONST(ARITHMETIC_OVERFLOW)` without writing the output. Requires a non-NULL output pointer.

---

### Example

```c
int32_t result = 0;
OPSTATUS status = ARITHMETIC_TYPED_FUNC(Add, I32)(7, 5, &result);
/* SUCCESS: result == 12 */
```

---

# Arithmetic Sub

Subtracts the right integer from the left.

### Syntax

#### Macro form

```c
OPSTATUS ARITHMETIC_TYPED_FUNC(Sub, I32)(int32_t a, int32_t b, int32_t *outResult);
```

#### Direct form

```c
OPSTATUS Math_Arithmetic_Sub_I32(int32_t a, int32_t b, int32_t *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `outResult` | `int32_t *outResult` | Writable result pointer; required. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Checks signed overflow. For unsigned specializations, subtracting a larger value from a smaller one is reported as `ARITHMETIC_OVERFLOW`, rather than wrapping.

---

### Example

```c
uint32_t result = 0;
OPSTATUS status = ARITHMETIC_TYPED_FUNC(Sub, U32)(3U, 5U, &result);
/* status == STATUS_CONST(ARITHMETIC_OVERFLOW) */
```

---

# Arithmetic Mul

Multiplies two integers with checked overflow.

### Syntax

#### Macro form

```c
OPSTATUS ARITHMETIC_TYPED_FUNC(Mul, I32)(int32_t a, int32_t b, int32_t *outResult);
```

#### Direct form

```c
OPSTATUS Math_Arithmetic_Mul_I32(int32_t a, int32_t b, int32_t *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `outResult` | `int32_t *outResult` | Writable result pointer; required. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Checks signed and unsigned multiplication. The signed minimum multiplied by -1 is rejected; zero times any number produces zero.

---

### Example

```c
int32_t result = 0;
OPSTATUS status = ARITHMETIC_TYPED_FUNC(Mul, I32)(-4, 6, &result);
/* SUCCESS: result == -24 */
```

---

# Arithmetic Div

Computes integer quotient with error detection.

### Syntax

#### Macro form

```c
OPSTATUS ARITHMETIC_TYPED_FUNC(Div, I32)(int32_t a, int32_t b, int32_t *outResult);
```

#### Direct form

```c
OPSTATUS Math_Arithmetic_Div_I32(int32_t a, int32_t b, int32_t *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `outResult` | `int32_t *outResult` | Writable result pointer; required. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Divisor zero returns `DIVISION_BY_ZERO`. For signed types, `MIN / -1` returns `ARITHMETIC_OVERFLOW`. Division follows C integer rules and truncates toward zero.

---

### Example

```c
int32_t quotient = 0;
OPSTATUS status = ARITHMETIC_TYPED_FUNC(Div, I32)(9, 2, &quotient);
/* SUCCESS: quotient == 4 */
```

---

# Arithmetic Mod

Computes the C integer remainder.

### Syntax

#### Macro form

```c
OPSTATUS ARITHMETIC_TYPED_FUNC(Mod, I32)(int32_t a, int32_t b, int32_t *outResult);
```

#### Direct form

```c
OPSTATUS Math_Arithmetic_Mod_I32(int32_t a, int32_t b, int32_t *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `outResult` | `int32_t *outResult` | Writable result pointer; required. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Divisor zero returns `DIVISION_BY_ZERO`. Signed `MIN % -1` returns `ARITHMETIC_OVERFLOW`. This is C remainder, not an always-positive mathematical modulo.

---

### Example

```c
int32_t remainder = 0;
OPSTATUS status = ARITHMETIC_TYPED_FUNC(Mod, I32)(-9, 4, &remainder);
/* SUCCESS: remainder == -1 */
```

---

# Arithmetic Clamp package

Header: `Cosmeron/Modules/Math/Arithmetic/Clamp.h`

Bounded and saturating integer arithmetic, returning values directly.

Specializations: `I8, I16, I32, I64, U8, U16, U32, U64`. The function prototypes below use `I32` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`ClampAdd`](#arithmetic-clampadd) | Adds integers then limits the result to a supplied interval. |
| [`ClampSub`](#arithmetic-clampsub) | Subtracts integers then limits the result to a supplied interval. |
| [`ClampMul`](#arithmetic-clampmul) | Multiplies integers then limits the result to a supplied interval. |
| [`SaturatingAdd`](#arithmetic-saturatingadd) | Adds integers with saturation at the type limits. |
| [`SaturatingSub`](#arithmetic-saturatingsub) | Subtracts integers with saturation at the type limits. |
| [`SaturatingMul`](#arithmetic-saturatingmul) | Multiplies integers with saturation at the type limits. |

---

# Arithmetic ClampAdd

Adds integers then limits the result to a supplied interval.

### Syntax

#### Macro form

```c
int32_t ARITHMETIC_TYPED_FUNC(ClampAdd, I32)(int32_t a, int32_t b, int32_t minimum, int32_t maximum);
```

#### Direct form

```c
int32_t Math_Arithmetic_ClampAdd_I32(int32_t a, int32_t b, int32_t minimum, int32_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `minimum` | `int32_t minimum` | Lower endpoint; see bound-ordering remarks. |
| `maximum` | `int32_t maximum` | Upper endpoint; see bound-ordering remarks. |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

If `minimum > maximum`, the implementation swaps them. When checked addition would overflow, it returns the bound corresponding to overflow direction. Otherwise it clamps the exact result.

---

### Example

```c
int32_t result = ARITHMETIC_TYPED_FUNC(ClampAdd, I32)(7, 8, 0, 10);
/* result == 10 */
```

---

# Arithmetic ClampSub

Subtracts integers then limits the result to a supplied interval.

### Syntax

#### Macro form

```c
int32_t ARITHMETIC_TYPED_FUNC(ClampSub, I32)(int32_t a, int32_t b, int32_t minimum, int32_t maximum);
```

#### Direct form

```c
int32_t Math_Arithmetic_ClampSub_I32(int32_t a, int32_t b, int32_t minimum, int32_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `minimum` | `int32_t minimum` | Lower endpoint; see bound-ordering remarks. |
| `maximum` | `int32_t maximum` | Upper endpoint; see bound-ordering remarks. |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Normalizes inverted bounds by swapping them. A checked arithmetic underflow/overflow returns the appropriate bound; otherwise the computed result is clamped.

---

### Example

```c
int32_t result = ARITHMETIC_TYPED_FUNC(ClampSub, I32)(2, 9, 0, 10);
/* result == 0 */
```

---

# Arithmetic ClampMul

Multiplies integers then limits the result to a supplied interval.

### Syntax

#### Macro form

```c
int32_t ARITHMETIC_TYPED_FUNC(ClampMul, I32)(int32_t a, int32_t b, int32_t minimum, int32_t maximum);
```

#### Direct form

```c
int32_t Math_Arithmetic_ClampMul_I32(int32_t a, int32_t b, int32_t minimum, int32_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |
| `minimum` | `int32_t minimum` | Lower endpoint; see bound-ordering remarks. |
| `maximum` | `int32_t maximum` | Upper endpoint; see bound-ordering remarks. |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Normalizes inverted bounds. If multiplication overflows, the result is the upper bound for positive overflow or lower bound for negative overflow.

---

### Example

```c
int32_t result = ARITHMETIC_TYPED_FUNC(ClampMul, I32)(4, 4, 0, 10);
/* result == 10 */
```

---

# Arithmetic SaturatingAdd

Adds integers with saturation at the type limits.

### Syntax

#### Macro form

```c
int32_t ARITHMETIC_TYPED_FUNC(SaturatingAdd, I32)(int32_t a, int32_t b);
```

#### Direct form

```c
int32_t Math_Arithmetic_SaturatingAdd_I32(int32_t a, int32_t b);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Equivalent to `ClampAdd` using the integer type's minimum and maximum. Never wraps on arithmetic overflow.

---

### Example

```c
int8_t result = ARITHMETIC_TYPED_FUNC(SaturatingAdd, I8)(INT8_MAX, 1);
/* result == INT8_MAX */
```

---

# Arithmetic SaturatingSub

Subtracts integers with saturation at the type limits.

### Syntax

#### Macro form

```c
int32_t ARITHMETIC_TYPED_FUNC(SaturatingSub, I32)(int32_t a, int32_t b);
```

#### Direct form

```c
int32_t Math_Arithmetic_SaturatingSub_I32(int32_t a, int32_t b);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Equivalent to `ClampSub` over the full representable type range. An unsigned underflow saturates to zero.

---

### Example

```c
uint8_t result = ARITHMETIC_TYPED_FUNC(SaturatingSub, U8)(0U, 1U);
/* result == 0 */
```

---

# Arithmetic SaturatingMul

Multiplies integers with saturation at the type limits.

### Syntax

#### Macro form

```c
int32_t ARITHMETIC_TYPED_FUNC(SaturatingMul, I32)(int32_t a, int32_t b);
```

#### Direct form

```c
int32_t Math_Arithmetic_SaturatingMul_I32(int32_t a, int32_t b);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Equivalent to `ClampMul` over the type's complete integer range. For signed types, saturation respects result sign.

---

### Example

```c
uint8_t result = ARITHMETIC_TYPED_FUNC(SaturatingMul, U8)(UINT8_MAX, 2U);
/* result == UINT8_MAX */
```

---

# Value Between package

Header: `Cosmeron/Modules/Math/Value/Between.h`

Strict interval membership and five-state position classification.

Specializations: `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128`. The function prototypes below use `I32` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`IsBetween`](#value-isbetween) | Tests strict interval membership. |
| [`Between`](#value-between) | Classifies a value relative to two endpoints. |

---

# Value IsBetween

Tests strict interval membership.

### Syntax

#### Macro form

```c
bool VALUE_TYPED_FUNC(IsBetween, I32)(int32_t value, int32_t minimum, int32_t maximum);
```

#### Direct form

```c
bool Math_Value_IsBetween_I32(int32_t value, int32_t minimum, int32_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `int32_t value` | Value to classify or constrain. |
| `minimum` | `int32_t minimum` | Lower endpoint; see bound-ordering remarks. |
| `maximum` | `int32_t maximum` | Upper endpoint; see bound-ordering remarks. |

---

### Return value

`bool`: true when the condition holds, false otherwise.

---

### Remarks

Returns true only when `value > minimum && value < maximum`. Equality at either endpoint returns false. The function does not reorder inverted endpoints.

---

### Example

```c
bool inside = VALUE_TYPED_FUNC(IsBetween, I32)(5, 1, 9);
/* inside == true; either endpoint would be false */
```

---

# Value Between

Classifies a value relative to two endpoints.

### Syntax

#### Macro form

```c
MATH_TYPE(RangeResult) VALUE_TYPED_FUNC(Between, I32)(int32_t value, int32_t minimum, int32_t maximum);
```

#### Direct form

```c
Math_RangeResult Math_Value_Between_I32(int32_t value, int32_t minimum, int32_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `int32_t value` | Value to classify or constrain. |
| `minimum` | `int32_t minimum` | Lower endpoint; see bound-ordering remarks. |
| `maximum` | `int32_t maximum` | Upper endpoint; see bound-ordering remarks. |

---

### Return value

A value of `MATH_TYPE(RangeResult)`.

---

### Remarks

Returns one of `MATH_RANGE_CONST(LOWER)`, `HIGHER`, `EQUAL_MINIMUM`, `EQUAL_MAXIMUM`, or `BETWEEN`. Both bounds are compared in the stated order; reversed bounds are not normalized. If both bounds equal the value, `EQUAL_MINIMUM` wins.

---

### Example

```c
MATH_TYPE(RangeResult) position = VALUE_TYPED_FUNC(Between, I32)(1, 1, 9);
/* position == MATH_RANGE_CONST(EQUAL_MINIMUM) */
```

---

# Value Clamp package

Header: `Cosmeron/Modules/Math/Value/Clamp.h`

Direct value restriction to an explicit numeric interval.

Specializations: `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128`. The function prototypes below use `I32` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`Clamp`](#value-clamp) | Clamps a scalar value to the indicated range. |

---

# Value Clamp

Clamps a scalar value to the indicated range.

### Syntax

#### Macro form

```c
int32_t VALUE_TYPED_FUNC(Clamp, I32)(int32_t value, int32_t minimum, int32_t maximum);
```

#### Direct form

```c
int32_t Math_Value_Clamp_I32(int32_t value, int32_t minimum, int32_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `int32_t value` | Value to classify or constrain. |
| `minimum` | `int32_t minimum` | Lower endpoint; see bound-ordering remarks. |
| `maximum` | `int32_t maximum` | Upper endpoint; see bound-ordering remarks. |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Returns minimum for a value below it, maximum for a value above it, otherwise the input. **Unlike `Arithmetic/Clamp`, this function does not swap reversed bounds.** Use ordered bounds. Floating-point NaNs are not handled specially.

---

### Example

```c
int32_t result = VALUE_TYPED_FUNC(Clamp, I32)(12, 0, 10);
/* result == 10 */
```

---

# Value MaxAndMin package

Header: `Cosmeron/Modules/Math/Value/MaxAndMin.h`

Binary extrema and variadic extrema with status/out-parameter conventions.

Specializations: `I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F128`. The function prototypes below use `I32` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`Max`](#value-max) | Returns the larger of two values. |
| [`Min`](#value-min) | Returns the smaller of two values. |
| [`Smallest`](#value-smallest) | Finds the smallest value among variadic arguments. |
| [`Biggest`](#value-biggest) | Finds the largest value among variadic arguments. |

---

# Value Max

Returns the larger of two values.

### Syntax

#### Macro form

```c
int32_t VALUE_TYPED_FUNC(Max, I32)(int32_t a, int32_t b);
```

#### Direct form

```c
int32_t Math_Value_Max_I32(int32_t a, int32_t b);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Uses `a > b ? a : b`; when values are equal it returns the second operand. Floating NaNs follow ordinary C comparisons, not a dedicated NaN policy.

---

### Example

```c
int32_t highest = VALUE_TYPED_FUNC(Max, I32)(7, 12);
/* highest == 12 */
```

---

# Value Min

Returns the smaller of two values.

### Syntax

#### Macro form

```c
int32_t VALUE_TYPED_FUNC(Min, I32)(int32_t a, int32_t b);
```

#### Direct form

```c
int32_t Math_Value_Min_I32(int32_t a, int32_t b);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `int32_t a` | First operand (or leading coefficient in an equation). |
| `b` | `int32_t b` | Second operand (or linear coefficient in an equation). |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Uses `a < b ? a : b`; equal operands select the second. Floating NaNs are not treated specially.

---

### Example

```c
double lowest = VALUE_TYPED_FUNC(Min, F64)(3.0, -2.0);
/* lowest == -2.0 */
```

---

# Value Smallest

Finds the smallest value among variadic arguments.

### Syntax

#### Macro form

```c
OPSTATUS VALUE_TYPED_FUNC(Smallest, I32)(int32_t *outResult, size_t size, ...);
```

#### Direct form

```c
OPSTATUS Math_Value_Smallest_I32(int32_t *outResult, size_t size, ...);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `outResult` | `int32_t *outResult` | Pointer receiving the selected extreme value. |
| `size` | `size_t size` | Number of following variadic arguments; must be nonzero. |
| `?` | `...` | Input parameter. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Requires non-NULL output and `size > 0`; `size` must equal the exact count of following values. Observe C default argument promotions: I8/I16/U8/U16 generally pass `int`, F32 passes `double`, F128 passes `long double`. Types and count MUST match the implementation's `va_arg` type; mismatches cause undefined behavior.

---

### Example

```c
int32_t smallest = 0;
OPSTATUS status = VALUE_TYPED_FUNC(Smallest, I32)(
    &smallest, 4, 7, 3, 9, 5);
/* SUCCESS: smallest == 3 */
```

---

# Value Biggest

Finds the largest value among variadic arguments.

### Syntax

#### Macro form

```c
OPSTATUS VALUE_TYPED_FUNC(Biggest, I32)(int32_t *outResult, size_t size, ...);
```

#### Direct form

```c
OPSTATUS Math_Value_Biggest_I32(int32_t *outResult, size_t size, ...);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `outResult` | `int32_t *outResult` | Pointer receiving the selected extreme value. |
| `size` | `size_t size` | Number of following variadic arguments; must be nonzero. |
| `?` | `...` | Input parameter. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Requires a valid output and nonzero count, and consumes exactly `size` arguments. Use the correct promoted types for each suffix; `F32` variadic arguments are read as `double`. A wrong count or wrong argument type has undefined behavior.

---

### Example

```c
int32_t largest = 0;
OPSTATUS status = VALUE_TYPED_FUNC(Biggest, I32)(
    &largest, 4, 7, 3, 9, 5);
/* SUCCESS: largest == 9 */
```

---

# Equation Linear package

Header: `Cosmeron/Modules/Math/Equation/Linear.h`

Solve a·x + b = 0 with explicit solution cardinality.

Specializations: `F32, F64, F128`. The function prototypes below use `F64` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`Linear`](#equation-linear) | Solves `a*x + b = 0` over floating-point types. |

---

# Equation Linear

Solves `a*x + b = 0` over floating-point types.

### Syntax

#### Macro form

```c
OPSTATUS EQUATION_TYPED_FUNC(Linear, F64)(double a, double b, double *outResult, MATH_TYPE(Solution);
```

#### Direct form

```c
OPSTATUS Math_Equation_Linear_F64(double a, double b, double *outResult, MATH_TYPE(Solution);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `double a` | First operand (or leading coefficient in an equation). |
| `b` | `double b` | Second operand (or linear coefficient in an equation). |
| `outResult` | `double *outResult` | Writable result pointer; required. |
| `Solution` | `MATH_TYPE(Solution` | Input parameter. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

Writes `SOLUTION_ONE` and `-b/a` when `a != 0`; if `a == 0`, writes `SOLUTION_INFINITE` when `b == 0`, or `SOLUTION_NONE` otherwise. When no unique root exists, `outResult` is not written. Uses exact comparisons to zero and does not validate finite coefficients.

---

### Example

```c
double root = 0.0;
MATH_TYPE(Solution) solutions;
OPSTATUS status = EQUATION_TYPED_FUNC(Linear, F64)(2.0, -8.0, &root, &solutions);
/* SUCCESS: root == 4, solutions == SOLUTION_ONE */
```

---

# Equation Quadratic package

Header: `Cosmeron/Modules/Math/Equation/Quadratic.h`

Discriminant, real roots and (when supported) complex roots of quadratic equations.

Specializations: `F32, F64, F128`. The function prototypes below use `F64` as the representative suffix; replace it with another supported suffix and its matching C type when needed.

## Function summary

| Operation | Description |
| --- | --- |
| [`QuadraticDiscriminant`](#equation-quadraticdiscriminant) | Evaluates the discriminant of `a*x² + b*x + c`. |
| [`Quadratic`](#equation-quadratic) | Solves a quadratic equation for real roots. |
| [`QuadraticComplex`](#equation-quadraticcomplex) | Solves a quadratic equation allowing complex roots. |

---

# Equation QuadraticDiscriminant

Evaluates the discriminant of `a*x² + b*x + c`.

### Syntax

#### Macro form

```c
double EQUATION_TYPED_FUNC(QuadraticDiscriminant, F64)(double a, double b, double c);
```

#### Direct form

```c
double Math_Equation_QuadraticDiscriminant_F64(double a, double b, double c);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `double a` | First operand (or leading coefficient in an equation). |
| `b` | `double b` | Second operand (or linear coefficient in an equation). |
| `c` | `double c` | Constant coefficient of a quadratic equation. |

---

### Return value

A value of the selected numeric C type.

---

### Remarks

Returns `b*b - 4*a*c` using native floating-point arithmetic; it does not return OPSTATUS, validate finiteness or protect from intermediate overflow. Its sign determines how many real roots exist for non-degenerate finite coefficients.

---

### Example

```c
double delta = EQUATION_TYPED_FUNC(QuadraticDiscriminant, F64)(1.0, -3.0, 2.0);
/* delta == 1 */
```

---

# Equation Quadratic

Solves a quadratic equation for real roots.

### Syntax

#### Macro form

```c
OPSTATUS EQUATION_TYPED_FUNC(Quadratic, F64)(double a, double b, double c, double *outResult1, double *outResult2, MATH_TYPE(Solution);
```

#### Direct form

```c
OPSTATUS Math_Equation_Quadratic_F64(double a, double b, double c, double *outResult1, double *outResult2, MATH_TYPE(Solution);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `double a` | First operand (or leading coefficient in an equation). |
| `b` | `double b` | Second operand (or linear coefficient in an equation). |
| `c` | `double c` | Constant coefficient of a quadratic equation. |
| `outResult1` | `double *outResult1` | Writable first root pointer; required. |
| `outResult2` | `double *outResult2` | Writable second root pointer; required. |
| `Solution` | `MATH_TYPE(Solution` | Input parameter. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

If `a == 0`, handles the linear/constant case; if discriminant is negative, reports `SOLUTION_NONE` without writing either root. Discriminant zero reports `SOLUTION_ONE` with both outputs equal; positive discriminant reports `SOLUTION_MULTIPLE` with two outputs. Square root uses an internal iterative implementation (no `-lm` required). Floating coefficients are not checked for NaN/infinity and numeric cancellation may reduce accuracy.

---

### Example

```c
double first = 0, second = 0;
MATH_TYPE(Solution) solutions;
OPSTATUS status = EQUATION_TYPED_FUNC(Quadratic, F64)(
    1.0, -3.0, 2.0, &first, &second, &solutions);
/* SUCCESS: roots are 1 and 2 (order not guaranteed). */
```

---

# Equation QuadraticComplex

Solves a quadratic equation allowing complex roots.

### Syntax

#### Macro form

```c
OPSTATUS EQUATION_TYPED_FUNC(QuadraticComplex, F64)(double a, double b, double c, double _Complex *outResult1, double _Complex *outResult2, MATH_TYPE(Solution);
```

#### Direct form

```c
OPSTATUS Math_Equation_QuadraticComplex_F64(double a, double b, double c, double _Complex *outResult1, double _Complex *outResult2, MATH_TYPE(Solution);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `a` | `double a` | First operand (or leading coefficient in an equation). |
| `b` | `double b` | Second operand (or linear coefficient in an equation). |
| `c` | `double c` | Constant coefficient of a quadratic equation. |
| `outResult1` | `double _Complex *outResult1` | Writable first root pointer; required. |
| `outResult2` | `double _Complex *outResult2` | Writable second root pointer; required. |
| `Solution` | `MATH_TYPE(Solution` | Input parameter. |

---

### Return value

`OPSTATUS`. On success returns `STATUS_CONST(SUCCESS)`; otherwise returns a relevant status, described below.

---

### Remarks

**Only generated when `!COMPILER_MSVC`.** Uses C `_Complex` output types. Negative discriminant produces conjugate complex roots; zero discriminant reports one repeated root, and the `a == 0` cases are handled as linear/constant. The function still uses a real internal square root for `|discriminant|`, with no separate math library link. Use finite coefficients; no general numerical-stability guarantee.

---

### Example

```c
#if !COMPILER_MSVC
double _Complex first = 0, second = 0;
MATH_TYPE(Solution) solutions;
OPSTATUS status = EQUATION_TYPED_FUNC(QuadraticComplex, F64)(
    1.0, 0.0, 1.0, &first, &second, &solutions);
/* SUCCESS: roots +i and -i; solutions == SOLUTION_MULTIPLE. */
#endif
```

---

# Complete examples

## Checked integer operations

```c
#include "Cosmeron/Modules/Math/Arithmetic/Basic.h"

int main(void) {
    int32_t result = 0;
    OPSTATUS status = ARITHMETIC_TYPED_FUNC(Add, I32)(
        INT32_MAX, 1, &result);

    if (status != STATUS_CONST(ARITHMETIC_OVERFLOW))
        return 1;

    status = ARITHMETIC_TYPED_FUNC(Div, I32)(9, 2, &result);
    if (status != STATUS_CONST(SUCCESS) || result != 4)
        return 2;

    status = ARITHMETIC_TYPED_FUNC(Mod, I32)(-9, 4, &result);
    return status == STATUS_CONST(SUCCESS) && result == -1 ? 0 : 3;
}
```

## Saturating arithmetic and range classification

```c
#include "Cosmeron/Modules/Math/Arithmetic/Clamp.h"
#include "Cosmeron/Modules/Math/Value/Between.h"

int main(void) {
    int8_t safe = ARITHMETIC_TYPED_FUNC(SaturatingAdd, I8)(
        INT8_MAX, 1);
    MATH_TYPE(RangeResult) relation =
        VALUE_TYPED_FUNC(Between, I32)(10, 0, 10);

    return safe == INT8_MAX &&
           relation == MATH_RANGE_CONST(EQUAL_MAXIMUM) ? 0 : 1;
}
```

## Variadic extrema with default argument promotions

```c
#include "Cosmeron/Modules/Math/Value/MaxAndMin.h"

int main(void) {
    float smallest = 0.0f;
    OPSTATUS status = VALUE_TYPED_FUNC(Smallest, F32)(
        &smallest, 3, 3.0, 1.0, 2.0); /* double varargs */
    return status == STATUS_CONST(SUCCESS) &&
           smallest == 1.0f ? 0 : 1;
}
```

For `I8`/`I16` and most narrow unsigned types, use `int` variadic arguments; for `F32`, the implementation reads `double`; for `F128`, it reads `long double`. The exact expected vararg type is part of each specialization, not the generic numeric type alone.

## Real equation solver

```c
#include "Cosmeron/Modules/Math/Equation/Quadratic.h"

int main(void) {
    double x1 = 0.0, x2 = 0.0;
    MATH_TYPE(Solution) solutions = MATH_CONST(SOLUTION_NONE);
    OPSTATUS status = EQUATION_TYPED_FUNC(Quadratic, F64)(
        1.0, -3.0, 2.0, &x1, &x2, &solutions);

    if (status != STATUS_CONST(SUCCESS))
        return 1;
    if (solutions != MATH_CONST(SOLUTION_MULTIPLE))
        return 2;
    return (x1 == 1.0 && x2 == 2.0) ||
           (x1 == 2.0 && x2 == 1.0) ? 0 : 3;
}
```

## Complex quadratic roots

```c
#include "Cosmeron/Modules/Math/Equation/Quadratic.h"

int main(void) {
#if !COMPILER_MSVC
    double _Complex x1 = 0.0, x2 = 0.0;
    MATH_TYPE(Solution) solutions = MATH_CONST(SOLUTION_NONE);

    OPSTATUS status = EQUATION_TYPED_FUNC(QuadraticComplex, F64)(
        1.0, 0.0, 1.0, &x1, &x2, &solutions);

    return status == STATUS_CONST(SUCCESS) &&
           solutions == MATH_CONST(SOLUTION_MULTIPLE) ? 0 : 1;
#else
    /* The complex specialization is not generated under MSVC. */
    return 0;
#endif
}
```

---

# Build and portability

Compile the existing math test suite from the repository root using:

```sh
make -C Codespace/Tests/Math run
```

Its Makefile targets C11 with `-Wall -Wextra -Wpedantic -Werror` and has empty default `LDLIBS`. The quadratic square root is implemented by an internal iterative calculation instead of a call to `sqrt`, avoiding a separate `-lm` dependency in the tested configuration.

**Floating-point caveat:** the in-house square-root implementation assumes ordinary finite inputs. The real/complex quadratic solvers do not check arbitrary infinity or NaN coefficients. For extremely large/small magnitudes or near-canceling roots, floating-point overflow, underflow, or loss of accuracy may occur. In particular, a nonfinite positive discriminant is not a supported input to the iterative square root.

**Compiler caveat:** the `QuadraticComplex` family is guarded by `#if !COMPILER_MSVC` and uses C `_Complex` and `I`. Real-valued equation solvers remain instantiated for `F32`, `F64`, `F128` on both branches.

---

# Notes

- The 22 entries above are distinct **function templates**; most instantiate across 8 integer or 11 integer/floating suffixes, and equation functions across three floating suffixes.
- The namespace utilities in `Math.space`, including `MATH_FUNC`, `MATH_TYPED_FUNC`, `MATH_TYPE`, `MATH_CONST`, and `MATH_RANGE_CONST`, are public preprocessor access forms rather than independent runtime functions.
- The code in `Impl/*.impl` supplies implementations and should not be included directly by consumer code.
- This reference was checked against the current package headers and implementation. Example syntax and Markdown links are verified independently; it does not claim a new CI run or a local build.
