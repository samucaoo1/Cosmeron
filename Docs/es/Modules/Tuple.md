# Tuple

El módulo **Tuple** proporciona estructuras POD tipadas y de disposición fija para agrupar de dos a cinco valores. Incluye tanto tipos de tupla posicionales como alias semánticos de dos valores, con funciones generadas para empaquetar y desempaquetar.

## Encabezados

~~~c
#include "Cosmeron/Modules/Tuple/Tuple.h"
~~~

El encabezado agregado incluye los templates `Pair`, `Dual`, `Triple`, `Quad` y `Penta`.

---

# Visión general

| Tipo | Aridad | Uso previsto |
| --- | ---: | --- |
| `Pair<T>` | 2 | Par ordenado genérico. |
| `Dual<T>` | 2 | Dos valores relacionados con alias específicos del dominio, como coordenada, tamaño, intervalo o componentes complejos. |
| `Triple<T>` | 3 | Tres valores relacionados, como una coordenada 3D o canales RGB. |
| `Quad<T>` | 4 | Cuatro valores relacionados, como un rectángulo o canales RGBA. |
| `Penta<T>` | 5 | Cinco valores ordenados. |

Cada especialización es un `STRUCT` con almacenamiento C ordinario. No tiene propiedad de heap, asignación oculta ni receptor de objeto. Se admiten la inicialización directa de estructuras y la asignación.

## Tipos de elementos integrados

`TUPLE_TYPE_TABLE` en `Modules/Tuple/Impl/Common.inc` proporciona estos sufijos de elementos:

- Boolean;
- enteros con signo y sin signo: Int8/16/32/64 y UInt8/16/32/64;
- anchos de caracteres: Char, Char8, Char16 y Char32;
- punto flotante: Float32, Float64, Float, Double y LongDouble;
- tipos enteros/tamaño de C: Int, UInt, Size, PtrDiff, IntPtr y UIntPtr;
- FloatComplex, DoubleComplex y LongDoubleComplex cuando el compilador no es MSVC.

Por ejemplo, `MODULE_TYPED(Tuple, Pair, Int32)` nombra un par de valores `int32_t`.

---

# Referencia de la API

Cada template de tupla proporciona las mismas dos funciones de paquete:

| Función | Contrato |
| --- | --- |
| `Pack(value1, value2, ...)` | Devuelve una tupla inicializada con los valores proporcionados. |
| `Unpack(tuple, out1, out2, ...)` | Copia los elementos de la tupla a punteros de salida del llamador y devuelve `OPSTATUS`. |

La aridad determina la cantidad de argumentos. `Unpack` rechaza un puntero de salida nulo con `INVALID_ARGUMENT`.

## Pair

Los campos de `Pair` incluyen los nombres canónicos `value1`/`first` y `value2`/`second`, los alias `a`/`b` y `x1`/`x2`, además de los alias de array `value[2]` y `values[2]`.

~~~c
MODULE_TYPED(Tuple, Pair, Int32) pair =
    MODULE_FUNC(Tuple, Pair, Int32, Pack)(10, 20);

int32_t first = 0;
int32_t second = 0;
OPSTATUS status = MODULE_FUNC(Tuple, Pair, Int32, Unpack)(
    pair, &first, &second);
~~~

## Dual

`Dual` es un POD de dos valores cuyos alias del primer campo son `value1`, `a`, `x1`, `x`, `col`, `length`, `real`, `state1`, `begin`, `first` y `start`. Los alias del segundo campo son `value2`, `b`, `x2`, `y`, `row`, `width`, `imaginary`, `state2`, `end`, `second` y `last`.

~~~c
MODULE_TYPED(Tuple, Dual, UInt16) dimensions =
    MODULE_FUNC(Tuple, Dual, UInt16, Pack)(80U, 25U);

uint16_t columns = dimensions.col;
uint16_t rows = dimensions.row;
~~~

Utiliza el alias que exprese el significado de los valores en el código consumidor. Como son alias de union, escribe una interpretación semántica y léela mediante el alias correspondiente; no trates los alias como almacenamiento independiente.

## Triple, Quad y Penta

- `Triple` expone `first`/`second`/`third`, `a`/`b`/`c`, `x`/`y`/`z`, `length`/`width`/`height`, `red`/`green`/`blue` y los alias `value[3]`/`values[3]`.
- `Quad` expone `first` a `fourth`, alias posicionales, `value[4]`/`values[4]`, alias geométricos `left`, `right`, `top`, `bottom` y alias de color `red`, `green`, `blue`, `alpha`.
- `Penta` expone `first` a `fifth`, `value[5]` y `values[5]`.

Por ejemplo, para un valor similar a un rectángulo, `MODULE_FUNC(Tuple, Quad, UInt16, Pack)(left, right, top, bottom)` devuelve un `Quad` cuyos campos pueden leerse mediante esos alias semánticos.

Los alias exactos están pensados para facilitar la nomenclatura, pero los valores siguen ocupando una única representación POD de tupla compartida.

---

# Uso de templates

Los encabezados de tupla instancian automáticamente la tabla de tipos integrada actual. Las definiciones públicas de los templates están disponibles en los archivos `.def` correspondientes:

- `Pair.def`
- `Dual.def`
- `Triple.def`
- `Quad.def`
- `Penta.def`

El motor común de templates puede utilizarse para instanciar otro tipo de elemento compatible. El sufijo del tipo generado y el tipo de elemento C deben proporcionarse en el mismo orden esperado por la macro correspondiente `TEMPLATE_Tuple_<Name>(SUFFIX, TYPE)`.

---

# Contratos y limitaciones

- Los valores Tuple se almacenan por valor y no asignan memoria.
- `Pack` devuelve la tupla completamente inicializada por valor.
- `Unpack` escribe cada elemento en un puntero de salida distinto, propiedad del llamador.
- El tipo solo es tan seguro de copiar como su tipo de elemento. Las especializaciones integradas utilizan tipos escalares C; un tipo de elemento personalizado con propiedad o invariantes autorreferenciales no debe tratarse como trivialmente copiable sin comprobar su contrato.
- Los tipos Tuple no proporcionan longitud dinámica, iteración, comprobaciones de límites ni elementos heterogéneos.
- Utiliza una estructura dedicada cuando los elementos tengan tipos C diferentes o necesiten invariantes específicos del dominio.

## Módulos relacionados

- [Types](Types.md) — nombres canónicos de tipos primitivos.
- [Module](Module.md) — estructuras POD generadas y motor de templates.

## Encabezados de origen

- `Codespace/Cosmeron/Modules/Tuple/Tuple.h`
- `Codespace/Cosmeron/Modules/Tuple/Pair.def`
- `Codespace/Cosmeron/Modules/Tuple/Dual.def`
- `Codespace/Cosmeron/Modules/Tuple/Triple.def`
- `Codespace/Cosmeron/Modules/Tuple/Quad.def`
- `Codespace/Cosmeron/Modules/Tuple/Penta.def`
