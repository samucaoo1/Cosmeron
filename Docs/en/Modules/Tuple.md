# Tuple

The **Tuple** module provides fixed-layout, typed POD structures for grouping two to five values. It includes both positional tuple types and semantic two-value aliases, with generated pack and unpack functions.

## Headers

~~~c
#include "Cosmeron/Modules/Tuple/Tuple.h"
~~~

The aggregate header includes `Pair`, `Dual`, `Triple`, `Quad` and `Penta` templates.

---

# Overview

| Type | Arity | Intended use |
| --- | ---: | --- |
| `Pair<T>` | 2 | Generic ordered pair. |
| `Dual<T>` | 2 | Two related values with domain-specific aliases such as coordinate, size, interval or complex components. |
| `Triple<T>` | 3 | Three related values, such as a 3D coordinate or RGB channels. |
| `Quad<T>` | 4 | Four related values, such as a rectangle or RGBA channels. |
| `Penta<T>` | 5 | Five ordered values. |

Each specialization is a `STRUCT` with ordinary C storage. It has no heap ownership, hidden allocation or object receiver. Direct structure initialization and assignment are supported.

---

## Built-in element types

The `TUPLE_TYPE_TABLE` in `Modules/Tuple/Impl/Common.inc` supplies these element suffixes:

- Boolean;
- signed and unsigned integers: Int8/16/32/64 and UInt8/16/32/64;
- character widths: Char, Char8, Char16 and Char32;
- floating point: Float32, Float64, Float, Double and LongDouble;
- C integer/size types: Int, UInt, Size, PtrDiff, IntPtr and UIntPtr;
- FloatComplex, DoubleComplex and LongDoubleComplex when the compiler is not MSVC.

For example, `MODULE_TYPED(Tuple, Pair, Int32)` names a pair of `int32_t` values.

---

# API reference

Every tuple template provides the same two package functions:

| Function | Contract |
| --- | --- |
| `Pack(value1, value2, ...)` | Returns a tuple initialized from the supplied values. |
| `Unpack(tuple, out1, out2, ...)` | Copies tuple elements to caller-provided output pointers and returns `OPSTATUS`. |

The arity determines the number of arguments. `Unpack` rejects a null output pointer with `INVALID_ARGUMENT`.

## Pair

The `Pair` fields include the canonical names `value1`/`first` and `value2`/`second`, aliases `a`/`b` and `x1`/`x2`, as well as the array aliases `value[2]` and `values[2]`.

~~~c
MODULE_TYPED(Tuple, Pair, Int32) pair =
    MODULE_FUNC(Tuple, Pair, Int32, Pack)(10, 20);

int32_t first = 0;
int32_t second = 0;
OPSTATUS status = MODULE_FUNC(Tuple, Pair, Int32, Unpack)(
    pair, &first, &second);
~~~

## Dual

`Dual` is a two-value POD whose first field aliases are `value1`, `a`, `x1`, `x`, `col`, `length`, `real`, `state1`, `begin`, `first` and `start`. Its second field aliases are `value2`, `b`, `x2`, `y`, `row`, `width`, `imaginary`, `state2`, `end`, `second` and `last`.

~~~c
MODULE_TYPED(Tuple, Dual, UInt16) dimensions =
    MODULE_FUNC(Tuple, Dual, UInt16, Pack)(80U, 25U);

uint16_t columns = dimensions.col;
uint16_t rows = dimensions.row;
~~~

Use the alias that expresses the meaning of the values in the consuming code. Because these are union aliases, write one semantic interpretation and read it through the corresponding alias; do not treat the aliases as independent storage.

## Triple, Quad and Penta

- `Triple` exposes `first`/`second`/`third`, `a`/`b`/`c`, `x`/`y`/`z`, `length`/`width`/`height`, `red`/`green`/`blue`, and `value[3]`/`values[3]` aliases.
- `Quad` exposes `first` through `fourth`, positional aliases, `value[4]`/`values[4]`, geometric aliases `left`, `right`, `top`, `bottom` and color aliases `red`, `green`, `blue`, `alpha`.
- `Penta` exposes `first` through `fifth`, `value[5]` and `values[5]`.

For a rectangle-like value, for example, `MODULE_FUNC(Tuple, Quad, UInt16, Pack)(left, right, top, bottom)` returns a `Quad` whose fields can be read using those semantic aliases.

The exact aliases are intended for convenient naming, but the values still occupy a single shared POD tuple representation.

---

# Template use

The tuple headers instantiate the current built-in type table automatically. The public template definitions are available in the corresponding `.def` files:

- `Pair.def`
- `Dual.def`
- `Triple.def`
- `Quad.def`
- `Penta.def`

The common template engine can be used to instantiate another compatible element type. The generated type suffix and C element type must be supplied in the same order expected by the matching `TEMPLATE_Tuple_<Name>(SUFFIX, TYPE)` macro.

---

# Contracts and limitations

- Tuple values are stored by value and do not allocate memory.
- `Pack` returns the fully initialized tuple by value.
- `Unpack` writes each element to a distinct caller-owned output pointer.
- The type is only as safe to copy as its element type. The built-in specializations use scalar C types; a custom element type with ownership or self-referential invariants should not be treated as trivially copyable without checking its contract.
- Tuple types do not provide dynamic length, iteration, bounds checks or heterogeneous element types.
- Use a dedicated struct when the elements have different C types or need domain-specific invariants.

## Related modules

- [Types](Types.md) — canonical primitive names.
- [Module](Module.md) — generated POD structures and template engine.

## Source headers

- `Codespace/Cosmeron/Modules/Tuple/Tuple.h`
- `Codespace/Cosmeron/Modules/Tuple/Pair.def`
- `Codespace/Cosmeron/Modules/Tuple/Dual.def`
- `Codespace/Cosmeron/Modules/Tuple/Triple.def`
- `Codespace/Cosmeron/Modules/Tuple/Quad.def`
- `Codespace/Cosmeron/Modules/Tuple/Penta.def`
