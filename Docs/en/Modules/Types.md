# Types

The **Type** core module provides canonical primitive type metadata, type traits, type-erased storage, tagged unions, fixed-width byte tags, and generic `Optional`/`Result` wrappers. The neighboring **Cast** module provides checked conversions between primitive types; it is documented separately in [Cast](Cast.md).

```c
#include "Cosmeron/Core/Types/Primitive.h"
#include "Cosmeron/Core/Types/Traits.h"
#include "Cosmeron/Core/Types/Optional.h"
#include "Cosmeron/Core/Types/Result.h"
#include "Cosmeron/Core/Types/ByteTag.h"
#include "Cosmeron/Core/Types/Any.h"
#include "Cosmeron/Core/Types/TaggedUnion.h"
```

Include `Cast/Cast.h` when checked conversions are needed. These are distinct interfaces: Type describes values and their behavior; Cast converts scalar values between supported representations.

---

# Overview

| Header | API / type | Purpose |
| --- | --- | --- |
| `Primitive.h` | `MODULE_TYPE(Type, ...)` | Canonical namespaced primitive aliases and type categories |
| `Traits.h` | `MODULE_TYPE(Type, Traits)` | Runtime descriptor for lifecycle, copying, moving, comparison and hashing |
| `Optional.h` | `TEMPLATE_Optional` | A value that may or may not be present |
| `Result.h` | `TEMPLATE_Result` | A value or a structured `Condition_Error` |
| `ByteTag.h` | `MODULE_TYPE(Type, ByteTag...)` | Fixed-width integer tags with byte/string views |
| `Any.h` | `TEMPLATE_Any` | Type-erased, small-buffer-optimized value storage |
| `TaggedUnion.h` | `TType_TaggedUnion` | Closed tagged payload for primitive and raw data categories |

---

# Primitive types and categories

`Primitive.h` exposes namespaced aliases generated from the primitive type table. Use `MODULE_TYPE(Type, Int32)`, `MODULE_TYPE(Type, UInt64)`, `MODULE_TYPE(Type, Size)` and other table names rather than assuming every target has identical native widths.

The `Type_Category` enum classifies supported values. `MODULE_FUNC(Type, Category, ToString)(category)` returns a textual category name. The primitive type table is reused by traits, comparisons and generated generic facilities.

---

# Type traits

`MODULE_TYPE(Type, Traits)` is a descriptor containing type size/alignment, flags and callbacks for lifecycle and value operations. It allows generic infrastructure to manipulate a value without knowing its C type at the call site.

The public Traits package provides:

- `Is.Valid`, `Is.Equal`, `Is.Trivial`, `Is.TrivialCopy`, `Is.TrivialCompare`;
- `Is.Numeric`, `Is.Signed`, `Is.Integer`, `Is.Float`, `Is.Dynamic`;
- `HasFlags(flags)` and `NeedsDispose()`;
- `Init(instance)` / `Init.Array(array, count)`;
- `Dispose(instance)` / `Dispose.Array(array, count)`;
- `Copy(destination, source)` / `Copy.Array(...)`;
- `Move(destination, source)` / `Move.Array(...)`;
- `Swap(left, right)`, `Compare(left, right)`, `Equals(left, right)`;
- `Hash(instance)` and `Hash.Bytes(data, size)`;
- built-in descriptor getters for primitive types and `CString()`.

A custom descriptor uses the `TraitInit`, `TraitDispose`, `TraitCopy`, `TraitMove`, `TraitCompare` and `TraitHash` callables declared by the header. The callbacks define the semantics of that type; do not mark a type trivial unless raw byte operations are valid for its lifecycle and representation.

---

# Optional

`TEMPLATE_Optional(SUFFIX, TYPE)` generates an object with a presence flag and a value of `TYPE`. Instantiate the template for the type needed, then use `MODULE_TYPED(Optional, SUFFIX)` and `API`.

| API | Behavior |
| --- | --- |
| `Some(value)` | Stores a value and marks it present |
| `None()` | Clears the optional |
| `FromPointer(ptr)` | Stores `*ptr` when non-null, otherwise produces None |
| `Clear()` | Clears the presence state and zeroes the stored value |
| `HasValue()` / `Is.Some()` / `Is.None()` | Tests the state |
| `Value(outValue)` | Copies the value to an output pointer; returns `NOT_FOUND` when empty |
| `Value.Or(fallback)` | Returns the value or a fallback |
| `Value.Pointer()` / `MutablePointer()` | Returns a pointer to the contained value, or null |
| `Map(mapper, outOptional)` | Transforms a present value |
| `AndThen(mapper, outOptional)` | Chains an optional-producing closure |
| `Filter(predicate)` | Clears the optional when the predicate rejects its value |

```c
TEMPLATE_Optional(I32, int32_t);
MODULE_TYPED(Optional, I32) maybeCount = {0};

API(&maybeCount, Some, 42);
int32_t count = 0;
OPSTATUS status = API(&maybeCount, Value, &count);
```

Optional does not own dynamically referenced resources inside `TYPE`. Copying a pointer value copies the pointer, not the pointed-to object.

---

# Result

`TEMPLATE_Result(SUFFIX, TYPE)` generates a value-or-error object. Its error alternative stores a `MODULE_TYPE(Condition, Error)`.

| API | Behavior |
| --- | --- |
| `Ok(value)` | Stores a successful value |
| `Err(&error)` | Copies a Condition Error into the error alternative |
| `FromStatus(status)` | Builds a Result from a status |
| `Is.Ok()` / `Is.Error()` | Tests the active alternative |
| `Value(outValue)` | Copies the value; returns the stored error status in the error state |
| `Value.Or(fallback)` | Returns the value or fallback |
| `Error(outError)` | Copies the stored Error when the result is an error |
| `Map(mapper, outResult)` | Maps the successful value |
| `AndThen(mapper, outResult)` | Chains a fallible Result-producing closure |

`FromStatus(SUCCESS)` creates a successful Result without a meaningful payload value; prefer `Ok(value)` when a value is required. Result is useful when a function should return one object that carries either data or a diagnostic, rather than returning a status separately from an output parameter.

---

~~~c
TEMPLATE_Result(I32, int32_t);
MODULE_TYPED(Result, I32) result = {0};
MODULE_TYPE(Condition, Error) error = {0};

API(&error, FromStatus, STATUS_CONST(OUT_OF_RANGE));
OPSTATUS status = API(&result, Err, &error);

int32_t value = 0;
if (status == STATUS_CONST(SUCCESS))
    status = API(&result, Value, &value);
~~~

# ByteTag

`ByteTag.h` instantiates fixed-width tag objects over unsigned integer widths declared by `TYPE_BYTETAG_TYPE_TABLE`. The object stores both a numeric value and a printable text representation.

The generated API includes `From.String(chars)`, `From.UInt(value)`, `From.Bytes(bytes, length)`, `Read.BE(source)`, `Read.LE(source)`, `Write.BE(destination)`, `Write.LE(destination)`, `To.String(destination, capacity)`, `To.UInt()`, `Text()`, `IsPrintable()` and additional comparison/validity observers from the template.

The header also supplies `TYPE_BYTETAG_MAKE_16`, `TYPE_BYTETAG_MAKE_32` and `TYPE_BYTETAG_MAKE_64` packing macros. Byte order in `Read`/`Write` is explicit; do not rely on host endianness.

---

# Any: type-erased values

`TEMPLATE_Any(NAME, SBO_BYTES)` generates a type-erased object with a traits pointer and small-buffer storage. Values that fit the inline capacity are stored inside the object; larger values use heap storage.

The generated API includes lifecycle (`Init`, `Reset`, `Dispose`), value management (`Emplace`, `MoveFrom`, `Clone`, `Move`, `Swap`), state (`HasValue`, `Is.Empty`, `Is.Inline`, `Is.Type`), inspection (`TraitsOf`, `Capacity`, `RawPointer`, `MutableRawPointer`), access (`Get`, `Borrow`, `Borrow.Mutable`), interoperability (`FromTaggedUnion`, `ToTaggedUnion`), and value semantics (`Compare`, `Equals`, `Hash`, `Traits`).

`Emplace` receives a `Type_Traits` descriptor plus a source instance and stores a copy/move according to the requested operation. The descriptor callbacks govern initialization, disposal, copying, moving, comparison and hashing. Always call `Dispose` before an Any object leaves its owning scope; use `Move`/`MoveFrom` to transfer ownership instead of copying internal storage bytes manually.

---

# TaggedUnion

`TaggedUnion` is a closed tagged value type. Its tag identifies the payload alternative, and its traits descriptor describes the active value.

The package provides lifecycle (`Init`, `Clear`); generated primitive alternatives (`Set_<Primitive>`, `From_<Primitive>`, `Get_<Primitive>`); string, bytes and pointer alternatives (`Set_CString`/`From_CString`/`Get_CString`, `Set_Bytes`/`From_Bytes`/`Get_Bytes`, and `Set_RawPointer`/`From_RawPointer`/`Get_RawPointer`); coercions (`Coerce.Int64`, `Coerce.UInt64`, `Coerce.Float64`); inspection (`Is.Empty`, `Is.Tag`, `Is.Numeric`, `Is.Integer`, `Is.Signed`, `Is.Float`, `TagName`, `CategoryOf`, `TraitsOf`, `PayloadPointer`); and value semantics (`Compare`, `Equals`, `Hash`, `Traits`).

`ByteSlice` is a non-owning `(data, size)` view. For byte and raw-pointer alternatives, understand whether an operation copies or stores a reference before allowing the source memory to expire. The active tag controls interpretation; do not read a payload through an alternative that does not match the tag.

---

# Usage guidance

- Use Optional for a value that may be absent but does not need an error explanation.
- Use Result when a computation produces either a value or a `Condition_Error`.
- Use Any for open-ended type-erased storage described by Traits.
- Use TaggedUnion for a known, closed set of payload categories.
- Use Traits when generic code must manage lifecycle or semantics of an unknown C type.
- Use ByteTag for compact identifiers, signatures, magic values and fixed-size byte tags; it is not a general string container.
