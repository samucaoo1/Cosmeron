# Conversión de tipos (Cast)

El módulo **Cast** proporciona conversiones generadas y verificadas entre tipos primitivos C compatibles. Pertenece a **Core** y complementa a `Types`: Types define aliases y metadatos de tipos primitivos, mientras que Cast define conversiones con comprobaciones de rango cuando son necesarias.

## Encabezados

```c
#include "Cosmeron/Core/Types/Cast/Cast.h"
```

El encabezado agregado incluye `Converting.h`, `Integer.h` y `Float.h`.

---

# Visión general

| Encabezado | Propósito |
| --- | --- |
| `Converting.h` | Macros de especificación de conversiones y motor de generación de código |
| `Integer.h` | Conversiones verificadas entre enteros |
| `Float.h` | Conversiones entre coma flotante y enteros |

Las funciones generadas devuelven `OPSTATUS` y escriben el valor convertido mediante un puntero de salida. Esto hace explícitas las conversiones que pueden reducir el rango o cambiar el signo.

---

# Referencia de la API

## Conversiones entre enteros

El paquete Integer genera operaciones de conversión para los tipos de origen `Uint64`, `Uint32`, `Uint16`, `Uint8`, `Int64`, `Int32`, `Int16`, `Int8` y `Size`. Solo genera los pares de destino declarados en `Integer.h`; un par arbitrario no está disponible automáticamente.

El nombre de la operación generada concatena el tipo de origen, el literal `to` y el sufijo de destino. La siguiente tabla enumera los sufijos de destino exactos declarados en `Integer.h`; no representa el producto cartesiano completo de tipos de origen y destino.

| Origen | Sufijos de destino generados |
| --- | --- |
| `Uint64` | `32`, `16`, `8`, `I64`, `I32`, `I16`, `I8`, `Size` |
| `Uint32` | `16`, `8`, `I64`, `I32`, `I16`, `I8` |
| `Uint16` | `8`, `I64`, `I32`, `I16`, `I8` |
| `Uint8` | `I64`, `I32`, `I16`, `I8` |
| `Int64` | `32`, `16`, `8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Int32` | `16`, `8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Int16` | `8`, `U64`, `U32`, `U16`, `U8` |
| `Int8` | `U64`, `U32`, `U16`, `U8` |
| `Size` | `U64`, `U32`, `U16`, `U8`, `I64`, `I32` |

Por ejemplo:

```c
uint32_t out = 0;
OPSTATUS status =
    MODULE_FUNC(Cast, Integer, Uint64to32)(value, &out);
```

La operación valida el puntero de salida y devuelve `ARITHMETIC_OVERFLOW` cuando el valor está fuera del rango del tipo de destino. Una conversión fallida no escribe el resultado.

---

## Conversiones de coma flotante

El paquete Float declara conversiones entre `Float`, `Double`, `LongDouble`, determinados tipos enteros con y sin signo, y `Size`. Su tabla de conversiones es explícita; no representa el producto cartesiano completo de todos los tipos primitivos.

Los pares exactos de sufijos de origen y destino son:

| Origen | Sufijos de destino generados |
| --- | --- |
| `Float` | `Double`, `LongDouble`, `I64`, `I32`, `I16`, `I8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Double` | `Float`, `LongDouble`, `I64`, `I32`, `I16`, `I8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `LongDouble` | `Float`, `Double`, `I64`, `I32`, `I16`, `I8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Int64`, `Int32`, `Uint64`, `Uint32`, `Size` | `Float`, `Double`, `LongDouble` |

```c
double out = 0.0;
OPSTATUS status =
    MODULE_FUNC(Cast, Float, FloattoDouble)(input, &out);
```

Las operaciones de coma flotante a entero comprueban los límites configurados del destino antes de convertir y rechazan NaN y los valores fuera de rango. La reducción de precisión entre tipos de coma flotante comprueba el límite máximo indicado por la entrada de la tabla. Las conversiones de entero a coma flotante son conversiones directas y pueden perder precisión cuando el formato de destino no puede representar exactamente el valor de origen. Las conversiones de coma flotante a entero utilizan el cast de C después de la validación; por tanto, los valores fraccionarios se truncan hacia cero en lugar de redondearse al entero más cercano.

Comprueba `OPSTATUS` antes de utilizar la salida. El desbordamiento se informa como `ARITHMETIC_OVERFLOW`; un puntero de salida nulo devuelve `INVALID_ARGUMENT`.

---

## Generador de conversiones

`Converting.h` expone el vocabulario de especificación utilizado por los encabezados de los paquetes:

| Macro | Significado |
| --- | --- |
| `TO(name, type, category)` | Declara una conversión sin una tupla explícita de límites |
| `TO_EX(name, type, category, limits...)` | Declara una conversión con información explícita de rango/límites |
| `CONVERTING_OPERATIONS(source, ...)` | Genera operaciones normales de paquete |
| `CONST_CONVERTING_OPERATIONS(source, ...)` | Genera operaciones const |
| `CONVERTING_INLINE(source, ...)` | Genera operaciones de conversión inline |
| `CONST_CONVERTING_INLINE(source, ...)` | Genera operaciones const inline |

Estas macros son principalmente herramientas de generación de código para definir tablas de conversión. La mayoría del código de aplicación debería llamar a las funciones de conversión generadas en lugar de utilizar las macros directamente.

---

# Contratos y limitaciones

- Prefiere Cast a un cast de C sin comprobación cuando el par necesario esté disponible y se requiera validar el rango en tiempo de ejecución.
- La seguridad de rango no equivale a exactitud: un entero representable aún puede perder precisión al convertirse a coma flotante.
- Solo se generan las conversiones enumeradas en `Integer.h` y `Float.h`.
- Cast no proporciona conversiones para structs definidos por el usuario, punteros, cadenas ni tipos numéricos del módulo independiente `Numeric`.
- La conversión de unidades, la política de redondeo y la validación semántica siguen siendo responsabilidad de la aplicación.

## Módulos relacionados

- [Types](Types.md) — aliases y metadatos de tipos primitivos.
- [Numeric](Numeric.md) — representaciones racionales, de punto fijo, complejas y numéricas de múltiples limbs.
- [Module](Module.md) — funciones de paquete generadas y acceso a espacios de nombres.

## Encabezados de origen

- `Codespace/Cosmeron/Core/Types/Cast/Cast.h`
- `Codespace/Cosmeron/Core/Types/Cast/Converting.h`
- `Codespace/Cosmeron/Core/Types/Cast/Integer.h`
- `Codespace/Cosmeron/Core/Types/Cast/Float.h`
