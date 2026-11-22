# Algoritmos (Algorithm)

El módulo **Algorithm** proporciona primitivas reutilizables de comparación y repetición. Su API pública se divide en los paquetes `Comparison` y `Repeat`; hay templates de repetición tipada para tipos de elementos definidos por el usuario.

## Encabezados

~~~c
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Algorithm/Repeat.h"
~~~

---

# Visión general

## Comparison

Una comparación devuelve `CMPOUT`, un resultado de tres valores:

| Constante | Valor | Significado |
| --- | ---: | --- |
| `COMPARISON_CONST(LOWER)` | -1 | El operando izquierdo precede al derecho. |
| `COMPARISON_CONST(EQUAL)` | 0 | Los operandos son iguales al compararlos. |
| `COMPARISON_CONST(HIGHER)` | 1 | El operando izquierdo sigue al derecho. |

Utiliza este resultado en lugar de depender de cualquier entero negativo o positivo. `COMPARISON_CONST` proporciona las constantes con namespace y `MODULE_ENUM(Algorithm, Comparison, Result)` nombra el tipo enum.

El paquete también expone `ToString`, comparaciones de tipos primitivos ordenados, `CString` y `Bytes`. Cuando el compilador lo admite, la implementación añade comparaciones de números complejos.

## Repeat

El paquete `Repeat` ejecuta un delegate una cantidad fija de veces, opcionalmente pasando un índice que comienza en cero. Admite procedimientos, operaciones que pueden fallar y predicados, así como patrones de bytes repetidos, valores primitivos y punteros a cadenas C.

El template tipado añade generación de valores para un tipo C elegido. Es útil cuando el valor repetido debe producirse mediante un delegate en lugar de copiar una constante.

---

# Referencia de la API

## Paquete Comparison

Encabezado: `Cosmeron/Core/Algorithm/Comparison.h`.

| Función | Contrato | Resultado |
| --- | --- | --- |
| `ToString` | Convierte un valor `CMPOUT` en su descripción simbólica. | `const char *` |
| Funciones de tipos primitivos | Comparan los valores primitivos correspondientes. Los nombres proceden de `PRIMITIVE_TYPE_TABLE`. | `CMPOUT` |
| `CString` | Compara dos cadenas terminadas en nulo. | `CMPOUT` |
| `Bytes` | Compara lexicográficamente dos tramos de bytes y utiliza sus longitudes para resolver empates de prefijo común. | `CMPOUT` |

La tabla primitiva incluye tipos booleanos, enteros con y sin signo, caracteres, coma flotante y tipos de tamaño dependiente de la arquitectura. La implementación también proporciona comparaciones de coma flotante compleja en compiladores distintos de MSVC. Los sufijos de las funciones de tipos primitivos siguen los nombres exactos de `PRIMITIVE_TYPE_TABLE` (por ejemplo, `Int32`), no abreviaturas.

### Comparación primitiva

Utiliza `MODULE_FUNC(Algorithm, Comparison, <PrimitiveName>)` con el tipo C correspondiente. Por ejemplo:

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, Int32)(left, right);
if (order == COMPARISON_CONST(LOWER)) {
    /* left is smaller */
}
~~~

### CString

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, CString)(left, right);
~~~

Un puntero nulo se ordena antes que uno no nulo. Dos punteros idénticos se comparan como iguales. Las cadenas no nulas se comparan según el orden de cadenas C proporcionado por la implementación.

### Bytes

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, Bytes)(
    leftData, leftSize, rightData, rightSize);
~~~

Primero se compara el prefijo común; si los bytes son iguales, el tramo más corto se ordena primero. Un tramo nulo se ordena antes que uno no nulo. No pases rangos de memoria inválidos.

## Paquete Repeat

Encabezado: `Cosmeron/Core/Algorithm/Repeat.h`.

| Función | Descripción |
| --- | --- |
| `Procedure` | Llama a un procedimiento sin argumentos `count` veces. |
| `Procedure.Indexed` | Llama a un procedimiento con índices de cero a `count - 1`. |
| `Operation` | Llama a una operación que puede fallar hasta que falle o terminen las repeticiones. |
| `Operation.Indexed` | Igual que `Operation`, pasando el índice actual. |
| `Predicate` | Devuelve si todas las llamadas repetidas al predicado tienen éxito. |
| `Predicate.Indexed` | Igual que `Predicate`, pasando el índice actual. |
| `Bytes` | Copia un patrón de bytes en posiciones consecutivas de un búfer de destino. |
| Funciones de tipos primitivos | Rellena un array con un valor primitivo; los nombres siguen `PRIMITIVE_TYPE_TABLE`. |
| `CString` | Rellena un array de posiciones `const char *` con el mismo puntero. |

### Firmas de delegates

Los tipos invocables son delegates generados por el paquete:

| Delegate | Firma | Uso |
| --- | --- | --- |
| `Procedure` | `void (void)` | Efecto secundario sin resultado. |
| `ProcedureIndexed` | `void (size_t index)` | Efecto secundario que depende de la iteración. |
| `Operation` | `OPSTATUS (void)` | Trabajo que puede fallar. |
| `OperationIndexed` | `OPSTATUS (size_t index)` | Trabajo indexado que puede fallar. |
| `Predicate` | `bool (void)` | Condición que debe seguir siendo verdadera. |
| `PredicateIndexed` | `bool (size_t index)` | Condición indexada. |

Un delegate almacena una función y un puntero de contexto opcional. `DELEGATE_IS_VALID` comprueba si la función está presente y `DELEGATE_CALL` la invoca. Utiliza los auxiliares de enlace o promoción generados para el tipo de delegate declarado en el paquete.

### Repetición que puede fallar

~~~c
OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Operation)(count, operation);
~~~

Para un recuento distinto de cero, un delegate de operación inválido devuelve `INVALID_ARGUMENT`. El primer estado distinto de `SUCCESS` se devuelve inmediatamente; las iteraciones posteriores no se ejecutan. Un recuento cero tiene éxito sin invocar el delegate.

### Repetición de bytes

~~~c
unsigned char pattern[2] = {0xAA, 0x55};
unsigned char output[6];

OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Bytes)(
    output, pattern, sizeof(pattern), 3U);
~~~

El búfer resultante contiene tres copias del patrón de dos bytes. Un recuento cero o un tamaño de patrón cero tiene éxito sin escribir. Para trabajo distinto de cero, ambos punteros deben ser válidos, los rangos de origen y destino no deben solaparse y el tamaño total del destino debe ser representable.

### Rellenar arrays primitivos

~~~c
int32_t values[4];

OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Int32)(
    values, 42, 4U);
~~~

La operación escribe el valor proporcionado en cada uno de los `count` elementos. Un recuento cero tiene éxito sin exigir un puntero de destino. Las escrituras no nulas requieren un destino válido y rechazan el desbordamiento de la multiplicación del número de elementos.

### Rellenar arrays de punteros a cadenas C

~~~c
const char *labels[3];

OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, CString)(
    labels, "ready", 3U);
~~~

Solo se copian los punteros; la cadena apuntada no se duplica y sigue perteneciendo a su almacenamiento original.

## Template Repeat tipado

`Repeat.def` define `TEMPLATE_Algorithm_Repeat(SUFFIX, TYPE)`. Genera una especialización de paquete con:

| Función | Descripción |
| --- | --- |
| `Fill` | Rellena un array tipado con un valor. |
| `Generate` | Rellena un array invocando un generador de valores sin argumentos. |
| `Generate.Indexed` | Rellena un array invocando un generador con el índice del elemento. |

Las firmas de los delegates generados son `TYPE (void)` y `TYPE (size_t index)`. Cada generador debe ser válido cuando el recuento sea distinto de cero. Las operaciones devuelven `OPSTATUS` y comprueban la validez del destino y la aritmética de tamaños antes de escribir.

Instancia el template mediante el motor común:

~~~c
TEMPLATE_INSTANTIATE(Algorithm_Repeat, Point, Point);
~~~

Los nombres del tipo público y de las funciones de la especialización generada utilizan el sufijo proporcionado. Define el tipo del template y las implementaciones de delegates antes de utilizar el paquete generado.

---

# Contratos y comportamiento

- Las funciones `OPERATION` devuelven `OPSTATUS`; comprueba `STATUS_CONST(SUCCESS)` antes de asumir que se escribieron los búferes de salida.
- Las funciones `PROCEDURE` no devuelven estado. Un delegate de procedimiento inválido se trata como una operación sin efecto.
- Las funciones `PREDICATE` devuelven `bool`; un recuento de repetición cero es verdadero por vacuidad.
- Los delegates indexados reciben índices que comienzan en cero.
- La repetición es síncrona. El paquete no crea hilos ni programa trabajo en segundo plano.
- `CString` repite valores de puntero, no el contenido de las cadenas.
- `Bytes` rechaza tramos de origen y destino solapados; no es una operación general de movimiento con solapamiento.

## Despacho

Las declaraciones actuales de `Comparison.h` y `Repeat.h` deshabilitan las function tables. Llama directamente a las funciones del paquete mediante `MODULE_FUNC`; no utilices `PACKAGE_API` para estos paquetes. Es una propiedad de compilación de estos encabezados, no una opción de ejecución.

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, Int32)(left, right);
OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Operation)(count, operation);
~~~

`MODULE_FUNC` mantiene la llamada vinculada al símbolo público generado y evita exigir una function table del paquete.

## Módulos relacionados

- [Bit](Bit.md) — primitivas de bits y bitsets.
- [Memory](Memory.md) — operaciones de bytes, intercambio y asignación.
- [Module](Module.md) — declaraciones de paquetes, callables, delegates e invocación.
- [Preprocessor](Preprocessor.md) — mecanismos de templates y macros.

## Encabezados de origen

- `Codespace/Cosmeron/Core/Algorithm/Comparison.h`
- `Codespace/Cosmeron/Core/Algorithm/Repeat.h`
- `Codespace/Cosmeron/Core/Algorithm/Repeat.def`
- `Codespace/Cosmeron/Core/Algorithm/Impl/Comparison.impl`
- `Codespace/Cosmeron/Core/Algorithm/Impl/Repeat.impl`
