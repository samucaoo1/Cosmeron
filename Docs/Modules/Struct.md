# Struct

The **Struct** module provides small, fixed-size homogeneous data groups with two, three, four or five elements. Every group is defined using C11 unions and anonymous structures, allowing the same element to be addressed by several meaningful field names or by an indexed array.

The types contain values directly. They do **not** allocate memory, own external pointers, or require initialization/destruction functions.

There is currently **no single `Struct.h` aggregate header**. Include the family headers you use:

```c
#include "Cosmeron/Modules/Struct/TPair.h"
#include "Cosmeron/Modules/Struct/TDual.h"
#include "Cosmeron/Modules/Struct/TTriple.h"
#include "Cosmeron/Modules/Struct/TQuad.h"
#include "Cosmeron/Modules/Struct/TPenta.h"
```

---

# Overview

| Family | Header | Elements | Indexed view | Purpose |
| --- | --- | ---: | --- | --- |
| [`TPair`](#tpair-package) | `TPair.h` | 2 | `values[2]` | Two same-typed values with compact pair/coordinate aliases. |
| [`TDual`](#tdual-package) | `TDual.h` | 2 | `value[2]` | Two same-typed values with broader aliases for ranges, dimensions, coordinates and state. |
| [`TTriple`](#ttriple-package) | `TTriple.h` | 3 | `values[3]` | Three values with 3D-position, dimension and numbered-state aliases. |
| [`TQuad`](#tquad-package) | `TQuad.h` | 4 | `values[4]` | Four values, including rectangle-edge and 4D-position aliases. |
| [`TPenta`](#tpenta-package) | `TPenta.h` | 5 | `values[5]` | Five values with positional, named and numbered aliases. |

## Type access

### Macro form

```c
TPAIR_TYPE(int32) point = { .x = 10, .y = 20 };
```

### Direct form

```c
Struct_TPair_int32 point = { .x = 10, .y = 20 };
```

With the default namespace these are the same generated type. If `COSMERON_NAMESPACE` is customized before including the headers, the direct `Struct_...` names gain the configured namespace prefix; the macro form is namespace-independent.

## Conversion functions

Each family exposes two generated **Cast** functions: one to group individual values and one to extract them. Their public function macros are in the **Cast** namespace, rather than `TPAIR_FUNC` or `STRUCT_FUNC`.

### Macro form

```c
TPAIR_TYPE(int32) pair = CAST_TYPE_TO_STRUCT(int32, TPair)(10, 20);
int32_t first = 0, second = 0;
CAST_STRUCT_TO_TYPE(TPair, int32)(pair, &first, &second);
```

### Direct form

```c
Struct_TPair_int32 pair = Cast_int32_To_TPair(10, 20);
int32_t first = 0, second = 0;
Cast_TPair_To_int32(pair, &first, &second);
```

The generated `TPAIR_FUNC(SUFFIX, FUNC)`, `TDUAL_FUNC`, `TTRIPLE_FUNC`, `TQUAD_FUNC`, `TPENTA_FUNC` and general `STRUCT_FUNC` macros describe potential family-specific function naming. **The current five packages do not instantiate additional public functions using those macros.** Do not mistake a function-name macro for an existing implementation.

---

# API reference

## Function summary

| Family | Function | Returns | Description |
| --- | --- | --- | --- |
| TPair | [`ToTPair`](#tpair-totpair) | `TPair` value | Constructs 2 same-typed elements by value |
| TPair | [`FromTPair`](#tpair-fromtpair) | `void` | Copies 2 stored elements into output arguments |
| TDual | [`ToTDual`](#tdual-totdual) | `TDual` value | Constructs 2 same-typed elements by value |
| TDual | [`FromTDual`](#tdual-fromtdual) | `void` | Copies 2 stored elements into output arguments |
| TTriple | [`ToTTriple`](#ttriple-tottriple) | `TTriple` value | Constructs 3 same-typed elements by value |
| TTriple | [`FromTTriple`](#ttriple-fromttriple) | `void` | Copies 3 stored elements into output arguments |
| TQuad | [`ToTQuad`](#tquad-totquad) | `TQuad` value | Constructs 4 same-typed elements by value |
| TQuad | [`FromTQuad`](#tquad-fromtquad) | `void` | Copies 4 stored elements into output arguments |
| TPenta | [`ToTPenta`](#tpenta-totpenta) | `TPenta` value | Constructs 5 same-typed elements by value |
| TPenta | [`FromTPenta`](#tpenta-fromtpenta) | `void` | Copies 5 stored elements into output arguments |

Function names `ToTPair` / `FromTPair` in this table are descriptive operation labels. The real generated C functions use `Cast_<suffix>_To_<family>` and `Cast_<family>_To_<suffix>`, as illustrated in each syntax section.

---

# Supported type specializations

All five families have these **11 built-in specializations**:

| C element type | Suffix |
| --- | --- |
| `uint8_t` | `uint8` |
| `uint16_t` | `uint16` |
| `uint32_t` | `uint32` |
| `uint64_t` | `uint64` |
| `int8_t` | `int8` |
| `int16_t` | `int16` |
| `int32_t` | `int32` |
| `int64_t` | `int64` |
| `float` | `float` |
| `double` | `double` |
| `long double` | `longdouble` |

Additional specializations:

| Specialization | TPair | TDual | TTriple | TQuad | TPenta |
| --- | --- | --- | --- | --- | --- |
| `size_t` (suffix `size`) | No | Yes | No | No | No |
| `float _Complex` (suffix `float_complex`) | No | Yes | Yes | Yes | Yes |
| `double _Complex` (suffix `double_complex`) | No | Yes | Yes | Yes | Yes |
| `long double _Complex` (suffix `longdouble_complex`) | No | Yes | Yes | Yes | Yes |

The three complex specializations are conditionally generated when `!COMPILER_MSVC`; they are not present under the MSVC branch.

**Total:** 68 built-in family/type specializations, generating **136 concrete Cast functions** (two per specialization). The reference below describes the **10 public operation templates**, without repeating identical documentation for every suffix.

---

# Data layout and field aliases

Each field slot is a union of names referring to the same value. The outer union also exposes an indexed view. Aliases are not additional independent elements: assigning `.x` changes the first stored value, also visible as `.value1` or `.a`.

| Family | First slot | Second slot | Additional slot names |
| --- | --- | --- | --- |
| TPair | `first`, `x`, `value1` | `second`, `y`, `value2` | None |
| TDual | `start`, `length`, `col`, `real` | `end`, `width`, `row`, `imaginary` | None |
| TTriple | `x`, `length` | `y`, `width` | `z`, `height` |
| TQuad | `x`, `left` | `y`, `right` | `z`, `top`; `w`, `bottom` |
| TPenta | `x`, `c1` | `y`, `c2` | `z`/`c3`, `w`/`c4`, `t`/`c5` |

The complete alias tables appear in the package sections.

**Array naming:** `TDual` uses `value[2]` (singular), while `TPair`, `TTriple`, `TQuad` and `TPenta` use `values[N]` (plural). This distinction is intentional in the current headers.

C11 supports the anonymous structures and unions used by these declarations. The library's tests verify multiple indexed/named views and selected size properties. Do not assume that every arbitrary custom element type yields identical binary layouts across compilers, ABIs or serialization formats. These types are intended for in-process values, not automatically portable wire structures.

---

# General contracts

- **Value semantics:** constructors return a structure/union **by value**; extractors receive a copy of the group **by value** and write the individual items through pointers.
- **No status return:** constructors return a concrete typed value and extractors return `void`; there is no `OPSTATUS` failure contract for these two operations.
- **Output pointers:** extractor implementations dereference every output pointer without NULL checks. Pass valid writable addresses of the matching element type.
- **Aliases:** all symbolic names for the same position refer to the same conceptual element. Choosing a semantic name such as `top`, `start` or `height` does not add behavior or coordinate transformations.
- **No deep ownership:** if a custom specialization uses a pointer type, values are copied as pointers; no pointed-to allocation is automatically freed.
- **No arithmetic:** the current module supplies grouping and extraction, not mathematical vector operations, comparison, serialization, sorting or dynamic container management.

---

# TPair package

Header: `Cosmeron/Modules/Struct/TPair.h`

Two same-typed values with compact pair/coordinate aliases.

2 positions of the same C element type, with the built-in type `TPAIR_TYPE(int32)` (direct: `Struct_TPair_int32`).

## Field aliases

| Zero-based position | Canonical field | Alternative names | Indexed access |
| ---: | --- | --- | --- |
| 0 | `value1` | `first`, `a`, `x1`, `x` | `values[0]` |
| 1 | `value2` | `second`, `b`, `x2`, `y` | `values[1]` |

All names in a row address the same position. Values can be constructed directly with a C initializer, through a Cast constructor or by assignment.

```c
TPAIR_TYPE(int32) pair = CAST_TYPE_TO_STRUCT(int32, TPair)(10, 20);
```

### Function summary

| Function | Description |
| --- | --- |
| [`ToTPair`](#tpair-totpair) | Groups 2 scalar values as one `TPair` |
| [`FromTPair`](#tpair-fromtpair) | Extracts the 2 scalar values through output pointers |

---

# TPair ToTPair

Builds a `TPair` directly from 2 input values of the selected element type.

### Syntax

#### Macro form

```c
static inline TPAIR_TYPE(int32) CAST_TYPE_TO_STRUCT(int32, TPair)(int32_t value1, int32_t value2);
```

#### Direct form

```c
static inline Struct_TPair_int32 Cast_int32_To_TPair(int32_t value1, int32_t value2);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value1` | `int32_t` | Value written to element index `0` (`.value1`). |
| `value2` | `int32_t` | Value written to element index `1` (`.value2`). |

---

### Return value

`TPAIR_TYPE(int32)`: a new `TPair` value containing the supplied elements in their original order. The family is instantiated for other supported suffixes using the corresponding element type.

---

### Remarks

- No dynamic allocation, failure status or implicit numeric conversion is performed.
- The returned value can be assigned or passed to another function like an ordinary C value.
- This function groups values; it does not share the storage of caller arguments.
- To use a different specialization, replace the suffix in `CAST_TYPE_TO_STRUCT(int32, TPair)`, the element C type, and the result type consistently.

---

### Example

```c
TPAIR_TYPE(int32) pair = CAST_TYPE_TO_STRUCT(int32, TPair)(10, 20);
/* pair.first == 10 */
/* pair.values[1] == 20 */
```

---

# TPair FromTPair

Copies all 2 elements from an existing `TPair` value into independent caller-provided output variables.

### Syntax

#### Macro form

```c
static inline void CAST_STRUCT_TO_TYPE(TPair, int32)(TPAIR_TYPE(int32) pair, int32_t *var1, int32_t *var2);
```

#### Direct form

```c
static inline void Cast_TPair_To_int32(Struct_TPair_int32 pair, int32_t *var1, int32_t *var2);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `pair` | `TPAIR_TYPE(int32)` | Input group **passed by value**. |
| `var1` | `int32_t *` | Required writable pointer for element `0`. |
| `var2` | `int32_t *` | Required writable pointer for element `1`. |

---

### Return value

None (`void`). The element values are written to the supplied pointers.

---

### Remarks

- The implementation assigns `*var1 = pair.value1` through `*var2 = pair.value2` in order.
- **No NULL-pointer validation** occurs. Every output pointer must point to writable storage of the proper element type.
- The input group itself is not modified: it is passed by value.
- Output variables may be independent from the original group.

---

### Example

```c
TPAIR_TYPE(int32) pair = CAST_TYPE_TO_STRUCT(int32, TPair)(10, 20);
int32_t item1 = 0;
int32_t item2 = 0;
CAST_STRUCT_TO_TYPE(TPair, int32)(pair, &item1, &item2);
/* item1 == 10; item2 == 20 */
```

---

# TDual package

Header: `Cosmeron/Modules/Struct/TDual.h`

Two same-typed values with broader aliases for ranges, dimensions, coordinates and state.

2 positions of the same C element type, with the built-in type `TDUAL_TYPE(int32)` (direct: `Struct_TDual_int32`).

## Field aliases

| Zero-based position | Canonical field | Alternative names | Indexed access |
| ---: | --- | --- | --- |
| 0 | `value1` | `a`, `x1`, `x`, `col`, `length`, `real`, `state1`, `begin`, `first`, `start` | `value[0]` |
| 1 | `value2` | `b`, `x2`, `y`, `row`, `width`, `imaginary`, `state2`, `end`, `second`, `last` | `value[1]` |

All names in a row address the same position. Values can be constructed directly with a C initializer, through a Cast constructor or by assignment.

```c
TDUAL_TYPE(int32) dual = CAST_TYPE_TO_STRUCT(int32, TDual)(10, 20);
```

### Function summary

| Function | Description |
| --- | --- |
| [`ToTDual`](#tdual-totdual) | Groups 2 scalar values as one `TDual` |
| [`FromTDual`](#tdual-fromtdual) | Extracts the 2 scalar values through output pointers |

---

# TDual ToTDual

Builds a `TDual` directly from 2 input values of the selected element type.

### Syntax

#### Macro form

```c
static inline TDUAL_TYPE(int32) CAST_TYPE_TO_STRUCT(int32, TDual)(int32_t value1, int32_t value2);
```

#### Direct form

```c
static inline Struct_TDual_int32 Cast_int32_To_TDual(int32_t value1, int32_t value2);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value1` | `int32_t` | Value written to element index `0` (`.value1`). |
| `value2` | `int32_t` | Value written to element index `1` (`.value2`). |

---

### Return value

`TDUAL_TYPE(int32)`: a new `TDual` value containing the supplied elements in their original order. The family is instantiated for other supported suffixes using the corresponding element type.

---

### Remarks

- No dynamic allocation, failure status or implicit numeric conversion is performed.
- The returned value can be assigned or passed to another function like an ordinary C value.
- This function groups values; it does not share the storage of caller arguments.
- To use a different specialization, replace the suffix in `CAST_TYPE_TO_STRUCT(int32, TDual)`, the element C type, and the result type consistently.

---

### Example

```c
TDUAL_TYPE(int32) dual = CAST_TYPE_TO_STRUCT(int32, TDual)(10, 20);
/* dual.a == 10 */
/* dual.value[1] == 20 */
```

---

# TDual FromTDual

Copies all 2 elements from an existing `TDual` value into independent caller-provided output variables.

### Syntax

#### Macro form

```c
static inline void CAST_STRUCT_TO_TYPE(TDual, int32)(TDUAL_TYPE(int32) dual, int32_t *var1, int32_t *var2);
```

#### Direct form

```c
static inline void Cast_TDual_To_int32(Struct_TDual_int32 dual, int32_t *var1, int32_t *var2);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `dual` | `TDUAL_TYPE(int32)` | Input group **passed by value**. |
| `var1` | `int32_t *` | Required writable pointer for element `0`. |
| `var2` | `int32_t *` | Required writable pointer for element `1`. |

---

### Return value

None (`void`). The element values are written to the supplied pointers.

---

### Remarks

- The implementation assigns `*var1 = dual.value1` through `*var2 = dual.value2` in order.
- **No NULL-pointer validation** occurs. Every output pointer must point to writable storage of the proper element type.
- The input group itself is not modified: it is passed by value.
- Output variables may be independent from the original group.

---

### Example

```c
TDUAL_TYPE(int32) dual = CAST_TYPE_TO_STRUCT(int32, TDual)(10, 20);
int32_t item1 = 0;
int32_t item2 = 0;
CAST_STRUCT_TO_TYPE(TDual, int32)(dual, &item1, &item2);
/* item1 == 10; item2 == 20 */
```

---

# TTriple package

Header: `Cosmeron/Modules/Struct/TTriple.h`

Three values with 3D-position, dimension and numbered-state aliases.

3 positions of the same C element type, with the built-in type `TTRIPLE_TYPE(int32)` (direct: `Struct_TTriple_int32`).

## Field aliases

| Zero-based position | Canonical field | Alternative names | Indexed access |
| ---: | --- | --- | --- |
| 0 | `value1` | `a`, `x1`, `x`, `length`, `state1` | `values[0]` |
| 1 | `value2` | `b`, `x2`, `y`, `width`, `state2` | `values[1]` |
| 2 | `value3` | `c`, `x3`, `z`, `height`, `state3` | `values[2]` |

All names in a row address the same position. Values can be constructed directly with a C initializer, through a Cast constructor or by assignment.

```c
TTRIPLE_TYPE(int32) triple = CAST_TYPE_TO_STRUCT(int32, TTriple)(10, 20, 30);
```

### Function summary

| Function | Description |
| --- | --- |
| [`ToTTriple`](#ttriple-tottriple) | Groups 3 scalar values as one `TTriple` |
| [`FromTTriple`](#ttriple-fromttriple) | Extracts the 3 scalar values through output pointers |

---

# TTriple ToTTriple

Builds a `TTriple` directly from 3 input values of the selected element type.

### Syntax

#### Macro form

```c
static inline TTRIPLE_TYPE(int32) CAST_TYPE_TO_STRUCT(int32, TTriple)(int32_t value1, int32_t value2, int32_t value3);
```

#### Direct form

```c
static inline Struct_TTriple_int32 Cast_int32_To_TTriple(int32_t value1, int32_t value2, int32_t value3);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value1` | `int32_t` | Value written to element index `0` (`.value1`). |
| `value2` | `int32_t` | Value written to element index `1` (`.value2`). |
| `value3` | `int32_t` | Value written to element index `2` (`.value3`). |

---

### Return value

`TTRIPLE_TYPE(int32)`: a new `TTriple` value containing the supplied elements in their original order. The family is instantiated for other supported suffixes using the corresponding element type.

---

### Remarks

- No dynamic allocation, failure status or implicit numeric conversion is performed.
- The returned value can be assigned or passed to another function like an ordinary C value.
- This function groups values; it does not share the storage of caller arguments.
- To use a different specialization, replace the suffix in `CAST_TYPE_TO_STRUCT(int32, TTriple)`, the element C type, and the result type consistently.

---

### Example

```c
TTRIPLE_TYPE(int32) triple = CAST_TYPE_TO_STRUCT(int32, TTriple)(10, 20, 30);
/* triple.a == 10 */
/* triple.values[2] == 30 */
```

---

# TTriple FromTTriple

Copies all 3 elements from an existing `TTriple` value into independent caller-provided output variables.

### Syntax

#### Macro form

```c
static inline void CAST_STRUCT_TO_TYPE(TTriple, int32)(TTRIPLE_TYPE(int32) triple, int32_t *var1, int32_t *var2, int32_t *var3);
```

#### Direct form

```c
static inline void Cast_TTriple_To_int32(Struct_TTriple_int32 triple, int32_t *var1, int32_t *var2, int32_t *var3);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `triple` | `TTRIPLE_TYPE(int32)` | Input group **passed by value**. |
| `var1` | `int32_t *` | Required writable pointer for element `0`. |
| `var2` | `int32_t *` | Required writable pointer for element `1`. |
| `var3` | `int32_t *` | Required writable pointer for element `2`. |

---

### Return value

None (`void`). The element values are written to the supplied pointers.

---

### Remarks

- The implementation assigns `*var1 = triple.value1` through `*var3 = triple.value3` in order.
- **No NULL-pointer validation** occurs. Every output pointer must point to writable storage of the proper element type.
- The input group itself is not modified: it is passed by value.
- Output variables may be independent from the original group.

---

### Example

```c
TTRIPLE_TYPE(int32) triple = CAST_TYPE_TO_STRUCT(int32, TTriple)(10, 20, 30);
int32_t item1 = 0;
int32_t item2 = 0;
int32_t item3 = 0;
CAST_STRUCT_TO_TYPE(TTriple, int32)(triple, &item1, &item2, &item3);
/* item1 == 10; item3 == 30 */
```

---

# TQuad package

Header: `Cosmeron/Modules/Struct/TQuad.h`

Four values, including rectangle-edge and 4D-position aliases.

4 positions of the same C element type, with the built-in type `TQUAD_TYPE(int32)` (direct: `Struct_TQuad_int32`).

## Field aliases

| Zero-based position | Canonical field | Alternative names | Indexed access |
| ---: | --- | --- | --- |
| 0 | `value1` | `a`, `x1`, `x`, `left`, `state1` | `values[0]` |
| 1 | `value2` | `b`, `x2`, `y`, `right`, `state2` | `values[1]` |
| 2 | `value3` | `c`, `x3`, `z`, `top`, `y1`, `state3` | `values[2]` |
| 3 | `value4` | `d`, `x4`, `w`, `bottom`, `y2`, `state4` | `values[3]` |

All names in a row address the same position. Values can be constructed directly with a C initializer, through a Cast constructor or by assignment.

```c
TQUAD_TYPE(int32) quad = CAST_TYPE_TO_STRUCT(int32, TQuad)(1, 2, 3, 4);
```

### Function summary

| Function | Description |
| --- | --- |
| [`ToTQuad`](#tquad-totquad) | Groups 4 scalar values as one `TQuad` |
| [`FromTQuad`](#tquad-fromtquad) | Extracts the 4 scalar values through output pointers |

---

# TQuad ToTQuad

Builds a `TQuad` directly from 4 input values of the selected element type.

### Syntax

#### Macro form

```c
static inline TQUAD_TYPE(int32) CAST_TYPE_TO_STRUCT(int32, TQuad)(int32_t value1, int32_t value2, int32_t value3, int32_t value4);
```

#### Direct form

```c
static inline Struct_TQuad_int32 Cast_int32_To_TQuad(int32_t value1, int32_t value2, int32_t value3, int32_t value4);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value1` | `int32_t` | Value written to element index `0` (`.value1`). |
| `value2` | `int32_t` | Value written to element index `1` (`.value2`). |
| `value3` | `int32_t` | Value written to element index `2` (`.value3`). |
| `value4` | `int32_t` | Value written to element index `3` (`.value4`). |

---

### Return value

`TQUAD_TYPE(int32)`: a new `TQuad` value containing the supplied elements in their original order. The family is instantiated for other supported suffixes using the corresponding element type.

---

### Remarks

- No dynamic allocation, failure status or implicit numeric conversion is performed.
- The returned value can be assigned or passed to another function like an ordinary C value.
- This function groups values; it does not share the storage of caller arguments.
- To use a different specialization, replace the suffix in `CAST_TYPE_TO_STRUCT(int32, TQuad)`, the element C type, and the result type consistently.

---

### Example

```c
TQUAD_TYPE(int32) quad = CAST_TYPE_TO_STRUCT(int32, TQuad)(10, 20, 30, 40);
/* quad.a == 10 */
/* quad.values[3] == 40 */
```

---

# TQuad FromTQuad

Copies all 4 elements from an existing `TQuad` value into independent caller-provided output variables.

### Syntax

#### Macro form

```c
static inline void CAST_STRUCT_TO_TYPE(TQuad, int32)(TQUAD_TYPE(int32) quad, int32_t *var1, int32_t *var2, int32_t *var3, int32_t *var4);
```

#### Direct form

```c
static inline void Cast_TQuad_To_int32(Struct_TQuad_int32 quad, int32_t *var1, int32_t *var2, int32_t *var3, int32_t *var4);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `quad` | `TQUAD_TYPE(int32)` | Input group **passed by value**. |
| `var1` | `int32_t *` | Required writable pointer for element `0`. |
| `var2` | `int32_t *` | Required writable pointer for element `1`. |
| `var3` | `int32_t *` | Required writable pointer for element `2`. |
| `var4` | `int32_t *` | Required writable pointer for element `3`. |

---

### Return value

None (`void`). The element values are written to the supplied pointers.

---

### Remarks

- The implementation assigns `*var1 = quad.value1` through `*var4 = quad.value4` in order.
- **No NULL-pointer validation** occurs. Every output pointer must point to writable storage of the proper element type.
- The input group itself is not modified: it is passed by value.
- Output variables may be independent from the original group.

---

### Example

```c
TQUAD_TYPE(int32) quad = CAST_TYPE_TO_STRUCT(int32, TQuad)(10, 20, 30, 40);
int32_t item1 = 0;
int32_t item2 = 0;
int32_t item3 = 0;
int32_t item4 = 0;
CAST_STRUCT_TO_TYPE(TQuad, int32)(quad, &item1, &item2, &item3, &item4);
/* item1 == 10; item4 == 40 */
```

---

# TPenta package

Header: `Cosmeron/Modules/Struct/TPenta.h`

Five values with positional, named and numbered aliases.

5 positions of the same C element type, with the built-in type `TPENTA_TYPE(int32)` (direct: `Struct_TPenta_int32`).

## Field aliases

| Zero-based position | Canonical field | Alternative names | Indexed access |
| ---: | --- | --- | --- |
| 0 | `value1` | `a`, `x1`, `x`, `c1`, `y1`, `state1` | `values[0]` |
| 1 | `value2` | `b`, `x2`, `y`, `c2`, `y2`, `state2` | `values[1]` |
| 2 | `value3` | `c`, `x3`, `z`, `c3`, `y3`, `state3` | `values[2]` |
| 3 | `value4` | `d`, `x4`, `w`, `c4`, `y4`, `state4` | `values[3]` |
| 4 | `value5` | `e`, `x5`, `t`, `c5`, `y5`, `state5` | `values[4]` |

All names in a row address the same position. Values can be constructed directly with a C initializer, through a Cast constructor or by assignment.

```c
TPENTA_TYPE(int32) penta = CAST_TYPE_TO_STRUCT(int32, TPenta)(1, 2, 3, 4, 5);
```

### Function summary

| Function | Description |
| --- | --- |
| [`ToTPenta`](#tpenta-totpenta) | Groups 5 scalar values as one `TPenta` |
| [`FromTPenta`](#tpenta-fromtpenta) | Extracts the 5 scalar values through output pointers |

---

# TPenta ToTPenta

Builds a `TPenta` directly from 5 input values of the selected element type.

### Syntax

#### Macro form

```c
static inline TPENTA_TYPE(int32) CAST_TYPE_TO_STRUCT(int32, TPenta)(int32_t value1, int32_t value2, int32_t value3, int32_t value4, int32_t value5);
```

#### Direct form

```c
static inline Struct_TPenta_int32 Cast_int32_To_TPenta(int32_t value1, int32_t value2, int32_t value3, int32_t value4, int32_t value5);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value1` | `int32_t` | Value written to element index `0` (`.value1`). |
| `value2` | `int32_t` | Value written to element index `1` (`.value2`). |
| `value3` | `int32_t` | Value written to element index `2` (`.value3`). |
| `value4` | `int32_t` | Value written to element index `3` (`.value4`). |
| `value5` | `int32_t` | Value written to element index `4` (`.value5`). |

---

### Return value

`TPENTA_TYPE(int32)`: a new `TPenta` value containing the supplied elements in their original order. The family is instantiated for other supported suffixes using the corresponding element type.

---

### Remarks

- No dynamic allocation, failure status or implicit numeric conversion is performed.
- The returned value can be assigned or passed to another function like an ordinary C value.
- This function groups values; it does not share the storage of caller arguments.
- To use a different specialization, replace the suffix in `CAST_TYPE_TO_STRUCT(int32, TPenta)`, the element C type, and the result type consistently.

---

### Example

```c
TPENTA_TYPE(int32) penta = CAST_TYPE_TO_STRUCT(int32, TPenta)(10, 20, 30, 40, 50);
/* penta.a == 10 */
/* penta.values[4] == 50 */
```

---

# TPenta FromTPenta

Copies all 5 elements from an existing `TPenta` value into independent caller-provided output variables.

### Syntax

#### Macro form

```c
static inline void CAST_STRUCT_TO_TYPE(TPenta, int32)(TPENTA_TYPE(int32) penta, int32_t *var1, int32_t *var2, int32_t *var3, int32_t *var4, int32_t *var5);
```

#### Direct form

```c
static inline void Cast_TPenta_To_int32(Struct_TPenta_int32 penta, int32_t *var1, int32_t *var2, int32_t *var3, int32_t *var4, int32_t *var5);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `penta` | `TPENTA_TYPE(int32)` | Input group **passed by value**. |
| `var1` | `int32_t *` | Required writable pointer for element `0`. |
| `var2` | `int32_t *` | Required writable pointer for element `1`. |
| `var3` | `int32_t *` | Required writable pointer for element `2`. |
| `var4` | `int32_t *` | Required writable pointer for element `3`. |
| `var5` | `int32_t *` | Required writable pointer for element `4`. |

---

### Return value

None (`void`). The element values are written to the supplied pointers.

---

### Remarks

- The implementation assigns `*var1 = penta.value1` through `*var5 = penta.value5` in order.
- **No NULL-pointer validation** occurs. Every output pointer must point to writable storage of the proper element type.
- The input group itself is not modified: it is passed by value.
- Output variables may be independent from the original group.

---

### Example

```c
TPENTA_TYPE(int32) penta = CAST_TYPE_TO_STRUCT(int32, TPenta)(10, 20, 30, 40, 50);
int32_t item1 = 0;
int32_t item2 = 0;
int32_t item3 = 0;
int32_t item4 = 0;
int32_t item5 = 0;
CAST_STRUCT_TO_TYPE(TPenta, int32)(penta, &item1, &item2, &item3, &item4, &item5);
/* item1 == 10; item5 == 50 */
```

---

# Custom type generation

Each header provides these compile-time macros:

| Family | Type generator | Full type and Cast implementation |
| --- | --- | --- |
| TPair | `TPAIR_STRUCT(TYPE, SUFFIX)` | `TPAIR_IMPLEMENT_ALL(TYPE, SUFFIX)` |
| TDual | `TDUAL_STRUCT(TYPE, SUFFIX)` | `TDUAL_IMPLEMENT_ALL(TYPE, SUFFIX)` |
| TTriple | `TTRIPLE_STRUCT(TYPE, SUFFIX)` | `TTRIPLE_IMPLEMENT_ALL(TYPE, SUFFIX)` |
| TQuad | `TQUAD_STRUCT(TYPE, SUFFIX)` | `TQUAD_IMPLEMENT_ALL(TYPE, SUFFIX)` |
| TPenta | `TPENTA_STRUCT(TYPE, SUFFIX)` | `TPENTA_IMPLEMENT_ALL(TYPE, SUFFIX)` |

The full generator emits the typedef, two Cast prototypes and their inline definitions. It should be invoked at **file scope**, and a given family/suffix should only be generated once in a translation unit. Ensure the `TYPE` is usable as a C member type and that `SUFFIX` is a valid identifier token.

## Example: a pair of custom application values

```c
#include "Cosmeron/Modules/Struct/TPair.h"

typedef struct TPoint {
    int32_t x;
    int32_t y;
} TPoint;

/* File scope: generate Struct_TPair_point and two Cast functions. */
TPAIR_IMPLEMENT_ALL(TPoint, point)

int main(void) {
    TPoint a = {1, 2}, b = {3, 4};
    TPAIR_TYPE(point) pair = CAST_TYPE_TO_STRUCT(point, TPair)(a, b);

    TPoint first = {0}, second = {0};
    CAST_STRUCT_TO_TYPE(TPair, point)(pair, &first, &second);

    return first.x == 1 && second.y == 4 ? 0 : 1;
}
```

The type generator specializes a group of **homogeneous TPoint values**. It does not create a heterogeneous pair in which the two element types differ.

### Built-in specializations are already generated

```c
/* Already available after including TDual.h: */
TDUAL_TYPE(size) bounds = { .start = 2, .end = 8 };
size_t count = bounds.end - bounds.start;
```

Avoid calling `TDUAL_IMPLEMENT_ALL(size_t, size)` again after including `TDual.h`, as the identifiers already exist. The same applies to the other built-in pairs of type and suffix.

---

# Complete examples

## Pair and coordinate aliases

```c
#include "Cosmeron/Modules/Struct/TPair.h"

int main(void) {
    TPAIR_TYPE(int32) position = CAST_TYPE_TO_STRUCT(int32, TPair)(8, 12);
    position.x = 16; /* Same first position as .first, .a, .value1. */

    int32_t x = 0, y = 0;
    CAST_STRUCT_TO_TYPE(TPair, int32)(position, &x, &y);

    return x == 16 && y == 12 && position.values[0] == 16 ? 0 : 1;
}
```

## Dual as a half-open index interval

```c
#include "Cosmeron/Modules/Struct/TDual.h"

int main(void) {
    TDUAL_TYPE(size) range = { .start = 2, .end = 8 };

    /* This program interprets [start, end) as a half-open interval.
       The struct itself does not enforce that interpretation. */
    size_t length = range.end - range.start;
    return length == 6 && range.value[0] == 2 ? 0 : 1;
}
```

## Triple dimensions

```c
#include "Cosmeron/Modules/Struct/TTriple.h"

int main(void) {
    TTRIPLE_TYPE(uint16) dimensions = {
        .length = 10, .width = 20, .height = 30
    };
    return dimensions.values[0] == 10 &&
           dimensions.values[2] == 30 ? 0 : 1;
}
```

## Quad rectangle boundaries

```c
#include "Cosmeron/Modules/Struct/TQuad.h"

int main(void) {
    TQUAD_TYPE(int32) edges = {
        .left = 10, .right = 100, .top = 20, .bottom = 60
    };
    /* The first two slots are left/right;
       the last two are top/bottom. */
    int32_t width = edges.right - edges.left;
    int32_t height = edges.bottom - edges.top;
    return width == 90 && height == 40 ? 0 : 1;
}
```

## Five-element group

```c
#include "Cosmeron/Modules/Struct/TPenta.h"

int main(void) {
    TPENTA_TYPE(uint8) values = {
        .c1 = 1, .c2 = 2, .c3 = 3, .c4 = 4, .c5 = 5
    };
    return values.values[0] == 1 &&
           values.values[4] == 5 ? 0 : 1;
}
```

## Complex-type specialization (non-MSVC branch)

```c
#include "Cosmeron/Modules/Struct/TDual.h"

int main(void) {
#if !COMPILER_MSVC
    TDUAL_TYPE(double_complex) numbers =
        CAST_TYPE_TO_STRUCT(double_complex, TDual)(
            1.0 + 2.0 * I, 3.0 + 4.0 * I);
    /* Each element is _Complex double in its own right.
       The .real/.imaginary aliases name two whole elements,
       not the real and imaginary components of one C complex number. */
    (void)numbers;
#endif
    return 0;
}
```

---

# Build and limitations

The current test suite checks Cast round-trips, built-in type variants, selected layouts, named-field aliases and combined header inclusion:

```sh 
make -C Codespace/Tests/Struct run
```

The makefile uses `-std=c11 -Wall -Wextra -Wpedantic -Werror` and no extra link libraries by default. This is a documentation of available tests, **not** a claim that the examples above were compiled separately during this documentation update.

Struct's core API has **no heap allocation**, no status handling, no automatic deep copying of pointed-to resources, no field validation and no arithmetic. For dynamic storage and traversal use the Container module; for numerical operations use Math.

A named field like `TDual.real` or `TDual.imaginary` only refers to the first or second stored element. It does not call `creal` or `cimag`, and the type specialization `double_complex` has two complete complex elements.

---

# Notes

- The API reference describes the ten public Cast operation templates, with 136 concrete built-in implementations across 68 types.
- The `*_STRUCT` and `*_IMPLEMENT_ALL` macros generate declarations/definitions. They are not runtime functions and should be kept distinct from the Cast calls.
- The actual header array identifiers are `TPair.values`, `TDual.value`, `TTriple.values`, `TQuad.values`, and `TPenta.values`.
- The public names are generated through `STRUCT_TYPE`, `CAST_TYPE_TO_STRUCT`, and `CAST_STRUCT_TO_TYPE`. The `TPAIR_FUNC` and related macros exist as naming helpers but are not separately instantiated operations.
- No implementation source files are changed by this documentation.
