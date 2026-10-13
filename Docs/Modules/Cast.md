# Cast

The **Cast** core currently provides naming and namespace infrastructure for conversions. It does **not yet expose any public conversion functions** in the `Cast` directory. This reference documents the actual macros present in `Cast.space` without inventing cast operations.

```c
#include "Cosmeron/Core/Cast/Cast.space"
```

---

# Overview

`Cast.space` defines the same namespace conventions used by other Cosmeron core modules.

| Macro | Purpose |
| --- | --- |
| `CAST_MOD` | Module token `Cast` |
| `CAST_CMOD` | Uppercase module token `CAST` |
| `CAST_NS(NAME)` | Public namespace qualification |
| `CAST_CNS(NAME)` | Constant namespace qualification |
| `CAST_INS(NAME)` | Internal namespace qualification |
| `CAST_CINS(NAME)` | Internal constant qualification |
| `CAST_TYPE(NAME)` | Public type qualification |
| `CAST_FUNC(NAME)` | Public function qualification |
| `CAST_CONST(NAME)` | Public constant qualification |
| `CAST_TYPE_TO_STRUCT(SUFFIX, TYPE)` | Name for a conversion from a primitive type to a struct |
| `CAST_STRUCT_TO_TYPE(TYPE, SUFFIX)` | Name for a conversion from a struct to a primitive type |

# API reference

## CAST_TYPE_TO_STRUCT

### Syntax

#### Macro form

```c
CAST_TYPE_TO_STRUCT(SUFFIX, TYPE)
```

#### Direct form

The macro expands to `GNS4(LIB_PREFIX(CAST_MOD), SUFFIX, To, TYPE)`. With default prefix conventions it composes a name of the form `Cast_<SUFFIX>_To_<TYPE>`.

### Parameters

- `SUFFIX`: source type's namespace suffix.
- `TYPE`: destination type's identifier token.

### Return value

A preprocessor-generated identifier, **not** a C value or a conversion result.

### Remarks

This is a naming macro. Using the result as a callable symbol requires a corresponding function to have been declared and defined elsewhere. `Cast.space` does not itself create such a function.

### Example

```c
/* Naming illustration only, not a declaration or implemented conversion: */
#define EXAMPLE_CONVERSION_NAME CAST_TYPE_TO_STRUCT(I32, TDual)
```

## CAST_STRUCT_TO_TYPE

### Syntax

#### Macro form

```c
CAST_STRUCT_TO_TYPE(TYPE, SUFFIX)
```

#### Direct form

The macro expands to `GNS4(LIB_PREFIX(CAST_MOD), TYPE, To, SUFFIX)`; under default conventions it produces a name of the form `Cast_<TYPE>_To_<SUFFIX>`.

### Parameters

- `TYPE`: source struct's identifier token.
- `SUFFIX`: destination primitive type's suffix.

### Return value

A generated C identifier, not a converted value.

### Remarks

No conversion implementation is provided by the macro. A matching separately defined function is needed before invoking the expanded identifier.

### Example

```c
/* Identifier generation, not an available function by itself: */
#define EXAMPLE_REVERSE_NAME CAST_STRUCT_TO_TYPE(TDual, I32)
```

# Naming and namespace

`CAST_TYPE`, `CAST_FUNC` and `CAST_CONST` delegate to the namespace macros. Their expansion depends on `LIB_PREFIX`, `LIB_PREFIX_CONST`, and the shared `GNS*`/`CNS*` macros.

# Requirements

- C11 preprocessor.
- `Core/Namespace.h` (included by `Cast.space`).
- No mandatory link libraries or dynamic initialization.

# See also

- [Struct](Struct.md): actual typed structures and conversion operations.
- [Type](Type.md): type utilities.
- [Algorithm](Algorithm.md): other core infrastructure.
