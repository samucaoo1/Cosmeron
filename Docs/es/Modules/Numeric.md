# Numeric

El módulo **Numeric** proporciona representaciones numéricas adicionales construidas a partir de tipos enteros y de punto flotante de C: fracciones racionales exactas, valores complejos, punto fijo e enteros/valores decimales de ancho fijo con múltiples limbs.

## Encabezados

~~~c
#include "Cosmeron/Modules/Numeric/Numeric.h"
~~~

El encabezado agregado incluye las declaraciones Rational, Complex, Fixed, Bigint y Decimal. También se pueden incluir encabezados individuales cuando solo se necesita una representación.

---

# Visión general

## Especializaciones disponibles

| Familia | Representación | Especializaciones integradas actuales |
| --- | --- | --- |
| Rational | Numerador y denominador de un tipo entero. | `Int32`, `Int64` |
| Complex | Componentes real e imaginaria de un tipo de punto flotante. | `Float32`, `Float64`, `Float128` |
| Fixed | Entero con signo sin escalar más una cantidad de bits fraccionarios fijada en compilación. | `Q8_8`, `Q16_16` |
| Bigint | Entero sin signo de ancho fijo y múltiples limbs. | 128, 256, 512, 1024 bits |
| Decimal | Valor de ancho fijo con múltiples limbs y la misma interfaz aritmética de enteros. | 128, 256, 512, 1024 bits |

Los sufijos se generan mediante las tablas de `Modules/Numeric/Impl/Common.inc`. Los objetos Bigint/Decimal con sufijo de ancho almacenan limbs de 64 bits; su ancho queda fijado por el tipo seleccionado.

---

# Referencia de la API

## Rational

Encabezado: `Rational.h`. Tipo: `MODULE_TYPED(Numeric, Rational, Int32)` o `MODULE_TYPED(Numeric, Rational, Int64)`.

Un racional se almacena como `numerator / denominator`. `Make` rechaza un denominador cero, normaliza el signo del denominador y reduce la fracción mediante el máximo común divisor.

| Función | Contrato |
| --- | --- |
| `Gcd(a, b)` | Devuelve el resultado auxiliar del máximo común divisor. |
| `Make(numerator, denominator, outValue)` | Construye un racional normalizado. |
| `Add(a, b, outValue)` | Suma dos fracciones. |
| `Sub(a, b, outValue)` | Resta dos fracciones. |
| `Mul(a, b, outValue)` | Multiplica dos fracciones. |
| `Div(a, b, outValue)` | Divide por la segunda fracción; su numerador no debe ser cero. |
| `Compare(a, b)` | Devuelve el orden `CMPOUT`. |
| `ToFloat64(value)` | Convierte la fracción a `double`. |

~~~c
MODULE_TYPED(Numeric, Rational, Int32) third, sixth, half;

OPSTATUS status = MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 3, &third);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 6, &sixth);
}
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Numeric, Rational, Int32, Add)(third, sixth, &half);
}

double result = MODULE_FUNC(Numeric, Rational, Int32, ToFloat64)(half);
~~~

Las operaciones Rational utilizan el tipo entero C subyacente seleccionado. No son operaciones racionales de precisión arbitraria; el llamador debe mantener los valores intermedios dentro del rango representable por ese tipo.

## Complex

Encabezado: `Complex.h`. Especializaciones: `Float32`, `Float64` y `Float128`.

Un valor complejo almacena los campos `real` e `imag` y representa `real + imag * i`.

| Función | Contrato |
| --- | --- |
| `Make(real, imag)` | Construye un valor complejo. |
| `Add(a, b)`, `Sub(a, b)`, `Mul(a, b)` | Devuelven el resultado aritmético. |
| `Magnitude.Squared(value)` | Devuelve `real² + imag²`. |
| `Magnitude(value)` | Calcula la magnitud. |
| `Div(a, b, outValue)` | Divide en un valor de salida; rechaza un divisor de magnitud cero. |
| `Conjugate(value)` | Devuelve el conjugado complejo. |
| `Equals(a, b)` | Comprueba la igualdad exacta de los componentes. |

~~~c
MODULE_TYPED(Numeric, Complex, Float64) value =
    MODULE_FUNC(Numeric, Complex, Float64, Make)(3.0, 4.0);

double magnitude =
    MODULE_FUNC(Numeric, Complex, Float64, Magnitude)(value);
~~~

`Equals` compara ambos componentes exactamente; no es una comparación con epsilon/tolerancia. La implementación de magnitud utiliza un cálculo iterativo interno y los casos límite de punto flotante siguen sujetos a los límites normales de precisión.

## Fixed-point

Encabezado: `Fixed.h`. Tipos integrados: `Q8_8` y `Q16_16`.

Un valor de punto fijo almacena un entero bruto con signo. El sufijo codifica la configuración de almacenamiento y bits fraccionarios: `Q8_8` utiliza 8 bits fraccionarios y `Q16_16` utiliza 16.

| Función | Contrato |
| --- | --- |
| `From.Raw(raw)` | Construye directamente desde el entero bruto almacenado. |
| `From.Int(value)` | Convierte un entero a la escala de punto fijo. |
| `From.Float64(value)` | Convierte un double a la escala de punto fijo. |
| `To.Int(value)` | Convierte a la parte entera. |
| `To.Float64(value)` | Convierte a double. |
| `Add(a, b)`, `Sub(a, b)`, `Mul(a, b)` | Devuelven un valor de punto fijo. |
| `Div(a, b, outValue)` | Divide en un valor de salida; devuelve un error si el divisor es cero. |
| `Compare(a, b)` | Devuelve el orden `CMPOUT`. |

~~~c
MODULE_TYPED(Numeric, Fixed, Q16_16) a =
    MODULE_FUNC(Numeric, Fixed, Q16_16, From_Float64)(2.5);
MODULE_TYPED(Numeric, Fixed, Q16_16) b =
    MODULE_FUNC(Numeric, Fixed, Q16_16, From_Int)(3);

MODULE_TYPED(Numeric, Fixed, Q16_16) product =
    MODULE_FUNC(Numeric, Fixed, Q16_16, Mul)(a, b);
double converted =
    MODULE_FUNC(Numeric, Fixed, Q16_16, To_Float64)(product);
~~~

La multiplicación y la conversión utilizan el tipo entero intermedio más ancho configurado para la especialización. La aritmética es de ancho fijo; comprueba el rango de entrada y no presupongas saturación.

## Bigint y Decimal

Encabezados: `Bigint.h` y `Decimal.h`. Tipos estándar: desde `MODULE_TYPE(Numeric, Bigint128)` hasta `Bigint1024`, y desde `MODULE_TYPE(Numeric, Decimal128)` hasta `Decimal1024`.

Son objetos de ancho fijo respaldados por arrays de limbs de 64 bits, con el limb menos significativo en el índice cero. No son enteros que crecen dinámicamente. `Decimal` no es un tipo decimal de punto flotante ni de escala fija: utiliza la misma representación de limbs enteros y el mismo modelo aritmético, con formato de texto decimal. Utiliza `Fixed` o `Rational` cuando necesites valores fraccionarios.

| Grupo | Métodos |
| --- | --- |
| Estado | `Clear` |
| Aritmética | `Add`, `Sub`, `Mul`, `Div`, `Mod`, `Increment`, `Decrement` |
| Salidas de división | `Div.Mod(divisor, outQuotient, outRemainder)` |
| Bit a bit | `And`, `Or`, `Xor`, `Not` |
| Desplazamientos | `Shift.Left`, `Shift.Right` |
| Rotaciones | `Rotate.Left`, `Rotate.Right` |
| Bits individuales | `Bit.Set`, `Bit.Clear`, `Bit.Check` |
| Comparación | `Compare(other, outResult)`, `Equal`, `NotEqual`, `LessThan`, `GreaterThan`, `LessOrEqual`, `GreaterOrEqual` |
| Formato | `ToCString.Base(base, buffer, bufferSize)`, `ToCString(buffer, bufferSize)` |
| Consulta | `IsZero` |

`Compare` escribe un valor `CMPOUT` mediante `outResult` y devuelve `OPSTATUS`; los predicados relacionales devuelven `bool`. Los métodos aritméticos modifican el receptor y devuelven `OPSTATUS`. `Div` escribe el cociente en el receptor. La variante `Div.Mod` escribe el cociente y el resto en dos objetos de salida distintos sin modificar el receptor. `Mod` escribe el resto en el receptor. Los métodos de formato escriben una representación terminada en nulo en un búfer del llamador; comprueba el estado y la capacidad.

~~~c
MODULE_TYPE(Numeric, Bigint128) left = {0};
MODULE_TYPE(Numeric, Bigint128) right = {0};

OBJECT_CALL_0(&left, Clear);
OBJECT_CALL_0(&right, Clear);

/* Set the low limb when constructing a small test value. */
left.limb[0] = 40U;
right.limb[0] = 2U;

OPSTATUS status = OBJECT_CALL(&left, Add, &right);
~~~

La representación por limbs está expuesta en el tipo generado. Escribir limbs directamente es apropiado para inicialización de bajo nivel y serialización, pero para la aritmética habitual utiliza las operaciones proporcionadas para que se informe del overflow.

---

# Errores y comportamiento aritmético

- Las operaciones que pueden fallar devuelven `OPSTATUS`. Una operación fallida puede dejar el receptor sin cambios cuando la implementación utiliza un resultado temporal; aun así, utiliza el contrato documentado y no presupongas comportamiento transaccional en todos los casos.
- `INVALID_ARGUMENT` se utiliza para punteros inválidos y combinaciones de salida inválidas. El estado de divisor cero depende de la API: la división de limbs Bigint/Decimal informa `DIVISION_BY_ZERO`, mientras que Rational, Fixed y Complex actualmente informan `INVALID_ARGUMENT` para denominadores/divisores cero.
- `ARITHMETIC_OVERFLOW` se devuelve cuando una operación de ancho fijo con múltiples limbs no puede representar el resultado.
- El almacenamiento Rational y de punto fijo está limitado por los tipos enteros C subyacentes.
- Los valores Complex son de punto flotante y pueden presentar redondeo, overflow, underflow y comportamiento NaN.
- La capacidad de Bigint/Decimal es fija según el ancho del tipo; el overflow se informa, no se agrega otro limb.

## Templates

Las especializaciones Rational y Complex se generan a partir de sus tablas de tipos. Fixed utiliza una fila de especialización de cuatro campos: sufijo, tipo de almacenamiento, tipo intermedio más ancho y cantidad de bits fraccionarios. Bigint y Decimal utilizan la tabla de anchos.

El motor común de templates permite instanciaciones personalizadas cuando un encabezado de módulo expone la definición correspondiente. Mantén el formato de fila consistente con los parámetros del template; un ancho ausente de la tabla integrada actual no queda disponible automáticamente en el encabezado agregado.

## Módulos relacionados

- [Math](Math.md) — aritmética escalar comprobada, límites y ecuaciones.
- [Bit](Bit.md) — operaciones escalares de bits.
- [Types](Types.md) — traits de tipos y valores etiquetados.
- [Module](Module.md) — modelo de declaración de templates y objetos.

## Encabezados de origen

- `Codespace/Cosmeron/Modules/Numeric/Rational.def`
- `Codespace/Cosmeron/Modules/Numeric/Complex.def`
- `Codespace/Cosmeron/Modules/Numeric/Fixed.def`
- `Codespace/Cosmeron/Modules/Numeric/Bigint.def`
- `Codespace/Cosmeron/Modules/Numeric/Decimal.def`
- `Codespace/Cosmeron/Modules/Numeric/Impl/Common.inc`
