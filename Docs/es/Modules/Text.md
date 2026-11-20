# Text

El módulo **Text** proporciona clasificación y codificación Unicode, vistas sin ownership, strings mutables, utilidades para C strings, formato, texto con atributos y grids de caracteres respaldados por memoria. Es una API C11 organizada en packages, estructuras tipadas y objetos.

## Headers

~~~c
#include "Cosmeron/Modules/Text/Text.h"
~~~

El agregador incluye las siguientes áreas públicas:

| Header | Funcionalidad |
| --- | --- |
| `Unicode/Codepoint.h` | Clasificación de puntos de código Unicode. |
| `Encoding/UTF8.h`, `Encoding/UTF16.h` | Codificación, decodificación y validación UTF-8 y UTF-16. |
| `Unicode/Width.h` | Ancho aproximado en columnas de visualización. |
| `View/View.h` | Vistas tipadas de strings sin ownership. |
| `String/String.h` | Strings mutables con ownership y utilidades UTF-8. |
| `String/CString/CString.h` | Operaciones con C strings. |
| `String/Builder/Builder.h` | Construcción incremental de strings UTF-8. |
| `Format/Format.h`, `Format/BytesHuman.h` | Formato acotado y conversión de tamaños en bytes a texto. |
| `Grid/Attribute.h`, `Grid/Char.h`, `Grid/Grid.h` | Planos de atributos y grids de texto respaldados por memoria. |
| `Attributed/Attributed.h` | Texto UTF-8 con tramos estilizados. |

---

# Descripción general

Text se divide en validación/codificación Unicode, vistas sin ownership, strings con ownership, builders incrementales, formato acotado, grids de caracteres/atributos en memoria y salida UTF-8 con atributos. Son primitivas de datos y formato; no proporcionan entrada de terminal ni un backend de renderizado.

---

# Referencia de la API

## Unicode y codificaciones

### Codepoint

Header: `Unicode/Codepoint.h`. Package: `Text_Codepoint`.

El package expone predicados bajo `Is`:

| Función | Significado |
| --- | --- |
| `Valid(codepoint)` | Comprueba si el valor está dentro de U+0000..U+10FFFF. |
| `Scalar(codepoint)` | Comprueba si el valor es un escalar Unicode (no un surrogate). |
| `ASCII(codepoint)` | Comprueba si el valor está dentro de U+0000..U+007F. |
| `Control(codepoint)` | Comprueba los rangos de control C0 y C1. |
| `Whitespace(codepoint)` | Comprueba los rangos de whitespace implementados explícitamente por el módulo. |

~~~c
bool scalar = MODULE_FUNC(Text, Codepoint, Is, Scalar)(codepoint);
~~~

Usa `Scalar` antes de codificar. `Valid` por sí solo permite puntos de código surrogate, que no son valores escalares Unicode.

### UTF8

Header: `Encoding/UTF8.h`. Los buffers de entrada y salida utilizan bytes `unsigned char`.

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

La capacidad del buffer de salida se mide en bytes. La decodificación recibe la cantidad de bytes disponibles y escribe el punto de código decodificado y la cantidad de bytes consumidos mediante parámetros de salida. Comprueba cada status antes de leer los valores de salida.

### UTF16

Header: `Encoding/UTF16.h`. La entrada y la salida utilizan unidades de código `MODULE_TYPE(Text, Char16)`.

| Función | Contrato |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Informa de una o dos unidades de código UTF-16. |
| `Encode(codepoint, output, capacity, outUnits)` | Codifica un escalar en un buffer UTF-16 proporcionado por el llamador. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodifica un punto de código e informa de las unidades consumidas. |
| `Validate(input, size)` | Valida un intervalo completo de unidades de código UTF-16. |

La capacidad UTF-16 y los valores de tamaño consumido se miden en unidades de código, no en bytes. Un escalar suplementario requiere un par surrogate.

### Ancho de visualización

Header: `Unicode/Width.h`. Package: `Text_Width`.

| Función | Contrato |
| --- | --- |
| `Codepoint(codepoint, outWidth)` | Informa del ancho de visualización de un punto de código. |
| `UTF8(input, size, outColumns, outUnits)` | Mide las columnas de visualización de un intervalo UTF-8 e informa de los bytes consumidos. |

El cálculo del ancho es una estimación orientada a la visualización, no un motor completo de composición tipográfica o diseño de terminal. Las marcas combinantes y los caracteres anchos pueden afectar al número de columnas de forma distinta al número de unidades de código codificadas.

---

## Vistas y strings

### View

Header: `View/View.h`.

Los tipos no propietarios generados son `MODULE_TYPE(Text, View8)`, `View16` y `View32`. Apuntan a un almacenamiento existente y llevan una longitud explícita.

| Función | Descripción |
| --- | --- |
| `Create(data, length)` | Crea una vista sobre un puntero y una cantidad de unidades de código. |
| `FromCStr(cstr)` | Crea una vista a partir de un string terminado. |
| `Empty()` | Devuelve una vista vacía. |
| `Length`, `IsEmpty`, `Data` | Inspeccionan la vista. |
| `At(index, outValue)` | Lee una unidad de código con comprobación de límites. |
| `Slice(start, count)`, `Prefix(count)`, `Suffix(count)` | Devuelven una subvista. |
| `Compare(left, right)`, `Equal` | Comparan vistas. |
| `Find` | Busca una unidad de código, otra vista o una C string. |
| `StartsWith`, `EndsWith` | Comprueban un prefijo o sufijo. |

Una vista no asigna memoria, no copia ni añade un terminador. Solo es válida mientras el almacenamiento subyacente siga siendo válido y no se mueva. Las longitudes y los índices son unidades de código, salvo cuando se utiliza una operación específicamente consciente de puntos de código.

### Strings con ownership

Header: `String/String.h`. El agregador genera `String8`, `String16` y `String32`. Los tres utilizan la misma API generada; el tipo del elemento es distinto:

| Tipo | Tipo del elemento | Unidad de longitud |
| --- | --- | --- |
| `String8` | `Text_Char8` (`uint8_t`) | Bytes/unidades de código |
| `String16` | `Text_Char16` (`uint16_t`) | Unidades de código de 16 bits |
| `String32` | `Text_Char32` (`uint32_t`) | Unidades de código de 32 bits |

Los métodos de objeto generados son:

| Grupo | Métodos | Comportamiento |
| --- | --- | --- |
| Ciclo de vida | `Create(initialCapacity)`, `Destroy()`, `Clone(source)` | Crea, libera o clona el almacenamiento propio. |
| Capacidad | `Reserve(requiredCapacity)`, `ShrinkToFit()` | Aumenta o reduce la capacidad. |
| Construcción | `FromBuffer(buffer, count)`, `FromCStr(cstr)`, `FromView(view)` | Inicializa desde unidades de código, un string terminado o una vista. |
| Mutación | `Clear()`, `AppendBuffer(buffer, count)`, `Append(cstr)`, `AppendString(other)`, `AppendView(view)`, `PushBack(value)`, `PopBack(outValue)`, `SetAt(index, value)`, `Insert(index, buffer, count)`, `InsertChar(index, value)`, `Erase(index, count)` | Actualiza la secuencia almacenada de unidades de código. |
| Acceso | `Length()`, `Capacity()`, `IsEmpty()`, `CStr()`, `At(index, outValue)` | Lee tamaño, capacidad, estado vacío, datos o una unidad de código. |
| Vistas y subcadenas | `View()`, `Slice(start, count)`, `Substr(start, count, outSubstr)` | Crea una vista prestada o una nueva subcadena con ownership. |
| Comparación | `Compare(other)`, `Equal(other)`, `Equal.CStr(cstr)`, `Equal.View(view)`, `StartsWith(prefix)`, `EndsWith(suffix)` | Compara contenido o comprueba prefijos/sufijos. |
| Búsqueda | `Find(needle, outIndex)`, `Find.Str(needle, outIndex)` | Encuentra una unidad de código o un string terminado. |

`String` almacena una unidad de código cero al final, pero la longitud informada excluye ese terminador. Las longitudes y los índices son unidades de código, no recuentos de escalares Unicode. En `String8`, una unidad de código es un byte UTF-8; operaciones como `At`, `SetAt`, `Erase` y `Slice` no garantizan límites de puntos de código.

~~~c
MODULE_TYPE(Text, String8) text = {0};
const MODULE_TYPE(Text, Char8) input[] = "Cosmeron";

OPSTATUS status = OBJECT_CALL(&text, Create, 16U);
if (status == STATUS_CONST(SUCCESS)) {
    status = OBJECT_CALL(&text, Append, input);
}
if (status == STATUS_CONST(SUCCESS)) {
    size_t length = OBJECT_CALL_0(&text, Length);
    const MODULE_TYPE(Text, Char8) *data = OBJECT_CALL_0(&text, CStr);
    (void)length;
    (void)data;
}
(void)OBJECT_CALL_0(&text, Destroy);
~~~

El string es propietario de su asignación de respaldo. Un puntero devuelto por `CStr` es prestado y puede invalidarse por mutación, cambios de capacidad o destrucción. Usa `View`/`Slice` cuando sea suficiente un intervalo sin ownership y mantén vivo al propietario mientras se utilice la vista.

### Utilidades de C strings

Header: `String/CString/CString.h`. Package: `Text_CString`.

El package proporciona:

| Grupo | Métodos |
| --- | --- |
| Longitud | `Length`, `Length.Bounded` |
| Clasificación | `IsEmpty`, `IsAscii`, `IsAscii.Bounded` |
| Copia y concatenación | `Copy`, `Copy.Bounded`, `Concat`, `Concat.Bounded` |
| Comparación | `Compare`, `Compare.Bounded`, `Compare.IgnoreCase`, `Compare.IgnoreCaseBounded` |
| Igualdad | `Equal`, `Equal.Bounded`, `Equal.IgnoreCase`, `Equal.IgnoreCaseBounded` |
| Prefijo/sufijo | `StartsWith`, `StartsWith.Bounded`, `StartsWith.IgnoreCase`, `EndsWith`, `EndsWith.Bounded`, `EndsWith.IgnoreCase` |
| Búsqueda | `FindChar`, `FindLastChar`, `FindSubstring` y sus variantes acotadas |
| Span | `Span`, `ComplementSpan` |
| Trim | `Trim`, `Trim.Start`, `Trim.End` |
| Conteo/reemplazo | `CountChar`, `CountChar.Bounded`, `ReplaceChar`, `ReplaceChar.Bounded` |
| Asignación | `Duplicate`, `Duplicate.Bounded`, `Free` |

Prefiere las operaciones acotadas siempre que se conozca la extensión disponible y la entrada pueda no contener un terminador dentro de ese intervalo. `Copy` y `Concat` reciben la capacidad del destino. `Duplicate` asigna un nuevo string; libéralo con `Free`.

### String Builder

Header: `String/Builder/Builder.h`. Tipo: `MODULE_TYPE(Text, Builder)`.

Builder es un buffer UTF-8 de crecimiento dinámico diseñado para composición incremental.

| Método | Descripción |
| --- | --- |
| `Create(initialCapacity)`, `Destroy` | Crea/libera la asignación de respaldo. |
| `Clear`, `Reserve(newCapacity)`, `ShrinkToFit` | Controla el contenido y la capacidad. |
| `Append.Char` | Añade un carácter de un byte. |
| `Append.CStr` | Añade una C string. |
| `Append.Buffer` | Añade un buffer de bytes de tamaño explícito. |
| `Append.View` | Añade una vista UTF-8. |
| `Append.Codepoint` | Añade un punto de código Unicode. |
| `Append.Format`, `Append.FormatVariadic` | Añade salida formateada. |
| `BuildString(outString)` | Materializa el contenido como un String8 con ownership. |
| `Length`, `Capacity`, `IsEmpty`, `CStr`, `View` | Observa el contenido actual. |

Builder es propietario de su buffer. `CStr` y `View` devuelven referencias prestadas y no deben conservarse entre operaciones que puedan realocar el buffer ni después de su destrucción.

---

## Formato

### Format

Header: `Format/Format.h`. Package: `Text_Format`.

| Función | Contrato |
| --- | --- |
| `Variadic(buffer, capacity, outWritten, format, arguments)` | Formateador acotado que recibe una `va_list`. |
| `Variadic.Unsafe(buffer, outWritten, format, arguments)` | Formateador sin límite; el llamador debe garantizar espacio suficiente. |
| `Buffer(buffer, capacity, outWritten, format, ...)` | Wrapper variádico seguro y práctico. |
| `Unsafe(buffer, outWritten, format, ...)` | Wrapper variádico inseguro y práctico. |

Prefiere `Buffer` o `Variadic`. Las formas inseguras omiten intencionadamente el argumento de capacidad de salida y solo deben utilizarse cuando el tamaño del destino esté garantizado.

### Tamaños de bytes legibles para humanos

Header: `Format/BytesHuman.h`. Package: `Text_BytesHuman`.

| Función | Descripción |
| --- | --- |
| `Format(buffer, capacity, outWritten, bytes)` | Formatea una cantidad de bytes usando el estándar de unidades y la precisión predeterminados. |
| `Format.Ex(buffer, capacity, outWritten, bytes, unitStandard, precision)` | Selecciona unidades IEC/SI y precisión. |
| `Parse(text, outBytes)` | Analiza un valor textual de tamaño en bytes y lo convierte a `uint64_t`. |

El enum de configuración define base IEC 1024, base SI 1000, precisión predeterminada 2 y precisión máxima 6. El formateador escribe en un buffer propiedad del llamador e informa de la cantidad de bytes escritos.

---

## Grids de caracteres y atributos

### Alcance y diseño

Las operaciones de Grid trabajan con celdas respaldadas por memoria. El módulo **no** escribe en stdout, controla un terminal, lee stdin, llama a ncurses ni muestra un canvas. El renderizado y la entrada de terminal pertenecen a una capa de aplicación/backend.

Las variantes de caracteres utilizan `Char8`, `Char16` y `Char32`. Las dimensiones y posiciones del grid utilizan `MODULE_TYPED(Tuple, Dual, UInt16)`; las regiones rectangulares utilizan `MODULE_TYPED(Tuple, Quad, UInt16)`.

### Grid solo de caracteres

Header: `Grid/Char.h`. Los tipos de objeto generados siguen `MODULE_TYPED(Text, Char, Char8)`, `Char16` y `Char32`.

El grid Char almacena únicamente caracteres. Sus métodos de objeto incluyen `Create(size)`, `Destroy`, `Resize(size)`, `Recreate(size)`, `Clone(source)`, `Clear`, `ReadCell(position, outCharacter)`, `WriteCell(position, character)`, `Fill(character)`, `FillRegion(region, character)`, `Read(region, destination)` y `Write`/`Write.Region`. `IsValid` comprueba el estado del objeto.

### Grid de texto compuesto

Header: `Grid/Grid.h`. Los tipos de objeto generados son `MODULE_TYPED(Text, Grid, Char8)`, `Char16` y `Char32`.

Un grid compuesto almacena un plano de caracteres junto con un plano de atributos. Sus métodos incluyen `Create`, `Destroy`, `Resize`, `Recreate`, `Clone`, `Clear`, `Clear.With`, `ReadCell`, `WriteCell`, `Fill`, `FillRegion`, `Read`, `Write`, `WriteRegion` y `Blit`/`Blit.Region`.

El grid es propietario de sus planos de respaldo. Una operación de resize/recreate/destroy puede invalidar punteros de celdas o datos previamente prestados. Utiliza el status devuelto por todas las operaciones mutables.

### Plano de atributos y colores

Header: `Grid/Attribute.h`.

La API de atributos expone `MODULE_TYPE(Text, Attribute, Cell)` y `MODULE_TYPE(Text, Attribute, Grid)`. Los flags de atributo incluyen negrita, tenue, cursiva, blink, inverso, oculto, tachado, sobrelineado, fondo transparente, continuación de unidad de código y primer plano/fondo predeterminados.

El package `Attribute` expone `Default`, `IsEmpty`, `IsValid`, `Create`, `Destroy`, `Resize`, `Recreate`, `Clone`, `Clear`, `Clear.With`, `Fill`, `ReadCell`, `WriteCell`, `FillRegion`, `Read`, `Write` y `Write.Region`.

`Grid_Color` proporciona `RGB(red, green, blue)`, `Red(color)`, `Green(color)`, `Blue(color)` y `WindowsPalette()`. La paleta de Windows es una tabla estática de 16 colores; es una utilidad de color, no un backend de terminal.

---

## Texto con atributos

Header: `Attributed/Attributed.h`. Tipo: `MODULE_TYPE(Text, Attributed)`.

El texto con atributos almacena contenido UTF-8 con tramos de atributos asociados.

| Método | Descripción |
| --- | --- |
| `Create(initialCapacity)`, `Destroy` | Crea/libera el almacenamiento de texto y tramos. |
| `Clear` | Limpia el texto y los tramos. |
| `Clear.Spans` | Elimina los tramos de estilo y conserva el texto. |
| `SetText(text, length)`, `AppendText(text, length)` | Sustituye o añade un intervalo de bytes UTF-8 de tamaño explícito. |
| `AddSpan(start, length, attribute)` | Asocia una celda de atributo a un rango de texto. |
| `GetAttributeAt(index, outAttribute)` | Obtiene el atributo en un índice de byte. |
| `RenderAnsi(outBuffer, capacity, outWritten)` | Renderiza secuencias de escape ANSI en un buffer propiedad del llamador. |
| `Text`, `Length`, `SpanCount`, `Spans` | Inspecciona el contenido actual. |

`Attributed.h` desactiva la FunctionTable de objetos Text; llama a sus métodos directamente mediante `MODULE_FUNC(Text, Attributed, Method)` en lugar de usar `OBJECT_CALL`. Los arrays de texto y tramos son gestionados por el objeto Attributed. Los punteros de texto/tramos devueltos son prestados. `RenderAnsi` solo produce un string; no lo imprime ni configura el terminal.

---

# Contratos y reglas de ownership

- Los encoders, formateadores, operaciones de grid y operaciones de string que escriben en buffers devuelven `OPSTATUS`; compruébalo antes de leer las salidas.
- Las longitudes UTF-8 son bytes; las longitudes UTF-16 son unidades de código; los recuentos de puntos de código son una unidad distinta.
- Una View no es propietaria. String y Builder son propietarios de sus buffers.
- Los objetos Grid y Attributed son propietarios de su almacenamiento de respaldo y deben destruirse.
- Los punteros devueltos por `Data`, `CStr`, `Text` y `Spans` son prestados y pueden invalidarse por mutación o destrucción.
- El grid es una estructura de memoria off-screen, no una UI de terminal, un renderizador ni un subsistema de entrada.

## Módulos relacionados

- [Tuple](Tuple.md) — dimensiones y regiones del grid.
- [Memory](Memory.md) — asignación y operaciones con bytes.
- [Algorithm](Algorithm.md) — resultados de comparación utilizados por las vistas de texto.
- [Module](Module.md) — packages, objetos y especializaciones tipadas generadas.

## Headers de origen

- `Codespace/Cosmeron/Modules/Text/Text.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Codepoint.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Width.h`
- `Codespace/Cosmeron/Modules/Text/Encoding/UTF8.h`
- `Codespace/Cosmeron/Modules/Text/Encoding/UTF16.h`
- `Codespace/Cosmeron/Modules/Text/View/View.def`
- `Codespace/Cosmeron/Modules/Text/String/String.def`
- `Codespace/Cosmeron/Modules/Text/String/CString/CString.h`
- `Codespace/Cosmeron/Modules/Text/String/Builder/Builder.h`
- `Codespace/Cosmeron/Modules/Text/Format/Format.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Attribute.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Char.def`
- `Codespace/Cosmeron/Modules/Text/Grid/Grid.def`
- `Codespace/Cosmeron/Modules/Text/Attributed/Attributed.h`
