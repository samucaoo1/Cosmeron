# Types

Core algebraic, tagged, and primitive types (`Codespace/Cosmeron/Core/Types/`).

## Headers

- `Cosmeron/Core/Types/Primitive.h` — Canonical namespaced primitive types (`MODULE_TYPE(Type, Int32)`, `MODULE_TYPE(Type, UInt64)`, etc.).
- `Cosmeron/Core/Types/Optional.h` / `Optional.def` — `Optional<T>` (`Some`, `None`, `IsSome`, `IsNone`, `Value`, `ValueOr`, `Map`, `AndThen`, `Filter`).
- `Cosmeron/Core/Types/Result.h` / `Result.def` — `Result<T>` (`Ok`, `Err`, `FromStatus`, `IsOk`, `IsError`, `Value`, `ValueOr`, `Error`, `Map`, `AndThen`).
- `Cosmeron/Core/Types/ByteTag.h` / `ByteTag.def` — Fixed-width byte tags (`ByteTag16`, `ByteTag32`, `ByteTag64`) with big-endian/string conversion.
- `Cosmeron/Core/Types/Any.h`, `TaggedUnion.h`, `Traits.h` — Type-erased `Any`, tagged unions, and lifecycle traits.

See [Examples/Core/Types.c](../../../Codespace/Examples/Core/Types.c).
