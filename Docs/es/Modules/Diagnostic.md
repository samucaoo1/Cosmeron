# Diagnóstico (Diagnostic)

El módulo **Diagnostic** almacena metadatos del origen del código, formatea errores en búferes propiedad del llamador y captura trazas de pila cuando la plataforma de destino ofrece soporte. Sus encabezados `Context`, `Format` y `Stacktrace` deshabilitan las function tables; utiliza llamadas directas `MODULE_FUNC(Diagnostic, ...)`.

## Encabezados

~~~c
#include "Cosmeron/Core/Diagnostic/Context.h"
#include "Cosmeron/Core/Diagnostic/Format.h"
#include "Cosmeron/Core/Diagnostic/Stacktrace.h"
~~~

## Visión general

| Componente | Propósito |
| --- | --- |
| `Context` | Asocia un diagnóstico con archivo, línea, función, descripción y datos opacos del usuario. |
| `Format` | Produce texto simple, detallado, encadenado o solo de contexto en un búfer proporcionado por quien llama. |
| `Stacktrace` | Captura direcciones de marcos y las formatea sin poseer un array de marcos asignado dinámicamente. |

Los metadatos de diagnóstico son independientes del código de error. Utiliza `Condition_Error` para el fallo y adjunta o formatea un `Diagnostic_Context` cuando importan la ubicación y la explicación.

## Referencia de la API

### Objeto Context

Encabezado: `Context.h`. Tipo: `MODULE_TYPE(Diagnostic, Context)`.

| Método | Descripción |
| --- | --- |
| `Create(file, line, function, description, userData)` | Establece el origen del contexto y un puntero opcional a datos opacos. |
| `Clear()` | Restablece todos los campos. |
| `HasOrigin()` | Indica si hay un archivo de origen. |
| `File()`, `Line()`, `Function()`, `Description()`, `UserData()` | Devuelve los campos almacenados. |

~~~c
MODULE_TYPE(Diagnostic, Context) context = {0};

OPSTATUS status = MODULE_FUNC(Diagnostic, Context, Create)(
    &context, __FILE__, (uint32_t)__LINE__, __func__,
    "Loading configuration", NULL);
~~~

Las cadenas y el puntero `userData` son prestados; el objeto no copia su contenido. Asegúrate de que el almacenamiento referenciado siga siendo válido durante el uso del contexto.

### Paquete Format

Encabezado: `Format.h`.

Cada formateador es una `OPERATION` y recibe un búfer de salida, su capacidad y un contador de salida opcional para los bytes escritos.

| Función | Descripción |
| --- | --- |
| `Simple(error, outBuffer, capacity, outWritten)` | Formatea el estado y el mensaje de un `Condition_Error`. |
| `Detailed(error, outBuffer, capacity, outWritten)` | Formatea un error con su contexto de diagnóstico. |
| `Chain(error, outBuffer, capacity, outWritten)` | Formatea la cadena de causas del error. |
| `Context(context, outBuffer, capacity, outWritten)` | Formatea un valor de contexto de diagnóstico. |

~~~c
char output[256];
size_t written = 0U;

OPSTATUS status = MODULE_FUNC(Diagnostic, Format, Simple)(
    &error, output, sizeof(output), &written);
if (status == STATUS_CONST(SUCCESS)) {
    /* output contains the formatted text */
}
~~~

El llamador es propietario del búfer. Pasa siempre su capacidad real y comprueba el estado devuelto antes de utilizar la salida. Una operación puede informar de espacio insuficiente; no supongas que un búfer truncado contiene un diagnóstico completo.

### Objeto Stacktrace

Encabezado: `Stacktrace.h`. Tipo: `MODULE_TYPE(Diagnostic, Stacktrace)`.

El objeto almacena hasta el máximo configurado de direcciones en su propio array fijo de marcos; la configuración actual establece un máximo predeterminado de 32.

| Método | Descripción |
| --- | --- |
| `Capture(skipFrames, maxFrames)` | Captura hasta el número solicitado de marcos después de omitir los primeros indicados. |
| `Clear()` | Restablece el número de marcos capturados. |
| `FrameCount()` | Devuelve el número de direcciones capturadas. |
| `FrameAt(index, outAddress)` | Copia una dirección de marco al puntero de salida. |
| `Format(outBuffer, capacity, outWritten)` | Formatea las direcciones capturadas en un búfer del llamador. |

~~~c
MODULE_TYPE(Diagnostic, Stacktrace) trace = {0};

OPSTATUS status = MODULE_FUNC(Diagnostic, Stacktrace, Capture)(
    &trace, 0U, 16U);

size_t count = MODULE_FUNC(Diagnostic, Stacktrace, FrameCount)(&trace);
~~~

La captura depende de la plataforma y debe considerarse un mejor esfuerzo. El objeto almacena direcciones sin procesar; no garantiza nombres de símbolos, resolución de líneas de origen ni un desenrollador portable en todos los destinos. Comprueba el estado de `Capture`, `FrameAt` y `Format`.

## Flujo de trabajo habitual

1. Construye un `Condition_Error` con un estado y un mensaje.
2. Adjunta un `Diagnostic_Context` para conservar el origen, cuando esté disponible.
3. Utiliza `Format.Simple` para registros compactos, `Format.Detailed` para error y origen, o `Format.Chain` cuando importen las causas.
4. Captura una traza en un límite donde la plataforma lo admita y luego formátala en un búfer del llamador.

## Módulos relacionados

- [Condition](Condition.md) — estados, objetos de error, panic y excepciones.
- [Memory](Memory.md) — búferes administrados por el llamador y propiedad de memoria.
- [Module](Module.md) — convenciones de métodos de objetos y paquetes.

## Encabezados de origen

- `Codespace/Cosmeron/Core/Diagnostic/Context.h`
- `Codespace/Cosmeron/Core/Diagnostic/Format.h`
- `Codespace/Cosmeron/Core/Diagnostic/Stacktrace.h`
