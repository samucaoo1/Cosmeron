# Math

El módulo **Math** proporciona aritmética entera comprobada, aritmética limitada y saturada, auxiliares de rangos de valores, extremos y solucionadores de ecuaciones lineales/cuadráticas de valores reales. Sus declaraciones son templates C11 instanciados para los sufijos de tipo enumerados en las tablas de tipos del módulo.

## Encabezados

~~~c
#include "Cosmeron/Modules/Math/Math.h"
~~~

El encabezado agregado incluye los paquetes actuales:

| Paquete | Encabezado público | Propósito |
| --- | --- | --- |
| Arithmetic | `Arithmetic/Basic.h` | Operaciones enteras con y sin signo comprobadas. |
| ClampedArithmetic | `Arithmetic/Clamped.h` | Aritmética entera limitada y saturada. |
| Between | `Value/Between.h` | Clasificación y pertenencia a intervalos. |
| Clamp | `Value/Clamp.h` | Limita un valor a un intervalo. |
| MaxAndMin | `Value/MaxAndMin.h` | Mínimo y máximo binarios. |
| LinearEquation | `Equation/Linear.h` | Resuelve una ecuación lineal. |
| QuadraticEquation | `Equation/Quadratic.h` | Discriminante, auxiliar de raíz cuadrada y resolución cuadrática. |

---

# Visión general

## Especializaciones de tipos

Los sufijos públicos forman parte de los nombres generados. Utiliza el sufijo exacto en lugar de suponer un alias C.

| Sufijo | Tipo C | Paquetes disponibles |
| --- | --- | --- |
| `Bool` | `bool` | Value |
| `Int8`, `Int16`, `Int32`, `Int64` | Entero con signo de ancho fijo | Arithmetic, ClampedArithmetic, Value |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | Entero sin signo de ancho fijo | Arithmetic, ClampedArithmetic, Value |
| `Char` | `char` | Value |
| `Float32` | `float` | Value, Equation |
| `Float64` | `double` | Value, Equation |
| `Float128` | `long double` | Value, Equation |
| `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr` | Tipos C de tamaño dependiente de la arquitectura | Value |

`Float128` es el sufijo para `long double` de C. No garantiza que todas las plataformas utilicen una representación real de 128 bits.

---

# Referencia de la API

## Arithmetic

Encabezado: `Arithmetic/Basic.h`.

Los paquetes con y sin signo se nombran `MODULE_FUNC(Math, Arithmetic, <Suffix>, <Operation>)`.

| Función | Firma | Comportamiento |
| --- | --- | --- |
| `Add` | `(TYPE a, TYPE b, TYPE *outResult)` | Suma comprobada. |
| `Sub` | `(TYPE a, TYPE b, TYPE *outResult)` | Resta comprobada. |
| `Mul` | `(TYPE a, TYPE b, TYPE *outResult)` | Multiplicación comprobada. |
| `Div` | `(TYPE a, TYPE b, TYPE *outResult)` | División comprobada. |
| `Mod` | `(TYPE a, TYPE b, TYPE *outResult)` | Resto comprobado. |

~~~c
int32_t sum = 0;
OPSTATUS status = MODULE_FUNC(Math, Arithmetic, Int32, Add)(20, 22, &sum);

if (status == STATUS_CONST(SUCCESS)) {
    /* sum is 42 */
}
~~~

Todas las operaciones requieren un puntero de salida no nulo. El overflow y la aritmética inválida se informan mediante `OPSTATUS`; la salida solo se escribe cuando la operación tiene éxito. La división o el resto por cero son inválidos. El mínimo entero con signo dividido por -1 se informa como fuera de rango.

## ClampedArithmetic

Encabezado: `Arithmetic/Clamped.h`.

Cada especialización con o sin signo ofrece dos grupos:

| Variante | Métodos | Parámetros | Resultado |
| --- | --- | --- | --- |
| `Clamp` | `Add`, `Sub`, `Mul` | `a, b, minimum, maximum` | Devuelve el resultado aritmético restringido al intervalo indicado. |
| `Saturating` | `Add`, `Sub`, `Mul` | `a, b` | Devuelve un valor saturado a los límites representables del tipo. |

~~~c
int32_t bounded = MODULE_FUNC(Math, ClampedArithmetic, Int32, Clamp, Add)(
    INT32_MAX, 10, -100, 100);

int32_t saturated = MODULE_FUNC(Math, ClampedArithmetic, Int32, Saturating, Add)(
    INT32_MAX, 10);
~~~

La variante Clamp delega en aritmética comprobada. Si la operación intermedia desborda, elige el límite correspondiente; de lo contrario, limita el valor calculado. La variante Saturating utiliza el mínimo/máximo integrado del tipo entero seleccionado. Estas funciones devuelven un valor directamente, no un `OPSTATUS`.

## Between

Encabezado: `Value/Between.h`.

| Función | Descripción |
| --- | --- |
| `Classify(value, minimum, maximum)` | Devuelve un `MODULE_ENUM(Math, RangeResult)` que indica por debajo, dentro, por encima o intervalo inválido. |
| `Is(value, minimum, maximum)` | Devuelve true solo cuando el valor está dentro del intervalo inclusivo. |

Los valores de `RangeResult` son `IN_RANGE`, `BELOW_RANGE`, `ABOVE_RANGE` y `INVALID_RANGE`. El intervalo incluye ambos extremos. Si `minimum > maximum`, `Classify` devuelve `INVALID_RANGE` e `Is` devuelve false.

~~~c
MODULE_ENUM(Math, RangeResult) position =
    MODULE_FUNC(Math, Between, Int32, Classify)(value, 10, 20);

bool accepted = MODULE_FUNC(Math, Between, Int32, Is)(value, 10, 20);
~~~

## Clamp

Encabezado: `Value/Clamp.h`.

`Apply(value, minimum, maximum)` devuelve el valor restringido al intervalo inclusivo. Si los extremos están invertidos, la implementación los intercambia antes de limitar.

~~~c
int32_t bounded = MODULE_FUNC(Math, Clamp, Int32, Apply)(42, 0, 10);
/* bounded is 10 */
~~~

Es un auxiliar que devuelve un valor y no informa de un intervalo inválido; los límites invertidos se normalizan.

## MaxAndMin

Encabezado: `Value/MaxAndMin.h`.

El paquete solo proporciona las funciones binarias `Max(a, b)` y `Min(a, b)` para cada especialización de tipo ordenado. Devuelven directamente el valor seleccionado.

~~~c
int32_t maximum = MODULE_FUNC(Math, MaxAndMin, Int32, Max)(left, right);
int32_t minimum = MODULE_FUNC(Math, MaxAndMin, Int32, Min)(left, right);
~~~

La implementación actual no contiene APIs variádicas `Smallest` o `Biggest`.

## LinearEquation

Encabezado: `Equation/Linear.h`.

Resuelve la ecuación:

\[
a x + b = 0
\]

`Solve(a, b, outResult, outSolution)` devuelve `OPSTATUS` y escribe la clasificación de la solución en `MODULE_ENUM(Math, Solution)`.

| Condición | Clasificación | Resultado |
| --- | --- | --- |
| `a != 0` | `ONE` | `x = -b / a` |
| `a == 0` y `b == 0` | `INFINITE` | No hay raíz única; el valor de salida se establece en cero. |
| `a == 0` y `b != 0` | `NONE` | No hay raíz; el valor de salida se establece en cero. |

~~~c
double root = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, LinearEquation, Float64, Solve)(
    2.0, -8.0, &root, &count);
~~~

Ambos punteros de salida son obligatorios. Una llamada exitosa indica que la ecuación fue clasificada, pero no implica que exista una raíz única; inspecciona `count`.

## QuadraticEquation

Encabezado: `Equation/Quadratic.h`.

El paquete expone `Sqrt(x)`, `Discriminant(a, b, c)` y `Solve(a, b, c, outResult1, outResult2, outSolution)`.

Para:

\[
a x^2 + b x + c = 0
\]

el discriminante es:

\[
\Delta = b^2 - 4ac
\]

| Condición | Clasificación |
| --- | --- |
| `a == 0` | Delega en el solucionador lineal. |
| `Δ < 0` | `COMPLEX`; el solucionador real no devuelve raíces complejas. |
| `Δ == 0` | `ONE`; ambas salidas reciben la raíz repetida. |
| `Δ > 0` | `TWO`; las salidas reciben las dos raíces reales. |

~~~c
double x1 = 0.0;
double x2 = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, QuadraticEquation, Float64, Solve)(
    1.0, -3.0, 2.0, &x1, &x2, &count);
~~~

Los punteros de salida y el puntero de solución son obligatorios. La API Quadratic actual clasifica un discriminante negativo como complejo, pero no expone un objeto de resultado de raíces complejas ni un solucionador complejo separado. `Sqrt` utiliza un cálculo iterativo acotado; utiliza entradas finitas normales y considera la precisión de punto flotante, el overflow y el underflow.

---

# Contratos y manejo de errores

- Las operaciones `Arithmetic` devuelven `OPSTATUS` y escriben mediante un puntero de salida solo cuando tienen éxito.
- `ClampedArithmetic`, `Clamp`, `Between.Is` y `MaxAndMin` devuelven valores o clasificaciones booleanas directamente.
- `Between.Classify` es el único auxiliar de intervalo que distingue un intervalo inválido de un valor fuera del intervalo ordinario.
- `LinearEquation.Solve` y `QuadraticEquation.Solve` pueden tener éxito e informar `NONE`, `INFINITE` o `COMPLEX`. Inspecciona siempre el enum de solución.
- El enum de solución es `MODULE_ENUM(Math, Solution)`; la clasificación de intervalo utiliza `MODULE_ENUM(Math, RangeResult)`.
- Las operaciones enteras están limitadas por su tipo C seleccionado. Las operaciones de punto flotante heredan la precisión y el comportamiento de valores excepcionales de C.

## Implementación y portabilidad

Los templates y las tablas de instanciación están en los encabezados de cada paquete y en `Impl/Common.inc`. Incluye el encabezado público, no `Impl/*.impl` ni `.def`, desde el código consumidor. El encabezado agregado `Math.h` no depende de una biblioteca matemática separada para su implementación actual de ecuaciones.

## Módulos relacionados

- [Numeric](Numeric.md) — valores de punto fijo, racionales y de múltiples limbs.
- [Algorithm](Algorithm.md) — comparación y repetición genérica.
- [Types](Types.md) — nombres de tipos primitivos y traits.
- [Module](Module.md) — declaraciones de templates y acceso a funciones con namespace.

## Encabezados de origen

- `Codespace/Cosmeron/Modules/Math/Math.h`
- `Codespace/Cosmeron/Modules/Math/Impl/Common.inc`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Basic.def`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Clamped.def`
- `Codespace/Cosmeron/Modules/Math/Value/Between.def`
- `Codespace/Cosmeron/Modules/Math/Value/Clamp.def`
- `Codespace/Cosmeron/Modules/Math/Value/MaxAndMin.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Linear.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Quadratic.def`
