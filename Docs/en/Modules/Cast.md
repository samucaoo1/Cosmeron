# Cast

The **Cast** module provides generated, checked conversions between supported C primitive types. It belongs to **Core** and complements `Type`: Type defines primitive aliases and metadata, while Cast defines conversions with range checks where required.

```c
#include "Cosmeron/Core/Types/Cast/Cast.h"
```

The aggregate header includes `Converting.h`, `Integer.h` and `Float.h`.

---

# Overview

| Header | Purpose |
| --- | --- |
| `Converting.h` | Conversion specification macros and code-generation engine |
| `Integer.h` | Integer-to-integer checked conversions |
| `Float.h` | Floating-point and integer/floating-point conversions |

Generated functions return `OPSTATUS` and write the converted value through an output pointer. This makes narrowing and sign-sensitive conversions explicit.

---

# Integer conversions

The Integer package generates conversion operations for source types `Uint64`, `Uint32`, `Uint16`, `Uint8`, `Int64`, `Int32`, `Int16`, `Int8` and `Size`. It only generates destination pairs declared by `Integer.h`; an arbitrary pair is not automatically available.

The generated operation name concatenates the source type, the literal `to`, and the destination suffix. For example:

```c
uint32_t out = 0;
OPSTATUS status =
    MODULE_FUNC(Cast, Integer, Uint64to32)(value, &out);
```

The operation validates its output pointer and returns `ARITHMETIC_OVERFLOW` when the value is outside the target range. A failed conversion does not write the result.

---

# Floating-point conversions

The Float package declares conversions among `Float`, `Double`, `LongDouble`, selected signed and unsigned integer types, and `Size`. Its conversion table is explicit; it is not a complete Cartesian product of every primitive type.

```c
double out = 0.0;
OPSTATUS status =
    MODULE_FUNC(Cast, Float, FloattoDouble)(input, &out);
```

Float-to-integer operations check configured target bounds before casting. Float narrowing uses the maximum bound provided by its table entry. Integer-to-floating conversions are direct conversions and may lose precision when the destination format cannot exactly represent the source value.

Check `OPSTATUS` before using the output. Overflow is reported as `ARITHMETIC_OVERFLOW`; a null output pointer returns `INVALID_ARGUMENT`.

---

# Conversion generator

`Converting.h` exposes the specification vocabulary used by the package headers:

| Macro | Meaning |
| --- | --- |
| `TO(name, type, category)` | Declares a conversion without an explicit limits tuple |
| `TO_EX(name, type, category, limits...)` | Declares a conversion with explicit range/limit information |
| `CONVERTING_OPERATIONS(source, ...)` | Generates ordinary package operations |
| `CONST_CONVERTING_OPERATIONS(source, ...)` | Generates const operations |
| `CONVERTING_INLINE(source, ...)` | Generates inline conversion operations |
| `CONST_CONVERTING_INLINE(source, ...)` | Generates const inline conversion operations |

These are primarily code-generation tools for defining conversion tables. Most application code should call generated conversion functions rather than use the macros directly.

---

# Guidance and limitations

- Prefer Cast over an unchecked C cast when the API provides the required pair and runtime range validation is needed.
- Range safety is not the same as exactness: a representable integer may still lose precision when converted to floating point.
- Only conversions explicitly listed in `Integer.h` and `Float.h` are generated.
- Cast does not provide conversions for arbitrary user-defined structs, pointers, strings or numeric types from the separate `Numeric` module.
- Unit conversion, rounding policy and semantic validation remain application responsibilities.
