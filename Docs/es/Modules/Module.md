# Module

El núcleo **Module** proporciona el sistema declarativo de API C de Cosmeron. Genera tipos con namespace, enums, constantes, structs, objetos, paquetes, callbacks, closures, templates y function tables opcionales a partir de declaraciones en los encabezados.

## Encabezados

~~~c
#include "Cosmeron/Core/Module/Module.h"
~~~

El encabezado agregado incluye los mecanismos de namespace, declaraciones, objetos, paquetes, callbacks, closures, despacho, function tables, templates y ámbitos de implementación utilizados por los encabezados públicos de los módulos.

---

# Visión general

## Modelo de declaraciones

El modelo de declaraciones se organiza así:

| Construcción | Propósito |
| --- | --- |
| `DOMAIN` | Separa áreas amplias de la biblioteca, como `Core` y `Modules`. |
| `MODULE` | Agrupa una familia relacionada de declaraciones bajo un namespace público. |
| `PACKAGE` | Agrupa funciones libres que comparten un nombre de paquete. |
| `OBJECT` | Declara un tipo con estado y métodos que reciben un receptor de objeto implícito. |
| `STRUCT` | Declara un tipo de datos C simple sin receptor de método implícito. |
| `ISTRUCT` | Declara una estructura simple interna al módulo. |
| `VARIANT` | Agrupa operaciones relacionadas bajo una ruta anidada de método/función. |
| `CALLBACK` | Declara la firma de un callback de puntero a función. |
| `CLOSURE` | Declara una firma invocable que transporta un puntero de contexto. |

Una declaración no es un sistema de clases en tiempo de ejecución. Los objetos y paquetes son structs, funciones y macros C generados. La capa Module no proporciona descubrimiento dinámico de tipos ni recolección automática de basura.

---

# Referencia de la API

## Namespace y nombres públicos

### Acceso al namespace

Encabezado: `Namespace.h` (incluido por `Module.h`).

Utiliza las macros semánticas de nombres en lugar de construir identificadores públicos manualmente:

| Macro | Propósito |
| --- | --- |
| `MODULE_TYPE(Module, Type)` | Nombra un tipo público no basado en template. |
| `MODULE_TYPED(Module, Type, Suffix)` | Nombra una especialización tipada generada. |
| `MODULE_ENUM(Module, Enum)` | Nombra un enum del módulo. |
| `MODULE_CONST(Module, Constant)` | Nombra una constante del módulo. |
| `MODULE_FUNC(Module, Function)` | Nombra una función de paquete u objeto. |
| `MODULE_FUNC(Module, Package, Function)` | Nombra una función de un paquete. |
| `MODULE_CLOSURE(Module, Package, Name)` | Nombra un tipo closure generado en un paquete. |
| `MODULE_CALLBACK(Module, Name)` | Nombra un tipo callback declarado por un módulo. |
| `MODULE_NS(...)` | Forma una secuencia de tokens de namespace de módulo. |
| `NS(...)` / `CNS(...)` | Forma tokens de namespace ordinarios o constantes. |

Por ejemplo:

~~~c
MODULE_TYPE(Type, Traits) descriptor;
MODULE_TYPED(Numeric, Rational, Int32) fraction;
MODULE_ENUM(Algorithm, Comparison, Result) order;
~~~

El identificador real se produce mediante el prefijo de biblioteca configurado y las reglas de namespace. El código de aplicación debe preferir estas macros de acceso frente a nombres expandidos codificados manualmente.

### Configuración

El encabezado de namespace define las macros de prefijo y política de namespace utilizadas por el proyecto. Configúralas antes de incluir los encabezados públicos de módulos. La política de function tables también es una elección de compilación; un módulo, paquete u objeto puede definir una opción de desactivación antes de incluir su encabezado.

No cambies la configuración de namespace o function tables entre unidades de traducción que intercambien los tipos afectados. Esto puede producir nombres o layouts de objetos incompatibles.

La configuración de namespace y las macros de acceso público se proporcionan en `Core/Module/Namespace.h`.

## Packages

Un paquete es la construcción adecuada para operaciones que no necesitan un receptor de objeto.

### Declaración

~~~c
MODULE(DOMAIN(Modules), Example,
    PACKAGE(Math,
        FUNCTION(int, Add, (int left, int right)),
        OPERATION(Divide, (int left, int right, int *outValue))
    )
);
~~~

La declaración de paquete genera funciones con namespace y, cuando está habilitada, una function table tipada. La categoría de función determina su contrato:

| Declaración | Contrato de retorno |
| --- | --- |
| `PROCEDURE` | Sin valor de retorno; se utiliza para efectos secundarios. |
| `FUNCTION` | Devuelve un valor directamente. |
| `PREDICATE` | Devuelve `bool`. |
| `OPERATION` | Devuelve `OPSTATUS` y puede escribir parámetros de salida. |
| `COMPARISON` | Devuelve el resultado de comparación de tres vías de la biblioteca. |

Utiliza la categoría aplicable más específica y mantén el trabajo que puede fallar bajo `OPERATION`.

### Invocación

La macro canónica de invocación de funciones de paquete mediante function table es `PACKAGE_API`:

~~~c
int sum = PACKAGE_API(Example_Math, Add, 10, 20);
~~~

Para una llamada directa con namespace que evita la function table, utiliza `MODULE_FUNC`:

~~~c
int sum = MODULE_FUNC(Example, Math, Add)(10, 20);
~~~

Las function tables de paquete son opcionales. Si están deshabilitadas, utiliza la forma directa en lugar de intentar el despacho de paquete.

## Objects

Un objeto combina un struct C con métodos que reciben un puntero a la instancia como primer parámetro receptor oculto.

### Declaración

~~~c
MODULE(DOMAIN(Modules), Counter,
    OBJECT(Counter,
        FIELDS(
            int value;
        ),
        PROCEDURE(Reset, ()),
        OPERATION(Add, (int amount)),
        CONST_FUNCTION(int, Value, ())
    )
);
~~~

El receptor es proporcionado por la API de objeto generada. Un método declarado con `CONST_` es de solo lectura respecto a su contrato de receptor.

### Invocación

La macro canónica de despacho de objetos es `API`; `OBJECT_CALL` y `OBJECT_CALL_0` son alias de compatibilidad:

~~~c
MODULE_TYPE(Counter, Counter) counter = {0};

API(&counter, Reset);
OPSTATUS status = API(&counter, Add, 5);
int value = API(&counter, Value);
~~~

Para una llamada directa sin despacho mediante function table de objeto, utiliza el método generado con namespace:

~~~c
MODULE_FUNC(Counter, Counter, Add)(&counter, 5);
~~~

Utiliza `API` cuando el objeto tenga una function table válida y se pretenda despacho en tiempo de ejecución. Utiliza llamadas directas `MODULE_FUNC` cuando la configuración elimine las function tables o se desee despacho directo. Nunca invoques un método sobre un objeto sin inicializar o inválido.

### Herencia y extensiones

La capa actual de declaraciones Object incluye `OBJECT_EXTENDS` y conversiones tipadas como `OBJECT_AS`. Son mecanismos de declaración/layout C en tiempo de compilación; no proporcionan herencia virtual ni gestión automática de vida útil. Utilízalos solo cuando el layout base/derivado y el contrato del receptor sean explícitos.

## Structs y datos internos

Utiliza `STRUCT` para un tipo de datos POD público y `ISTRUCT` para un detalle de implementación. Un struct no tiene receptor generado ni obtiene despacho de métodos por el mero hecho de declararse dentro de un módulo.

Para tipos de valor de layout fijo, prefiere la inicialización directa de campos y la asignación ordinaria cuando el contrato de propiedad del tipo lo permita. Los tipos que requieran inicialización, clonación, movimiento o liberación personalizados deben utilizar las facilidades específicas de objetos o traits, en lugar de tratarse como trivialmente copiables.

## Callbacks y closures

### Callback

Un callback es un puntero a función con una firma declarada. No transporta estado por sí mismo.

`Callback.h` proporciona `TCALLBACK(Name)` para el tipo generado, `CALLBACK_FUNC(Name)` para nombres auxiliares y `CALLBACK_EMPTY` como valor de callback nulo.

### Closure

Una closure combina un puntero a función con un puntero de contexto. Es adecuada para callbacks que necesitan referirse a estado propiedad del llamador sin utilizar una variable global.

`Closure.h` proporciona `TCLOSURE(Name)` y `CLOSURE_FUNC(Name)`. Las macros comunes de llamada/validación son:

| Macro | Comportamiento |
| --- | --- |
| `CLOSURE_IS_VALID(closure)` | Comprueba si la closure tiene una función. |
| `CLOSURE_CALL(closure, ...)` | Invoca la closure. |
| `CLOSURE_EMPTY` | Valor de closure vacío. |

Las declaraciones Closure especifican el contrato de retorno: `CLOSURE_FUNCTION`, `CLOSURE_PROCEDURE`, `CLOSURE_OPERATION`, `CLOSURE_PREDICATE` o `CLOSURE_COMPARISON`. El puntero de contexto de una closure es prestado; la capa Module no administra la vida útil del objeto apuntado.

## Templates

La implementación canónica de templates de módulos utiliza un archivo `.def` de modo dual. Un template puede generar una especialización de struct, objeto o paquete. Debe incluirse desde su encabezado público y puede instanciarse explícitamente o a partir de una tabla de tipos X-macro.

### Instanciar una especialización

Los auxiliares comunes de templates son:

| Macro | Propósito |
| --- | --- |
| `TEMPLATE_INSTANTIATE(Name, args...)` | Instancia un template con nombre. |
| `TEMPLATE_INSTANTIATE_TABLE(Name, Table)` | Instancia un template para cada fila devuelta por la tabla. |
| `TEMPLATE_INSTANTIATE_TABLE(Module, Name, Table)` | Instancia un template cualificado por módulo para cada fila. |

~~~c
TEMPLATE_INSTANTIATE_TABLE(Numeric, Rational, NUMERIC_RATIONAL_TYPE_TABLE);
~~~

Una fila de tabla es una tupla de argumentos. Mantén su orden idéntico al orden de parámetros de la macro template correspondiente.

El motor de templates también expone auxiliares de declaración/implementación de nivel inferior, pero los encabezados públicos de módulos deben encapsular esos detalles. Los consumidores deben utilizar los puntos de entrada de instanciación documentados por el módulo, en lugar de invocar macros internas de implementación.

### Convención .def de modo dual

El archivo `.def` de un template define la macro template y admite instanciación directa cuando se definen las macros de configuración esperadas. El encabezado público normalmente incluye definiciones internas compartidas, declara su paquete/objeto e incluye la definición del template. No incluyas archivos internos `Impl` o `Common.inc` directamente desde el código de aplicación.

## Implementación y ámbitos

Las implementaciones no inline se emiten mediante declaraciones explícitas de implementación de módulo. El sistema de declaraciones actual proporciona `MODULE_IMPLEMENT`, `OBJECT_IMPLEMENT_EX` y `PACKAGE_IMPLEMENT_EX`. Los auxiliares de ámbito como `MODULE_SCOPE`, `OBJECT_SCOPE` y `PACKAGE_SCOPE` organizan implementaciones relacionadas y terminan con su macro `*_SCOPE_END` correspondiente.

Un encabezado público generalmente contiene declaraciones e incluye su unidad de implementación. Mantén los cuerpos de función en los archivos de implementación proporcionados por el módulo; los consumidores solo deben incluir el encabezado público.

## Function tables

Las function tables proporcionan una ruta de despacho tipada para paquetes y objetos. Son una salida opcional de compilación, no un runtime independiente de métodos virtuales.

- Utiliza `PACKAGE_API` para despachar funciones de paquete mediante una tabla de paquete.
- Utiliza `API` para despachar métodos de objeto mediante la tabla del objeto (`OBJECT_CALL` es un alias de compatibilidad).
- Utiliza `MODULE_FUNC` para invocación directa de símbolos.
- Configura las opciones de desactivación pertinentes antes de incluir los encabezados.
- Mantén la misma configuración entre unidades de traducción que compartan las declaraciones.

Deshabilitar una function table puede cambiar el layout del objeto generado. No mezcles objetos construidos con políticas de tabla diferentes.

## Guía práctica

- Utiliza `PACKAGE` para operaciones sin estado y `OBJECT` solo cuando el estado pertenezca a la instancia.
- Utiliza `STRUCT` para valores POD; no tiene receptor ni function table de objeto.
- Utiliza `OPERATION` siempre que una rutina pueda fallar y devolver `OPSTATUS`.
- Utiliza `FUNCTION` para resultados directos y `PREDICATE` para preguntas booleanas.
- Utiliza `CALLBACK` para punteros a función simples; utiliza `CLOSURE` cuando el contexto deba acompañar al callable.
- Mantén los detalles de templates en archivos `.def` y expón un encabezado público limpio.
- Trata todos los punteros pasados como contexto, datos de callback o campos según su contrato de propiedad documentado; el sistema Module no los posee automáticamente.

## Módulos relacionados

- [Algorithm](Algorithm.md) — uso práctico de closures en repetición y comparación.
- [Condition](Condition.md) — estados y resultados de operaciones que pueden fallar.
- [Types](Types.md) — descriptores de tipos primitivos y traits.
- [Preprocessor](Preprocessor.md) — auxiliares de tokens y tablas utilizados por la capa declarativa.

## Encabezados de origen

- `Codespace/Cosmeron/Core/Module/Module.h`
- `Codespace/Cosmeron/Core/Module/Namespace.h`
- `Codespace/Cosmeron/Core/Module/Object.h`
- `Codespace/Cosmeron/Core/Module/Package.h`
- `Codespace/Cosmeron/Core/Module/Callback/Callback.h`
- `Codespace/Cosmeron/Core/Module/Closure/Closure.h`
- `Codespace/Cosmeron/Core/Module/Common/Template.impl`
- `Codespace/Cosmeron/Core/Module/Common/Declarative.impl`
