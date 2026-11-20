# Types

The **Types** module collects reusable type-system primitives: the canonical C primitive type catalog, type traits, tagged values, type-erased storage, optional values, result values and fixed-width byte tags.

## Headers

~~~c
#include "Cosmeron/Core/Types/Primitive.h"
#include "Cosmeron/Core/Types/Traits.h"
#include "Cosmeron/Core/Types/TaggedUnion.h"
#include "Cosmeron/Core/Types/Any.h"
#include "Cosmeron/Core/Types/Optional.h"
#include "Cosmeron/Core/Types/Result.h"
#include "Cosmeron/Core/Types/ByteTag.h"
~~~

Include individual headers when only one facility is required. The `Impl` and `TypeTable` files are implementation details and must not be included directly by consumers.

---

# Overview

| Facility | Purpose | Storage/ownership |
| --- | --- | --- |
| Primitive | Canonical names for C built-in types, categories and primitive type tables. | Typedefs and enums; no allocation. |
| Traits | Describes size, alignment, categories, flags and lifecycle/comparison/hash callbacks for a type. | Static POD descriptor, normally shared by pointer. |
| TaggedUnion | Stores one tagged primitive or special value. | Inline tagged union; no allocation for built-in payloads. |
| Any | Stores one runtime-selected value using a type descriptor and small-buffer optimization. | Inline storage when it fits, otherwise heap storage. |
| Optional | Represents either a value or no value for one compile-time type. | Inline value. |
| Result | Represents either a value or a Condition error for one compile-time type. | Inline tagged union. |
| ByteTag | Stores a fixed-width byte tag with numeric, binary and printable-text conversions. | Inline fixed-width integer and character array. |

---

# API reference

## Primitive types

Header: `Primitive.h`.

The `Type` primitive catalog provides namespaced C typedefs through `MODULE_TYPE(Type, Name)`, including:

- Boolean: `Boolean`;
- signed integers: `Int8`, `Int16`, `Int32`, `Int64`;
- unsigned integers: `UInt8`, `UInt16`, `UInt32`, `UInt64`;
- character types: `Char`, `Char8`, `Char16`, `Char32`;
- floating point: `Float32`, `Float64`, `LongDouble`;
- architecture-sized types: `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr`;
- complex floating point types where supported by the compiler.

The header also exposes the primitive category enum and the X-macro tables used by other modules. The most useful aggregate tables are:

| Table | Contents |
| --- | --- |
| `PRIMITIVE_TYPE_TABLE_EXACT_INTEGER` | Fixed-width signed and unsigned integers. |
| `PRIMITIVE_TYPE_TABLE_INTEGER` | Fixed-width and architecture-sized integer types. |
| `PRIMITIVE_TYPE_TABLE_NUMERIC` | Integer, floating-point and complex numeric types. |
| `PRIMITIVE_TYPE_TABLE_ORDERED` | Types with a built-in ordering used by ordered algorithms. |
| `PRIMITIVE_TYPE_TABLE` | Ordered types plus complex types. |

Use the namespaced aliases when declaring interfaces shared with other Cosmeron modules. They make the intended type family explicit without replacing the underlying C ABI types.

---

## Type traits

Header: `Traits.h`.

`MODULE_TYPE(Type, Traits)` is a POD descriptor containing:

- type name, size and alignment;
- a `Type_Category` value and trait flags;
- optional callbacks for initialization, disposal, copy, move, comparison and hashing.

The package supplies built-in descriptors for primitive types and a descriptor for C strings.

### Introspection

| Method | Purpose |
| --- | --- |
| `Is.Valid`, `Is.Equal` | Validate a descriptor and compare descriptor identity/content. |
| `Is.Trivial` | Tests the trivial-lifecycle flag. |
| `Is.TrivialCopy`, `Is.TrivialCompare` | Tests whether copy/comparison may use trivial byte operations. |
| `Is.Numeric`, `Is.Signed`, `Is.Integer`, `Is.Float` | Tests numeric categories/flags. |
| `Is.Dynamic` | Tests whether the type carries dynamic lifecycle requirements. |
| `HasFlags` | Tests a bitmask of descriptor flags. |
| `NeedsDispose` | Tests whether the value needs disposal. |

### Type-erased lifecycle and value operations

| Method | Contract |
| --- | --- |
| `Init(traits, instance)` / `Init.Array` | Initializes one value or an array. |
| `Dispose(traits, instance)` / `Dispose.Array` | Disposes one value or an array. |
| `Copy(traits, destination, source)` / `Copy.Array` | Copies one value or an array using the registered policy. |
| `Move(traits, destination, source)` / `Move.Array` | Moves one value or an array using the registered policy. |
| `Swap(traits, left, right)` | Swaps two values according to the descriptor. |
| `Compare(traits, left, right)` | Returns a three-way comparison. |
| `Equals(traits, left, right)` | Tests equality. |
| `Hash(traits, instance)` | Returns the value hash. |
| `Hash.Bytes(data, size)` | Hashes a raw byte span. |

Built-in descriptor accessors are named after their primitive type, and `Traits.CString` provides the C-string descriptor.

A descriptor's callback pointers and the descriptor itself must outlive every operation that uses them. A dynamic value cannot safely be copied with `memcpy` unless its trait contract explicitly allows trivial copying.

---

## TaggedUnion

Header: `TaggedUnion.h`.

`MODULE_TYPE(Type, TaggedUnion)` stores one value selected by a `MODULE_ENUM(Type, Tag)` discriminator. Built-in tags cover the primitive table plus special categories such as empty, C string, byte slice and raw pointer.

The public API includes:

| Group | Operations |
| --- | --- |
| Lifecycle | `Init`, `Clear` |
| Primitive values | `Set_<Primitive>`, `From_<Primitive>`, `Get_<Primitive>` for primitive table entries. |
| Special values | `Set_CString`/`From_CString`/`Get_CString`; `Set_Bytes`/`From_Bytes`/`Get_Bytes`; `Set_RawPointer`/`From_RawPointer`/`Get_RawPointer`. |
| Numeric conversion | `Coerce.Int64`, `Coerce.UInt64`, `Coerce.Float64`. |
| Inspection | `Is.Empty`, `Is.Tag`, `Is.Numeric`, `Is.Integer`, `Is.Signed`, `Is.Float`, `TagName`, `CategoryOf`, `TraitsOf`, `PayloadPointer`. |
| Value operations | `Compare`, `Equals`, `Hash`, `Traits`. |

~~~c
MODULE_TYPE(Type, TaggedUnion) value = {0};

OPSTATUS status = MODULE_FUNC(Type, TaggedUnion, Set_Int32)(&value, 42);
int32_t extracted = 0;

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Type, TaggedUnion, Get_Int32)(
        &value, &extracted);
}
~~~

Getters are strict: they expect the active tag to match the requested type. Use the `Coerce` operations when a checked numeric conversion is intended. They write to an output pointer and report conversion failures through `OPSTATUS`.

`ByteSlice` stores a non-owning pointer and length. C-string, byte-span and raw-pointer payloads are borrowed values; TaggedUnion does not duplicate or free their backing memory.

---

## Any

Header: `Any.h`.

`MODULE_TYPE(Type, Any)` is a type-erased value container with small-buffer optimization. The default value uses the header's configured small-buffer capacity; generated size variants include `Any32`, `Any64`, `Any128` and `Any256`.

The object stores a pointer to `Type_Traits` and either inline bytes or a heap pointer. Its lifetime operations use the descriptor callbacks.

### Lifecycle and value management

| Method | Contract |
| --- | --- |
| `Init`, `Reset`, `Dispose` | Initialize, clear or dispose the stored value. |
| `Emplace(traits, sourceInstance)` | Copies a typed value into the container. |
| `MoveFrom(traits, sourceInstance)` | Moves a typed value into the container. |
| `Clone(destination, source)` | Clones a value with its registered trait policy. |
| `Move(destination, source)` | Transfers the stored value. |
| `Swap(left, right)` | Exchanges two containers. |

### Introspection and access

| Method | Contract |
| --- | --- |
| `HasValue`, `Is.Empty` | Tests whether a value is present. |
| `Is.Inline` | Tests whether the current payload fits in inline storage. |
| `Is.Type(expectedTraits)` | Tests descriptor identity. |
| `TraitsOf` | Returns the active descriptor, or null for an empty value. |
| `Capacity` | Returns inline storage capacity for the default specialization. |
| `RawPointer`, `MutableRawPointer` | Returns a borrowed pointer to the stored payload. |
| `Get(expectedTraits, outValue)` | Strictly copies a value of the expected type to caller storage. |
| `Borrow`, `Borrow.Mutable` | Returns a borrowed pointer after validating the expected traits. |

The package also provides primitive shortcuts `Set_<Primitive>` and `Get_<Primitive>`, conversions to/from `TaggedUnion`, comparison, equality and hashing.

~~~c
MODULE_TYPE(Type, Any) value = {0};
int32_t source = 42;
int32_t extracted = 0;

const MODULE_TYPE(Type, Traits) *traits =
    MODULE_FUNC(Type, Traits, Int32)();

OPSTATUS status = MODULE_FUNC(Type, Any, Emplace)(
    &value, traits, &source);

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Type, Any, Get)(
        &value, traits, &extracted);
}

MODULE_FUNC(Type, Any, Dispose)(&value);
~~~

The descriptor must accurately describe the source type, and the source object must be valid for the operation. A pointer returned by `RawPointer` or `Borrow` is borrowed from the Any object; it can become invalid after replacement, movement, disposal or any operation that changes its storage.

Always dispose an Any value before its storage goes out of scope if its descriptor requires disposal. The convenience primitive setters/getters do not change that lifecycle rule.

---

## Optional

Header: `Optional.h` and template definition `Optional.def`.

Instantiate a specialization for the C type you want to store:

~~~c
TEMPLATE_INSTANTIATE(Optional, Int32, int32_t);
MODULE_TYPED(Optional, Int32) value = {0};
~~~

The generated object stores `hasValue` plus an inline `value`.

| Method | Contract |
| --- | --- |
| `Some(value)` | Stores a value and marks the Optional as present. |
| `None()` / `Clear()` | Clears the stored value. |
| `FromPointer(ptr)` | Stores the pointed value, or creates None for a null pointer. |
| `HasValue`, `Is.Some`, `Is.None` | Tests the state. |
| `Value(outValue)` | Copies the stored value to output; returns `NOT_FOUND` when empty. |
| `Value.Or(fallback)` | Returns the stored value or the fallback. |
| `Value.Pointer` / `MutablePointer` | Returns a borrowed pointer or null. |
| `Map(mapper, outOptional)` | Transforms a present value into another value of the same specialization type. |
| `AndThen(binder, outOptional)` | Invokes a binder that writes an output Optional. |
| `Filter(predicate, outOptional)` | Preserves the value only when the predicate returns true. |

`Map`, `AndThen` and `Filter` use generated closures. They require valid input/output objects and valid closures. Optional does not allocate memory for its stored value.

---

## Result

Header: `Result.h` and template definition `Result.def`.

Result combines a value of one chosen type with `Condition_Error`:

~~~c
TEMPLATE_INSTANTIATE(Result, Int32, int32_t);
MODULE_TYPED(Result, Int32) result = {0};
~~~

| Method | Contract |
| --- | --- |
| `Ok(value)` | Stores a successful value. |
| `Err(error)` | Stores a Condition error. |
| `FromStatus(status)` | Creates success for `SUCCESS` or an Error from another status. |
| `Is.Ok`, `Is.Error` | Tests the active alternative. |
| `Value(outValue)` | Copies the value; on an error alternative, returns its error status. |
| `Value.Or(fallback)` | Returns the value or the fallback. |
| `Error(outError)` | Copies the stored error when the Result is an error. |
| `Map(mapper, outResult)` | Transforms a successful value and preserves an error. |
| `AndThen(binder, outResult)` | Invokes a binder for the successful value and propagates an error. |

Use Result when a function needs to return either a typed value or a richer error object. Use Optional when absence is a valid state and does not require an error explanation.

---

## ByteTag

Header: `ByteTag.h` and template definition `ByteTag.def`.

Built-in tags are `ByteTag16`, `ByteTag32` and `ByteTag64`. Each stores an unsigned integer of the selected width and a NUL-terminated text view of the tag bytes.

| Group | Operations |
| --- | --- |
| Construction | `From.String(chars)`, `From.UInt(value)`, `From.Bytes(bytes, length)` |
| Binary input | `Read.BE`, `Read.LE` |
| Binary output | `Write.BE`, `Write.LE` |
| Conversion | `To.String`, `To.UInt` |
| Inspection | `Text`, `IsPrintable`, `Equal`, `Compare` |

`From.String` and `From.Bytes` pack into the fixed tag width and pad unused positions with spaces. `Read.BE`/`Read.LE` consume exactly the tag width from a caller-provided source pointer; `Write.BE`/`Write.LE` write exactly that width to a destination pointer. They use explicit byte ordering, independent of host byte order. The caller must provide at least the tag width in readable or writable bytes. `To.String` requires a destination buffer large enough for all tag bytes plus the terminating NUL. Non-printable bytes are represented through the module's printable-text formatting rather than assumed to be valid display characters.

The compile-time helpers in the header can construct common constants from byte literals. Treat ByteTag as a compact identifier or FourCC-style tag, not as a general character string.

---

# Contracts and ownership

- A POD descriptor or tagged value does not automatically manage the lifetime of pointers stored inside it.
- `Traits` is the authority for type-erased initialization, copy, move, disposal, comparison and hashing.
- `Any` may allocate when the value does not fit its inline capacity; always follow its disposal contract.
- `TaggedUnion` getters are strict type/tag reads; use `Coerce` for numeric conversions.
- `Optional` and `Result` are compile-time typed and store their alternatives inline.
- `ByteTag` has a fixed width. Use explicit endian operations for serialized data.
- Template specializations must use the exact argument order and type expected by the corresponding `.def` template.

## Related modules

- [Condition](Condition.md) — Error and OPSTATUS used by Result and fallible type operations.
- [Algorithm](Algorithm.md) — comparison results and callbacks used by traits.
- [Memory](Memory.md) — raw allocation and allocator interfaces.
- [Module](Module.md) — generated POD, package, object and template declarations.

## Source headers

- `Codespace/Cosmeron/Core/Types/Primitive.h`
- `Codespace/Cosmeron/Core/Types/Traits.h`
- `Codespace/Cosmeron/Core/Types/TaggedUnion.h`
- `Codespace/Cosmeron/Core/Types/Any.h`
- `Codespace/Cosmeron/Core/Types/Optional.def`
- `Codespace/Cosmeron/Core/Types/Result.def`
- `Codespace/Cosmeron/Core/Types/ByteTag.def`
