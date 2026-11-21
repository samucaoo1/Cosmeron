# Text

El módulo **Text** proporciona clasificación y transcodificación Unicode, utilidades para C strings, vistas sin ownership, strings mutables con ownership, formato desacoplado por streaming y grids de caracteres y atributos respaldados por memoria. Es una API C11 organizada en packages, estructuras POD puras y objetos.

## Headers

~~~c
#include "Cosmeron/Modules/Text/Text.h"
~~~

El agregador incluye las siguientes áreas públicas:

| Header | Funcionalidad |
| --- | --- |
| `Unicode/Codepoint.h` | Clasificación de puntos de código Unicode. |
| `Unicode/UTF8.h`, `Unicode/UTF16.h` | Codificación, decodificación, validación, conteo y transcodificación UTF-8 y UTF-16. |
| `Unicode/Width.h` | Ancho aproximado en columnas de visualización. |
| `CString/CString.h` | Operaciones stateless con C strings terminadas en NUL. |
| `View/View.h` | Vistas tipadas de strings sin ownership (`View8`, `View16`, `View32`). |
| `String/String.h` | Strings mutables con ownership (`String8`, `String16`, `String32`) y utilidades UTF-8. |
| `Format/Writer.h` | Sink de texto desacoplado (`Counter`, `ToBuffer`, `ToClosure`, `ToString`) y escritores tipados. |
| `Format/Format.h` | Formato acotado seguro y basado en `Writer` con adaptadores `%!`. |
| `Format/BytesHuman.h` | Formato y análisis de tamaños de bytes legibles para humanos. |
| `Grid/Attribute.h`, `Grid/Char.h`, `Grid/Grid.h` | Planos de atributos, utilidades de color y grids de texto respaldados por memoria. |

---

# Descripción general

Text separa las primitivas de bajo nivel (Unicode y C strings) de las vistas sin ownership, strings mutables con ownership, formato desacoplado (`Writer` + `Format` + `BytesHuman`) y grids off-screen de caracteres y atributos. Ninguna de estas áreas realiza E/S de terminal ni acopla `Text` a módulos externos como `Tuple` o `Numeric` en el formateador.

---

# Referencia de la API

## Unicode y codificaciones

### Codepoint

Header: `Unicode/Codepoint.h`. Package: `Text_Codepoint`.

El package expone predicados bajo la familia `Is` (`VARIANT`):

| Función | Significado |
| --- | --- |
| `IsValid(codepoint)` | Comprueba si el valor está dentro de U+0000..U+10FFFF. |
| `IsScalar(codepoint)` | Comprueba si el valor es un escalar Unicode (no un surrogate). |
| `IsAscii(codepoint)` | Comprueba si el valor está dentro de U+0000..U+007F. |
| `IsControl(codepoint)` | Comprueba los rangos de control C0 y C1. |
| `IsWhitespace(codepoint)` | Comprueba los rangos de whitespace Unicode implementados por el módulo. |

~~~c
bool scalar = MODULE_FUNC(Text, Codepoint, IsScalar)(codepoint);
~~~

Usa `IsScalar` antes de codificar. `IsValid` por sí solo permite puntos de código surrogate, que no son valores escalares Unicode.

### UTF8

Header: `Unicode/UTF8.h`. Los buffers de entrada y salida utilizan bytes `unsigned char`.

| Función | Contrato |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Informa del número de bytes UTF-8 necesarios. |
| `Encode(codepoint, output, capacity, outUnits)` | Codifica un escalar en un buffer de bytes proporcionado por el llamador. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodifica una secuencia e informa de los bytes consumidos. |
| `Validate(input, size)` | Devuelve si todo el intervalo de bytes es UTF-8 válido. |
| `Count(input, size, outCodepoints, outUnits)` | Cuenta puntos de código e informa de las unidades consumidas. |

~~~c
unsigned char encoded[4];
size_t written = 0U;

OPSTATUS status = MODULE_FUNC(Text, UTF8, Encode)(
    (MODULE_TYPE(Text, Char32))0x1F642,
    encoded, sizeof(encoded), &written);
~~~

### UTF16

Header: `Unicode/UTF16.h`. La entrada y la salida utilizan unidades de código `MODULE_TYPE(Text, Char16)`.

| Función | Contrato |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Informa de una o dos unidades de código UTF-16. |
| `Encode(codepoint, output, capacity, outUnits)` | Codifica un escalar en un buffer UTF-16 proporcionado por el llamador. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodifica un punto de código e informa de las unidades consumidas. |
| `Validate(input, size)` | Valida un intervalo completo de unidades de código UTF-16. |
| `Count(input, size, outCodepoints, outUnits)` | Cuenta puntos de código e informa de las unidades consumidas. |
| `FromUTF8(input, size, output, capacity, outUnits)` | Transcodifica bytes UTF-8 a unidades UTF-16 (pasa `NULL, 0U` para medir). |
| `ToUTF8(input, size, output, capacity, outBytes)` | Transcodifica unidades UTF-16 a bytes UTF-8 (pasa `NULL, 0U` para medir). |

### Ancho de visualización

Header: `Unicode/Width.h`. Package: `Text_Width`.

| Función | Contrato |
| --- | --- |
| `Codepoint(codepoint, outWidth)` | Informa del ancho de visualización (`0`, `1` o `2`) de un punto de código. |
| `UTF8(input, size, outColumns, outUnits)` | Mide las columnas de visualización de un intervalo UTF-8 e informa de los bytes consumidos. |

---

## C strings, Vistas y Strings

### CString

Header: `CString/CString.h`. Package: `Text_CString`.

| Grupo | Métodos |
| --- | --- |
| Longitud | `Length`, `LengthBounded` |
| Clasificación | `IsEmpty`, `IsAscii`, `IsAsciiBounded` |
| Copia y concatenación | `Copy`, `CopyBounded`, `Concat`, `ConcatBounded` |
| Comparación | `Compare`, `CompareBounded`, `CompareIgnoreCase`, `CompareIgnoreCaseBounded` |
| Igualdad | `Equal`, `EqualBounded`, `EqualIgnoreCase`, `EqualIgnoreCaseBounded` |
| Prefijo/sufijo | `StartsWith`, `StartsWithBounded`, `StartsWithIgnoreCase`, `EndsWith`, `EndsWithBounded`, `EndsWithIgnoreCase` |
| Búsqueda | `FindChar`, `FindCharBounded`, `FindLastChar`, `FindLastCharBounded`, `FindCStr`, `FindCStrBounded` |
| Span | `Span`, `ComplementSpan` |
| Trim | `Trim`, `TrimLeft`, `TrimRight` |
| Conteo/reemplazo | `CountChar`, `CountCharBounded`, `ReplaceChar`, `ReplaceCharBounded` |
| Asignación | `Duplicate`, `DuplicateBounded`, `Free` |

Prefiere las operaciones `*Bounded` siempre que se conozca la extensión disponible y la entrada pueda no contener un terminador dentro de ese intervalo.

### View

Header: `View/View.h`.

Los tipos sin ownership generados son `MODULE_TYPE(Text, View8)`, `View16` y `View32`. Las longitudes y los índices están en unidades de código, salvo cuando se usan los helpers conscientes de puntos de código UTF-8 (`View8_*Codepoints`, `View8_Validate`, `View8_Trim*`).

| Función | Descripción |
| --- | --- |
| `Create(data, length)`, `FromCStr(cstr)`, `Empty()` | Crean vistas sin asignación de memoria. |
| `Length`, `IsEmpty`, `Data`, `At(index, outValue)` | Inspeccionan el contenido de la vista. |
| `Slice(start, count)`, `Prefix(count)`, `Suffix(count)` | Devuelven subvistas. |
| `Compare(left, right)`, `Equal`, `EqualCStr` | Comparan vistas. |
| `Find`, `FindView`, `FindCStr`, `StartsWith*`, `EndsWith*` | Buscan y comprueban prefijos/sufijos. |
| `View8_Validate`, `View8_CountCodepoints`, `View8_NextCodepoint`, `View8_Trim*`, `View8_SliceCodepoints` | Operaciones UTF-8 por puntos de código en `View8`. |

### Strings con ownership

Header: `String/String.h`. Genera `String8` (`char`), `String16` (`Text_Char16`) y `String32` (`Text_Char32`).

| Grupo | Métodos |
| --- | --- |
| Ciclo de vida | `Create(initialCapacity)`, `Destroy()`, `Clone(src)` |
| Capacidad | `Reserve(requiredCapacity)`, `ShrinkToFit()` |
| Construcción | `FromBuffer(buffer, count)`, `FromCStr(cstr)`, `FromView(view)` |
| Mutación | `Clear()`, `AppendBuffer(buffer, count)`, `AppendCStr(cstr)`, `AppendString(other)`, `AppendView(view)`, `PushBack(value)`, `PopBack(outChar)`, `SetAt(index, value)`, `Insert(index, buffer, count)`, `InsertChar(index, value)`, `Erase(index, count)` |
| Acceso | `Length()`, `Capacity()`, `IsEmpty()`, `CStr()`, `At(index, outChar)` |
| Vistas y subcadenas | `View()`, `Slice(start, count)`, `Substr(start, count, outSubstr)` |
| Comparación | `Compare(other)`, `Equal(other)`, `EqualCStr(cstr)`, `EqualView(view)`, `StartsWith(prefix)`, `EndsWith(suffix)` |
| Búsqueda | `Find(needle, outIndex)`, `FindCStr(needle, outIndex)` |
| UTF-8 (`String8_*`) | `AppendCodepoint`, `CountCodepoints`, `Validate`, `Trim`, `TrimLeft`, `TrimRight`, `Format`, `FormatVariadic`, `AppendFormat`, `AppendFormatVariadic`, `SliceCodepoints` |

`String8` también expone `MODULE_FUNC(Text, Writer, ToString)(&str)` para que `Writer` y `Format` escriban directamente en una `String8` con crecimiento dinámico en una sola pasada.

~~~c
MODULE_TYPE(Text, String8) text = {0};
if (MODULE_FUNC(Text, String8, Create)(&text, 16U) == STATUS_CONST(SUCCESS)) {
    (void)MODULE_FUNC(Text, String8, Format)(&text, "Hello %s!", "Cosmeron");
    (void)MODULE_FUNC(Text, String8, Destroy)(&text);
}
~~~

---

## Formato

### Writer

Header: `Format/Writer.h`. Tipo: `MODULE_TYPE(Text, Writer)`. Package: `Text_Writer`.

`Writer` desacopla la generación de texto del destino de salida:

| Constructor | Destino |
| --- | --- |
| `ToBuffer(buffer, capacity)` | Buffer `char` propiedad del llamador (siempre terminado en NUL; registra truncamiento). |
| `Counter()` | Modo de solo medición (cuenta bytes sin almacenar). |
| `ToClosure(sink)` | Cualquier destino `MODULE_CLOSURE(Text, WriterSink)`. |
| `ToString(str)` | Declarado en `String/String.h`; añade directamente a una `String8`. |

Primitivas tipadas: `WriteBuffer`, `WriteChar`, `WriteCodepoint`, `WriteRepeat`, `WriteCStr`, `WriteView`, `WriteBool`, `WriteInt`, `WriteUInt`, `WriteFloat`, `WriteHex`, `WriteEscaped`, además de inspección (`Written`, `IsTruncated`, `Status`) y `Finish`.

### Format

Header: `Format/Format.h`. Package: `Text_Format`.

| Función | Contrato |
| --- | --- |
| `To(writer, format, ...)` / `ToVariadic(writer, format, args)` | Escribe salida formateada en cualquier `Text_Writer`. |
| `Buffer(buffer, capacity, outWritten, format, ...)` / `BufferVariadic(...)` | Formateador acotado seguro a buffer (`NULL, 0U` mide los bytes necesarios). |

Admite especificadores estándar C11/C23 (`%d`/`%i`, `%u`, `%o`, `%x`/`%X`, `%b`/`%B`, `%f`/`%F`/`%e`/`%E`/`%g`/`%G`/`%a`/`%A`, `%c`/`%lc`, `%s`/`%ls`, `%p`, `%%`, ancho/precisión/`*`, `hh`/`h`/`l`/`ll`/`j`/`z`/`t`/`L`/`w8`/`w16`/`w32`/`w64`) y extensiones nativas de Cosmeron:
- `%v`: `MODULE_TYPE(Text, View8)` por valor.
- `%U`: `MODULE_TYPE(Text, Char32)` codificado en UTF-8.
- `%t`: `bool` (`true` / `false`).
- `%q` / `%#q`: C-string escapada (con o sin comillas).
- `%!` / `%(spec)!`: par adaptador personalizado `(MODULE_CALLBACK(Text, Format, Adapter) adapter, const void *value)` para formatear tipos externos sin acoplar `Text` a otros módulos.

### Tamaños de bytes legibles para humanos

Header: `Format/BytesHuman.h`. Package: `Text_BytesHuman`. Enum: `MODULE_ENUM(Text, BytesHuman, Unit)` (`IEC`, `SI`).

| Función | Descripción |
| --- | --- |
| `Write(writer, bytes)` / `WriteEx(writer, bytes, unitStandard, precision)` | Escribe el tamaño formateado en un `Text_Writer`. |
| `Format(buffer, capacity, outWritten, bytes)` / `FormatEx(...)` | Formato acotado a un buffer del llamador. |
| `Parse(text, outBytes)` | Analiza un tamaño textual en bytes a `uint64_t` con protección contra overflow. |

---

## Grids de caracteres y atributos

### Alcance y diseño

Las operaciones de Grid trabajan sobre celdas respaldadas por memoria off-screen (`MODULE_TYPED(Tuple, Dual, UInt16)` para dimensiones/posiciones y `MODULE_TYPED(Tuple, Quad, UInt16)` para regiones). No escriben en stdout ni controlan un terminal.

- `Grid/Attribute.h`: expone `MODULE_TYPE(Text, Attribute, Cell)`, `MODULE_TYPE(Text, Attribute, Grid)`, `MODULE_ENUM(Text, Attribute, Flag)`, `MODULE_ENUM(Text, Attribute, Underline)`, operaciones de plano `Attribute_*` y `Attribute_Color` (`SPACE(Color, ...)` con `RGB`, `Red`, `Green`, `Blue` y `WindowsPalette`).
- `Grid/Char.h`: genera `MODULE_TYPED(Text, Char, Char8)`, `Char16` y `Char32`.
- `Grid/Grid.h`: genera `MODULE_TYPED(Text, Grid, Char8)`, `Char16` y `Char32`, combinando planos de caracteres y atributos con `Blit` / `BlitRegion`.

---

# Contratos y reglas de ownership

- Los encoders, formateadores, operaciones de grid y operaciones de string que escriben en buffers devuelven `OPSTATUS`; compruébalo antes de leer salidas.
- Las longitudes UTF-8 son bytes; las longitudes UTF-16 son unidades de código; los recuentos de puntos de código son recuentos de escalares Unicode.
- `View` no tiene ownership; `String` y `Grid` son propietarios de sus asignaciones y deben destruirse.
- Los punteros devueltos por `Data`, `CStr` y `WindowsPalette` son prestados.

## Módulos relacionados

- [Tuple](Tuple.md) — dimensiones y regiones del grid.
- [Memory](Memory.md) — asignación y operaciones con bytes.
- [Algorithm](Algorithm.md) — resultados de comparación utilizados por vistas y C strings.
- [Module](Module.md) — packages, objetos, closures y especializaciones tipadas.

## Headers de origen

- `Codespace/Cosmeron/Modules/Text/Text.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Codepoint.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/UTF8.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/UTF16.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Width.h`
- `Codespace/Cosmeron/Modules/Text/CString/CString.h`
- `Codespace/Cosmeron/Modules/Text/View/View.def`
- `Codespace/Cosmeron/Modules/Text/String/String.def`
- `Codespace/Cosmeron/Modules/Text/Format/Writer.h`
- `Codespace/Cosmeron/Modules/Text/Format/Format.h`
- `Codespace/Cosmeron/Modules/Text/Format/BytesHuman.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Attribute.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Char.def`
- `Codespace/Cosmeron/Modules/Text/Grid/Grid.def`
