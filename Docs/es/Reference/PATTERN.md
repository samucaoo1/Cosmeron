# Patrón arquitectónico y de código de Cosmeron

Este documento describe la arquitectura, las convenciones de nombres, las declaraciones y los patrones de implementación del ecosistema Cosmeron.

> **Filosofía central:** prefiere las construcciones nativas de C a las macros del preprocesador. Utiliza macros para vincular namespaces, generar código declarativo y realizar procesamiento interno. Los números mágicos y las capas de compatibilidad obsoletas están prohibidos.

## 1. Capas y jerarquía arquitectónica

Cosmeron organiza el código en cuatro niveles declarativos:

```
DOMAIN
  └── MODULE
        ├── PACKAGE
        ├── STRUCT
        └── OBJECT
              ├── SPACE
              └── VARIANT
```

- **DOMAIN** agrupa módulos por área arquitectónica o directorio, como Core, Container, Text, Security e IO.
- **MODULE** es la unidad fundamental de compilación y semántica, como Memory, File, Bit y Text. Puede existir sin DOMAIN; el dominio es solo organizativo.
- **PACKAGE** reúne funciones libres, eventos, callbacks y subnamespaces estáticos. No tiene receptor `self`, estructura de instancia ni tipo con prefijo `T`.
- **STRUCT** es un contenedor POD puro, sin puntero `api`, FunctionTable ni receptor. Genera un tipo público con prefijo `T` y admite inicializadores designados de C.
- **ISTRUCT** es un POD interno, con nombre bajo `Module_Internal_` y sin prefijo `T`.
- **INTERNAL / INTERNAL_EX** declaran infraestructura y auxiliares internos, normalmente como funciones `static inline`, sin coste de FunctionTable.
- **OBJECT** representa una instancia con estado, campos, métodos vinculados a `self`, auxiliares de ciclo de vida y tipo público con prefijo `T`.
- **SPACE** crea un subnamespace dentro de PACKAGE u OBJECT.
- **VARIANT** agrupa una familia plana de sobrecargas bajo un prefijo común.

### 1.1 Identidad del módulo

Ambas formas son válidas:

```c
MODULE(DOMAIN(Core), Memory, ...);
MODULE(Memory, ...);
```

En ambos casos, los símbolos C canónicos siguen el namespace del módulo, con el prefijo de biblioteca configurado cuando corresponda.

### 1.2 SPACE y VARIANT

`SPACE(Name, ...)` crea símbolos y tablas anidadas. `VARIANT(Family, ...)` agrupa operaciones relacionadas: la operación base usa el nombre de la familia y las variantes añaden un sufijo.

## 2. Nombres y convenciones de mayúsculas

Todo debe permanecer dentro del namespace del módulo/package siempre que C lo permita. Las excepciones se limitan a macros públicas de conveniencia aprobadas previamente, `OPSTATUS` y aliases cortos de comparación terminados en `OUT`.

| Elemento | Convención | Ejemplo |
|---|---|---|
| Domain, Module, Package y Object | PascalCase | `Core`, `Memory`, `Repeat` |
| Tipo público | PascalCase con `T` | `TVector_uint32`, `TMemoryPool` |
| Enum de salida/tipo interno | PascalCase sin `T` | `ComparisonResult`, `SeekOrigin` |
| Función/método | PascalCase | `Memory_Copy`, `Vector_Push` |
| Constante/item de enum | SNAKE_CASE | `SUCCESS`, `OUT_OF_RANGE` |
| Macro del preprocesador | SNAKE_CASE | `MODULE`, `PACKAGE_CALL` |
| Macro interna | `COSMERON_MACRO_INTERNAL_*` | `COSMERON_MACRO_INTERNAL_PKG_CALL_0` |
| Parámetro, variable o campo | camelCase | `destination`, `bufferCapacity` |

### 2.1 Regla del prefijo T

Utiliza `T` solo en tipos destinados al uso directo por parte del consumidor. Los tipos de objetos generados reciben el prefijo automáticamente. Los enums de salida y tipos internos no usan `T`. Evita nombres genéricos ambiguos como `TPool` y `TNode`; prefiere nombres semánticos explícitos como `TMemoryPool` y `TGraphNode`.

### 2.2 Macros de namespace

Prefiere las macros semánticas `MODULE_TYPE`, `MODULE_TYPED`, `MODULE_ENUM`, `MODULE_CONST`, `MODULE_FUNC`, `MODULE_NS` y `DOMAIN_NS` a las concatenaciones manuales. `NS_JOIN` realiza la unión plana en tiempo constante de expansión.

No crees archivos `.space` ni macros locales de namespace en headers, como `FOO_NS`, `FOO_FUNC`, `FOO_TYPE`, `FOO_CONST`, `GNS2` o `CNS2`. Utiliza las macros canónicas de `Core/Module/Namespace.h`.

### 2.3 Organización de archivos y Common.inc

No crees headers sintéticos `Types.h`: los tipos y enums pertenecen al header de la funcionalidad correspondiente. Reserva `Common.inc` exclusivamente para infraestructura interna compartida dentro del módulo, evitando duplicaciones entre packages. Los consumidores no deben incluirlo directamente.

### 2.4 Configuración del namespace de la biblioteca

`LIBRARY_NAMESPACE_CONFIG` o las flags de compilación pueden configurar el prefijo de los símbolos:

- `BARE`: sin prefijo de biblioteca, por ejemplo `Text_TString`;
- `SHORT`: prefijo corto, por ejemplo `Cos_Text_TString`;
- `STANDARD`: prefijo predeterminado `Cosmeron_`;
- `FULL`: también incluye el dominio, por ejemplo `Cosmeron_Core_Text_TString`.

## 3. Contratos de funciones y métodos

Las funciones y métodos deben pertenecer a bloques declarativos y seguir contratos explícitos:

| Contrato | Macro | Retorno | Significado |
|---|---|---|---|
| Operación falible | `OPERATION(Name, Params)` | `OPSTATUS` | Acción que informa un status |
| Predicado | `PREDICATE(Name, Params)` | `bool` | Consulta lógica |
| Función de valor | `FUNCTION(Ret, Name, Params)` | `Ret` | Produce un valor |
| Procedimiento | `PROCEDURE(Name, Params)` | `void` | Acción sin fallo reportable |
| Comparación de tres vías | `COMPARISON(Name, Params)` | `CMPOUT` | Devuelve LOWER, EQUAL o HIGHER |

### 3.1 Receptor y variantes inline

En OBJECT, los métodos reciben `self`; los métodos de solo lectura usan `CONST_*` y un receptor constante. Las definiciones inline utilizan el sufijo `_INLINE` e incorporan el cuerpo en la declaración.

```c
OPERATION_INLINE(Clear, (), { self->count = 0; return STATUS(SUCCESS); })
PREDICATE_INLINE(IsEmpty, (size_t size), { return size == 0U; })
```

### 3.2 Errores y parámetros

- Los parámetros `out` obligatorios no pueden ser nulos; en ese caso, devuelve `STATUS(INVALID_ARGUMENT)`.
- Modifica un parámetro `out` solo cuando el retorno sea `STATUS(SUCCESS)`.
- Los punteros son prestados por defecto; marca las entradas de solo lectura con `const`.
- Nunca devuelvas enteros arbitrarios como status. Utiliza constantes con nombre, como `STATUS(SUCCESS)` y `STATUS(OUT_OF_RANGE)`.

## 4. Declaraciones en headers

Los headers declaran funcionalidades mediante `MODULE(...)`, que puede contener PACKAGE, OBJECT, ENUM, SPACE, VARIANT o templates.

```c
MODULE(DOMAIN(Core), StreamBuffer,
    ENUM(SeekOrigin, SEEK_TABLE),
    STRUCT(Point,
        FIELDS(
            int32_t x;
            int32_t y;
        )
    ),
    OBJECT(Buffer,
        FIELDS(
            char data[64];
            size_t size;
        ),
        OPERATION(Clear, ()),
        OPERATION(Write, (const char *src, size_t count)),
        CONST_FUNCTION(size_t, Length, ()),
        CONST_PREDICATE(IsEmpty, ()),
        CONST_COMPARISON(Compare, (const StreamBuffer_TBuffer *other))
    ),
    PACKAGE(Utils,
        FUNCTION(size_t, StrLen, (const char *str)),
        PREDICATE(IsStrEmpty, (const char *str)),
        SPACE(Transform,
            PROCEDURE(ToUpper, (char *dest, const char *src))
        ),
        VARIANT(Alloc,
            FUNCTION(void *, (size_t size)),
            FUNCTION(void *, AndFill, (size_t size, char fillChar))
        )
    ),
    TEMPLATE_PACKAGE(BitOps, (SUFFIX, TYPE),
        OPERATION(Set, (TYPE *value, uint8_t bit)),
        PREDICATE(Check, (TYPE value, uint8_t bit))
    )
);
```

## 5. Templates

Cosmeron utiliza un mecanismo canónico de templates `.def` de modo dual y fuente única de verdad. El cuerpo declarativo se escribe una sola vez en el archivo `<Target>.def`.

### 5.1 Modos de uso

1. **Exportación de macro (recomendado):** incluir el `.def` sin parámetros definidos previamente. El archivo exporta `TEMPLATE_<Module>_<Target>`, permitiendo instanciación bajo demanda con `TEMPLATE_INSTANTIATE` o por lotes con `TEMPLATE_INSTANTIATE_TABLE`.
2. **Instanciación por inclusión directa:** definir previamente los parámetros, como `SUFFIX` y `TYPE`, incluir el `.def` y permitir que instancie el template y elimine los parámetros con `#undef`.

### 5.2 Reglas

1. El cuerpo del template debe existir en un único lugar; no dupliques listas de miembros ni campos.
2. Utiliza `TEMPLATE_<Module>_<Target>(PARAMS...)` como nombre de macro.
3. Los tipos públicos generados siguen `T<Target>_<suffix>`; los tipos internos omiten `T`.
4. El orden de parámetros es homogéneo: identificador `SUFFIX`, tipos de datos y, por último, metadatos como capacidad o política.
5. Aridades habituales: `(SUFFIX, TYPE)`; `(SUFFIX, FIRST_TYPE, SECOND_TYPE)`; o combinaciones con `CAPACITY`.
6. Las filas X-macro deben poder desempaquetarse directamente mediante `PP_UNWRAP ROW`.
7. Los STRUCT POD y templates con métodos inline son autocontenidos. No requieren macros stub `_IMPLEMENT`; los métodos no inline se implementan en `.impl` cuando existan.

### 5.3 Patrón recomendado para templates de módulo

Cuando un template de módulo genera packages u objetos parametrizados directamente en el namespace del módulo:

- Utiliza `MODULE_TEMPLATE` en el `.def` para distinguir las definiciones-template de las declaraciones fijas.
- Coloca algoritmos y constantes independientes del tipo/ancho en `INTERNAL(...)` dentro de `Impl/Common.inc`, sin FunctionTable.
- Declara constantes numéricas internas mediante X-Macro y ENUM; evita `#define` numéricos sin nombre y números mágicos.
- Agrupa familias relacionadas con `VARIANT`.

## 6. Ámbito de las implementaciones (.impl / .c)

Los archivos de implementación utilizan ámbito léxico mediante `*_SCOPE` y `*_SCOPE_END()`. No se utilizan macros finalizadoras `PACKAGE_IMPLEMENT` ni `OBJECT_IMPLEMENT`.

### 6.1 Implementación de PACKAGE

```c
#pragma once

PACKAGE_SCOPE(StreamBuffer, Utils)

PACKAGE_FUNCTION_IMPLEMENT_EX(StreamBuffer_Utils, size_t, StrLen, (const char *str)) {
    if (!str) return 0U;
    return strlen(str);
}

PACKAGE_PREDICATE_IMPLEMENT_EX(StreamBuffer_Utils, IsStrEmpty, (const char *str)) {
    return !str || str[0] == '\\0';
}

SPACE_SCOPE(Transform)
PACKAGE_PROCEDURE_IMPLEMENT_EX(StreamBuffer_Utils_Transform, ToUpper,
                               (char *dest, const char *src)) {
    if (!dest || !src) return;
    for (size_t i = 0; src[i] != '\\0'; ++i) {
        char c = src[i];
        if (c >= 'a' && c <= 'z') c = (char)(c - ('a' - 'A'));
        dest[i] = c;
    }
}
SPACE_SCOPE_END()
PACKAGE_SCOPE_END()
```

### 6.2 Implementación de OBJECT

```c
#pragma once

OBJECT_SCOPE(StreamBuffer, Buffer)

OBJECT_OPERATION_IMPLEMENT_EX(StreamBuffer_Buffer, Clear, ()) {
    if (!self) return STATUS(INVALID_ARGUMENT);
    self->size = 0U;
    self->data[0] = '\\0';
    return STATUS(SUCCESS);
}

OBJECT_FUNCTION_IMPLEMENT_EX(StreamBuffer_Buffer, size_t, Length, ()) {
    if (!self) return 0U;
    return self->size;
}

OBJECT_PREDICATE_IMPLEMENT_EX(StreamBuffer_Buffer, IsEmpty, ()) {
    return !self || self->size == 0U;
}
OBJECT_SCOPE_END()
```

## 7. Callbacks, closures y eventos declarativos

### 7.1 Callbacks puros (TCALLBACK)

Los callbacks son punteros de función puros y sin estado, sin coste adicional: ocupan exactamente `sizeof(void *)`. Pueden declararse de forma independiente o en bloques variádicos dentro de MODULE, PACKAGE u OBJECT.

Declaraciones disponibles: `CALLBACK_OPERATION`, `CALLBACK_PREDICATE`, `CALLBACK_FUNCTION`, `CALLBACK_PROCEDURE` y `CALLBACK_COMPARISON`.

```c
MODULE(DOMAIN(Core), MyModule,
    CALLBACK(
        FUNCTION(int, Transform, (int x)),
        PROCEDURE(Log, (int code))
    )
);
```

Utiliza `CALLBACK_CALL` o `CALLBACK_INVOKE` para invocar, `CALLBACK_IS_VALID` para validar y `CALLBACK_EMPTY` para representar ausencia (`NULL`).

### 7.2 Closures contextuales (TCLOSURE)

Las closures combinan un puntero de función y un contexto prestado opcional en una estructura POD de dos punteros. Hay declaraciones correspondientes a operación, predicado, función, procedimiento y comparación.

- `CLOSURE_FUNC(Name_Bind)(fn, ctx)` vincula un destino con estado que recibe `void *context` al final.
- `CLOSURE_FUNC(Name_FromCallback)(cb)` promueve un callback puro sin wrapper.
- `CLOSURE_CALL` selecciona la llamada directa cuando el contexto es nulo o la llamada contextual cuando no lo es.
- `CLOSURE_ADAPT_OBJECT_METHOD` adapta métodos con receptor a firmas de closure.
- `CLOSURE_AS_CLOSURE` crea una closure a partir de una instancia de objeto.

### 7.3 Eventos

`EVENT_PROCEDURE` y `EVENT_OPERATION` ofrecen notificación observador 1-a-N sin asignación, utilizando TCLOSURE. Los slots están integrados en estructuras o packages. La emisión crea un snapshot local de los listeners activos, haciendo que la iteración sea segura frente a la reentrancia. `OBJECT_MUTATE` actualiza un campo y emite el evento cuando cambia.

## 8. FunctionTables y desactivación en compilación

Los packages y objects pueden despachar mediante FunctionTable. Los packages utilizan una tabla singleton y `PACKAGE_CALL`; los objects usan `obj->api` y `OBJECT_CALL`.

La tabla puede eliminarse durante la compilación:

- `MODULE_DISABLE_FUNCTION_TABLE`: todas las tablas;
- `PACKAGE_DISABLE_FUNCTION_TABLE`: packages;
- `OBJECT_DISABLE_FUNCTION_TABLE`: objects;
- flags específicas por módulo/objetivo, evaluadas durante la expansión sin afectar a los módulos siguientes;
- tag declarativa `NO_FUNCTION_TABLE` dentro de la declaración.

Cuando se desactiva, no se emite ninguna estructura ni instancia de FunctionTable.

## 9. Enums y patrón de tablas X-macro

Todo enum debe utilizar una tabla X-macro desacoplada, siguiendo el patrón Row-Tuple:

```c
#define SEEK_COL_NAME(NAME, VALUE, DESC)  NAME
#define SEEK_COL_VALUE(NAME, VALUE, DESC) VALUE
#define SEEK_COL_DESC(NAME, VALUE, DESC)  DESC
#define SEEK_GET(FIELD, ROW) PP_OP_CAT2(SEEK_COL_, FIELD) ROW

#define SEEK_TABLE(X) \\
  X((SET, 0, "Seek from beginning")) \\
  X((CUR, 1, "Seek from current position")) \\
  X((END, 2, "Seek from end"))
```

Los enums pueden declararse en MODULE, PACKAGE u OBJECT, o fuera de ellos con `ENUM_DEF` y `ENUM_DEF_EX`. Presets de dispatch:

- `PLAIN` / `NAME_VALUE`: emite la constante con nombre y valor.
- `QUALIFIED`: incluye el nombre del enum para evitar colisiones.
- `NAME_ONLY`: emite solo el nombre y permite que C incremente el valor.
- `QUALIFIED_NAME`: nombre cualificado sin valor explícito.
- Dispatch personalizado: macro `DISPATCH(TARGET, ENUM_NAME, ROW)`.

## 10. Gramática canónica de consumo

Los consumidores deben utilizar la gramática unificada de `Namespace.h`:

```c
MODULE_TYPE(StreamBuffer, Buffer) buf;
MODULE_TYPE(Geometry, Point) pt = { .x = 10, .y = 20 };
MODULE_TYPED(Geometry, Pair, int) pair = { .first = 1, .second = 2 };
MODULE_TYPED(Outcome, Optional, int32) opt;

MODULE_ENUM(File, SeekOrigin) origin = MODULE_CONST(File, CUR);
MODULE_NS(StreamBuffer, Utils)
MODULE_FUNC(StreamBuffer, Utils, StrLen)("test");
DOMAIN_NS(Core, Algorithm, Repeat);

OPSTATUS status = STATUS(SUCCESS);

MODULE_CALL(StreamBuffer, Buffer, Init, (&buf));
MODULE_CALL(StreamBuffer, Buffer, Clear, (&buf));
size_t len = MODULE_CALL(StreamBuffer, Utils, StrLen, ("test"));
MODULE_CALL(StreamBuffer, Utils, Transform, ToUpper, (dest, src));

StreamBuffer_Buffer_Clear(&buf);
StreamBuffer_Utils_StrLen("test");
```

## 11. Reglas estrictas de refactorización (sin retrocompatibilidad)

1. Elimina los archivos `.space`; nunca los crees, mantengas ni incluyas.
2. Elimina las macros boilerplate de namespace en los headers.
3. No crees `Types.h` sintéticos; mantén los tipos junto a sus funcionalidades y reserva `Common.inc` para utilidades internas compartidas.
4. No crees macros blueprint que dupliquen declaraciones de templates.
5. No mantengas capas de retrocompatibilidad, wrappers ni typedefs heredados.
6. No utilices números mágicos; usa status con nombre y enums X-macro.
7. No hagas detección ad hoc de plataforma/compilador. Utiliza exclusivamente `Core/Preprocessor/Detect.h`.
8. Los identificadores, comentarios, mensajes de commit y documentación del código deben permanecer en inglés.

## 12. Detección centralizada de plataforma y entorno

Todo código dependiente del entorno debe incluir `Core/Preprocessor/Detect.h` o `Detect/Detect.h`. La detección proporciona constantes de compilación normalizadas a 0 o 1:

- **Sistema operativo:** `OS_WINDOWS`, `OS_LINUX`, `OS_MAC`, `OS_IOS`, `OS_BSD`, `OS_FREEBSD`, `OS_OPENBSD`, `OS_NETBSD`, `OS_DRAGONFLY`, `OS_SOLARIS`, `OS_AIX`, `OS_HAIKU`, `OS_QNX`, `OS_HURD`, `OS_REDOX`, `OS_WASI`, `OS_EMSCRIPTEN`, `OS_POSIX`, `OS_UNIX`, `OS_ANDROID`, `OS_OHOS`.
- **Compilador:** `COMPILER_CLANG`, `COMPILER_GCC`, `COMPILER_MSVC`, `COMPILER_INTEL`, `COMPILER_KNOWN`.
- **Arquitectura:** `PROCESSOR_X86`, `PROCESSOR_X64`, `PROCESSOR_ARM`, `PROCESSOR_ARM64`, `PROCESSOR_RISCV`, `PROCESSOR_WASM`, `PROCESSOR_PPC`, `PROCESSOR_PPC64`, `PROCESSOR_MIPS`, `PROCESSOR_LOONGARCH64`, `PROCESSOR_S390X`, `PROCESSOR_KNOWN`.
- **Biblioteca C:** `LIBC_GLIBC`, `LIBC_MUSL`, `LIBC_BIONIC`, `LIBC_MSVC`, `LIBC_APPLE`, `LIBC_BSD`, `LIBC_UCLIBC`, `LIBC_NEWLIB`, `LIBC_WASI`, `LIBC_EMSCRIPTEN`, `LIBC_RELIBC`, `LIBC_KNOWN`.

## Archivos de referencia

- `Codespace/Cosmeron/Core/Module/Namespace.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect.h`


---

## Complemento de especificación: detalles de templates, closures y enums

### Archivos de definición de templates en modo dual

El archivo `.def` es la única fuente de verdad del template. El cuerpo declarativo debe escribirse una sola vez, utilizando la DSL canónica (`TEMPLATE_STRUCT`, `TEMPLATE_PACKAGE` o `TEMPLATE_OBJECT`). Hay dos modos de uso:

1. **Exportación de macro (principal y recomendada):** el archivo se incluye sin parámetros definidos previamente y exporta `TEMPLATE_<Module>_<Target>`. La macro puede instanciarse bajo demanda mediante `TEMPLATE_INSTANTIATE(Module, Target, Suffix, ConcreteType, ...)` o en lote mediante `TEMPLATE_INSTANTIATE_TABLE(Module, Target, TYPE_TABLE)`.
2. **Instanciación mediante inclusión directa:** el consumidor define los parámetros (por ejemplo, `SUFFIX` y `TYPE`) antes de incluir el archivo. El propio `.def` instancia el template y ejecuta `#undef` sobre los parámetros, restaurando un entorno limpio.

Ejemplo de definición:

```c
#ifndef TEMPLATE_Struct_Triple
#define TEMPLATE_Struct_Triple(SUFFIX, TYPE) \\
  TEMPLATE_STRUCT(Struct, Triple, SUFFIX, FIELDS( \\
      union { \\
        struct { TYPE first; TYPE second; TYPE third; }; \\
        TYPE values[3]; \\
      }; \\
  ))
#endif

#if defined(SUFFIX) && defined(TYPE)
  TEMPLATE_Struct_Triple(SUFFIX, TYPE);
  #undef SUFFIX
  #undef TYPE
#endif
```

El orden de los parámetros es uniforme: primero el identificador `SUFFIX`, después los tipos de datos y, por último, los metadatos estáticos, como capacidad o política. Ejemplos de aridad: `(SUFFIX, TYPE)`, `(SUFFIX, FIRST_TYPE, SECOND_TYPE)`, `(SUFFIX, TYPE, CAPACITY)` y `(SUFFIX, KEY_TYPE, VALUE_TYPE, CAPACITY)`. Las tablas X-macro deben desempaquetarse directamente con `PP_UNWRAP ROW`, sin macros intermedias de enlace.

Las estructuras POD y los templates cuyos métodos son inline son autocontenidos. El motor de templates no requiere macros stub `_IMPLEMENT`; los archivos `.impl` solo son necesarios cuando existen miembros no inline.

### Patrón de referencia para templates de módulo

Cuando un template de módulo genera packages u objects parametrizados directamente en el namespace del módulo:

1. Utiliza `MODULE_TEMPLATE` en el `.def`, en lugar de una declaración fija `MODULE`, para identificar explícitamente una definición-template.
2. Coloca los algoritmos independientes del tipo y del ancho en `INTERNAL(...)`, dentro de `Impl/Common.inc`. Estas funciones auxiliares no deben generar FunctionTable.
3. Declara las constantes numéricas internas mediante una tabla X-macro y `ENUM`; evita los `#define` numéricos sin nombre y los números mágicos. Las máscaras de 64 bits deben ser constantes con nombre dentro de los auxiliares `INTERNAL`.
4. Agrupa familias relacionadas en bloques `VARIANT`, por ejemplo operaciones de máscara, recuento de bits, logaritmo, índice y rotación.

### Layout y despacho de closures

Una closure contextual combina un puntero a función y un contexto prestado opcional en una estructura POD de dos punteros (`sizeof == 2 * sizeof(void *)`). El miembro de función puede compartir una unión con el callback puro correspondiente.

- `CLOSURE_FUNC(Name_Bind)(fn, ctx)` vincula una función que recibe `void *context` como último argumento.
- `CLOSURE_FUNC(Name_FromCallback)(cb)` convierte un callback puro sin crear un wrapper.
- `CLOSURE_CALL(cl, args...)` comprueba el contexto: si es nulo, invoca directamente el callback; en caso contrario, elimina el centinela de contexto nulo e invoca la función contextual con el contexto al final de los argumentos.
- `CLOSURE_ADAPT_OBJECT_METHOD` adapta un método que recibe `CLASS *self` a la firma de closure.
- `CLOSURE_AS_CLOSURE(METHOD_FUNC, self)` crea una closure lista para usar a partir de una instancia de object.

### Eventos declarativos

`EVENT_PROCEDURE` y `EVENT_OPERATION` implementan notificaciones observador de uno a muchos sin asignación dinámica, mediante `TCLOSURE`. Los slots están integrados en structs o packages. La emisión crea una copia local (snapshot) de los listeners activos antes de invocarlos, protegiendo la iteración frente a la reentrancia. `OBJECT_MUTATE(self, ClassName, fieldName, newValue, EventName)` actualiza un campo y emite el evento cuando el valor realmente cambia.

### Despacho de enums

El patrón Row-Tuple permite extraer columnas de cada fila X-macro. Los presets de dispatch son:

- `PLAIN` o `NAME_VALUE`: emite una constante cualificada con nombre y valor explícito.
- `QUALIFIED`: incluye también el nombre del enum, evitando colisiones entre elementos homónimos.
- `NAME_ONLY`: emite solo el nombre y deja que el compilador C incremente los valores.
- `QUALIFIED_NAME`: emite el nombre cualificado, sin valor explícito.
- Dispatch personalizado: recibe `DISPATCH(TARGET, ENUM_NAME, ROW)` y puede transformar la fila, por ejemplo en máscaras de bits.

Las declaraciones fuera de `MODULE` utilizan `ENUM_DEF` o `ENUM_DEF_EX`, que reciben el namespace de destino, el nombre del enum, la tabla y, en la forma extendida, el preset de dispatch.


### Ejemplos complementarios de referencia

#### Tabla de tipos para instanciación en lote

```c
#define STRUCT_TYPE_TABLE(X) \\
  X((uint8, uint8_t))        \\
  X((uint16, uint16_t))      \\
  X((uint32, uint32_t))
```

Cada fila proporciona el sufijo y el tipo C correspondiente. El desempaquetado debe seguir siendo compatible con `PP_UNWRAP ROW`.

#### Estructura POD de closure

```c
typedef struct TClosure_Name {
  union {
    TCallback_Name callback;
    Name_Function function;
  };
  void *context;
} TClosure_Name;
```

El puntero de contexto es prestado: la closure no adquiere la propiedad del objeto apuntado.

#### Declaración variádica de callbacks

```c
MODULE(DOMAIN(Core), MyModule,
  CALLBACK(
    FUNCTION(int, Transform, (int x)),
    PROCEDURE(Log, (int code))
  )
);
```

#### Declaración variádica de closures

```c
MODULE(DOMAIN(Core), MyModule,
  CLOSURE(
    FUNCTION(int, Compute, (int x)),
    PREDICATE(Validate, (int x))
  )
);
```

#### Declaraciones independientes de enum

```c
ENUM_DEF(MODULE_NS(File, Stream), Seek, SEEK_TABLE);
ENUM_DEF_EX(MODULE_NS(File, Stream), Seek, SEEK_TABLE, QUALIFIED);
```
