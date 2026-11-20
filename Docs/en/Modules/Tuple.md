# Tuple

The **Tuple** module provides fixed-arity, homogeneous tuple structures. Each specialization is a small C value type with positional fields, array-style access and generated `Pack`/`Unpack` helpers. It belongs to **Modules**.

```c
#include "Cosmeron/Modules/Tuple/Tuple.h"
```

The aggregate header includes `Pair`, `Dual`, `Triple`, `Quad` and `Penta`.

---

# Overview

| Type family | Arity | Typical use |
| --- | ---: | --- |
| `Pair` | 2 | Two values of the same type |
| `Dual` | 2 | Two values with domain-oriented aliases |
| `Triple` | 3 | Three components such as RGB, XYZ or dimensions |
| `Quad` | 4 | Four components such as a rectangle or RGBA |
| `Penta` | 5 | Five related values |

All members of a specialization share the same C type. These are not heterogeneous tuples like `std::tuple` or Python tuples.

## Built-in type specializations

`TUPLE_TYPE_TABLE` generates specializations for `bool`, signed and unsigned 8/16/32/64-bit integers, `char`, `uint8_t`/`uint16_t`/`uint32_t` character units, `float`, `double`, `long double`, `int`, `unsigned int`, `size_t`, `ptrdiff_t`, `intptr_t` and `uintptr_t`. Complex floating-point specializations are included on non-MSVC compilers.

Use `MODULE_TYPED(Tuple, Family, Suffix)` for a generated type and `MODULE_FUNC(Tuple, Family, Suffix, Function)` for generated operations. The suffix identifies a row in `TUPLE_TYPE_TABLE`, such as `Int32`, `Float64` or `Size`.

---

# Pair

`Pair` exposes several aliases for its two positions:

| Position | Aliases |
| --- | --- |
| First | `value1`, `first`, `a`, `x1`, `x` |
| Second | `value2`, `second`, `b`, `x2`, `y` |

The same storage is available as `value[2]` or `values[2]`.

# Dual

`Dual` is also a two-value structure, but its aliases are intentionally broader for domain-oriented code.

| First value | Second value |
| --- | --- |
| `value1`, `a`, `x1`, `x`, `col`, `length`, `real`, `state1`, `begin`, `first`, `start` | `value2`, `b`, `x2`, `y`, `row`, `width`, `imaginary`, `state2`, `end`, `second`, `last` |

It also exposes `value[2]` and `values[2]`.

# Triple

`Triple` exposes `value1`/`first`/`a`/`x1`/`x`/`length`/`red`, `value2`/`second`/`b`/`x2`/`y`/`width`/`green`, and `value3`/`third`/`c`/`x3`/`z`/`height`/`blue`. It also exposes `value[3]` and `values[3]`.

# Quad

`Quad` exposes four positions with aliases useful for coordinate, rectangle and color data: first (`value1`, `first`, `a`, `x1`, `x`, `left`, `red`), second (`value2`, `second`, `b`, `x2`, `y`, `right`, `green`), third (`value3`, `third`, `c`, `x3`, `z`, `top`, `blue`), and fourth (`value4`, `fourth`, `d`, `x4`, `w`, `bottom`, `alpha`). It also exposes `value[4]` and `values[4]`.

# Penta

`Penta` exposes five positions through `value1`/`first`/`a` through `value5`/`fifth`/`e`, plus `value[5]` and `values[5]`.

---

# Packing and unpacking

Each family generates `Pack(v1, ..., vN)` which returns a tuple with its members filled in order, and `Unpack(tuple, &v1, ..., &vN)` which writes stored positions to output pointers and returns `OPSTATUS`.

```c
MODULE_TYPED(Tuple, Pair, Int32) coordinate =
    MODULE_FUNC(Tuple, Pair, Int32, Pack)(12, 34);

int32_t x = 0, y = 0;
OPSTATUS status =
    MODULE_FUNC(Tuple, Pair, Int32, Unpack)(coordinate, &x, &y);
```

A three-component value uses the same pattern:

```c
MODULE_TYPED(Tuple, Triple, Float32) rgb =
    MODULE_FUNC(Tuple, Triple, Float32, Pack)(0.2f, 0.4f, 0.8f);
```

---

# Usage guidance

- Prefer named aliases (`col`, `row`, `red`, `green`, `blue`) when they clarify a position.
- Prefer `value[N]` when iterating over homogeneous components.
- Prefer `Pair` for a generic pair, and `Dual` when a pair's semantic aliases are useful.
- These tuple structures do not allocate memory or manage the lifetime of pointed-to values.
- Anonymous struct/union aliases are part of the public representation; compile with a toolchain mode supported by the project's headers.
