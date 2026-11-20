# Math

O módulo **Math** fornece aritmética entera comprobada, aritmética limitada y saturada, auxiliares de rangos de valores, extremos y solucionadores de ecuaciones lineales/cuadráticas de valores reales. Sus declaraciones son templates C11 instanciados para los sufijos de tipo enumerados en las tablas de tipos del módulo.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Math/Math.h"
~~~

O cabeçalho agregador inclui los pacotes actuales:

| Pacote | Encabezado público | Finalidade |
| --- | --- | --- |
| Arithmetic | `Arithmetic/Basic.h` | Operaciones inteiras con y sem sinal comprobadas. |
| ClampedArithmetic | `Arithmetic/Clamped.h` | Aritmética entera limitada y saturada. |
| Between | `Value/Between.h` | Classificação y pertenencia a intervalos. |
| Clamp | `Value/Clamp.h` | Limita un valor a un intervalo. |
| MaxAndMin | `Value/MaxAndMin.h` | Mínimo y máximo binarios. |
| LinearEquation | `Equation/Linear.h` | Resuelve una equação lineal. |
| QuadraticEquation | `Equation/Quadratic.h` | Discriminante, auxiliar de raiz cuadrada y resolución cuadrática. |

---

# Visão geral

## Especializaciones de tipos

Os sufixos públicos forman parte de los nombres generados. Utilize el sufijo exacto em vez de suponer un alias C.

| Sufijo | Tipo C | Pacotes disponibles |
| --- | --- | --- |
| `Bool` | `bool` | Value |
| `Int8`, `Int16`, `Int32`, `Int64` | Entero com sinal de ancho fijo | Arithmetic, ClampedArithmetic, Value |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | Entero sem sinal de ancho fijo | Arithmetic, ClampedArithmetic, Value |
| `Char` | `char` | Value |
| `Float32` | `float` | Value, Equation |
| `Float64` | `double` | Value, Equation |
| `Float128` | `long double` | Value, Equation |
| `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr` | Tipos C de tamaño dependiente de la arquitectura | Value |

`Float128` es el sufijo para `long double` de C. No garantiza que todas las plataformas utilicen una representación real de 128 bits.

---

# Referência da API

## Arithmetic

Cabeçalho: `Arithmetic/Basic.h`.

Los pacotes con y sem sinal se nombran `MODULE_FUNC(Math, Arithmetic, <Suffix>, <Operation>)`.

| Função | Firma | Comportamento |
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

Todas as operações requieren un ponteiro de salida no nulo. El overflow y la aritmética inválida se informan mediante `OPSTATUS`; la salida solo é escrito cuando la operación tiene éxito. La división o el resto por cero son inválidos. El mínimo inteiro com sinal dividido por -1 se informa como fora da faixa.

## ClampedArithmetic

Cabeçalho: `Arithmetic/Clamped.h`.

Cada especialização con o sem sinal ofrece dos grupos:

| Variante | Métodos | Parámetros | Resultado |
| --- | --- | --- | --- |
| `Clamp` | `Add`, `Sub`, `Mul` | `a, b, minimum, maximum` | Retorna el resultado aritmético restringido al intervalo indicado. |
| `Saturating` | `Add`, `Sub`, `Mul` | `a, b` | Retorna um valor saturado a los limites representables del tipo. |

~~~c
int32_t bounded = MODULE_FUNC(Math, ClampedArithmetic, Int32, Clamp, Add)(
    INT32_MAX, 10, -100, 100);

int32_t saturated = MODULE_FUNC(Math, ClampedArithmetic, Int32, Saturating, Add)(
    INT32_MAX, 10);
~~~

A variante Clamp delega en aritmética comprobada. Si la operación intermedia desborda, elige el limite correspondiente; de lo contrario, limita el valor calculado. A variante Saturating utilize el mínimo/máximo integrado del tipo inteiro seleccionado. Estas funções retornam un valor diretamente, no un `OPSTATUS`.

## Between

Cabeçalho: `Value/Between.h`.

| Função | Descrição |
| --- | --- |
| `Classify(value, minimum, maximum)` | Retorna un `MODULE_ENUM(Math, RangeResult)` que indica abaixo, dentro, acima o intervalo inválido. |
| `Is(value, minimum, maximum)` | Retorna true solo cuando el valor está dentro del intervalo inclusivo. |

Os valores de `RangeResult` son `IN_RANGE`, `BELOW_RANGE`, `ABOVE_RANGE` y `INVALID_RANGE`. El intervalo inclui ambos extremos. Si `minimum > maximum`, `Classify` retorna `INVALID_RANGE` e `Is` retorna false.

~~~c
MODULE_ENUM(Math, RangeResult) position =
    MODULE_FUNC(Math, Between, Int32, Classify)(value, 10, 20);

bool accepted = MODULE_FUNC(Math, Between, Int32, Is)(value, 10, 20);
~~~

## Clamp

Cabeçalho: `Value/Clamp.h`.

`Apply(value, minimum, maximum)` retorna el valor restringido al intervalo inclusivo. Si los extremos están invertidos, la implementação los intercambia antes de limitar.

~~~c
int32_t bounded = MODULE_FUNC(Math, Clamp, Int32, Apply)(42, 0, 10);
/* bounded is 10 */
~~~

Es un auxiliar que retorna un valor y no informa de un intervalo inválido; los limites invertidos se normalizan.

## MaxAndMin

Cabeçalho: `Value/MaxAndMin.h`.

O pacote solo proporciona las funções binarias `Max(a, b)` y `Min(a, b)` para cada especialización de tipo ordenado. Retornan directamente el valor seleccionado.

~~~c
int32_t maximum = MODULE_FUNC(Math, MaxAndMin, Int32, Max)(left, right);
int32_t minimum = MODULE_FUNC(Math, MaxAndMin, Int32, Min)(left, right);
~~~

La implementação actual no contiene APIs variádicas `Smallest` o `Biggest`.

## LinearEquation

Cabeçalho: `Equation/Linear.h`.

Resuelve la equação:

\[
a x + b = 0
\]

`Solve(a, b, outResult, outSolution)` retorna `OPSTATUS` y escribe la clasificación de la solución en `MODULE_ENUM(Math, Solution)`.

| Condição | Classificação | Resultado |
| --- | --- | --- |
| `a != 0` | `ONE` | `x = -b / a` |
| `a == 0` y `b == 0` | `INFINITE` | No hay raiz única; el valor de salida se establece en cero. |
| `a == 0` y `b != 0` | `NONE` | No hay raiz; el valor de salida se establece en cero. |

~~~c
double root = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, LinearEquation, Float64, Solve)(
    2.0, -8.0, &root, &count);
~~~

Ambos os ponteiros de salida son obligatorios. Una llamada exitosa indica que la equação fue clasificada, pero no implica que exista una raiz única; inspecione `count`.

## QuadraticEquation

Cabeçalho: `Equation/Quadratic.h`.

O pacote expone `Sqrt(x)`, `Discriminant(a, b, c)` y `Solve(a, b, c, outResult1, outResult2, outSolution)`.

Para:

\[
a x^2 + b x + c = 0
\]

el discriminante es:

\[
\Delta = b^2 - 4ac
\]

| Condição | Classificação |
| --- | --- |
| `a == 0` | Delega en el solucionador lineal. |
| `Δ < 0` | `COMPLEX`; el solucionador real no retorna raízes complejas. |
| `Δ == 0` | `ONE`; ambas salidas reciben la raiz repetida. |
| `Δ > 0` | `TWO`; las salidas reciben las dos raízes reales. |

~~~c
double x1 = 0.0;
double x2 = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, QuadraticEquation, Float64, Solve)(
    1.0, -3.0, 2.0, &x1, &x2, &count);
~~~

Los ponteiros de salida y el ponteiro de solución son obligatorios. A API Quadratic actual clasifica un discriminante negativo como complejo, pero no expone un objeto de resultado de raízes complejas ni un solucionador complejo separado. `Sqrt` utilize un cálculo iterativo limitado; utilize entradas finitas normales y considera la precisión de ponto flutuante, el overflow y el underflow.

---

# Contratos y manejo de errores

- As operações `Arithmetic` retornam `OPSTATUS` y escriben mediante un ponteiro de salida solo cuando tienen éxito.
- `ClampedArithmetic`, `Clamp`, `Between.Is` y `MaxAndMin` retornam valores o clasificaciones booleanas directamente.
- `Between.Classify` es el único auxiliar de intervalo que distingue un intervalo inválido de un valor fora do intervalo ordinario.
- `LinearEquation.Solve` y `QuadraticEquation.Solve` pueden tener éxito e informar `NONE`, `INFINITE` o `COMPLEX`. Inspecciona sempre el enum de solución.
- El enum de solución es `MODULE_ENUM(Math, Solution)`; la clasificación de intervalo utilize `MODULE_ENUM(Math, RangeResult)`.
- As operações inteiras están limitadas por su tipo C seleccionado. As operações de ponto flutuante heredan la precisión y el comportamento de valores excepcionais de C.

## Implementación y portabilidadee

Los templates y las tablas de instanciación están en los encabezados de cada pacote y en `Impl/Common.inc`. Incluye el encabezado público, no `Impl/*.impl` ni `.def`, desde el código consumidor. O cabeçalho agregador `Math.h` no depende de una biblioteca matemática separada para su implementação actual de ecuaciones.

## Módulos relacionados

- [Numeric](Numeric.md) — valores de punto fijo, racionales y de múltiples limbs.
- [Algorithm](Algorithm.md) — comparación y repetición genérica.
- [Types](Types.md) — nombres de tipos primitivos y traits.
- [Module](Module.md) — declaraciones de templates y acceso a funções con namespace.

## Cabeçalhos de origen

- `Codespace/Cosmeron/Modules/Math/Math.h`
- `Codespace/Cosmeron/Modules/Math/Impl/Common.inc`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Basic.def`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Clamped.def`
- `Codespace/Cosmeron/Modules/Math/Value/Between.def`
- `Codespace/Cosmeron/Modules/Math/Value/Clamp.def`
- `Codespace/Cosmeron/Modules/Math/Value/MaxAndMin.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Linear.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Quadratic.def`
