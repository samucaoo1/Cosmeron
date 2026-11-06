# Struct

The **Struct** module provides small, fixed-size homogeneous data groups with two, three, four or five elements. Every group is defined using C11 unions and anonymous structures, allowing the same element to be addressed by several meaningful domain field names or by an indexed array.

The types contain values directly. They do **not** allocate memory, own external pointers, or require dynamic initialization or destruction functions.

```c
/* Aggregate header including all families */
#include "Cosmeron/Modules/Struct/Struct.h"

/* Or individual family headers */
#include "Cosmeron/Modules/Struct/TPair.h"
#include "Cosmeron/Modules/Struct/TDual.h"
#include "Cosmeron/Modules/Struct/TTriple.h"
#include "Cosmeron/Modules/Struct/TQuad.h"
#include "Cosmeron/Modules/Struct/TPenta.h"
```

---

# Architecture & Design

The module is built entirely on Cosmeron's canonical architectural patterns:

1. **Declarative Packages (`PACKAGE`)**:
   Each family specialization is declared as a package (`Struct_TPair_int32`, `Struct_TDual_size`, etc.), exposing both static inline functions and dispatch function tables (`Struct_TPair_int32_FunctionTable`).
2. **Universal Template Engine (`TEMPLATE_<FAMILY>` / `TEMPLATE_<FAMILY>_IMPLEMENT`)**:
   Every family supports generic instantiation for any type via `TEMPLATE_DECLARE(Family, Suffix, Type)` in headers and `TEMPLATE_IMPLEMENT(Family, Suffix, Type)` in implementation units.
3. **Decoupled X-Macro Table (`STRUCT_TYPE_TABLE`)**:
   Single source of truth for standard types, suffixes, and complex-type gating using parenthesized row tuples and decoupled column selectors (`STRUCT_TYPE_COL_*`, `STRUCT_TYPE_GET`).
4. **Strict Function Contracts**:
   - `FUNCTION`: Value-producing construction (`Create`).
   - `OPERATION`: Fallible extraction returning `OPSTATUS` (`Extract`) with pointer validation.
   - `PROCEDURE`: Infallible in-place unpacking returning `void` (`Unpack`).
5. **Zero Retrocompatibility & Zero Ambiguity**:
   Elimination of legacy monolithic instantiation macros (`*_IMPLEMENT_ALL`) and manual prototype macros (`*_PROTOTYPE`). Fully compatible with `PACKAGE_DISABLE_FUNCTION_TABLE`.
6. **Dual Array Views**:
   Unions expose both `values[N]` and `value[N]` for indexed element access without layout penalties.

---

# Families Overview

| Family | Header | Elements | Indexed Views | Primary Aliases |
| --- | --- | ---: | --- | --- |
| [`TPair`](#tpair) | `TPair.h` | 2 | `value[2]`, `values[2]` | `first`/`second`, `x`/`y`, `a`/`b`, `x1`/`x2`, `value1`/`value2` |
| [`TDual`](#tdual) | `TDual.h` | 2 | `value[2]`, `values[2]` | `start`/`last`, `begin`/`end`, `real`/`imaginary`, `col`/`row`, `length`/`width`, `state1`/`state2` |
| [`TTriple`](#ttriple) | `TTriple.h` | 3 | `value[3]`, `values[3]` | `x`/`y`/`z`, `a`/`b`/`c`, `x1`/`x2`/`x3`, `length`/`width`/`height`, `state1`/`state2`/`state3` |
| [`TQuad`](#tquad) | `TQuad.h` | 4 | `value[4]`, `values[4]` | `left`/`right`/`top`/`bottom`, `x`/`y`/`z`/`w`, `x1`/`x2`/`y1`/`y2`, `a`/`b`/`c`/`d`, `state1`..`state4` |
| [`TPenta`](#tpenta) | `TPenta.h` | 5 | `value[5]`, `values[5]` | `x`/`y`/`z`/`w`/`t`, `c1`..`c5`, `y1`..`y5`, `a`..`e`, `state1`..`state5` |

---

# Invocation Styles

All Struct operations support canonical calling styles:

### 1. Direct Form
Call the namespaced package function directly:

```c
Struct_TPair_int32 pair = Struct_TPair_int32_Create(10, 20);
int32_t x = 0, y = 0;
OPSTATUS status = Struct_TPair_int32_Extract(pair, &x, &y);
```

### 2. Macro Form
Uses the canonical namespace generator `TPAIR_FUNC(SUFFIX, NAME)`:

```c
Struct_TPair_int32 pair = TPAIR_FUNC(int32, Create)(10, 20);
```

### 3. Declarative Package Call
Dispatches through `PACKAGE_CALL` or the package function table:

```c
Struct_TPair_int32 pair = PACKAGE_CALL(TPAIR_PKG(int32), Create, 10, 20);

const Struct_TPair_int32_FunctionTable *ft = Struct_TPair_int32_GetFunctionTable();
Struct_TPair_int32 pair2 = ft->Create(30, 40);
```

### 4. Cast Helpers
Each family generates canonical Cast functions:

```c
Struct_TPair_int32 pair = Cast_int32_To_TPair(10, 20);
int32_t first = 0, second = 0;
Cast_TPair_To_int32(pair, &first, &second);

/* Or generic macros */
TPAIR_TYPE(int32) p = CAST_TYPE_TO_STRUCT(int32, TPair)(10, 20);
CAST_STRUCT_TO_TYPE(TPair, int32)(p, &first, &second);
```

---

# Standard Types & X-Macro Table

Standard types are registered in `STRUCT_TYPE_TABLE(X)`:

```c
#define STRUCT_TYPE_COL_SUFFIX(SUFFIX, TYPE) SUFFIX
#define STRUCT_TYPE_COL_TYPE(SUFFIX, TYPE)   TYPE
#define STRUCT_TYPE_GET(FIELD, ROW) PP_OP_CAT2(STRUCT_TYPE_COL_, FIELD) ROW

#define STRUCT_STANDARD_TYPE_TABLE(X) \
  X((uint8, uint8_t))                 \
  X((uint16, uint16_t))               \
  X((uint32, uint32_t))               \
  X((uint64, uint64_t))               \
  X((size, size_t))                   \
  X((int8, int8_t))                   \
  X((int16, int16_t))                 \
  X((int32, int32_t))                 \
  X((int64, int64_t))                 \
  X((float, float))                   \
  X((double, double))                 \
  X((longdouble, long double))

#if !COMPILER_MSVC
#define STRUCT_COMPLEX_TYPE_TABLE(X)  \
  X((float_complex, float _Complex))  \
  X((double_complex, double _Complex))\
  X((longdouble_complex, long double _Complex))
#else
#define STRUCT_COMPLEX_TYPE_TABLE(X)
#endif
```

All 5 families are built-in instantiated for all 12 standard types and the 3 complex types (when `!COMPILER_MSVC`).

---

# Custom Template Instantiations

To instantiate any family for a custom user type (e.g., custom typedefs, pointers, or structs):

```c
#include "Cosmeron/Modules/Struct/TPair.h"

/* In headers (.h) */
TEMPLATE_DECLARE(TPAIR, custom16, uint16_t);

/* In implementation units (.impl / .c) */
TEMPLATE_IMPLEMENT(TPAIR, custom16, uint16_t);

/* Usage */
Struct_TPair_custom16 custom = Struct_TPair_custom16_Create(12, 34);
```

---

# Compile-Time Disabling of Function Tables

Function tables can be completely omitted at compile-time to eliminate dispatch table memory overhead:

```c
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#include "Cosmeron/Modules/Struct/Struct.h"
```

When defined, all `Create`, `Extract`, and `Unpack` functions remain available as zero-overhead `static inline` functions.

/* EOF */
