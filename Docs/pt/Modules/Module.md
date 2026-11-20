# Module

O núcleo **Module** fornece o sistema declarativo de API em C de Cosmeron. Gera tipos com namespace, enums, constantes, structs, objetos, pacotes, callbacks, closures, templates y function tables opcionales a partir de declarações nos headers.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Module/Module.h"
~~~

O cabeçalho agregadorr inclui los mecanismos de namespace, declarações, objetos, pacotes, callbacks, closures, despacho, function tables, templates y escopos de implementação utilizedos por los cabeçalhos públicos de los módulos.

---

# Visão geral

## Modelo de declarações

O modelo de declarações está organizado assim:

| Construção | Finalidade |
| --- | --- |
| `DOMAIN` | Separa áreas abrangentes de la biblioteca, como `Core` y `Modules`. |
| `MODULE` | Agrupa uma família relacionada de declarações bajo un namespace público. |
| `PACKAGE` | Agrupa funções livres que compartilham um nome de pacote. |
| `OBJECT` | Declara um tipo com estado y métodos que reciben un receptor implícito do objeto. |
| `STRUCT` | Declara um tipo de dados C simples sem receptor implícito de método. |
| `ISTRUCT` | Declara uma estrutura simples interna al módulo. |
| `VARIANT` | Agrupa operações relacionadas bajo una ruta anidada de método/função. |
| `CALLBACK` | Declara a assinatura de um callback de ponteiro para função. |
| `CLOSURE` | Declara uma assinatura invocável que carrega um ponteiro de contexto. |

Uma declaração não é un sistema de clases em tempo de execução. Los objetos y pacotes son structs, funções y macros C generados. A camada Module não fornece descubrimiento dinámico de tipos ni recolección automática de basura.

---

# Referência da API

## Namespace y nombres públicos

### Acceso al namespace

Cabeçalho: `Namespace.h` (incluido por `Module.h`).

Utilize las macros semánticas de nombres em vez de construir identificadores públicos manualmente:

| Macro | Finalidade |
| --- | --- |
| `MODULE_TYPE(Module, Type)` | Nomeia um tipo público no basado en template. |
| `MODULE_TYPED(Module, Type, Suffix)` | Nomeia uma especialização tipada generada. |
| `MODULE_ENUM(Module, Enum)` | Nomeia um enum del módulo. |
| `MODULE_CONST(Module, Constant)` | Nomeia uma constante del módulo. |
| `MODULE_FUNC(Module, Function)` | Nomeia uma função de pacote u objeto. |
| `MODULE_FUNC(Module, Package, Function)` | Nomeia uma função de un pacote. |
| `MODULE_CLOSURE(Module, Package, Name)` | Nomeia um tipo closure generado en un pacote. |
| `MODULE_CALLBACK(Module, Name)` | Nomeia um tipo callback declarado por un módulo. |
| `MODULE_NS(...)` | Forma uma sequência de tokens de namespace de módulo. |
| `NS(...)` / `CNS(...)` | Forma tokens de namespace ordinarios o constantes. |

Por ejemplo:

~~~c
MODULE_TYPE(Type, Traits) descriptor;
MODULE_TYPED(Numeric, Rational, Int32) fraction;
MODULE_ENUM(Algorithm, Comparison, Result) order;
~~~

O identificador real é produzido por meio de el prefijo de biblioteca configurado y las reglas de namespace. O código da aplicação deve preferir estas macros de acceso em vez de nomes expandidos codificados manualmente.

### Configuración

El cabeçalho de namespace define as macros de prefijo y política de namespace utilizedas por el proyecto. Configure-as antes de incluir los cabeçalhos públicos de módulos. La política de function tables también es una elección de compilación; un módulo, pacote u objeto puede definir una opción de desactivación antes de incluir su cabeçalho.

Não altere la configuración de namespace o function tables entre unidades de traducción que intercambien los tipos afectados. Isso pode produzir nombres o layouts de objetos incompatíveis.

A configuração de namespace y las macros de acceso público se proporcionan en `Core/Module/Namespace.h`.

## Packages

Um pacote é a construção adequada para operaciones que não precisam un receptor de objeto.

### Declaração

~~~c
MODULE(DOMAIN(Modules), Example,
    PACKAGE(Math,
        FUNCTION(int, Add, (int left, int right)),
        OPERATION(Divide, (int left, int right, int *outValue))
    )
);
~~~

La declaração de pacote gera funções con namespace y, quando habilitada, una function table tipada. A categoria da função determina su contrato:

| Declaração | Contrato de retorno |
| --- | --- |
| `PROCEDURE` | Sem valor de retorno; é utilizada para efectos secundarios. |
| `FUNCTION` | Retorna um valor diretamente. |
| `PREDICATE` | Retorna `bool`. |
| `OPERATION` | Retorna `OPSTATUS` y pode escrever parâmetros de salida. |
| `COMPARISON` | Retorna el resultado de comparação de tres vías de la biblioteca. |

Utilize a categoria aplicable mais específica y mantenha o trabalho que puede fallar bajo `OPERATION`.

### Invocação

La macro canónica de invocación de funções de pacote mediante function table es `PACKAGE_API`:

~~~c
int sum = PACKAGE_API(Example_Math, Add, 10, 20);
~~~

Para uma chamada direta con namespace que evita a function table, utilize `MODULE_FUNC`:

~~~c
int sum = MODULE_FUNC(Example, Math, Add)(10, 20);
~~~

Las function tables de pacote son opcionales. Si están desabilitadas, utilize la forma directa em vez de intentar el despacho de pacote.

## Objects

Un objeto combina un struct C con métodos que reciben un ponteiro a la instancia como primer parámetro receptor oculto.

### Declaração

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

### Invocação

La macro canónica de despacho de objetos es `API`; `OBJECT_CALL` y `OBJECT_CALL_0` son alias de compatibilidad:

~~~c
MODULE_TYPE(Counter, Counter) counter = {0};

API(&counter, Reset);
OPSTATUS status = API(&counter, Add, 5);
int value = API(&counter, Value);
~~~

Para uma chamada direta sin despacho mediante function table de objeto, utilize el método generado con namespace:

~~~c
MODULE_FUNC(Counter, Counter, Add)(&counter, 5);
~~~

Utilize `API` cuando el objeto tenga una function table válida y se pretenda despacho em tempo de execução. Utilize llamadas directas `MODULE_FUNC` cuando la configuración elimine las function tables o se desee despacho directo. Nunca invoques un método sobre un objeto sin inicializar o inválido.

### Herencia y extensiones

La capa actual de declarações Object inclui `OBJECT_EXTENDS` y conversiones tipadas como `OBJECT_AS`. Son mecanismos de declaração/layout C en tiempo de compilación; no proporcionan herencia virtual ni gestión automática de tempo de vida. Utilízalos solo cuando el layout base/derivado y el contrato del receptor sean explícitos.

## Structs y datos internos

Utilize `STRUCT` para un tipo de datos POD público y `ISTRUCT` para un detalle de implementação. Un struct no tiene receptor generado ni obtiene despacho de métodos por el mero hecho de declararse dentro de un módulo.

Para tipos de valor de layout fijo, prefiere la inicialización directa de campos y la asignación ordinaria cuando el contrato de propriedade del tipo lo permita. Los tipos que requieran inicialización, clonación, movimiento o liberación personalizados deben utilizer las facilidades específicas de objetos o traits, em vez de tratarse como trivialmente copiables.

## Callbacks y closures

### Callback

Un callback es un ponteiro para função con una firma declarada. No transporta estado por sí mismo.

`Callback.h` proporciona `TCALLBACK(Name)` para el tipo generado, `CALLBACK_FUNC(Name)` para nombres auxiliares y `CALLBACK_EMPTY` como valor de callback nulo.

### Closure

Una closure combina un ponteiro para função con un ponteiro de contexto. Es adecuada para callbacks que necesitan referirse a estado propriedade do chamador sin utilizer una variable global.

`Closure.h` proporciona `TCLOSURE(Name)` y `CLOSURE_FUNC(Name)`. Las macros comunes de llamada/validación son:

| Macro | Comportamento |
| --- | --- |
| `CLOSURE_IS_VALID(closure)` | Verifica si la closure tiene una função. |
| `CLOSURE_CALL(closure, ...)` | Invoca la closure. |
| `CLOSURE_EMPTY` | Valor de closure vacío. |

Las declarações Closure especifican el contrato de retorno: `CLOSURE_FUNCTION`, `CLOSURE_PROCEDURE`, `CLOSURE_OPERATION`, `CLOSURE_PREDICATE` o `CLOSURE_COMPARISON`. El ponteiro de contexto de una closure es prestado; la capa Module no administra la tempo de vida del objeto apuntado.

## Templates

La implementação canónica de templates de módulos utilize un archivo `.def` de modo dual. Un template puede generar una especialización de struct, objeto o pacote. Debe incluirse desde su cabeçalho público y puede instanciarse explícitamente o a partir de una tabla de tipos X-macro.

### Instanciar una especialización

Los auxiliares comunes de templates son:

| Macro | Finalidade |
| --- | --- |
| `TEMPLATE_INSTANTIATE(Name, args...)` | Instancia un template con nombre. |
| `TEMPLATE_INSTANTIATE_TABLE(Name, Table)` | Instancia un template para cada fila devuelta por la tabla. |
| `TEMPLATE_INSTANTIATE_TABLE(Module, Name, Table)` | Instancia un template cualificado por módulo para cada fila. |

~~~c
TEMPLATE_INSTANTIATE_TABLE(Numeric, Rational, NUMERIC_RATIONAL_TYPE_TABLE);
~~~

Una fila de tabla es una tupla de argumentos. Mantén su orden idéntico al orden de parámetros de la macro template correspondiente.

El motor de templates también expone auxiliares de declaração/implementação de nivel inferior, pero los cabeçalhos públicos de módulos deben encapsular esos detalles. Los consumidores deben utilizer los puntos de entrada de instanciación documentados por el módulo, em vez de invocar macros internas de implementação.

### Convención .def de modo dual

El archivo `.def` de un template define la macro template y admite instanciación directa cuando se definen las macros de configuración esperadas. El cabeçalho público normalmente inclui definiciones internas compartidas, declara su pacote/objeto e inclui la definición del template. No incluyas archivos internos `Impl` o `Common.inc` directamente desde el código da aplicação.

## Implementación y escopos

Las implementações no inline se emiten mediante declarações explícitas de implementação de módulo. El sistema de declarações actual proporciona `MODULE_IMPLEMENT`, `OBJECT_IMPLEMENT_EX` y `PACKAGE_IMPLEMENT_EX`. Los auxiliares de escopo como `MODULE_SCOPE`, `OBJECT_SCOPE` y `PACKAGE_SCOPE` organizan implementações relacionadas y terminan con su macro `*_SCOPE_END` correspondiente.

Un cabeçalho público generalmente contiene declarações e inclui su unidad de implementação. Mantén los cuerpos de função en los archivos de implementação proporcionados por el módulo; los consumidores solo deben incluir el cabeçalho público.

## Function tables

Las function tables proporcionan una ruta de despacho tipada para pacotes y objetos. Son una salida opcional de compilación, no un runtime independiente de métodos virtuales.

- Utilize `PACKAGE_API` para despachar funções de pacote por meio de uma tabela de pacote.
- Utilize `API` para despachar métodos de objeto mediante la tabla del objeto (`OBJECT_CALL` es un alias de compatibilidad).
- Utilize `MODULE_FUNC` para invocación directa de símbolos.
- Configura las opciones de desactivación pertinentes antes de incluir los cabeçalhos.
- Mantén la misma configuración entre unidades de traducción que compartan las declarações.

Desabilitar una function table puede cambiar el layout del objeto generado. No mezcles objetos construidos con políticas de tabla diferentes.

## Guía práctica

- Utilize `PACKAGE` para operaciones sin estado y `OBJECT` solo cuando el estado pertenezca a la instancia.
- Utilize `STRUCT` para valores POD; no tiene receptor ni function table de objeto.
- Utilize `OPERATION` sempre que una rutina pueda fallar y devolver `OPSTATUS`.
- Utilize `FUNCTION` para resultados directos y `PREDICATE` para preguntas booleanas.
- Utilize `CALLBACK` para ponteiros a função simples; utilize `CLOSURE` cuando el contexto deba acompañar al callable.
- Mantén los detalles de templates en archivos `.def` y expón un cabeçalho público limpio.
- Trata todos los ponteiros pasados como contexto, datos de callback o campos según su contrato de propriedade documentado; el sistema Module no los posee automáticamente.

## Módulos relacionados

- [Algorithm](Algorithm.md) — uso práctico de closures en repetición y comparación.
- [Condition](Condition.md) — estados y resultados de operaciones que pueden fallar.
- [Types](Types.md) — descriptores de tipos primitivos y traits.
- [Preprocessor](Preprocessor.md) — auxiliares de tokens y tablas utilizedos por la capa declarativa.

## Cabeçalhos de origen

- `Codespace/Cosmeron/Core/Module/Module.h`
- `Codespace/Cosmeron/Core/Module/Namespace.h`
- `Codespace/Cosmeron/Core/Module/Object.h`
- `Codespace/Cosmeron/Core/Module/Package.h`
- `Codespace/Cosmeron/Core/Module/Callback/Callback.h`
- `Codespace/Cosmeron/Core/Module/Closure/Closure.h`
- `Codespace/Cosmeron/Core/Module/Common/Template.impl`
- `Codespace/Cosmeron/Core/Module/Common/Declarative.impl`
