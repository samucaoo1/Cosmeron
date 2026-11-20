# Cosmeron Examples

Usage examples for the refactored C11 DSL modules in `Codespace/Cosmeron/`.

> Examples for modules awaiting refactoring live in `Roadmap/Examples/`.

## Structure

- `Core/`
  - `Algorithm.c` — `Comparison`, `Swap`, `Repeat`
  - `Bit.c` — `Bit` (32/64-bit) and `Bitset128`
  - `Condition.c` — `OPSTATUS`, `Error`, `DEFER`
  - `Diagnostic.c` — `Context` and `Format`
  - `Memory.c` — `Arena`, `Pool`, `Stream`
  - `Module.c` — `MODULE`, `OBJECT`, `EVENT_PROCEDURE`, `OBJECT_MUTATE`
  - `Preprocessor.c` — `Detect.h` normalized platform/compiler/architecture flags
  - `Types.c` — `Optional`, `Result`, `ByteTag32`
- `Modules/`
  - `Container.c` — `Array_Vector`, `Hash_Map`, `Iterator`
  - `Math.c` — `Clamp`, `Arithmetic` (checked & saturating), `Equation`
  - `Numeric.c` — `Rational`, `Complex`, `Fixed`, `Bigint128`
  - `Text.c` — `Builder`, `String8`, `View8`, `Format`
  - `Tuple.c` — `Pair`, `Triple`, `Quad`

## Building & Running

Compile any example with strict C11 flags:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L \
  Codespace/Examples/Modules/Text.c -o /tmp/example-text && /tmp/example-text
```
