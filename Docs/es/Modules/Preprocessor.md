# Preprocessor

El módulo **Preprocessor** proporciona utilidades para la metaprogramación con macros en C: conteo de argumentos, concatenación de tokens, selección booleana, expansión repetida, listas, generación de tokens y detección del destino.

Es **exclusivamente de tiempo de compilación**. A diferencia de los módulos comunes de Cosmeron, estas API no tienen funciones C invocables. Por ello, las entradas de *Forma directa* muestran expansiones literales o indican claramente que no existe un equivalente directo en C.

## Headers

```c
#include "Cosmeron/Core/Preprocessor/Arguments.h"
#include "Cosmeron/Core/Preprocessor/Boolean.h"
#include "Cosmeron/Core/Preprocessor/Operations.h"
#include "Cosmeron/Core/Preprocessor/Eval.h"
#include "Cosmeron/Core/Preprocessor/Foreach.h"
#include "Cosmeron/Core/Preprocessor/Map.h"
#include "Cosmeron/Core/Preprocessor/Unroll.h"
#include "Cosmeron/Core/Preprocessor/Sequence.h"
#include "Cosmeron/Core/Preprocessor/Tree.h"
#include "Cosmeron/Core/Preprocessor/Tokens.h"
#include "Cosmeron/Core/Preprocessor/While.h"
#include "Cosmeron/Core/Preprocessor/Detect/Detect.h"
```

---

# Descripción general

| Paquete | Header | Propósito |
| --- | --- | --- |
| [Operations](#operaciones-con-tokens) | `Operations.h` | Expansión y concatenación de tokens |
| [Arguments](#conteo-de-argumentos) | `Arguments.h` | Conteo de argumentos variádicos |
| [Boolean](#operaciones-booleanas) | `Boolean.h` | Selección condicional de tokens |
| [Eval](#control-de-expansión) | `Eval.h` | Expansiones recursivas diferidas |
| [Foreach](#foreach-y-map) | `Foreach.h` | Aplicación iterativa de macros multidimensional |
| [Map](#pp_map) | `Map.h` | Motor de evaluación anidable (Motor 2) |
| [Repetition](#repetición) | `Unroll.h` | Expansión repetida por índice e incremento/decremento |
| [Sequence](#selección-de-secuencias) | `Sequence.h` | Tuplas separadas por comas y secuencias entre paréntesis (SEQ) con fold funcional |
| [Tree](#reducción-en-árbol) | `Tree.h` | Reducción en árbol binario y concatenación de tokens en $O(\log_2 N)$ |
| [Tokens](#auxiliares-de-tokens) | `Tokens.h` | Emisión de tokens de puntuación |
| [While](#repetición-condicional) | `While.h` | Transformación repetida de estado |
| [Detect](#detección-de-plataforma) | `Detect/*.h` | Flags de compilador, sistema operativo, biblioteca C y procesador |
| [Compiling](#diagnósticos-de-inclusión) | `Compiling.inc` | Mensajes opcionales de diagnóstico de inclusión |

### Forma de macro

```c
#define MODULE Network
PP_OP_CAT2(MODULE, _Init) /* Network_Init */
```

### Forma directa

```c
Network_Init /* El identificador literal resultante. */
```

Ambas formas son equivalentes **como tokens**. No representan dos funciones públicas de tiempo de ejecución.

### Límites implementados

| Familia | Rango admitido |
| --- | --- |
| `PP_ARG_COUNT` | 1–256 argumentos no vacíos |
| `PP_FOREACH` (Dimensiones 1–5) | 1–256 argumentos no vacíos por dimensión |
| `PP_FOREACH_CHUNKS` (2D, 3D, 4D) | Hasta $256 \times 256 = 65.536$ (2D), $256^3 \approx 1,67 \times 10^7$ (3D), $256^4 \approx 4,29 \times 10^9$ (4D) |
| `PP_MAP` | Mapeo recursivo anidable sobre `PP_EVAL` (sujeto a límites de expansión) |
| `PP_BOOL` | Tokens enteros literales de 0 a 256 |
| `PP_OP_CAT` (variádico mediante Tree) y `PP_OP_CAT2`–`PP_OP_CAT32` | 1–32 operandos concatenados |
| `PP_REPEAT` | Conteos de 0 a 256 |
| `PP_DEC` y `PP_INC` | Entradas enteras de 0 a 256 |
| `PP_SEQ_GET_1`–`PP_SEQ_GET_256` | Índices de 1 a 256 |
| `PP_SEQ_*` (secuencias entre paréntesis) | 1–256 elementos para Size, Fold, Enum y Foreach |
| `PP_TREE_REDUCE` / `PP_TREE_JOIN` | 1–32 operandos directos ($O(\log_2 N)$, profundidad $\le 5$); hasta $32^4 = 1.048.576$ mediante Tree Chunks 4D |
| `PP_WHILE` | Limitado por la profundidad fija de expansión y los límites del compilador |

---

# Referencia de la API

## Resumen de macros

| Macro o familia | Descripción |
| --- | --- |
| `PP_OP_EXPAND` | Expansión normal de argumentos |
| `PP_OP_CAT` | Pegado primitivo de tokens |
| `PP_OP_CAT2` | Pegado expandido de dos tokens |
| `PP_OP_CAT3`–`PP_OP_CAT32` | Concatenación de múltiples tokens |
| `PP_ARG_COUNT` | Cuenta argumentos |
| `PP_BOOL` | Convierte tokens enteros admitidos a 0/1 |
| `PP_BOOL_NOT` | Niega |
| `PP_BOOL_IF` | Selecciona tokens condicionalmente |
| `PP_BOOL_IF_ELSE` | Elige una de dos expansiones |
| `PP_BOOL_AND` | AND lógico |
| `PP_BOOL_OR` | OR lógico |
| `PP_BOOL_XOR` | XOR lógico |
| `PP_EMPTY` | Expansión vacía |
| `PP_DEFER` | Difiere la invocación de una macro |
| `PP_OBSTRUCT` | Barrera de aplazamiento |
| `PP_EVAL` | Evaluación repetida |
| `PP_FOREACH` | Aplica una macro a cada elemento |
| `PP_MAP` | Mapeo recursivo anidable con separadores y contexto configurables |
| `PP_REPEAT` | Repite una macro por índice |
| `PP_DEC` | Decrementa un token entero limitado |
| `PP_SEQ_FIRST` | Selecciona el primero |
| `PP_SEQ_REST` | Selecciona la cola |
| `PP_SEQ_GET_1`–`PP_SEQ_GET_256` | Selecciona un elemento de una lista de argumentos |
| `PP_TOKEN_*` | Puntuación |
| `PP_WHILE` | Transformación condicional recursiva |
| Detección de compilador | Familia del compilador |
| Detección de sistema operativo | Sistema operativo de destino |
| Detección de procesador | Arquitectura de destino |
| Flag de mensaje de inclusión | Mensajes de compilación |

---

## Operaciones con tokens

Incluya `Cosmeron/Core/Preprocessor/Operations.h`. Las operaciones con tokens combinan o expanden **tokens del preprocesador**, no cadenas C.

### PP_OP_EXPAND

Reenvía una secuencia variádica de tokens mediante la expansión normal de argumentos.

**Sintaxis:** `PP_OP_EXPAND(...)`.

**Forma directa:** los tokens de argumento expandidos.

**Parámetro:** `...` — cualquier token que se deba expandir.

**Retorno:** no es un valor de ejecución C; la macro se expande a tokens del preprocesador.

**Observaciones**
- Definida como `#define PP_OP_EXPAND(...) __VA_ARGS__`.
- No actúa como un motor de expansión recursiva ilimitada; use `PP_EVAL` para patrones diferidos.

**Ejemplo**
```c
#define VALUE 42
int value = PP_OP_EXPAND(VALUE); /* int value = 42; */
```

### PP_OP_CAT

Pegado primitivo de dos tokens mediante `##`, sin preexpansión de los argumentos adyacentes.

**Sintaxis:** `PP_OP_CAT(a, b)`.

**Forma directa:** el token pegado literal, antes de un nuevo análisis.

**Parámetros:** `a` — primer token del preprocesador; `b` — segundo token.

**Retorno:** secuencia de tokens del preprocesador, no un valor C en tiempo de ejecución.

**Observaciones**
- Los argumentos adyacentes a `##` no se expanden primero.
- Úsela solo cuando sea intencional suprimir la expansión de los argumentos.
- El pegado debe producir un token válido; de lo contrario, el comportamiento del preprocesamiento no es válido.

**Ejemplo**
```c
#define ONE 1
#define NAME_1 7
int PP_OP_CAT(var_, ONE) = 3; /* int var_ONE = 3; */
```

### PP_OP_CAT2

Concatena dos operandos después de una capa adicional de expansión de macros.

**Sintaxis:** `PP_OP_CAT2(a, b)`.

**Forma directa:** el token concatenado después de expandir los argumentos, sujeto a un nuevo análisis normal.

**Parámetros:** `a` — primer operando; `b` — segundo operando.

**Observaciones**
- Llama a `PP_OP_CAT` mediante un wrapper, por lo que las macros de los argumentos pueden expandirse primero.
- Es la opción preferida para generar namespaces reutilizables.
- La concatenación sigue requiriendo un token válido.

**Ejemplo**
```c
#define PART Widget
int PP_OP_CAT2(PART, _Count) = 10;
/* int Widget_Count = 10; */
```

### PP_OP_CAT3 a PP_OP_CAT32

Concatena entre tres y dieciséis operandos mediante la composición de primitivas de concatenación con expansión.

**Sintaxis:** `PP_OP_CAT3(a, b, c)`, `PP_OP_CAT4(a, b, c, d)` y así sucesivamente hasta `PP_OP_CAT32`. La cantidad de argumentos requerida está indicada por el sufijo.

**Forma directa:** el token literal resultante; no existe una función C exportada equivalente.

**Parámetros:** cada argumento debe expandirse a un token que pueda pegarse válidamente a sus vecinos.

**Retorno:** un único token concatenado, posiblemente expandido después.

**Observaciones**
- La implementación define formas de hasta 16 operandos.
- El código de los módulos debe usar estas utilidades en lugar de aplicar `##` directamente fuera de la capa del preprocesador.

**Ejemplo**
```c
#define PREFIX Example
int PP_OP_CAT3(PREFIX, _, Field) = 1;
/* int Example_Field = 1; */
```

---

## Conteo de argumentos

Incluya `Cosmeron/Core/Preprocessor/Arguments.h`. La interfaz pública es `PP_ARG_COUNT`; la maquinaria `PP_ARG_RSEQ`, `PP_ARG_COUNT_IMPL` y `PP_ARG_COUNT_SELECT` implementa la consulta.

### PP_ARG_COUNT

Cuenta una lista no vacía de argumentos de macro del preprocesador.

**Sintaxis:** `PP_ARG_COUNT(item1, item2, ...)`.

**Forma directa:** literal entero que representa la cantidad de elementos.

**Parámetro:** `...` — lista separada por comas de 1–100 elementos.

**Retorno:** tokens del preprocesador, no un valor C en tiempo de ejecución.

**Observaciones**
- Admite hasta 100 argumentos.
- No se garantiza que una entrada vacía produzca cero.
- Se aplican las reglas del preprocesador para agrupar comas y paréntesis.

**Ejemplo**
```c
enum { count = PP_ARG_COUNT(alpha, beta, gamma) };
/* count == 3 */
```

---

## Operaciones booleanas

Incluya `Cosmeron/Core/Preprocessor/Boolean.h`. El dominio booleano está formado por **tokens enteros de 0 a 100**: 0 es falso y 1–100 son verdaderos. Estas macros no evalúan expresiones C arbitrarias, valores negativos ni literales enteros sin límite.

### PP_BOOL

Convierte un token entero admitido a 0 o 1.

**Sintaxis:** `PP_BOOL(x)`. **Forma directa:** 0 si `x` es 0; 1 si `x` está entre 1 y 100.

**Parámetro:** `x` — token entero de 0 a 100.

**Retorno:** token booleano literal 0 o 1.

**Observación:** convierte mediante una tabla de consulta interna generada.

**Ejemplo**
```c
int value = PP_BOOL(7); /* 1 */
```

### PP_BOOL_NOT

Niega un entero convertible a booleano.

**Sintaxis:** `PP_BOOL_NOT(x)`. **Forma directa:** 1 cuando `x` es 0; de lo contrario, 0.

**Parámetro:** `x` — token entero de 0 a 100.

**Retorno:** token literal 1 cuando `x` es 0; de lo contrario, 0.

**Observación:** normaliza con `PP_BOOL` antes de negar.

**Ejemplo**
```c
enum { yes = PP_BOOL_NOT(0), no = PP_BOOL_NOT(2) };
```

### PP_BOOL_IF

Emite tokens solo si la condición es verdadera.

**Sintaxis:** `PP_BOOL_IF(condition)(tokens)`.

**Forma directa:** la secuencia de tokens proporcionada o nada.

**Parámetros:** `condition` — token entero de 0 a 100; `tokens` — tokens proporcionados en la segunda invocación entre paréntesis.

**Retorno:** la secuencia proporcionada o una expansión vacía.

**Observación:** es una selección en tiempo de compilación, no un `if` de C.

**Ejemplo**
```c
PP_BOOL_IF(1)(int enabled = 1;)
PP_BOOL_IF(0)(int disabled = 1;)
```

### PP_BOOL_IF_ELSE

Elige una de dos secuencias de tokens entre paréntesis.

**Sintaxis:** `PP_BOOL_IF_ELSE(condition)(trueTokens)(falseTokens)`.

**Forma directa:** la secuencia seleccionada.

**Parámetros:** `condition` — token entero de 0 a 100; `trueTokens` — expansión usada para verdadero; `falseTokens` — expansión usada para falso.

**Retorno:** la secuencia de tokens elegida.

**Observación:** deben proporcionarse ambas ramas entre paréntesis.

**Ejemplo**
```c
enum { value = PP_BOOL_IF_ELSE(7)(11)(22) };
/* value == 11 */
```

### PP_BOOL_AND

Conjunción booleana.

**Sintaxis:** `PP_BOOL_AND(a, b)`. **Forma directa:** 1 si ambos son distintos de cero; de lo contrario, 0.

**Parámetros:** `a` y `b` — tokens enteros admitidos.

**Retorno:** token literal 1 si ambos son distintos de cero; de lo contrario, 0.

**Observación:** ambos operandos se normalizan a 0 o 1.

**Ejemplo**
```c
enum { value = PP_BOOL_AND(2, 4) }; /* 1 */
```

### PP_BOOL_OR

Disyunción inclusiva booleana.

**Sintaxis:** `PP_BOOL_OR(a, b)`. **Forma directa:** 1 si cualquiera es distinto de cero; de lo contrario, 0.

**Parámetros:** `a` y `b` — tokens enteros admitidos.

**Retorno:** token literal 1 si cualquiera es distinto de cero; de lo contrario, 0.

**Observación:** ambos operandos se normalizan.

**Ejemplo**
```c
enum { value = PP_BOOL_OR(0, 9) }; /* 1 */
```

### PP_BOOL_XOR

Disyunción exclusiva booleana.

**Sintaxis:** `PP_BOOL_XOR(a, b)`. **Forma directa:** 1 si exactamente uno es distinto de cero; de lo contrario, 0.

**Parámetros:** `a` y `b` — tokens enteros admitidos.

**Retorno:** token literal 1 si exactamente uno es distinto de cero; de lo contrario, 0.

**Observación:** ambos operandos se normalizan.

**Ejemplo**
```c
enum { value = PP_BOOL_XOR(0, 3) }; /* 1 */
```

---

## Control de expansión

Incluya `Cosmeron/Core/Preprocessor/Eval.h`. Estos auxiliares difieren y vuelven a analizar la expansión de macros. Implementan **recursión del preprocesador**, no bucles ni funciones en tiempo de ejecución.

### PP_EMPTY

Emite cero tokens.

**Sintaxis:** `PP_EMPTY()`. **Forma directa:** secuencia vacía de tokens.

**Parámetros:** ninguno. **Retorno:** ningún token.

**Observación:** se usa internamente como separador para el aplazamiento.

**Ejemplo**
```c
#define VALUE() 7
int number = PP_EMPTY() VALUE(); /* int number = 7; */
```

### PP_DEFER

Difiere la invocación de una macro tipo función insertando una barrera de macro vacía.

**Sintaxis:** `PP_DEFER(id)(arguments)`.

**Forma directa:** secuencia de tokens diferida, en lugar de una llamada inmediata a función.

**Parámetro:** `id` — identificador de macro tipo función.

**Retorno:** tokens del preprocesador, no un valor C en tiempo de ejecución.

**Observaciones**
- Definida como `id PP_EMPTY()`.
- Solo es útil con un nuevo análisis deliberado de macros; su sintaxis similar a una llamada no es una función C exportada.
- La cantidad de nuevos análisis antes de la invocación depende del contexto.

**Ejemplo**
```c
#define LATER() 5
#define REQUEST() PP_DEFER(LATER)()
/* REQUEST() usa una barrera de expansión alrededor de LATER. */
```

### PP_OBSTRUCT

Añade un aplazamiento más fuerte que permite la expansión indirecta recursiva de macros.

**Sintaxis:** `PP_OBSTRUCT(tokens...)`.

**Forma directa:** tokens proporcionados con una barrera adicional de macro vacía diferida.

**Parámetro:** `tokens...` — tokens del preprocesador que se deben obstaculizar temporalmente.

**Retorno:** tokens del preprocesador.

**Observaciones**
- Definida mediante `PP_DEFER(PP_EMPTY)()`.
- Normalmente se combina con `PP_EVAL` y recursión indirecta.
- La usa `PP_WHILE`.

**Ejemplo**
```c
#define INDIRECT() PP_EMPTY
PP_OBSTRUCT(INDIRECT)() /* Ejemplo de barrera de expansión. */
```

### PP_EVAL

Vuelve a analizar la entrada expandida mediante una cascada fija de auxiliares internos de expansión.

**Sintaxis:** `PP_EVAL(tokens...)`.

**Forma directa:** secuencia de tokens reanalizada después de la cascada fija de expansión.

**Parámetro:** `tokens...` — entrada que puede contener llamadas diferidas.

**Retorno:** tokens del preprocesador.

**Observaciones**
- La implementación usa `PP_EVAL1`, `PP_EVAL2` y `PP_EVAL3`.
- La profundidad de expansión es finita y depende del compilador.
- `PP_WHILE` ya aplica `PP_EVAL` internamente.

**Ejemplo**
```c
#define NUMBER 42
int result = PP_EVAL(NUMBER); /* 42 */
```

---

## Foreach y Map

`Foreach.h` proporciona un motor de iteración de macros multidimensional de alto rendimiento (`D1` a `D5`), compatible con hasta 256 argumentos por dimensión, además de un motor por bloques (`PP_FOREACH_CHUNKS`) para procesar conjuntos de datos grandes de hasta $256 \times 256 = 65.536$ elementos.

### PP_FOREACH

Aplica una macro una vez a cada elemento proporcionado, en orden, operando en la Dimensión 1 (bucle externo predeterminado).

**Sintaxis**
```c
PP_FOREACH(macro, a, b, c, ...)
PP_FOREACH_CTX(macro, context, a, b, c, ...)
PP_FOREACH_COMMA(macro, a, b, c, ...)
PP_FOREACH_COMMA_CTX(macro, context, a, b, c, ...)
PP_FOREACH_SEMI(macro, a, b, c, ...)
PP_FOREACH_SEMI_CTX(macro, context, a, b, c, ...)

PP_FOREACH_D(dimension, macro, ...)
PP_FOREACH_CTX_D(dimension, macro, context, ...)
PP_FOREACH_COMMA_D(dimension, macro, ...)
PP_FOREACH_SEMI_D(dimension, macro, ...)

/* Macros directas por dimensión: */
PP_FOREACH_D2(macro, ...)
PP_FOREACH_D2_CTX(macro, ctx, ...)
PP_FOREACH_D3_CTX(macro, ctx, ...)
PP_FOREACH_D4_CTX(macro, ctx, ...)
PP_FOREACH_D5_CTX(macro, ctx, ...)

/* Motor por bloques: */
PP_FOREACH_CHUNKS(macro, (chunk1), (chunk2), ...)
PP_FOREACH_CHUNKS_CTX(macro, context, (chunk1), (chunk2), ...)
PP_FOREACH_CHUNKS_COMMA(macro, (chunk1), (chunk2), ...)
PP_FOREACH_CHUNKS_SEMI(macro, context, (chunk1), (chunk2), ...)
```

**Forma directa:** expansiones yuxtapuestas `macro(a) macro(b) macro(c) ...`, o separadas por coma/punto y coma.

**Parámetros:** `macro` — macro unaria `macro(x)` o binaria `macro(context, x)`; `context` — argumento de contexto pasado como primer parámetro en invocaciones binarias; `...` — lista de 1–256 elementos (o bloques de elementos para `PP_FOREACH_CHUNKS`).

**Retorno:** tokens del preprocesador.

**Observaciones**
- Las variantes de separador insertan nada (`PP_FOREACH`), comas (`PP_FOREACH_COMMA`) o punto y coma (`PP_FOREACH_SEMI`).
- Las dimensiones `D1` a `D5` usan tokens internos distintos de llamada y despacho, lo que garantiza que no haya supresión de expansión al anidar iteraciones.
- Puede generar declaraciones, expresiones, tablas de funciones o entradas al estilo X-macro.

**Ejemplo**
```c
#define DECL_FIELD(name) int name;
struct Sample { PP_FOREACH(DECL_FIELD, width, height, depth) };
#undef DECL_FIELD
```

### PP_MAP

Motor de evaluación independiente y anidable (Motor 2), basado en `PP_EVAL`. A diferencia de `PP_FOREACH` (que concatena con espacios de forma predeterminada), `PP_MAP` usa comas por defecto y es completamente independiente de `PP_FOREACH`, lo que permite anidar `PP_MAP` dentro de `PP_FOREACH` sin supresión de expansión.

**Sintaxis**
```c
PP_MAP(macro, a, b, ...)
PP_MAP_CTX(macro, context, a, b, ...)
PP_MAP_SEMI(macro, a, b, ...)
PP_MAP_SEMI_CTX(macro, context, a, b, ...)
PP_MAP_NONE(macro, a, b, ...)
PP_MAP_NONE_CTX(macro, context, a, b, ...)
```

**Forma directa:** tokens emitidos separados por comas, punto y coma o ninguna separación.

**Parámetros:** `macro` — macro aplicada a cada elemento (unaria `macro(x)` o binaria con contexto `macro(ctx, x)`); `context` — contexto pasado a las invocaciones binarias; `...` — lista no vacía de argumentos.

**Retorno:** tokens del preprocesador.

**Observaciones**
- Separación por coma (`PP_MAP`), punto y coma (`PP_MAP_SEMI`) o ninguna (`PP_MAP_NONE`).
- Puede anidarse dentro de `PP_FOREACH` sin colisionar con los bloqueos de recursión.
- No construye una colección ni inserta separadores.
- Está definida en `Map.h`, que incluye `Foreach.h`.

**Ejemplo**
```c
#define ADD(item) +(item)
enum { sum = 0 PP_MAP(ADD, 1, 2, 3) }; /* 6 */
#undef ADD
```

---

## Repetición

Incluya `Cosmeron/Core/Preprocessor/Unroll.h`. La repetición se diferencia de foreach: genera índices numéricos en **orden descendente**, en lugar de recibir una lista explícita de elementos.

### PP_REPEAT

Invoca una macro unaria para cada índice entero desde `count` hasta 1.

**Sintaxis:** `PP_REPEAT(count, macro)`.

**Forma directa:** para un conteo de 3, `macro(3) macro(2) macro(1)`.

**Parámetros:** `count` — token entero de 0 a 5; `macro` — macro que recibe un índice generado.

**Retorno:** tokens del preprocesador.

**Observaciones**
- El conteo 0 se expande a nada.
- Los índices son descendentes: 3, 2, 1.
- No se insertan separadores automáticamente.
- Los conteos superiores a 5 no están definidos.

**Ejemplo**
```c
#define ADD_INDEX(index) +(index)
enum { sum = 0 PP_REPEAT(3, ADD_INDEX) }; /* 6 */
#undef ADD_INDEX
```

### PP_DEC

Decrementa un entero pequeño mediante un mapeo fijo de tokens, saturando en cero.

**Sintaxis:** `PP_DEC(x)`.

**Forma directa:** resultado literal: 0→0, 1→0, 2→1, 3→2, 4→3, 5→4.

**Parámetro:** `x` — token entero de 0 a 5.

**Retorno:** tokens del preprocesador.

**Observaciones**
- No es una macro aritmética general.
- Es útil como transición de estado para ejemplos limitados de `PP_WHILE`.

**Ejemplo**
```c
enum { n = PP_DEC(3), z = PP_DEC(0) }; /* n == 2; z == 0 */
```

---

## Selección de secuencias

`Sequence.h` maneja tanto **argumentos comunes separados por comas (tuplas)** como **secuencias entre paréntesis (`(a)(b)(c)...`)**, con capacidades completas de fold funcional y enumeración.

### PP_SEQ_FIRST

Selecciona el primer elemento de una lista de argumentos de macro.

**Sintaxis:** `PP_SEQ_FIRST(first, ...)`. **Forma directa:** secuencia de tokens del primer argumento.

**Parámetros:** `first` — argumento seleccionado; `...` — argumentos restantes, descartados.

**Retorno:** tokens del preprocesador.

**Observación:** los índices son puramente sintácticos y siguen las reglas estándar de análisis de argumentos de macros.

**Ejemplo**
```c
enum { n = PP_SEQ_FIRST(10, 20, 30) }; /* n == 10 */
```

### PP_SEQ_REST

Emite todos los argumentos posteriores al primero, conservando su formato separado por comas.

**Sintaxis:** `PP_SEQ_REST(first, ...)`. **Forma directa:** los argumentos restantes separados por comas.

**Parámetros:** `first` — argumento descartado; `...` — argumentos restantes.

**Retorno:** tokens del preprocesador.

**Observación:** la salida debe aparecer en un contexto gramatical que acepte una lista separada por comas.

**Ejemplo**
```c
int values[] = { PP_SEQ_REST(0, 10, 20, 30) };
/* values contiene 10, 20, 30 */
```

### PP_SEQ_GET_1 a PP_SEQ_GET_256

Selecciona la posición indicada, **basada en 1**, en una lista de argumentos separados por comas.

**Sintaxis:** `PP_SEQ_GET_1(a, ...)`, `PP_SEQ_GET_2(a, b, ...)`, `PP_SEQ_GET_3(a, b, c, ...)` y así sucesivamente hasta `PP_SEQ_GET_256`.

**Forma directa:** la secuencia literal de tokens seleccionada. Son macros, no funciones C.

**Parámetros:** lista separada por comas que contenga al menos tantos argumentos como el índice. Los argumentos posteriores no utilizados se ignoran.

**Retorno:** emite la secuencia de tokens en la posición seleccionada.

**Observaciones**
- Admite selectores del 1 al 256.
- No realiza indexación dinámica de un array en tiempo de ejecución.
- La ausencia de un argumento requerido no se resuelve mediante una comprobación de estado en tiempo de ejecución.

**Ejemplo**
```c
enum { second = PP_SEQ_GET_2(10, 20, 30) };
enum { fourth = PP_SEQ_GET_4(10, 20, 30, 40) };
/* second == 20; fourth == 40 */
```

### Secuencias entre paréntesis (PP_SEQ)

Las secuencias entre paréntesis representan colecciones con el formato `(a)(b)(c)...`, ofreciendo inmunidad total frente a comas internas en tipos o expresiones C (como `void (*)(int, int)` o inicializadores de estructuras).

| Macro | Descripción |
| --- | --- |
| `PP_SEQ_HEAD(seq)` | Extrae el primer elemento entre paréntesis `a`. |
| `PP_SEQ_TAIL(seq)` | Descarta el primer elemento y devuelve la secuencia restante `(b)(c)...`. |
| `PP_SEQ_SIZE(seq)` | Calcula la cantidad exacta de elementos (1–256) en una sola pasada de expansión, sin `PP_EVAL`. |
| `PP_SEQ_FOLD_LEFT(op, state, seq)` | Fold a la izquierda: `op(op(op(state, a), b), c)`. |
| `PP_SEQ_FOLD_RIGHT(op, state, seq)` | Fold a la derecha: `op(a, op(b, op(c, state)))`. |
| `PP_SEQ_ENUM(seq)` | Convierte `(a)(b)(c)` en elementos separados por comas: `a, b, c`. |
| `PP_SEQ_TO_TUPLE(seq)` | Convierte `(a)(b)(c)` en una tupla entre paréntesis: `(a, b, c)`. |
| `PP_SEQ_FOREACH(macro, seq)` | Emite `macro(a) macro(b) macro(c)`. |
| `PP_SEQ_FOREACH_CTX(macro, ctx, seq)` | Emite `macro(ctx, a) macro(ctx, b) macro(ctx, c)`. |

**Ejemplo**
```c
#define BIT_OR(acc, elem) ((acc) | (elem))
enum { flags = PP_SEQ_FOLD_LEFT(BIT_OR, 0, (1)(2)(4)) }; /* flags == 7 */

int values[] = { PP_SEQ_ENUM((10)(20)(30)) };
```

---

## Reducción en árbol

`Tree.h` implementa un **motor de reducción mediante árbol binario / divide y vencerás** (profundidad de expansión $O(\log_2 N)$).

En lugar de recursión lineal (`A → AB → ABC ...`), el motor divide las operaciones en mitades equilibradas. Funciona sin `PP_EVAL`, ofreciendo reducciones reentrantes y anidadas inmunes a bloqueos de recursión.

### Reductores principales del árbol

| Macro | Descripción |
| --- | --- |
| `PP_TREE_REDUCE(op, ...)` | Reduce 1–32 argumentos usando el operador binario `op(left, right)`, con profundidad $O(\log_2 N)$ ($\le 5$). |
| `PP_TREE_REDUCE_CTX(op, ctx, ...)` | Reduce 1–32 argumentos pasando el contexto a `op(ctx, left, right)`. |
| `PP_TREE_CAT(...)` | Une tokens directamente en un símbolo: `PP_TREE_CAT(A, B, C, D)` → `ABCD`. |
| `PP_TREE_JOIN(sep, ...)` | Une tokens con un separador personalizado: `PP_TREE_JOIN(_, A, B, C)` → `A_B_C`. |
| `PP_TREE_JOIN_UNDERSCORE(...)` | Variante especializada con guion bajo: `PP_TREE_JOIN_UNDERSCORE(Cos, Core, Mem)` → `Cos_Core_Mem`. |
| `PP_TREE_REDUCE_CHUNKS_2D(op, ...)` | Bloques Tree 2D ($32 \times 32 = 1.024$ elementos). |
| `PP_TREE_REDUCE_CHUNKS_3D(op, ...)` | Bloques Tree 3D ($32^3 = 32.768$ elementos). |
| `PP_TREE_REDUCE_CHUNKS_4D(op, ...)` | Bloques Tree 4D ($32^4 = 1.048.576$ elementos). |
| `PP_TREE_CAT_CHUNKS_2D/3D/4D(...)` | Pegado multidimensional de tokens mediante Chunks. |
| `PP_TREE_JOIN_UNDERSCORE_CHUNKS_2D/3D/4D(...)` | Unión multidimensional con guiones bajos mediante Chunks. |

**Ejemplo**
```c
#define ADD(a, b) (a + b)
enum { sum = PP_TREE_REDUCE(ADD, 1, 2, 3, 4, 5, 6, 7, 8) }; /* sum == 36, profundidad == 3 */

int PP_TREE_JOIN_UNDERSCORE(Cosmeron, Core, Memory, Alloc) = 42;
/* Cosmeron_Core_Memory_Alloc == 42 */

/* Tree Chunks 3D */
int sum3D = PP_TREE_REDUCE_CHUNKS_3D(ADD,
    ((1, 2), (3, 4)),
    ((5, 6), (7, 8))
); /* sum3D == 36 */
```

---

## Auxiliares de tokens

`Tokens.h` ofrece las siguientes macros tipo función para emitir tokens simples:

| Forma de macro | Expansión directa |
| --- | --- |
| `PP_TOKEN_EMPTY()` | Ningún token |
| `PP_TOKEN_COMMA()` | `,` |
| `PP_TOKEN_LPAREN()` | `(` |
| `PP_TOKEN_RPAREN()` | `)` |
| `PP_TOKEN_TILDE()` | `~` |

**Sintaxis**
```c
PP_TOKEN_EMPTY()
PP_TOKEN_COMMA()
PP_TOKEN_LPAREN()
PP_TOKEN_RPAREN()
PP_TOKEN_TILDE()
```

**Parámetros:** ninguno.

**Retorno:** la puntuación correspondiente o una secuencia vacía de tokens. No hay resultado en tiempo de ejecución.

**Observaciones**
- Son útiles cuando la puntuación debe aparecer después de otra fase de expansión de macros.
- `PP_TOKEN_EMPTY` y `PP_EMPTY` son auxiliares de expansión vacía similares, pero pertenecen a headers diferentes.
- Los paréntesis diferidos son sensibles al análisis de argumentos de macros; use `PP_TOKEN_LPAREN` y `PP_TOKEN_RPAREN` deliberadamente.

**Ejemplo**
```c
#include "Cosmeron/Core/Preprocessor/Tokens.h"
int values[] = { 1 PP_TOKEN_COMMA() 2 PP_TOKEN_COMMA() 3 };
/* Equivalente a: int values[] = { 1, 2, 3 }; */
```

---

## Repetición condicional

`While.h` combina `Boolean.h` y `Eval.h` para transformar repetidamente un estado expandible hasta que un predicado se vuelva falso.

### PP_WHILE

Aplica una operación mientras el predicado proporcionado se evalúe como verdadero.

**Sintaxis:** `PP_WHILE(pred, op, state)`.

**Forma directa:** el token del estado final, alcanzado cuando el predicado se vuelve falso.

**Parámetros:** `pred` — macro unaria que convierte el estado en un token booleano admitido; `op` — macro unaria que calcula el siguiente estado; `state` — token del estado inicial.

**Retorno:** tokens del preprocesador, no un valor C en tiempo de ejecución.

**Observaciones**
- La operación debe hacer que el predicado se vuelva falso en algún momento.
- La expansión está limitada por la profundidad de `PP_EVAL` y los límites del compilador; no es un bucle ilimitado en tiempo de ejecución.
- El `PP_DEC` actual solo admite estados de 0 a 5.
- Los auxiliares internos `PP_WHILE_IMPL` y `PP_WHILE_INDIRECT` implementan la recursión.

**Ejemplo**
```c
#define TEST(s) PP_BOOL(s)
#define STEP(s) PP_DEC(s)
enum { result = PP_WHILE(TEST, STEP, 3) };
/* result == 0 */
#undef TEST
#undef STEP
```

---

## Detección de plataforma

Los headers `Detect` exponen flags de tiempo de compilación 0/1. Una expresión agregada `*_KNOWN` es verdadera cuando se reconoce al menos una familia implementada. Esto es **detección del destino**, no sondeo de capacidades en tiempo de ejecución.

### Detección de compilador

Header: `Cosmeron/Core/Preprocessor/Detect/Compiler.h`

| Flag | Familia reconocida |
| --- | --- |
| `COMPILER_INTEL` | Macros del compilador Intel |
| `COMPILER_CLANG` | Clang (excepto Intel) |
| `COMPILER_MSVC` | MSVC (excepto Intel o Clang) |
| `COMPILER_GCC` | GCC (excepto los anteriores) |
| `COMPILER_KNOWN` | OR booleano de las cuatro familias |

**Sintaxis**
```c
#if COMPILER_CLANG
/* Implementación específica de Clang. */
#endif
```

**Forma directa:** se pueden usar directamente macros internas del compilador como `__clang__` o `_MSC_VER`, pero **no son intercambiables** con estas flags normalizadas y exclusivas.

**Parámetros:** ninguno. **Retorno:** 0 o 1 numérico para las flags de familia; una expresión booleana para `COMPILER_KNOWN`.

**Observaciones**
- Intel se comprueba primero, luego Clang, MSVC y GCC; las familias reconocidas son mutuamente excluyentes.
- clang-cl puede definir varias macros de proveedor, pero aquí se clasifica como Clang.
- Otros compiladores pueden dejar las cuatro flags en falso sin que la compilación sea inválida.

**Ejemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/Compiler.h"
#if COMPILER_MSVC
/* Rama MSVC */
#elif COMPILER_GCC
/* Rama GCC */
#endif
```

### Detección de sistema operativo

Header: `Cosmeron/Core/Preprocessor/Detect/OperationSystem.h`

| Flag | Condición / familia |
| --- | --- |
| `OS_WINDOWS` | `_WIN32` o `_WIN64` (excepto Cygwin) |
| `OS_CYGWIN` | Entorno Cygwin (`__CYGWIN__`) |
| `OS_MAC` | Apple / Mach (`__APPLE__ && __MACH__`) |
| `OS_IOS` | iOS / iPadOS / watchOS / tvOS |
| `OS_LINUX` | Kernel Linux (`__linux__` o `__linux`) |
| `OS_ANDROID` | Capa de userland Android (`__ANDROID__`) |
| `OS_OHOS` | Capa de userland OpenHarmony (`__OHOS__`) |
| `OS_FREEBSD` | FreeBSD (`__FreeBSD__`) |
| `OS_OPENBSD` | OpenBSD (`__OpenBSD__`) |
| `OS_NETBSD` | NetBSD (`__NetBSD__`) |
| `OS_DRAGONFLY` | DragonFly BSD (`__DragonFly__`) |
| `OS_SOLARIS` | Solaris / SunOS / Illumos (`__sun__`, `__sun` o `sun`) |
| `OS_AIX` | IBM AIX (`_AIX` o `__TOS_AIX__`) |
| `OS_HAIKU` | Haiku / BeOS (`__HAIKU__` o `__BEOS__`) |
| `OS_QNX` | QNX Neutrino (`__QNX__` o `__QNXNTO__`) |
| `OS_HURD` | GNU Hurd (`__GNU__` o `__gnu_hurd__`) |
| `OS_WASM` | WebAssembly (`__wasm__`, `__wasm32__`, `__wasm64__` o WASI/Emscripten) |
| `OS_EMSCRIPTEN` | SDK Emscripten (`__EMSCRIPTEN__`) |
| `OS_WASI` | WebAssembly System Interface (`__wasi__`) |
| `OS_REDOX` | Microkernel Redox OS (`__redox__`) |
| `OS_BSD` | Cualquier familia BSD (`OS_FREEBSD`, `OS_OPENBSD`, `OS_NETBSD`, `OS_DRAGONFLY`) |
| `OS_UNIX` | Sistemas tipo Unix (`OS_LINUX`, `OS_MAC`, `OS_BSD`, `OS_SOLARIS`, `OS_AIX`, `OS_HAIKU`, `OS_QNX`, `OS_HURD`, `OS_CYGWIN`, `OS_REDOX`) |
| `OS_POSIX` | Plataformas compatibles con POSIX (`OS_UNIX`, `OS_ANDROID`, `OS_OHOS`) |
| `OS_KNOWN` | Cualquier destino de sistema operativo reconocido |

**Sintaxis**
```c
#if OS_WINDOWS
/* Código específico de Windows */
#elif OS_POSIX
/* Destino reconocido de estilo POSIX */
#endif
```

**Forma directa:** se pueden comprobar manualmente macros del proveedor de la plataforma, como `_WIN32` o `__linux__`, pero no se garantiza que signifiquen exactamente lo mismo que las flags normalizadas de Cosmeron.

**Parámetros:** ninguno. **Retorno:** flags numéricas 0/1 y expresiones booleanas agregadas.

**Observaciones**
- `OS_UNIX` y `OS_POSIX` reconocen sistemas Unix estandarizados y plataformas compatibles con POSIX.
- `OS_ANDROID` y `OS_OHOS` actúan como capas de userland/ABI sobre `OS_LINUX`.
- En Windows, el header define `WIN32_LEAN_AND_MEAN` si aún no está definido.
- Estas macros no detectan el sistema operativo en tiempo de ejecución.

**Ejemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/OperationSystem.h"
#if OS_LINUX
/* Linux */
#elif OS_MAC
/* macOS */
#elif OS_FREEBSD
/* FreeBSD */
#elif OS_WINDOWS
/* Windows */
#endif
```

### Detección de procesador

Header: `Cosmeron/Core/Preprocessor/Detect/Processor.h`

| Flag | Arquitectura de destino reconocida |
| --- | --- |
| `PROCESSOR_X86` | x86 de 32 bits |
| `PROCESSOR_X64` | x86-64 |
| `PROCESSOR_ARM` | ARM de 32 bits |
| `PROCESSOR_ARM64` | AArch64 |
| `PROCESSOR_RISCV` | RISC-V |
| `PROCESSOR_KNOWN` | OR booleano de esas arquitecturas |

**Sintaxis**
```c
#if PROCESSOR_ARM64
/* Rama AArch64 */
#endif
```

**Forma directa:** se pueden consultar macros de arquitectura proporcionadas por el compilador, pero la forma `PROCESSOR_*` es la interfaz estable ofrecida por la biblioteca.

**Parámetros:** ninguno. **Retorno:** flags numéricas 0/1 o una expresión booleana agregada.

**Observaciones**
- La detección de arquitectura se refiere al destino de compilación, no a la máquina que ejecuta el binario.
- RISC-V se detecta como familia, sin distinguir entre 32 y 64 bits.
- Estas flags no comprueban extensiones de arquitectura como AVX o NEON.

**Ejemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/Processor.h"
#if PROCESSOR_X64
/* Destino x86-64 */
#elif PROCESSOR_ARM64
/* Destino ARM64 */
#endif
```

### Detección de la biblioteca estándar C

Header: `Cosmeron/Core/Preprocessor/Detect/Library.h`

| Flag | Biblioteca de runtime C del destino |
| --- | --- |
| `LIBC_GLIBC` | GNU C Library (glibc) |
| `LIBC_MUSL` | musl libc (estándar en Alpine y Void musl) |
| `LIBC_BIONIC` | Bionic libc de Android |
| `LIBC_MSVC` | Microsoft Universal CRT / MSVCRT |
| `LIBC_APPLE` | Apple Libc (macOS / iOS / Darwin) |
| `LIBC_BSD` | BSD libc (FreeBSD, OpenBSD, NetBSD, DragonFly) |
| `LIBC_UCLIBC` | uClibc / uClibc-ng |
| `LIBC_NEWLIB` | Red Hat Newlib (embebido / bare-metal) |
| `LIBC_WASI` | WebAssembly System Interface libc (wasi-libc) |
| `LIBC_EMSCRIPTEN` | libc de Emscripten |
| `LIBC_RELIBC` | relibc (biblioteca estándar C de Redox OS) |
| `LIBC_KNOWN` | Agregado booleano que indica cualquier libc reconocida |

También se definen aliases con prefijo `LIBRARY_*` (`LIBRARY_GLIBC`, `LIBRARY_MUSL`, etc.) por comodidad.

**Sintaxis**
```c
#if LIBC_GLIBC
/* Rutinas específicas de glibc */
#elif LIBC_MUSL
/* Entorno musl estándar */
#endif
```

**Forma directa:** se pueden comprobar manualmente macros de plataforma como `__GLIBC__` o `__BIONIC__`, pero `LIBC_*` proporciona flags normalizadas y mutuamente excluyentes entre las toolchains admitidas.

**Parámetros:** ninguno. **Retorno:** flags numéricas 0/1 y expresiones booleanas agregadas.

**Observaciones**
- Como musl intencionalmente no exporta una macro de proveedor, `LIBC_MUSL` se identifica en destinos Linux cuando glibc, uClibc, Bionic y Newlib están ausentes, o cuando se proporciona `__MUSL__`.
- La detección es mutuamente excluyente entre los entornos C hospedados estándar.

**Ejemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/Library.h"
#if LIBC_GLIBC
/* Extensiones GNU disponibles */
#elif LIBC_MUSL
/* Entorno musl estándar */
#endif
```

---

## Diagnósticos de inclusión

El archivo `Compiling.inc` puede emitir un mensaje del compilador durante la inclusión cuando se define `SHOW_INCLUDE_BUILD_MESSAGE`.

### SHOW_INCLUDE_BUILD_MESSAGE

Habilita un diagnóstico `#pragma message` dentro de `Compiling.inc`.

**Sintaxis**
```c
#define SHOW_INCLUDE_BUILD_MESSAGE
#include "Cosmeron/Core/Preprocessor/Compiling.inc"
```

**Forma directa:** no existe un equivalente como función; se puede escribir manualmente un `#pragma message` específico del compilador.

**Parámetros:** ninguno. **Retorno:** no hay retorno en tiempo de ejecución; emite un diagnóstico opcional del compilador cuando este admite la pragma.

**Observaciones**
- El mensaje emitido usa `__FILE__` en el lugar donde aparece la pragma, es decir, la ubicación de `Compiling.inc`, no automáticamente el header que lo incluye.
- Muchos headers incluyen `Compiling.inc`, por lo que la salida puede repetirse.
- La opción se comprueba con `#ifdef`; definirla como 0 también la activa.
- La portabilidad de `#pragma message` depende del compilador.

**Ejemplo**
```c
#define SHOW_INCLUDE_BUILD_MESSAGE
#include "Cosmeron/Core/Preprocessor/Operations.h"
/* Puede imprimir [Compiling: ...Compiling.inc]. */
```

---

# Ejemplo completo

El ejemplo siguiente combina concatenación, lógica booleana y declaraciones generadas mediante foreach:

```c
#include "Cosmeron/Core/Preprocessor/Operations.h"
#include "Cosmeron/Core/Preprocessor/Boolean.h"
#include "Cosmeron/Core/Preprocessor/Foreach.h"

#define PREFIX Example
#define FIELD(name) int name;

typedef struct {
    PP_FOREACH(FIELD, left, top, width, height)
} PP_OP_CAT2(PREFIX, _Rectangle);

enum {
    Example_Enabled = PP_BOOL_AND(3, 4)
};

#undef FIELD
#undef PREFIX
```

---

# Notas

- Todas las operaciones se ejecutan durante el preprocesamiento; estos utilitarios no requieren **flags de enlazado adicionales** ni una biblioteca de ejecución adicional.
- La concatenación de tokens de macro no es concatenación de cadenas. Las concatenaciones no válidas generan errores de preprocesamiento.
- `PP_BOOL` y los auxiliares de selección relacionados usan tablas de consulta; no pueden evaluar expresiones aritméticas o lógicas arbitrarias.
- `PP_FOREACH` procesa una lista no vacía; `PP_REPEAT` genera índices numéricos descendentes.
- La expansión recursiva puede estar limitada por el compilador y por la implementación finita de `PP_EVAL`.
- Las flags de detección representan las familias implementadas actualmente, no todas las plataformas admitidas por C11.
- Prefiera construcciones normales de C cuando la metaprogramación no aporte un beneficio concreto. Siga `../Reference/PATTERN.md` y use `PP_OP_CAT2` y auxiliares relacionados en lugar de `##` directamente en el código de los módulos.
- No existe un header agregado público `Preprocessor.h` ni una API de “forma directa” en tiempo de ejecución en la superficie pública actual. Incluya los headers públicos específicos (`Arguments.h`, `Boolean.h`, `Operations.h`, `Foreach.h` y `Compiling.inc`) requeridos por las macros utilizadas; no incluya fragmentos internos de implementación como sustitutos.

## Módulos relacionados

- [Module](Module.md) — DSL pública de declaraciones construida sobre auxiliares del preprocesador.
- [Algorithm](Algorithm.md) — ejemplos de templates tipados y declaraciones generadas.

## Headers de origen

- `Codespace/Cosmeron/Core/Preprocessor/Arguments.h`
- `Codespace/Cosmeron/Core/Preprocessor/Boolean.h`
- `Codespace/Cosmeron/Core/Preprocessor/Operations.h`
- `Codespace/Cosmeron/Core/Preprocessor/Eval.h`
- `Codespace/Cosmeron/Core/Preprocessor/Foreach.h`
- `Codespace/Cosmeron/Core/Preprocessor/Map.h`
- `Codespace/Cosmeron/Core/Preprocessor/Unroll.h`
- `Codespace/Cosmeron/Core/Preprocessor/Sequence.h`
- `Codespace/Cosmeron/Core/Preprocessor/Tree.h`
- `Codespace/Cosmeron/Core/Preprocessor/Tokens.h`
- `Codespace/Cosmeron/Core/Preprocessor/While.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect/Detect.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect/Compiler.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect/Library.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect/OperationSystem.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect/Processor.h`
- `Codespace/Cosmeron/Core/Preprocessor/Compiling.inc`
