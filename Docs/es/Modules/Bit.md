# Bits (Bit)

El módulo **Bit** proporciona operaciones de bits para enteros sin signo y bitsets de capacidad fija. `Bit` trabaja directamente con valores enteros escalares; `Bitset` almacena una cantidad de bits seleccionada en tiempo de compilación mediante limbs de 64 bits.

## Encabezados

~~~c
#include "Cosmeron/Core/Bit/Bit.h"
#include "Cosmeron/Core/Bit/Bitset.h"
~~~

---

# Visión general

## Operaciones escalares

El template `Bit` genera un paquete para un tipo entero sin signo. Los paquetes estándar se instancian para los anchos declarados por `BIT_WIDTH_TYPE_TABLE`. La API cubre bits individuales, máscaras, recuentos de bits, recuentos de ceros, auxiliares de potencias de dos, rotaciones, extracción/inserción de campos, inversión de bits e intercambio de bytes.

## Bitsets de tamaño fijo

`Bitset` genera un objeto cuya capacidad se fija en tiempo de compilación. Su almacenamiento es un array de limbs `uint64_t`; el último limb se enmascara para que los bits fuera de la capacidad declarada no sean observables. Un bitset resulta útil para flags, conjuntos de características, mapas compactos de pertenencia y álgebra de conjuntos mediante operaciones de bits cuando se conoce de antemano el número de posiciones.

Las especializaciones estándar son `Bitset128`, `Bitset256`, `Bitset512` y `Bitset1024`.

---

# Referencia de la API

## Paquete Bit escalar

Encabezado: `Cosmeron/Core/Bit/Bit.h`.

Para un paquete escalar generado con sufijo `<Width>`, el prefijo de función directa es `MODULE_FUNC(Bit, <Width>, ...)`. Los sufijos habituales incluyen `8`, `16`, `32` y `64`.

| Grupo | Funciones | Contrato |
| --- | --- | --- |
| Bit individual | `Set`, `Clear`, `Flip`, `Check` | Establece, limpia, invierte o comprueba un índice de bit que comienza en cero. |
| Máscara | `Mask.Set`, `Mask.Clear`, `Mask.Flip`, `Mask.CheckAll`, `Mask.CheckAny`, `Mask.Range` | Aplica o comprueba una máscara, o construye una máscara de rango inclusivo alto/bajo. |
| Recuentos | `Popcount`, `Count.LeadingZeros`, `Count.TrailingZeros` | Cuenta bits establecidos o ceros. |
| Propiedades | `Parity`, `IsPowerOfTwo` | Comprueba la paridad o si un valor no nulo tiene exactamente un bit establecido. |
| Potencias y logaritmos | `FloorPowerOfTwo`, `NextPowerOfTwo`, `Log2.Floor`, `Log2.Ceil` | Redondea a potencias de dos o calcula logaritmos enteros en base dos. |
| Índices de bits | `Index.LSB`, `Index.MSB` | Encuentra el índice del bit establecido menos o más significativo. |
| Bit establecido más bajo | `IsolateLowestSet`, `ClearLowestSet` | Aísla o limpia el bit establecido más bajo. |
| Rotación | `Rotate.Left`, `Rotate.Right` | Rota dentro del ancho entero seleccionado. |
| Representación | `ReverseBits`, `ByteSwap` | Invierte el orden de bits o bytes. |
| Campos de bits | `Extract`, `Insert` | Lee o reemplaza un campo de bits con extremos inclusivos. |

### Bits individuales

~~~c
uint32_t flags = 0U;

OPSTATUS status = MODULE_FUNC(Bit, 32, Set)(&flags, 5U);
bool enabled = MODULE_FUNC(Bit, 32, Check)(flags, 5U);
~~~

Las operaciones de mutación reciben un puntero al valor y devuelven `OPSTATUS`. `Check` recibe el valor por copia y devuelve `bool`. Los índices de bit comienzan en cero y deben ser menores que el ancho del tipo.

### Máscaras

`Mask.Set`, `Mask.Clear` y `Mask.Flip` son procedimientos: modifican todos los bits seleccionados por la máscara y no hacen nada si el puntero al valor es nulo. `Mask.CheckAll` comprueba si están establecidos todos los bits solicitados; `Mask.CheckAny` comprueba si hay al menos uno. `Mask.Range` puede fallar e informa de un puntero de salida inválido o de extremos invertidos/fuera del ancho mediante `OPSTATUS`.

`Mask.Range(high, low, outMask)` crea una máscara que cubre ambos extremos. Rechaza un rango invertido e índices fuera del ancho escalar.

~~~c
uint32_t fieldMask = 0U;
OPSTATUS status = MODULE_FUNC(Bit, 32, Mask, Range)(15U, 8U, &fieldMask);
~~~

### Recuentos, índices y potencias

`Popcount` devuelve la cantidad de bits establecidos. Los recuentos de ceros iniciales/finales se miden dentro del ancho del tipo seleccionado. `Index.LSB` e `Index.MSB` utilizan un parámetro de salida y devuelven `NOT_FOUND` cuando la entrada es cero.

`NextPowerOfTwo`, `Log2.Floor`, `Log2.Ceil` y las operaciones de índice pueden fallar porque algunas entradas no tienen un resultado válido o provocarían overflow. Comprueba su `OPSTATUS` antes de leer el parámetro de salida.

### Rotaciones y campos de bits

Los recuentos de rotación se ajustan mediante módulo del ancho entero. `Extract(value, high, low, outField)` e `Insert(value, high, low, fieldValue)` utilizan extremos inclusivos. Validan el puntero de salida o el puntero de entrada mutable y rechazan límites inválidos.

---

## Paquete Bitset

Encabezado: `Cosmeron/Core/Bit/Bitset.h`.

Los tipos de objeto generados son `MODULE_TYPE(Bitset128)`, `MODULE_TYPE(Bitset256)`, `MODULE_TYPE(Bitset512)` y `MODULE_TYPE(Bitset1024)`.

| Grupo | Métodos |
| --- | --- |
| Inicialización y estado base | `Clear`, `SetAll` |
| Álgebra de conjuntos | `And`, `Or`, `Xor`, `Not` |
| Desplazamientos | `ShiftLeft`, `ShiftRight` |
| Bit individual | `Bit.Set`, `Bit.Clear`, `Bit.Flip`, `Bit.Check`, `Bit.Test` |
| Rangos | `SetRange`, `ClearRange`, `FlipRange` |
| Consultas | `Count`, `Capacity`, `Any`, `All`, `None`, `Equal`, `Intersects` |
| Relaciones | `Is.Subset`, `Is.Disjoint` |
| Búsqueda | `Find.FirstSet`, `Find.FirstZero`, `Find.NextSet` |

### Ejemplo

~~~c
MODULE_TYPE(Bitset128) permissions = {0};

OPSTATUS status = MODULE_FUNC(Bitset128, Clear)(&permissions);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Bitset128, Bit, Set)(&permissions, 7U);
}

bool hasPermission = MODULE_FUNC(Bitset128, Bit, Test)(&permissions, 7U);
size_t setCount = MODULE_FUNC(Bitset128, Count)(&permissions);
~~~

`Bitset.h` deshabilita deliberadamente la function table de objeto, por lo que sus métodos generados deben llamarse directamente con `MODULE_FUNC(Bitset128, ...)`; `OBJECT_CALL` no está disponible para estos tipos. Las operaciones que modifican el estado devuelven `OPSTATUS`. La forma comprobada `Bit.Check` escribe un Boolean en un parámetro de salida; la forma práctica `Bit.Test` devuelve `bool` directamente y evalúa false para un objeto inválido o un índice fuera de rango. Los métodos `Find` devuelven `NOT_FOUND` cuando no existe un bit coincidente.

Las operaciones de rango utilizan un intervalo semiabierto `[startBit, endBit)`: el inicio se incluye y el final se excluye. El extremo final puede ser igual a la capacidad del bitset.

---

# Contratos y configuración

- Las funciones escalares `Bit` operan sobre tipos enteros sin signo; no pases un tipo con signo suponiendo que tendrá semántica de desplazamiento idéntica.
- Todos los índices de bit comienzan en cero.
- Los extremos de rango para extracción/inserción de máscaras escalares son inclusivos; los métodos de rango de Bitset utilizan intervalos semiabiertos.
- La capacidad de `Bitset` es fija según el sufijo del tipo; no crece en tiempo de ejecución.
- Los punteros inválidos y los índices fuera de rango se informan mediante operaciones que devuelven estado.
- `Bit.h` admite configuración de function tables mediante switches globales y específicos del paquete. `Bitset.h` deshabilita incondicionalmente su function table en la implementación actual, por lo que sus métodos siempre utilizan llamadas directas `MODULE_FUNC`.

~~~c
#define BIT_DISABLE_FUNCTION_TABLE 1
#include "Cosmeron/Core/Bit/Bit.h"
#include "Cosmeron/Core/Bit/Bitset.h"
~~~

## Módulos relacionados

- [Algorithm](Algorithm.md) — auxiliares de comparación y repetición.
- [Memory](Memory.md) — asignación en tiempo de ejecución y funciones relacionadas con bitsets.
- [Types](Types.md) — catálogo de tipos primitivos y operaciones con limbs.

## Encabezados de origen

- `Codespace/Cosmeron/Core/Bit/Bit.def`
- `Codespace/Cosmeron/Core/Bit/Bitset.def`
- `Codespace/Cosmeron/Core/Types/Impl/Common.inc`
