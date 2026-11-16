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
   Each family specialization is declared as a package (`Struct_TPair_Int32`, `Struct_TDual_Size`, etc.), exposing both static inline functions and dispatch function tables (`Struct_TPair_Int32_FunctionTable`).
2. **Universal Template Engine (`TEMPLATE_<FAMILY>` / `TEMPLATE_<FAMILY>_IMPLEMENT`)**:
   Every family supports generic instantiation for any type via `TEMPLATE_DECLARE(Family, Suffix, Type)` in headers and `TEMPLATE_IMPLEMENT(Family, Suffix, Type)` in implementation units.
3. **Unified Primitive X-Macro Table (`PRIMITIVE_TYPE_TABLE`)**:
   Integrated directly with `Core/Primitive/Types.h` (`PRIMITIVE_TYPE_TABLE`), using canonical `NAME` suffixes (`Int32`, `UInt8`, `Size`, `FloatComplex`, etc.) and decoupled column selectors (`PRIMITIVE_TYPE_GET(NAME, ROW)`, `PRIMITIVE_TYPE_GET(TYPE, ROW)`).
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
Struct_TPair_Int32 pair = Struct_TPair_Int32_Create(10, 20);
int32_t x = 0, y = 0;
OPSTATUS status = Struct_TPair_Int32_Extract(pair, &x, &y);
```

### 2. Macro Form
Uses the canonical namespace generator `TPAIR_FUNC(SUFFIX, NAME)`:

```c
Struct_TPair_Int32 pair = TPAIR_FUNC(Int32, Create)(10, 20);
```

### 3. Declarative Package Call
Dispatches through `PACKAGE_CALL` or the package function table:

```c
Struct_TPair_Int32 pair = PACKAGE_CALL(TPAIR_PKG(Int32), Create, 10, 20);

const Struct_TPair_Int32_FunctionTable *ft = Struct_TPair_Int32_GetFunctionTable();
Struct_TPair_Int32 pair2 = ft->Create(30, 40);
```

### 4. Cast Helpers
Each family generates canonical Cast functions:

```c
Struct_TPair_Int32 pair = Cast_Int32_To_TPair(10, 20);
int32_t first = 0, second = 0;
Cast_TPair_To_Int32(pair, &first, &second);

/* Or generic macros */
TPAIR_TYPE(Int32) p = CAST_TYPE_TO_STRUCT(Int32, TPair)(10, 20);
CAST_STRUCT_TO_TYPE(TPair, Int32)(p, &first, &second);
```

---

# Standard Types & X-Macro Table

Standard and complex primitive types are imported from `Core/Primitive/Types.h` via `PRIMITIVE_TYPE_TABLE(X)`:

- **Boolean**: `Boolean` (`bool`)
- **Signed**: `Int8`, `Int16`, `Int32`, `Int64`
- **Unsigned**: `UInt8`, `UInt16`, `UInt32`, `UInt64`
- **Character**: `Char`, `Char8`, `Char16`, `Char32`
- **Float**: `Float32`, `Float64`, `Float`, `Double`, `LongDouble`
- **Complex** (when `!COMPILER_MSVC`): `FloatComplex`, `DoubleComplex`, `LongDoubleComplex`
- **Architecture**: `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr`

All 5 families are built-in instantiated for every type in `PRIMITIVE_TYPE_TABLE(X)`.

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
