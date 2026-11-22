# Condición (Condition)

El módulo **Condition** reúne estados operativos, valores de error detallados, limpieza por ámbito, gestión de panic y marcos estructurados de excepción. Utiliza el mecanismo más sencillo que se ajuste al fallo: devuelve un `OPSTATUS` en operaciones habituales que pueden fallar, asocia un `Condition_Error` cuando resulta útil disponer de contexto o una cadena de causas y reserva los mecanismos de panic/excepción para contratos de flujo de control que realmente los requieran. Los encabezados de Condition deshabilitan la invocación mediante function table; llama directamente a las funciones de los paquetes y a los métodos de `Error` mediante `MODULE_FUNC(Condition, ...)`.

## Encabezados

~~~c
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Condition/Defer.h"
#include "Cosmeron/Core/Condition/Panic.h"
#include "Cosmeron/Core/Condition/Exception.h"
~~~

---

# Visión general

| Componente | Propósito |
| --- | --- |
| `Status` | Define `OPSTATUS`, categorías de estado y consultas simbólicas/de mensajes. |
| `Error` | Almacena un código de estado, mensaje, origen de diagnóstico y una causa opcional. |
| `Defer` | Ejecuta acciones de limpieza al salir de un ámbito léxico o durante el desenrollado de una excepción. |
| `Panic` | Informa de una aserción, precondición, invariante o ruta inalcanzable incumplida. |
| `Exception` | Proporciona flujo de control `TRY`/`CATCH`/`FINALLY` basado en `setjmp`/`longjmp`. |

---

# Referencia de la API

## Paquete Status

Encabezado: `Status.h`.

`OPSTATUS` es el tipo de estado canónico. Los valores de estado y categoría se generan a partir de `StatusTable/StatusTable.h`; utiliza los auxiliares públicos en lugar de codificar valores numéricos.

| Función | Descripción |
| --- | --- |
| `IsSuccess(status)` | Verdadero únicamente para `STATUS_CONST(SUCCESS)`. |
| `IsFailure(status)` | Verdadero para cualquier estado distinto del éxito. |
| `IsSpecial(status)` | Verdadero para los estados especiales de flujo declarados en la tabla. |
| `CategoryOf(status)` | Devuelve la categoría del estado. |
| `IsCategory(status, category)` | Comprueba la categoría del estado. |
| `CategoryName(category)` | Devuelve el nombre de la categoría. |
| `Name(status)` | Devuelve el nombre simbólico del estado. |
| `Message(status)` | Devuelve el mensaje predeterminado legible por humanos. |

~~~c
OPSTATUS status = STATUS_CONST(INVALID_ARGUMENT);

if (MODULE_FUNC(Condition, Status, IsFailure)(status)) {
    const char *name = MODULE_FUNC(Condition, Status, Name)(status);
    const char *message = MODULE_FUNC(Condition, Status, Message)(status);
}
~~~

## Objeto Error

Encabezado: `Error.h`.

`MODULE_TYPE(Condition, Error)` almacena un código `OPSTATUS`, un puntero al mensaje, un valor `Diagnostic_Context` y un puntero opcional a un error causante.

| Método | Contrato |
| --- | --- |
| `Create(code, message)` | Establece el estado y el mensaje proporcionado por quien llama. |
| `FromStatus(code)` | Crea un error utilizando el mensaje predeterminado de la tabla de estados. |
| `Clear()` | Limpia el valor del error. |
| `Code()`, `Message()` | Lee el estado y el mensaje. |
| `Is(code)`, `Is.Success()`, `Is.Failure()`, `Is.Category(category)` | Consulta el código almacenado. |
| `Category()` | Devuelve la categoría asociada al estado. |
| `Set.Context(context)` | Copia el contexto de diagnóstico al error. |
| `Set.Origin(file, line, function, description, userData)` | Establece los metadatos del origen. |
| `Set.Cause(cause)` | Asocia un error causante. |
| `Context()`, `HasContext()` | Lee o comprueba el contexto de diagnóstico. |
| `Cause()`, `HasCause()`, `RootCause()`, `Depth()` | Inspecciona la cadena de causas. |

~~~c
MODULE_TYPE(Condition, Error) error = {0};

OPSTATUS status = MODULE_FUNC(Condition, Error, Create)(
    &error, STATUS_CONST(INVALID_ARGUMENT), "Invalid account identifier");

if (status == STATUS_CONST(SUCCESS)) {
    const char *message =
        MODULE_FUNC(Condition, Error, Message)(&error);
}
~~~

Los campos de mensaje y de cadenas de diagnóstico son punteros prestados; el objeto Error no duplica las cadenas. Mantén su almacenamiento válido mientras el error las utilice. El puntero de la causa tampoco transfiere la propiedad.

## Paquete Defer

Encabezado: `Defer.h`.

El paquete expone `Cancel(node)`, `IsActive(node)` y `Execute(node)` para la representación del nodo defer. La mayoría de los usuarios utilizan macros en lugar de manipular los nodos directamente.

| Macro | Disponibilidad | Comportamiento |
| --- | --- | --- |
| `DEFER_NAMED(name, function, context)` | GCC y Clang | Registra un nodo de limpieza con nombre mediante el atributo cleanup del compilador. |
| `DEFER(function, context)` | GCC y Clang | Registra una limpieza con nombre automático. |
| `DEFER_TRAITS_NAMED(name, traits, instance)` | GCC y Clang | Registra la liberación mediante un descriptor `Type_Traits`. |
| `DEFER_TRAITS(traits, instance)` | GCC y Clang | Limpieza basada en traits con nombre automático. |
| `DEFER_CANCEL(name)` | GCC y Clang | Cancela una limpieza con nombre antes de salir del ámbito. |
| `DEFER_SCOPE(action)` | C11 | Ejecuta la acción indicada al salir del ámbito de una sola pasada de la macro. |
| `DEFER_TRY_SCOPE(action)` | C11 con Exception | Combina un marco try con una acción al salir del ámbito. |

~~~c
static void CloseHandle(void *context) {
    int *handle = (int *)context;
    if (handle != NULL) {
        *handle = -1;
    }
}

void UseHandle(void) {
    int handle = 12;
    DEFER(CloseHandle, &handle);
    /* handle is released when this lexical scope exits */
}
~~~

Las macros de limpieza con atributo `DEFER` son extensiones del compilador disponibles únicamente en GCC/Clang. Utiliza `DEFER_SCOPE(action)` cuando sea preferible un ámbito portable de una sola pasada en C11. Este último tiene la estructura de control de instrucciones `for` anidadas; evita saltos que omitan el incremento del bucle generado por la macro.

## Paquete Panic

Encabezado: `Panic.h`.

`Panic_Info` contiene el tipo de panic, el mensaje, la expresión que falló, el archivo, la línea y la función. El paquete expone `SetHandler(handler)`, `GetHandler()`, `ResetHandler()`, `KindName(kind)`, `Trigger(...)` y `Now(message)`.

Las macros públicas capturan automáticamente los campos de ubicación del código fuente:

| Macro | Contrato previsto |
| --- | --- |
| `PANIC(message)` | Informa explícitamente de un panic. |
| `ASSERT(condition)` | Aserción de desarrollo; se deshabilita cuando se define `COSMERON_DISABLE_DEBUG_ASSERT`. |
| `VERIFY(condition)` | Comprueba una condición en tiempo de ejecución y activa el handler configurado si falla. |
| `ENSURE(condition)` | Impone un contrato de precondición. |
| `UNREACHABLE()` | Informa de la ejecución de una ruta que no debería alcanzarse. |

Un panic no es un retorno de error ordinario. El handler configurado determina qué ocurre después del informe; no supongas que retorna ni que la ejecución puede continuar de forma segura.

## Paquete Exception

Encabezado: `Exception.h`.

El paquete utiliza `setjmp`/`longjmp` y un `Exception_Frame` asignado en la pila. Los métodos públicos son `PushFrame`, `PopFrame`, `RegisterCleanup`, `Throw`, `ThrowStatus`, `Rethrow`, `CurrentError` y `HasActiveFrame`.

| Macro | Propósito |
| --- | --- |
| `TRY` | Inicia un bloque protegido. |
| `CATCH` | Gestiona una excepción lanzada desde el bloque try activo. |
| `FINALLY` | Ejecuta el bloque final después del procesamiento try/catch. |
| `THROW(errorPtr)` | Lanza un Error previamente rellenado. |
| `THROW_STATUS(status, message)` | Crea y lanza un error a partir de un estado y de la ubicación en el código fuente. |
| `RETHROW()` | Marca la excepción capturada actual para propagarla al finalizar el marco. |
| `CURRENT_ERROR()` | Proporciona acceso al error almacenado en el marco activo. |

~~~c
TRY {
    /* fallible work */
    if (failed) {
        THROW_STATUS(STATUS_CONST(INVALID_ARGUMENT), "Bad input");
    }
} CATCH {
    const MODULE_TYPE(Condition, Error) *error = CURRENT_ERROR();
    (void)error;
} FINALLY {
    /* finalization for this frame */
}
~~~

Un `longjmp` no desenrolla objetos arbitrarios de la pila C ni ejecuta destructores al estilo C++. Registra explícitamente la limpieza cuando sea necesario. No saltes entre ámbitos que contengan objetos cuyas invariantes dependan de la limpieza normal de C. El marco es local a la pila y debe permanecer válido durante toda la secuencia try/catch/finally.

---

# Contratos y elección del mecanismo

- Devuelve `OPSTATUS` para fallos habituales de validación, E/S, asignación y aritmética.
- Utiliza `Condition_Error` cuando un estado necesite un mensaje, origen de diagnóstico o cadena causal.
- Utiliza `DEFER`/`DEFER_SCOPE` para una limpieza determinista en el flujo de control de C.
- Utiliza `PANIC`/`ASSERT`/`VERIFY`/`ENSURE` para contratos de programación incumplidos, no para entradas de usuario recuperables.
- Utiliza `TRY`/`CATCH` únicamente cuando la propagación no local de errores forme parte deliberadamente del diseño.

## Módulos relacionados

- [Diagnostic](Diagnostic.md) — contexto de origen y formato de errores.
- [Types](Types.md) — operaciones de ciclo de vida basadas en traits.
- [Module](Module.md) — declaraciones generadas de objetos, paquetes y delegates.

## Encabezados de origen

- `Codespace/Cosmeron/Core/Condition/Status.h`
- `Codespace/Cosmeron/Core/Condition/Error.h`
- `Codespace/Cosmeron/Core/Condition/Defer.h`
- `Codespace/Cosmeron/Core/Condition/Panic.h`
- `Codespace/Cosmeron/Core/Condition/Exception.h`
