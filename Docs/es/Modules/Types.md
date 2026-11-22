# Types

El módulo **Types** reúne primitivas reutilizables del sistema de tipos: el catálogo canónico de tipos primitivos de C, traits de tipos, valores tagged, almacenamiento con borrado de tipo, valores opcionales, valores de resultado y tags de bytes de ancho fijo.

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

Incluye headers individuales cuando solo se necesite una funcionalidad. Los archivos `Impl` y `TypeTable` son detalles de implementación y los consumidores no deben incluirlos directamente.

---

# Descripción general

| Funcionalidad | Propósito | Almacenamiento/ownership |
| --- | --- | --- |
| Primitive | Nombres canónicos para tipos nativos de C, categorías y tablas de tipos primitivos. | Typedefs y enums; sin asignación. |
| Traits | Describe tamaño, alineación, categorías, flags y callbacks de ciclo de vida/comparación/hash de un tipo. | Descriptor POD estático, normalmente compartido mediante puntero. |
| TaggedUnion | Almacena un primitivo o valor especial seleccionado mediante una tag. | Tagged union inline; sin asignación para payloads nativos. |
| Any | Almacena un valor seleccionado en runtime mediante un descriptor de tipo y small-buffer optimization. | Almacenamiento inline cuando cabe; de lo contrario, heap. |
| Optional | Representa un valor o la ausencia de valor para un tipo definido en compilación. | Valor inline. |
| Result | Representa un valor o un error Condition para un tipo definido en compilación. | Tagged union inline. |
| ByteTag | Almacena una tag de bytes de ancho fijo con conversiones numéricas, binarias y de texto imprimible. | Entero de ancho fijo y array de caracteres inline. |

---

# Referencia de la API

## Tipos primitivos

Header: `Primitive.h`.

El catálogo primitivo `Type` proporciona typedefs C con namespace mediante `MODULE_TYPE(Type, Name)`, incluidos:

- Booleano: `Boolean`;
- enteros con signo: `Int8`, `Int16`, `Int32`, `Int64`;
- enteros sin signo: `UInt8`, `UInt16`, `UInt32`, `UInt64`;
- tipos de carácter: `Char`, `Char8`, `Char16`, `Char32`;
- punto flotante: `Float32`, `Float64`, `LongDouble`;
- tipos dimensionados según la arquitectura: `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr`;
- tipos complejos de punto flotante cuando el compilador los admita.

El header también expone el enum de categorías primitivas y las tablas X-macro utilizadas por otros módulos. Las tablas agregadas más útiles son:

| Tabla | Contenido |
| --- | --- |
| `PRIMITIVE_TYPE_TABLE_EXACT_INTEGER` | Enteros con y sin signo de ancho fijo. |
| `PRIMITIVE_TYPE_TABLE_INTEGER` | Tipos enteros de ancho fijo y dimensionados según la arquitectura. |
| `PRIMITIVE_TYPE_TABLE_NUMERIC` | Tipos enteros, de punto flotante y numéricos complejos. |
| `PRIMITIVE_TYPE_TABLE_ORDERED` | Tipos con ordenación incorporada utilizados por algoritmos ordenados. |
| `PRIMITIVE_TYPE_TABLE` | Tipos ordenados más tipos complejos. |

Utiliza los aliases con namespace al declarar interfaces compartidas con otros módulos de Cosmeron. Hacen explícita la familia de tipos prevista sin reemplazar los tipos ABI subyacentes de C.

---

## Type traits

Header: `Traits.h`.

`MODULE_TYPE(Type, Traits)` es un descriptor POD que contiene:

- nombre, tamaño y alineación del tipo;
- un valor `Type_Category` y flags de traits;
- callbacks opcionales para inicialización, descarte, copia, movimiento, comparación y hashing.

El package proporciona descriptores incorporados para tipos primitivos y un descriptor para C strings.

### Introspección

| Método | Propósito |
| --- | --- |
| `Is.Valid`, `Is.Equal` | Valida un descriptor y compara la identidad/contenido de los descriptores. |
| `Is.Trivial` | Comprueba el flag de ciclo de vida trivial. |
| `Is.TrivialCopy`, `Is.TrivialCompare` | Comprueba si copia/comparación pueden utilizar operaciones triviales de bytes. |
| `Is.Numeric`, `Is.Signed`, `Is.Integer`, `Is.Float` | Comprueba categorías/flags numéricos. |
| `Is.Dynamic` | Comprueba si el tipo tiene requisitos de ciclo de vida dinámico. |
| `HasFlags` | Comprueba una máscara de bits de flags del descriptor. |
| `NeedsDispose` | Comprueba si el valor necesita descarte. |

### Operaciones de ciclo de vida y valor con borrado de tipo

| Método | Contrato |
| --- | --- |
| `Init(traits, instance)` / `Init.Array` | Inicializa un valor o un array. |
| `Dispose(traits, instance)` / `Dispose.Array` | Descarta un valor o un array. |
| `Copy(traits, destination, source)` / `Copy.Array` | Copia un valor o array usando la política registrada. |
| `Move(traits, destination, source)` / `Move.Array` | Mueve un valor o array usando la política registrada. |
| `Swap(traits, left, right)` | Intercambia dos valores según el descriptor. |
| `Compare(traits, left, right)` | Devuelve una comparación de tres vías. |
| `Equals(traits, left, right)` | Comprueba la igualdad. |
| `Hash(traits, instance)` | Devuelve el hash del valor. |
| `Hash.Bytes(data, size)` | Calcula el hash de un intervalo bruto de bytes. |

Los accesores de descriptores incorporados reciben el nombre de su tipo primitivo, y `Traits.CString` proporciona el descriptor de C string.

Los punteros de callback de un descriptor y el propio descriptor deben permanecer válidos durante todas las operaciones que los utilicen. Un valor dinámico no puede copiarse de forma segura con `memcpy`, salvo que el contrato de traits permita explícitamente la copia trivial.

---

## TaggedUnion

Header: `TaggedUnion.h`.

`MODULE_TYPE(Type, TaggedUnion)` almacena un valor seleccionado por un discriminador `MODULE_ENUM(Type, Tag)`. Las tags incorporadas cubren la tabla de tipos primitivos y categorías especiales como vacío, C string, intervalo de bytes y puntero raw.

La API pública incluye:

| Grupo | Operaciones |
| --- | --- |
| Ciclo de vida | `Init`, `Clear` |
| Valores primitivos | `Set_<Primitive>`, `From_<Primitive>`, `Get_<Primitive>` para las entradas de la tabla de tipos primitivos. |
| Valores especiales | `Set_CString`/`From_CString`/`Get_CString`; `Set_Bytes`/`From_Bytes`/`Get_Bytes`; `Set_RawPointer`/`From_RawPointer`/`Get_RawPointer`. |
| Conversión numérica | `Coerce.Int64`, `Coerce.UInt64`, `Coerce.Float64`. |
| Inspección | `Is.Empty`, `Is.Tag`, `Is.Numeric`, `Is.Integer`, `Is.Signed`, `Is.Float`, `TagName`, `CategoryOf`, `TraitsOf`, `PayloadPointer`. |
| Operaciones de valor | `Compare`, `Equals`, `Hash`, `Traits`. |

~~~c
MODULE_TYPE(Type, TaggedUnion) value = {0};

OPSTATUS status = MODULE_FUNC(Type, TaggedUnion, Set_Int32)(&value, 42);
int32_t extracted = 0;

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Type, TaggedUnion, Get_Int32)(
        &value, &extracted);
}
~~~

Los getters son estrictos: esperan que la tag activa coincida con el tipo solicitado. Utiliza las operaciones `Coerce` cuando se pretenda una conversión numérica comprobada. Escriben en un puntero de salida e informan de los fallos de conversión mediante `OPSTATUS`.

`ByteSlice` almacena un puntero y una longitud sin ownership. Los payloads de C string, intervalo de bytes y puntero raw son valores prestados; TaggedUnion no duplica ni libera su memoria de respaldo.

---

## Any

Header: `Any.h`.

`MODULE_TYPE(Type, Any)` es un contenedor de valores con borrado de tipo y small-buffer optimization. El valor predeterminado utiliza la capacidad de small buffer configurada en el header; las variantes generadas por tamaño incluyen `Any32`, `Any64`, `Any128` y `Any256`.

El objeto almacena un puntero a `Type_Traits` y bytes inline o un puntero al heap. Sus operaciones de ciclo de vida utilizan los callbacks del descriptor.

### Ciclo de vida y gestión de valores

| Método | Contrato |
| --- | --- |
| `Init`, `Reset`, `Dispose` | Inicializa, limpia o descarta el valor almacenado. |
| `Emplace(traits, sourceInstance)` | Copia un valor tipado al contenedor. |
| `MoveFrom(traits, sourceInstance)` | Mueve un valor tipado al contenedor. |
| `Clone(destination, source)` | Clona un valor mediante la política de traits registrada. |
| `Move(destination, source)` | Transfiere el valor almacenado. |
| `Swap(left, right)` | Intercambia dos contenedores. |

### Introspección y acceso

| Método | Contrato |
| --- | --- |
| `HasValue`, `Is.Empty` | Comprueba si hay un valor presente. |
| `Is.Inline` | Comprueba si el payload actual cabe en el almacenamiento inline. |
| `Is.Type(expectedTraits)` | Comprueba la identidad del descriptor. |
| `TraitsOf` | Devuelve el descriptor activo o null si el valor está vacío. |
| `Capacity` | Devuelve la capacidad del almacenamiento inline para la especialización predeterminada. |
| `RawPointer`, `MutableRawPointer` | Devuelve un puntero prestado al payload almacenado. |
| `Get(expectedTraits, outValue)` | Copia estrictamente un valor del tipo esperado al almacenamiento del llamador. |
| `Borrow`, `Borrow.Mutable` | Devuelve un puntero prestado después de validar los traits esperados. |

El package también proporciona atajos primitivos `Set_<Primitive>` y `Get_<Primitive>`, conversiones desde/hacia `TaggedUnion`, comparación, igualdad y hashing.

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

El descriptor debe describir correctamente el tipo de origen y el objeto de origen debe ser válido para la operación. Un puntero devuelto por `RawPointer` o `Borrow` es prestado del objeto Any; puede invalidarse después de una sustitución, movimiento, descarte o cualquier operación que cambie su almacenamiento.

Descarta siempre un valor Any antes de que su almacenamiento salga de ámbito si su descriptor requiere descarte. Los setters/getters primitivos de conveniencia no cambian esa regla de ciclo de vida.

---

## Optional

Header: `Optional.h` y definición de template `Optional.def`.

Instancia una especialización para el tipo C que quieras almacenar:

~~~c
TEMPLATE_INSTANTIATE(Optional, Int32, int32_t);
MODULE_TYPED(Optional, Int32) value = {0};
~~~

El objeto generado almacena `hasValue` más un `value` inline.

| Método | Contrato |
| --- | --- |
| `Some(value)` | Almacena un valor y marca Optional como presente. |
| `None()` / `Clear()` | Limpia el valor almacenado. |
| `FromPointer(ptr)` | Almacena el valor apuntado o crea None si el puntero es nulo. |
| `HasValue`, `Is.Some`, `Is.None` | Comprueba el estado. |
| `Value(outValue)` | Copia el valor a la salida; devuelve `NOT_FOUND` cuando está vacío. |
| `Value.Or(fallback)` | Devuelve el valor almacenado o el fallback. |
| `Value.Pointer` / `MutablePointer` | Devuelve un puntero prestado o null. |
| `Map(mapper, outOptional)` | Transforma un valor presente en otro valor del mismo tipo de especialización. |
| `AndThen(binder, outOptional)` | Invoca un binder que escribe un Optional de salida. |
| `Filter(predicate, outOptional)` | Conserva el valor solo cuando el predicado devuelve true. |

`Map`, `AndThen` y `Filter` utilizan delegates generados. Requieren objetos de entrada/salida válidos y delegates válidos. Optional no asigna memoria para el valor almacenado.

---

## Result

Header: `Result.h` y definición de template `Result.def`.

Result combina un valor de un tipo elegido con `Condition_Error`:

~~~c
TEMPLATE_INSTANTIATE(Result, Int32, int32_t);
MODULE_TYPED(Result, Int32) result = {0};
~~~

| Método | Contrato |
| --- | --- |
| `Ok(value)` | Almacena un valor exitoso. |
| `Err(error)` | Almacena un error Condition. |
| `FromStatus(status)` | Crea éxito para `SUCCESS` o un Error a partir de otro status. |
| `Is.Ok`, `Is.Error` | Comprueba la alternativa activa. |
| `Value(outValue)` | Copia el valor; en la alternativa de error, devuelve el status de ese error. |
| `Value.Or(fallback)` | Devuelve el valor o el fallback. |
| `Error(outError)` | Copia el error almacenado cuando Result contiene un error. |
| `Map(mapper, outResult)` | Transforma un valor exitoso y conserva un error. |
| `AndThen(binder, outResult)` | Invoca un binder para el valor exitoso y propaga un error. |

Usa Result cuando una función necesite devolver un valor tipado o un objeto de error más completo. Usa Optional cuando la ausencia sea un estado válido y no requiera una explicación del error.

---

## ByteTag

Header: `ByteTag.h` y definición de template `ByteTag.def`.

Las tags incorporadas son `ByteTag16`, `ByteTag32` y `ByteTag64`. Cada una almacena un entero sin signo del ancho seleccionado y una vista de texto terminada en NUL de los bytes de la tag.

| Grupo | Operaciones |
| --- | --- |
| Construcción | `From.String(chars)`, `From.UInt(value)`, `From.Bytes(bytes, length)` |
| Entrada binaria | `Read.BE`, `Read.LE` |
| Salida binaria | `Write.BE`, `Write.LE` |
| Conversión | `To.String`, `To.UInt` |
| Inspección | `Text`, `IsPrintable`, `Equal`, `Compare` |

`From.String` y `From.Bytes` empaquetan los datos en el ancho fijo de la tag y rellenan las posiciones no utilizadas con espacios. `Read.BE`/`Read.LE` consumen exactamente el ancho de la tag desde un puntero de origen proporcionado por el llamador; `Write.BE`/`Write.LE` escriben exactamente ese ancho en un puntero de destino. Utilizan un orden de bytes explícito, independiente del orden de bytes del host. El llamador debe proporcionar al menos el ancho de la tag en bytes legibles o escribibles. `To.String` requiere un buffer de destino lo bastante grande para todos los bytes de la tag más el NUL terminador. Los bytes no imprimibles se representan mediante el formato de texto imprimible del módulo, en lugar de asumirse como caracteres de visualización válidos.

Los helpers en tiempo de compilación del header pueden construir constantes comunes a partir de literales de bytes. Trata ByteTag como un identificador compacto o una tag de estilo FourCC, no como un string de caracteres genérico.

---

# Contratos y ownership

- Un descriptor POD o valor tagged no gestiona automáticamente el ciclo de vida de los punteros almacenados en su interior.
- `Traits` es la autoridad para inicialización, copia, movimiento, descarte, comparación y hashing con borrado de tipo.
- `Any` puede asignar memoria cuando el valor no cabe en su capacidad inline; sigue siempre su contrato de descarte.
- Los getters de `TaggedUnion` son lecturas estrictas de tipo/tag; utiliza `Coerce` para conversiones numéricas.
- `Optional` y `Result` son tipados en compilación y almacenan sus alternativas inline.
- `ByteTag` tiene ancho fijo. Utiliza operaciones endian explícitas para datos serializados.
- Las especializaciones de templates deben utilizar el orden exacto de argumentos y el tipo esperado por el template `.def` correspondiente.

## Módulos relacionados

- [Condition](Condition.md) — Error y OPSTATUS utilizados por Result y operaciones de tipo que pueden fallar.
- [Algorithm](Algorithm.md) — resultados de comparación y callbacks utilizados por traits.
- [Memory](Memory.md) — asignación raw e interfaces de allocators.
- [Module](Module.md) — declaraciones generadas de POD, package, objeto y templates.

## Headers de origen

- `Codespace/Cosmeron/Core/Types/Primitive.h`
- `Codespace/Cosmeron/Core/Types/Traits.h`
- `Codespace/Cosmeron/Core/Types/TaggedUnion.h`
- `Codespace/Cosmeron/Core/Types/Any.h`
- `Codespace/Cosmeron/Core/Types/Optional.def`
- `Codespace/Cosmeron/Core/Types/Result.def`
- `Codespace/Cosmeron/Core/Types/ByteTag.def`
