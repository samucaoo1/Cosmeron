# Preprocessor

O módulo **Preprocessor** fornece utilitários para metaprogramação com macros em C: contagem de argumentos, concatenação de tokens, seleção booleana, expansão repetida, listas, geração de tokens e detecção do alvo.

Ele é **exclusivamente de tempo de compilação**. Diferentemente dos módulos comuns do Cosmeron, essas APIs não possuem funções C invocáveis. Portanto, as entradas *Forma direta* abaixo mostram expansões literais ou deixam explícito que não existe equivalente direto em C.

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

# Visão geral

| Pacote | Header | Finalidade |
| --- | --- | --- |
| [Operations](#operações-com-tokens) | `Operations.h` | Expansão e concatenação de tokens |
| [Arguments](#contagem-de-argumentos) | `Arguments.h` | Contagem de argumentos variádicos |
| [Boolean](#operações-booleanas) | `Boolean.h` | Seleção condicional de tokens |
| [Eval](#controle-de-expansão) | `Eval.h` | Expansões recursivas adiadas |
| [Foreach](#foreach-e-map) | `Foreach.h` | Aplicação iterativa de macros em múltiplas dimensões |
| [Map](#pp_map) | `Map.h` | Motor de avaliação aninhável (Motor 2) |
| [Repetition](#repetição) | `Unroll.h` | Expansão repetida por índice e operações de incremento/decremento |
| [Sequence](#seleção-de-sequências) | `Sequence.h` | Tuplas separadas por vírgulas e sequências parentetizadas (SEQ) com fold funcional |
| [Tree](#redução-em-árvore) | `Tree.h` | Redução em árvore binária e concatenação de tokens em $O(\log_2 N)$ |
| [Tokens](#auxiliares-de-tokens) | `Tokens.h` | Emissão de tokens de pontuação |
| [While](#repetição-condicional) | `While.h` | Transformação repetida de estado |
| [Detect](#detecção-de-plataforma) | `Detect/*.h` | Flags de compilador, sistema operacional, biblioteca C e processador |
| [Compiling](#diagnósticos-de-inclusão) | `Compiling.inc` | Mensagens opcionais de diagnóstico de inclusão |

### Forma de macro

```c
#define MODULE Network
PP_OP_CAT2(MODULE, _Init) /* Network_Init */
```

### Forma direta

```c
Network_Init /* O identificador literal resultante. */
```

As duas formas são equivalentes **como tokens**. Elas não representam duas funções públicas de tempo de execução.

### Limites implementados

| Família | Intervalo suportado |
| --- | --- |
| `PP_ARG_COUNT` | 1–256 argumentos não vazios |
| `PP_FOREACH` (Dimensões 1–5) | 1–256 argumentos não vazios por dimensão |
| `PP_FOREACH_CHUNKS` (2D, 3D, 4D) | Até $256 \times 256 = 65.536$ (2D), $256^3 \approx 1,67 \times 10^7$ (3D), $256^4 \approx 4,29 \times 10^9$ (4D) |
| `PP_MAP` | Mapeamento recursivo aninhável sobre `PP_EVAL` (sujeito aos limites de expansão) |
| `PP_BOOL` | Tokens inteiros literais de 0 a 256 |
| `PP_OP_CAT` (variádico via Tree) e `PP_OP_CAT2`–`PP_OP_CAT32` | 1–32 operandos concatenados |
| `PP_REPEAT` | Contagens de 0 a 256 |
| `PP_DEC` e `PP_INC` | Entradas inteiras de 0 a 256 |
| `PP_SEQ_GET_1`–`PP_SEQ_GET_256` | Índices de 1 a 256 |
| `PP_SEQ_*` (sequências parentetizadas) | 1–256 elementos para Size, Fold, Enum e Foreach |
| `PP_TREE_REDUCE` / `PP_TREE_JOIN` | 1–32 operandos diretos ($O(\log_2 N)$, profundidade $\le 5$); até $32^4 = 1.048.576$ via Tree Chunks 4D |
| `PP_WHILE` | Limitado pela profundidade fixa de expansão e pelos limites do compilador |

---

# Referência da API

## Resumo das macros

| Macro ou família | Descrição |
| --- | --- |
| `PP_OP_EXPAND` | Expansão normal de argumentos |
| `PP_OP_CAT` | Concatenação primitiva de tokens |
| `PP_OP_CAT2` | Concatenação expandida de dois tokens |
| `PP_OP_CAT3`–`PP_OP_CAT32` | Concatenação de múltiplos tokens |
| `PP_ARG_COUNT` | Conta argumentos |
| `PP_BOOL` | Converte tokens inteiros suportados para 0/1 |
| `PP_BOOL_NOT` | Nega |
| `PP_BOOL_IF` | Seleciona tokens condicionalmente |
| `PP_BOOL_IF_ELSE` | Escolhe entre duas expansões |
| `PP_BOOL_AND` | AND lógico |
| `PP_BOOL_OR` | OR lógico |
| `PP_BOOL_XOR` | XOR lógico |
| `PP_EMPTY` | Expansão vazia |
| `PP_DEFER` | Adia a invocação de uma macro |
| `PP_OBSTRUCT` | Barreira de adiamento |
| `PP_EVAL` | Avaliação repetida |
| `PP_FOREACH` | Aplica uma macro a cada item |
| `PP_MAP` | Mapeamento recursivo aninhável com separadores e contexto configuráveis |
| `PP_REPEAT` | Repete uma macro por índice |
| `PP_DEC` | Decrementa um token inteiro limitado |
| `PP_SEQ_FIRST` | Seleciona o primeiro item |
| `PP_SEQ_REST` | Seleciona a cauda |
| `PP_SEQ_GET_1`–`PP_SEQ_GET_256` | Seleciona um item de uma lista de argumentos |
| `PP_TOKEN_*` | Pontuação |
| `PP_WHILE` | Transformação condicional recursiva |
| Detecção de compilador | Família do compilador |
| Detecção de sistema operacional | Sistema operacional de destino |
| Detecção de processador | Arquitetura de destino |
| Flag de mensagem de inclusão | Mensagens de compilação |

---

## Operações com tokens

Inclua `Cosmeron/Core/Preprocessor/Operations.h`. As operações com tokens combinam ou expandem **tokens do pré-processador**, não strings C.

### PP_OP_EXPAND

Encaminha uma sequência variádica de tokens pela expansão normal de argumentos.

**Sintaxe**

```c
PP_OP_EXPAND(...)
```

**Forma direta:** os tokens de argumento expandidos.

**Parâmetros:** `...` — quaisquer tokens a expandir.

**Retorno:** não é um valor de execução C; a macro expande para tokens do pré-processador.

**Observações**
- Definida como `#define PP_OP_EXPAND(...) __VA_ARGS__`.
- Não funciona como um mecanismo de expansão recursiva ilimitada; use `PP_EVAL` para padrões adiados.

**Exemplo**
```c
#define VALUE 42
int value = PP_OP_EXPAND(VALUE); /* int value = 42; */
```

### PP_OP_CAT

Concatenação primitiva de dois tokens usando `##`, sem pré-expansão dos argumentos adjacentes.

**Sintaxe:** `PP_OP_CAT(a, b)`.

**Forma direta:** o token concatenado literal, antes de uma nova varredura.

**Parâmetros:** `a` — primeiro token do pré-processador; `b` — segundo token.

**Retorno:** sequência de tokens do pré-processador, não um valor C em tempo de execução.

**Observações**
- Argumentos adjacentes a `##` não são expandidos primeiro.
- Use somente quando a supressão intencional da expansão dos argumentos for necessária.
- A concatenação deve produzir um token válido; caso contrário, o comportamento do pré-processamento é inválido.

**Exemplo**
```c
#define ONE 1
#define NAME_1 7
int PP_OP_CAT(var_, ONE) = 3; /* int var_ONE = 3; */
```

### PP_OP_CAT2

Concatena dois operandos após uma camada adicional de expansão de macros.

**Sintaxe:** `PP_OP_CAT2(a, b)`.

**Forma direta:** o token concatenado após a expansão dos argumentos, sujeito à nova varredura normal.

**Parâmetros:** `a` — primeiro operando; `b` — segundo operando.

**Observações**
- Chama `PP_OP_CAT` por meio de um wrapper, permitindo a expansão prévia das macros dos argumentos.
- É a opção preferencial para geração reutilizável de namespaces.
- A concatenação ainda exige um token válido.

**Exemplo**
```c
#define PART Widget
int PP_OP_CAT2(PART, _Count) = 10;
/* int Widget_Count = 10; */
```

### PP_OP_CAT3 a PP_OP_CAT32

Concatena de três a dezesseis operandos por meio da composição das primitivas de concatenação com expansão.

**Sintaxe:** `PP_OP_CAT3(a, b, c)`, `PP_OP_CAT4(a, b, c, d)` e assim sucessivamente até `PP_OP_CAT32`. A quantidade de argumentos exigida é indicada pelo sufixo.

**Forma direta:** o token literal resultante; não existe função C exportada correspondente.

**Parâmetros:** cada argumento deve expandir para um token que possa ser concatenado validamente aos tokens vizinhos.

**Retorno:** um único token concatenado, possivelmente expandido posteriormente.

**Observações**
- A implementação define formas para até 16 operandos.
- O código dos módulos deve usar esses utilitários em vez de aplicar `##` diretamente fora da camada de pré-processamento.

**Exemplo**
```c
#define PREFIX Example
int PP_OP_CAT3(PREFIX, _, Field) = 1;
/* int Example_Field = 1; */
```

---

## Contagem de argumentos

Inclua `Cosmeron/Core/Preprocessor/Arguments.h`. A interface pública é `PP_ARG_COUNT`; a maquinaria `PP_ARG_RSEQ`, `PP_ARG_COUNT_IMPL` e `PP_ARG_COUNT_SELECT` implementa a consulta.

### PP_ARG_COUNT

Conta uma lista não vazia de argumentos de macro do pré-processador.

**Sintaxe:** `PP_ARG_COUNT(item1, item2, ...)`.

**Forma direta:** literal inteiro que representa a quantidade de itens.

**Parâmetros:** `...` — lista separada por vírgulas com 1–100 itens.

**Retorno:** tokens do pré-processador, não um valor de execução C.

**Observações**
- Suporta até 100 argumentos.
- Não há garantia de que uma entrada vazia produza zero.
- Aplicam-se as regras do pré-processador para agrupamento por vírgulas e parênteses.

**Exemplo**
```c
enum { count = PP_ARG_COUNT(alpha, beta, gamma) };
/* count == 3 */
```

---

## Operações booleanas

Inclua `Cosmeron/Core/Preprocessor/Boolean.h`. O domínio booleano é formado por **tokens inteiros de 0 a 100**: 0 é falso e 1–100 são verdadeiros. Essas macros não avaliam expressões C arbitrárias, valores negativos ou literais inteiros sem limite.

### PP_BOOL

Converte um token inteiro suportado para 0 ou 1.

**Sintaxe:** `PP_BOOL(x)`. **Forma direta:** 0 se `x` for 0; 1 se `x` estiver entre 1 e 100.

**Parâmetro:** `x` — token inteiro de 0 a 100.

**Retorno:** token booleano literal 0 ou 1.

**Observação:** converte por meio de uma tabela de consulta interna gerada.

**Exemplo**
```c
int value = PP_BOOL(7); /* 1 */
```

### PP_BOOL_NOT

Nega um inteiro conversível em booleano.

**Sintaxe:** `PP_BOOL_NOT(x)`. **Forma direta:** 1 quando `x` é 0; caso contrário, 0.

**Parâmetro:** `x` — token inteiro de 0 a 100.

**Retorno:** token literal 1 quando `x` é 0; caso contrário, 0.

**Observação:** normaliza com `PP_BOOL` antes de negar.

**Exemplo**
```c
enum { yes = PP_BOOL_NOT(0), no = PP_BOOL_NOT(2) };
```

### PP_BOOL_IF

Emite tokens somente quando a condição é verdadeira.

**Sintaxe:** `PP_BOOL_IF(condition)(tokens)`.

**Forma direta:** a sequência de tokens fornecida ou nada.

**Parâmetros:** `condition` — token inteiro de 0 a 100; `tokens` — tokens fornecidos na segunda invocação entre parênteses.

**Retorno:** a sequência de tokens fornecida ou uma expansão vazia.

**Observação:** é uma seleção em tempo de compilação, não um `if` de C.

**Exemplo**
```c
PP_BOOL_IF(1)(int enabled = 1;)
PP_BOOL_IF(0)(int disabled = 1;)
```

### PP_BOOL_IF_ELSE

Escolhe uma de duas sequências de tokens entre parênteses.

**Sintaxe:** `PP_BOOL_IF_ELSE(condition)(trueTokens)(falseTokens)`.

**Forma direta:** a sequência selecionada.

**Parâmetros:** `condition` — token inteiro de 0 a 100; `trueTokens` — expansão usada para verdadeiro; `falseTokens` — expansão usada para falso.

**Retorno:** a sequência de tokens escolhida.

**Observação:** os dois ramos entre parênteses devem ser fornecidos.

**Exemplo**
```c
enum { value = PP_BOOL_IF_ELSE(7)(11)(22) };
/* value == 11 */
```

### PP_BOOL_AND

Conjunção booleana.

**Sintaxe:** `PP_BOOL_AND(a, b)`. **Forma direta:** 1 se ambos forem diferentes de zero; caso contrário, 0.

**Parâmetros:** `a` e `b` — tokens inteiros suportados.

**Retorno:** token literal 1 se ambos forem diferentes de zero; caso contrário, 0.

**Observação:** os dois operandos são normalizados para 0 ou 1.

**Exemplo**
```c
enum { value = PP_BOOL_AND(2, 4) }; /* 1 */
```

### PP_BOOL_OR

Disjunção inclusiva booleana.

**Sintaxe:** `PP_BOOL_OR(a, b)`. **Forma direta:** 1 se pelo menos um for diferente de zero; caso contrário, 0.

**Parâmetros:** `a` e `b` — tokens inteiros suportados.

**Retorno:** token literal 1 se pelo menos um for diferente de zero; caso contrário, 0.

**Observação:** ambos os operandos são normalizados.

**Exemplo**
```c
enum { value = PP_BOOL_OR(0, 9) }; /* 1 */
```

### PP_BOOL_XOR

Disjunção exclusiva booleana.

**Sintaxe:** `PP_BOOL_XOR(a, b)`. **Forma direta:** 1 se exatamente um dos operandos for diferente de zero; caso contrário, 0.

**Parâmetros:** `a` e `b` — tokens inteiros suportados.

**Retorno:** token literal 1 se exatamente um for diferente de zero; caso contrário, 0.

**Observação:** ambos os operandos são normalizados.

**Exemplo**
```c
enum { value = PP_BOOL_XOR(0, 3) }; /* 1 */
```

---

## Controle de expansão

Inclua `Cosmeron/Core/Preprocessor/Eval.h`. Esses auxiliares adiam e reexaminam a expansão de macros. Eles implementam **recursão no pré-processador**, não laços ou funções em tempo de execução.

### PP_EMPTY

Emite zero tokens.

**Sintaxe:** `PP_EMPTY()`. **Forma direta:** sequência vazia de tokens.

**Parâmetros:** nenhum. **Retorno:** nenhum token.

**Observação:** usado internamente como espaçador para adiamento.

**Exemplo**
```c
#define VALUE() 7
int number = PP_EMPTY() VALUE(); /* int number = 7; */
```

### PP_DEFER

Adia a invocação de uma macro do tipo função inserindo uma barreira de macro vazia.

**Sintaxe:** `PP_DEFER(id)(arguments)`.

**Forma direta:** sequência de tokens adiada, em vez de uma chamada imediata de função.

**Parâmetro:** `id` — identificador de uma macro do tipo função.

**Retorno:** tokens do pré-processador, não um valor C em tempo de execução.

**Observações**
- Definida como `id PP_EMPTY()`.
- Só é útil com uma nova varredura de macros deliberada; sua sintaxe semelhante a uma chamada não representa uma função C exportada.
- A quantidade de novas varreduras antes da invocação depende do contexto.

**Exemplo**
```c
#define LATER() 5
#define REQUEST() PP_DEFER(LATER)()
/* REQUEST() usa uma barreira de expansão ao redor de LATER. */
```

### PP_OBSTRUCT

Adiciona um adiamento mais forte, permitindo expansão indireta recursiva de macros.

**Sintaxe:** `PP_OBSTRUCT(tokens...)`.

**Forma direta:** tokens fornecidos com uma barreira adicional de macro vazia adiada.

**Parâmetro:** `tokens...` — tokens do pré-processador a bloquear temporariamente.

**Retorno:** tokens do pré-processador.

**Observações**
- Definida usando `PP_DEFER(PP_EMPTY)()`.
- Normalmente combinada com `PP_EVAL` e recursão indireta.
- Usada por `PP_WHILE`.

**Exemplo**
```c
#define INDIRECT() PP_EMPTY
PP_OBSTRUCT(INDIRECT)() /* Exemplo de barreira de expansão. */
```

### PP_EVAL

Reexamina a entrada expandida por meio de uma cascata fixa de auxiliares internos de expansão.

**Sintaxe:** `PP_EVAL(tokens...)`.

**Forma direta:** sequência de tokens reexaminada após a cascata fixa de expansão.

**Parâmetro:** `tokens...` — entrada que pode conter chamadas adiadas.

**Retorno:** tokens do pré-processador.

**Observações**
- A implementação usa `PP_EVAL1`, `PP_EVAL2` e `PP_EVAL3`.
- A profundidade de expansão é finita e depende do compilador.
- `PP_WHILE` já aplica `PP_EVAL` internamente.

**Exemplo**
```c
#define NUMBER 42
int result = PP_EVAL(NUMBER); /* 42 */
```

---

## Foreach e Map

`Foreach.h` fornece um motor de iteração de macros multidimensional de alto desempenho (`D1` a `D5`), com suporte a até 256 argumentos por dimensão, além de um motor de blocos (`PP_FOREACH_CHUNKS`) para processar conjuntos de dados grandes, de até $256 \times 256 = 65.536$ elementos.

### PP_FOREACH

Aplica uma macro uma vez a cada item fornecido, em ordem, operando na Dimensão 1 (laço externo padrão).

**Sintaxe**
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

/* Macros diretas por dimensão: */
PP_FOREACH_D2(macro, ...)
PP_FOREACH_D2_CTX(macro, ctx, ...)
PP_FOREACH_D3_CTX(macro, ctx, ...)
PP_FOREACH_D4_CTX(macro, ctx, ...)
PP_FOREACH_D5_CTX(macro, ctx, ...)

/* Motor de blocos: */
PP_FOREACH_CHUNKS(macro, (chunk1), (chunk2), ...)
PP_FOREACH_CHUNKS_CTX(macro, context, (chunk1), (chunk2), ...)
PP_FOREACH_CHUNKS_COMMA(macro, (chunk1), (chunk2), ...)
PP_FOREACH_CHUNKS_SEMI(macro, context, (chunk1), (chunk2), ...)
```

**Forma direta:** expansões justapostas `macro(a) macro(b) macro(c) ...`, ou separadas por vírgula/ponto e vírgula.

**Parâmetros:** `macro` — macro unária `macro(x)` ou binária `macro(context, x)`; `context` — argumento de contexto passado como primeiro parâmetro nas invocações binárias; `...` — lista de 1–256 itens (ou blocos de itens para `PP_FOREACH_CHUNKS`).

**Retorno:** tokens do pré-processador.

**Observações**
- As variantes de separador inserem nada (`PP_FOREACH`), vírgulas (`PP_FOREACH_COMMA`) ou ponto e vírgula (`PP_FOREACH_SEMI`).
- As dimensões `D1` a `D5` usam tokens internos distintos de chamada e despacho, garantindo ausência de supressão de expansão ao aninhar iterações.
- Pode gerar declarações, expressões, tabelas de funções ou entradas no estilo X-macro.

**Exemplo**
```c
#define DECL_FIELD(name) int name;
struct Sample { PP_FOREACH(DECL_FIELD, width, height, depth) };
#undef DECL_FIELD
```

### PP_MAP

Motor de avaliação independente e aninhável (Motor 2), baseado em `PP_EVAL`. Diferentemente de `PP_FOREACH` (que concatena com espaços por padrão), `PP_MAP` usa vírgulas por padrão e é totalmente separado de `PP_FOREACH`, permitindo aninhar `PP_MAP` dentro de `PP_FOREACH` sem supressão de expansão.

**Sintaxe**
```c
PP_MAP(macro, a, b, ...)
PP_MAP_CTX(macro, context, a, b, ...)
PP_MAP_SEMI(macro, a, b, ...)
PP_MAP_SEMI_CTX(macro, context, a, b, ...)
PP_MAP_NONE(macro, a, b, ...)
PP_MAP_NONE_CTX(macro, context, a, b, ...)
```

**Forma direta:** tokens emitidos separados por vírgulas, ponto e vírgula ou sem separador.

**Parâmetros:** `macro` — macro aplicada a cada item (unária `macro(x)` ou binária com contexto `macro(ctx, x)`); `context` — contexto passado às invocações binárias; `...` — lista não vazia de argumentos.

**Retorno:** tokens do pré-processador.

**Observações**
- Separação por vírgula (`PP_MAP`), ponto e vírgula (`PP_MAP_SEMI`) ou nenhum separador (`PP_MAP_NONE`).
- Pode ser aninhada em `PP_FOREACH` sem colisão nas travas de recursão.
- Não constrói uma coleção nem insere separadores além dos definidos pela variante.
- Definida em `Map.h`, que inclui `Foreach.h`.

**Exemplo**
```c
#define ADD(item) +(item)
enum { sum = 0 PP_MAP(ADD, 1, 2, 3) }; /* 6 */
#undef ADD
```

---

## Repetição

Inclua `Cosmeron/Core/Preprocessor/Unroll.h`. Diferentemente de foreach, a repetição gera índices numéricos em **ordem decrescente**, em vez de receber uma lista explícita de itens.

### PP_REPEAT

Invoca uma macro unária para cada índice inteiro, de `count` até 1.

**Sintaxe:** `PP_REPEAT(count, macro)`.

**Forma direta:** para contagem 3, `macro(3) macro(2) macro(1)`.

**Parâmetros:** `count` — token inteiro de 0 a 5; `macro` — macro que recebe um índice gerado.

**Retorno:** tokens do pré-processador.

**Observações**
- A contagem 0 produz uma expansão vazia.
- Os índices são decrescentes: 3, 2, 1.
- Nenhum separador é inserido automaticamente.
- Contagens acima de 5 não são definidas.

**Exemplo**
```c
#define ADD_INDEX(index) +(index)
enum { sum = 0 PP_REPEAT(3, ADD_INDEX) }; /* 6 */
#undef ADD_INDEX
```

### PP_DEC

Decrementa um inteiro pequeno por meio de um mapeamento fixo de tokens, saturando em zero.

**Sintaxe:** `PP_DEC(x)`.

**Forma direta:** resultado literal: 0→0, 1→0, 2→1, 3→2, 4→3, 5→4.

**Parâmetro:** `x` — token inteiro de 0 a 5.

**Retorno:** tokens do pré-processador.

**Observações**
- Não é uma macro de aritmética geral.
- É útil como transição de estado em exemplos limitados de `PP_WHILE`.

**Exemplo**
```c
enum { n = PP_DEC(3), z = PP_DEC(0) }; /* n == 2; z == 0 */
```

---

## Seleção de sequências

`Sequence.h` lida tanto com **argumentos comuns separados por vírgulas (tuplas)** quanto com **sequências parentetizadas (`(a)(b)(c)...`)**, oferecendo recursos funcionais completos de fold e enumeração.

### PP_SEQ_FIRST

Seleciona o primeiro item de uma lista de argumentos de macro.

**Sintaxe:** `PP_SEQ_FIRST(first, ...)`. **Forma direta:** sequência de tokens do primeiro argumento.

**Parâmetros:** `first` — argumento selecionado; `...` — argumentos restantes, descartados.

**Retorno:** tokens do pré-processador.

**Observação:** os índices são puramente sintáticos e seguem as regras padrão de análise dos argumentos de macros.

**Exemplo**
```c
enum { n = PP_SEQ_FIRST(10, 20, 30) }; /* n == 10 */
```

### PP_SEQ_REST

Emite todos os argumentos após o primeiro, preservando o formato separado por vírgulas.

**Sintaxe:** `PP_SEQ_REST(first, ...)`. **Forma direta:** os argumentos restantes separados por vírgulas.

**Parâmetros:** `first` — argumento descartado; `...` — argumentos restantes.

**Retorno:** tokens do pré-processador.

**Observação:** a saída deve aparecer em um contexto gramatical que aceite uma lista separada por vírgulas.

**Exemplo**
```c
int values[] = { PP_SEQ_REST(0, 10, 20, 30) };
/* values contém 10, 20, 30 */
```

### PP_SEQ_GET_1 a PP_SEQ_GET_256

Seleciona a posição **baseada em 1** indicada em uma lista de argumentos separados por vírgulas.

**Sintaxe:** `PP_SEQ_GET_1(a, ...)`, `PP_SEQ_GET_2(a, b, ...)`, `PP_SEQ_GET_3(a, b, c, ...)` e assim por diante até `PP_SEQ_GET_256`.

**Forma direta:** sequência literal de tokens selecionada. São macros, não funções C.

**Parâmetros:** lista separada por vírgulas contendo pelo menos tantos argumentos quanto o índice. Argumentos posteriores não utilizados são ignorados.

**Retorno:** emite a sequência de tokens na posição selecionada.

**Observações**
- Suporta seletores de 1 a 256.
- Não realiza indexação dinâmica de um array em tempo de execução.
- A ausência de um argumento obrigatório não é tratada por uma verificação de status em tempo de execução.

**Exemplo**
```c
enum { second = PP_SEQ_GET_2(10, 20, 30) };
enum { fourth = PP_SEQ_GET_4(10, 20, 30, 40) };
/* second == 20; fourth == 40 */
```

### Sequências parentetizadas (PP_SEQ)

Sequências parentetizadas representam coleções no formato `(a)(b)(c)...`, oferecendo imunidade total a vírgulas internas em tipos ou expressões C (como `void (*)(int, int)` ou inicializadores de structs).

| Macro | Descrição |
| --- | --- |
| `PP_SEQ_HEAD(seq)` | Extrai o primeiro elemento parentetizado `a`. |
| `PP_SEQ_TAIL(seq)` | Descarta o primeiro elemento e retorna a sequência restante `(b)(c)...`. |
| `PP_SEQ_SIZE(seq)` | Calcula a quantidade exata de elementos (1–256) em uma única passagem de expansão, sem `PP_EVAL`. |
| `PP_SEQ_FOLD_LEFT(op, state, seq)` | Fold à esquerda: `op(op(op(state, a), b), c)`. |
| `PP_SEQ_FOLD_RIGHT(op, state, seq)` | Fold à direita: `op(a, op(b, op(c, state)))`. |
| `PP_SEQ_ENUM(seq)` | Converte `(a)(b)(c)` em itens separados por vírgulas: `a, b, c`. |
| `PP_SEQ_TO_TUPLE(seq)` | Converte `(a)(b)(c)` em uma tupla entre parênteses: `(a, b, c)`. |
| `PP_SEQ_FOREACH(macro, seq)` | Emite `macro(a) macro(b) macro(c)`. |
| `PP_SEQ_FOREACH_CTX(macro, ctx, seq)` | Emite `macro(ctx, a) macro(ctx, b) macro(ctx, c)`. |

**Exemplo**
```c
#define BIT_OR(acc, elem) ((acc) | (elem))
enum { flags = PP_SEQ_FOLD_LEFT(BIT_OR, 0, (1)(2)(4)) }; /* flags == 7 */

int values[] = { PP_SEQ_ENUM((10)(20)(30)) };
```

---

## Redução em árvore

`Tree.h` implementa um **motor de redução em árvore binária / divisão e conquista** (profundidade de expansão $O(\log_2 N)$).

Em vez de recursão linear (`A → AB → ABC ...`), o motor divide as operações em metades balanceadas. Ele opera sem `PP_EVAL`, oferecendo reduções reentrantes e aninhadas imunes a bloqueios de recursão.

### Redutores principais da árvore

| Macro | Descrição |
| --- | --- |
| `PP_TREE_REDUCE(op, ...)` | Reduz 1–32 argumentos usando o operador binário `op(left, right)`, com profundidade $O(\log_2 N)$ ($\le 5$). |
| `PP_TREE_REDUCE_CTX(op, ctx, ...)` | Reduz 1–32 argumentos passando o contexto em `op(ctx, left, right)`. |
| `PP_TREE_CAT(...)` | Une tokens diretamente em um símbolo: `PP_TREE_CAT(A, B, C, D)` → `ABCD`. |
| `PP_TREE_JOIN(sep, ...)` | Une tokens com um separador personalizado: `PP_TREE_JOIN(_, A, B, C)` → `A_B_C`. |
| `PP_TREE_JOIN_UNDERSCORE(...)` | Variante especializada com sublinhado: `PP_TREE_JOIN_UNDERSCORE(Cos, Core, Mem)` → `Cos_Core_Mem`. |
| `PP_TREE_REDUCE_CHUNKS_2D(op, ...)` | Blocos Tree 2D ($32 \times 32 = 1.024$ elementos). |
| `PP_TREE_REDUCE_CHUNKS_3D(op, ...)` | Blocos Tree 3D ($32^3 = 32.768$ elementos). |
| `PP_TREE_REDUCE_CHUNKS_4D(op, ...)` | Blocos Tree 4D ($32^4 = 1.048.576$ elementos). |
| `PP_TREE_CAT_CHUNKS_2D/3D/4D(...)` | Concatenação multidimensional de tokens por meio de Chunks. |
| `PP_TREE_JOIN_UNDERSCORE_CHUNKS_2D/3D/4D(...)` | Junção multidimensional com sublinhados por meio de Chunks. |

**Exemplo**
```c
#define ADD(a, b) (a + b)
enum { sum = PP_TREE_REDUCE(ADD, 1, 2, 3, 4, 5, 6, 7, 8) }; /* sum == 36, profundidade == 3 */

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

`Tokens.h` oferece as seguintes macros do tipo função para emitir tokens simples:

| Forma de macro | Expansão direta |
| --- | --- |
| `PP_TOKEN_EMPTY()` | Nenhum token |
| `PP_TOKEN_COMMA()` | `,` |
| `PP_TOKEN_LPAREN()` | `(` |
| `PP_TOKEN_RPAREN()` | `)` |
| `PP_TOKEN_TILDE()` | `~` |

**Sintaxe**
```c
PP_TOKEN_EMPTY()
PP_TOKEN_COMMA()
PP_TOKEN_LPAREN()
PP_TOKEN_RPAREN()
PP_TOKEN_TILDE()
```

**Parâmetros:** nenhum.

**Retorno:** a pontuação correspondente ou uma sequência vazia de tokens. Não há resultado em tempo de execução.

**Observações**
- Úteis quando a pontuação precisa aparecer após outra etapa de expansão de macros.
- `PP_TOKEN_EMPTY` e `PP_EMPTY` são auxiliares de expansão vazia semelhantes, mas pertencem a headers diferentes.
- Parênteses adiados são sensíveis à análise dos argumentos de macros; use `PP_TOKEN_LPAREN` e `PP_TOKEN_RPAREN` deliberadamente.

**Exemplo**
```c
#include "Cosmeron/Core/Preprocessor/Tokens.h"
int values[] = { 1 PP_TOKEN_COMMA() 2 PP_TOKEN_COMMA() 3 };
/* Equivalente a: int values[] = { 1, 2, 3 }; */
```

---

## Repetição condicional

`While.h` combina `Boolean.h` e `Eval.h` para transformar repetidamente um estado expansível até que um predicado se torne falso.

### PP_WHILE

Aplica uma operação enquanto o predicado fornecido avaliar como verdadeiro.

**Sintaxe:** `PP_WHILE(pred, op, state)`.

**Forma direta:** o token do estado final, alcançado quando o predicado se torna falso.

**Parâmetros:** `pred` — macro unária que converte o estado em um token booleano suportado; `op` — macro unária que calcula o próximo estado; `state` — token do estado inicial.

**Retorno:** tokens do pré-processador, não um valor C em tempo de execução.

**Observações**
- A operação deve eventualmente fazer com que o predicado se torne falso.
- A expansão é limitada pela profundidade de `PP_EVAL` e pelos limites do compilador; não é um laço ilimitado em tempo de execução.
- O `PP_DEC` atual aceita somente estados de 0 a 5.
- Os auxiliares internos `PP_WHILE_IMPL` e `PP_WHILE_INDIRECT` implementam a recursão.

**Exemplo**
```c
#define TEST(s) PP_BOOL(s)
#define STEP(s) PP_DEC(s)
enum { result = PP_WHILE(TEST, STEP, 3) };
/* result == 0 */
#undef TEST
#undef STEP
```

---

## Detecção de plataforma

Os headers `Detect` expõem flags de tempo de compilação 0/1. Uma expressão agregada `*_KNOWN` é verdadeira quando pelo menos uma família implementada é reconhecida. Isso é **detecção do alvo**, não sondagem de capacidades em tempo de execução.

### Detecção de compilador

Header: `Cosmeron/Core/Preprocessor/Detect/Compiler.h`

| Flag | Família reconhecida |
| --- | --- |
| `COMPILER_INTEL` | Macros do compilador Intel |
| `COMPILER_CLANG` | Clang (exceto Intel) |
| `COMPILER_MSVC` | MSVC (exceto Intel ou Clang) |
| `COMPILER_GCC` | GCC (exceto os anteriores) |
| `COMPILER_KNOWN` | OR booleano das quatro famílias |

**Sintaxe**
```c
#if COMPILER_CLANG
/* Implementação específica para Clang. */
#endif
```

**Forma direta:** macros internas do compilador, como `__clang__` ou `_MSC_VER`, podem ser usadas diretamente, mas **não são intercambiáveis** com essas flags normalizadas e exclusivas.

**Parâmetros:** nenhum. **Retorno:** 0 ou 1 numérico para as flags de família; expressão booleana para `COMPILER_KNOWN`.

**Observações**
- Intel é verificado primeiro, depois Clang, MSVC e GCC; as famílias reconhecidas são mutuamente exclusivas.
- clang-cl pode definir várias macros de fornecedor, mas é classificado como Clang aqui.
- Outros compiladores podem deixar as quatro flags falsas sem que isso invalide a compilação.

**Exemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/Compiler.h"
#if COMPILER_MSVC
/* Ramo MSVC */
#elif COMPILER_GCC
/* Ramo GCC */
#endif
```

### Detecção de sistema operacional

Header: `Cosmeron/Core/Preprocessor/Detect/OperationSystem.h`

| Flag | Condição / família |
| --- | --- |
| `OS_WINDOWS` | `_WIN32` ou `_WIN64` (exceto Cygwin) |
| `OS_CYGWIN` | Ambiente Cygwin (`__CYGWIN__`) |
| `OS_MAC` | Apple / Mach (`__APPLE__ && __MACH__`) |
| `OS_IOS` | iOS / iPadOS / watchOS / tvOS |
| `OS_LINUX` | Kernel Linux (`__linux__` ou `__linux`) |
| `OS_ANDROID` | Camada de userland Android (`__ANDROID__`) |
| `OS_OHOS` | Camada de userland OpenHarmony (`__OHOS__`) |
| `OS_FREEBSD` | FreeBSD (`__FreeBSD__`) |
| `OS_OPENBSD` | OpenBSD (`__OpenBSD__`) |
| `OS_NETBSD` | NetBSD (`__NetBSD__`) |
| `OS_DRAGONFLY` | DragonFly BSD (`__DragonFly__`) |
| `OS_SOLARIS` | Solaris / SunOS / Illumos (`__sun__`, `__sun` ou `sun`) |
| `OS_AIX` | IBM AIX (`_AIX` ou `__TOS_AIX__`) |
| `OS_HAIKU` | Haiku / BeOS (`__HAIKU__` ou `__BEOS__`) |
| `OS_QNX` | QNX Neutrino (`__QNX__` ou `__QNXNTO__`) |
| `OS_HURD` | GNU Hurd (`__GNU__` ou `__gnu_hurd__`) |
| `OS_WASM` | WebAssembly (`__wasm__`, `__wasm32__`, `__wasm64__` ou WASI/Emscripten) |
| `OS_EMSCRIPTEN` | SDK Emscripten (`__EMSCRIPTEN__`) |
| `OS_WASI` | WebAssembly System Interface (`__wasi__`) |
| `OS_REDOX` | Microkernel Redox OS (`__redox__`) |
| `OS_BSD` | Qualquer família BSD (`OS_FREEBSD`, `OS_OPENBSD`, `OS_NETBSD`, `OS_DRAGONFLY`) |
| `OS_UNIX` | Sistemas semelhantes a Unix (`OS_LINUX`, `OS_MAC`, `OS_BSD`, `OS_SOLARIS`, `OS_AIX`, `OS_HAIKU`, `OS_QNX`, `OS_HURD`, `OS_CYGWIN`, `OS_REDOX`) |
| `OS_POSIX` | Plataformas compatíveis com POSIX (`OS_UNIX`, `OS_ANDROID`, `OS_OHOS`) |
| `OS_KNOWN` | Qualquer alvo de sistema operacional reconhecido |

**Sintaxe**
```c
#if OS_WINDOWS
/* Código específico para Windows */
#elif OS_POSIX
/* Alvo reconhecido no estilo POSIX */
#endif
```

**Forma direta:** macros do fornecedor da plataforma, como `_WIN32` ou `__linux__`, podem ser testadas manualmente, mas não há garantia de que tenham exatamente o mesmo significado das flags normalizadas do Cosmeron.

**Parâmetros:** nenhum. **Retorno:** flags numéricas 0/1 e expressões booleanas agregadas.

**Observações**
- `OS_UNIX` e `OS_POSIX` reconhecem sistemas Unix padronizados e plataformas compatíveis com POSIX.
- `OS_ANDROID` e `OS_OHOS` atuam como camadas de userland/ABI sobre `OS_LINUX`.
- No Windows, o header define `WIN32_LEAN_AND_MEAN` caso ainda não esteja definido.
- Essas macros não descobrem o sistema operacional em tempo de execução.

**Exemplo**
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

### Detecção de processador

Header: `Cosmeron/Core/Preprocessor/Detect/Processor.h`

| Flag | Arquitetura de destino reconhecida |
| --- | --- |
| `PROCESSOR_X86` | x86 de 32 bits |
| `PROCESSOR_X64` | x86-64 |
| `PROCESSOR_ARM` | ARM de 32 bits |
| `PROCESSOR_ARM64` | AArch64 |
| `PROCESSOR_RISCV` | RISC-V |
| `PROCESSOR_KNOWN` | OR booleano dessas arquiteturas |

**Sintaxe**
```c
#if PROCESSOR_ARM64
/* Ramo AArch64 */
#endif
```

**Forma direta:** macros de arquitetura fornecidas pelo compilador podem ser consultadas, mas a forma `PROCESSOR_*` é a interface estável oferecida pela biblioteca.

**Parâmetros:** nenhum. **Retorno:** flags numéricas 0/1 ou uma expressão booleana agregada.

**Observações**
- A detecção de arquitetura diz respeito ao alvo de compilação, não à máquina que executa o binário.
- RISC-V é detectado como uma família, sem distinguir 32 e 64 bits.
- Essas flags não verificam extensões de arquitetura, como AVX ou NEON.

**Exemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/Processor.h"
#if PROCESSOR_X64
/* Alvo x86-64 */
#elif PROCESSOR_ARM64
/* Alvo ARM64 */
#endif
```

### Detecção da biblioteca padrão C

Header: `Cosmeron/Core/Preprocessor/Detect/Library.h`

| Flag | Biblioteca de runtime C do alvo |
| --- | --- |
| `LIBC_GLIBC` | GNU C Library (glibc) |
| `LIBC_MUSL` | musl libc (padrão no Alpine e no Void musl) |
| `LIBC_BIONIC` | Bionic libc do Android |
| `LIBC_MSVC` | Microsoft Universal CRT / MSVCRT |
| `LIBC_APPLE` | Apple Libc (macOS / iOS / Darwin) |
| `LIBC_BSD` | BSD libc (FreeBSD, OpenBSD, NetBSD, DragonFly) |
| `LIBC_UCLIBC` | uClibc / uClibc-ng |
| `LIBC_NEWLIB` | Red Hat Newlib (embarcado / bare-metal) |
| `LIBC_WASI` | WebAssembly System Interface libc (wasi-libc) |
| `LIBC_EMSCRIPTEN` | libc do Emscripten |
| `LIBC_RELIBC` | relibc (biblioteca padrão C do Redox OS) |
| `LIBC_KNOWN` | Agregado booleano que indica qualquer libc reconhecida |

Aliases com prefixo `LIBRARY_*` (`LIBRARY_GLIBC`, `LIBRARY_MUSL` etc.) também são definidos por conveniência.

**Sintaxe**
```c
#if LIBC_GLIBC
/* Rotinas específicas da glibc */
#elif LIBC_MUSL
/* Ambiente musl padrão */
#endif
```

**Forma direta:** macros de plataforma como `__GLIBC__` ou `__BIONIC__` podem ser testadas manualmente, mas `LIBC_*` fornece flags normalizadas e mutuamente exclusivas entre as toolchains suportadas.

**Parâmetros:** nenhum. **Retorno:** flags numéricas 0/1 e expressões booleanas agregadas.

**Observações**
- Como a musl intencionalmente não exporta uma macro de fornecedor, `LIBC_MUSL` é identificada em alvos Linux quando glibc, uClibc, Bionic e Newlib estão ausentes, ou quando `__MUSL__` é fornecida.
- A detecção é mutuamente exclusiva entre ambientes C hospedados padrão.

**Exemplo**
```c
#include "Cosmeron/Core/Preprocessor/Detect/Library.h"
#if LIBC_GLIBC
/* Extensões GNU disponíveis */
#elif LIBC_MUSL
/* Ambiente musl padrão */
#endif
```

---

## Diagnósticos de inclusão

O arquivo `Compiling.inc` pode emitir uma mensagem do compilador no momento da inclusão quando `SHOW_INCLUDE_BUILD_MESSAGE` estiver definido.

### SHOW_INCLUDE_BUILD_MESSAGE

Habilita um diagnóstico `#pragma message` dentro de `Compiling.inc`.

**Sintaxe**
```c
#define SHOW_INCLUDE_BUILD_MESSAGE
#include "Cosmeron/Core/Preprocessor/Compiling.inc"
```

**Forma direta:** não existe equivalente como função; um `#pragma message` específico do compilador pode ser escrito manualmente.

**Parâmetros:** nenhum. **Retorno:** nenhum retorno em tempo de execução; emite um diagnóstico opcional do compilador quando a pragma é suportada.

**Observações**
- A mensagem emitida usa `__FILE__` no local em que a pragma aparece, isto é, o caminho de `Compiling.inc`, e não automaticamente o header que o incluiu.
- Muitos headers incluem `Compiling.inc`, portanto a saída pode se repetir.
- A opção é testada com `#ifdef`; defini-la como 0 ainda a ativa.
- A portabilidade de `#pragma message` depende do compilador.

**Exemplo**
```c
#define SHOW_INCLUDE_BUILD_MESSAGE
#include "Cosmeron/Core/Preprocessor/Operations.h"
/* Pode imprimir [Compiling: ...Compiling.inc]. */
```

---

# Exemplo completo

O exemplo abaixo combina concatenação, lógica booleana e declarações geradas por foreach:

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

# Observações

- Todas as operações são executadas durante o pré-processamento; esses utilitários não exigem **flags de linkedição adicionais** nem uma biblioteca de execução extra.
- A concatenação de tokens de macro não é concatenação de strings. Concatenações inválidas geram erros de pré-processamento.
- `PP_BOOL` e os auxiliares de seleção relacionados usam tabelas de consulta; não avaliam expressões aritméticas ou lógicas arbitrárias.
- `PP_FOREACH` processa uma lista não vazia; `PP_REPEAT` gera índices numéricos decrescentes.
- A expansão recursiva pode ser limitada pelo compilador e pela implementação finita de `PP_EVAL`.
- As flags de detecção representam as famílias atualmente implementadas, não todas as plataformas suportadas pelo C11.
- Prefira construções comuns de C quando a metaprogramação não trouxer um benefício concreto. Siga `../Reference/PATTERN.md), usando `PP_OP_CAT2` e auxiliares relacionados em vez de `##` diretamente no código dos módulos.
- Não existe um header agregado público `Preprocessor.h` nem uma API de “forma direta” em tempo de execução na superfície pública atual. Inclua os headers públicos específicos (`Arguments.h`, `Boolean.h`, `Operations.h`, `Foreach.h` e `Compiling.inc`) exigidos pelas macros utilizadas; não inclua fragmentos internos de implementação como substitutos.

## Módulos relacionados

- [Module](Module.md) — DSL pública de declaração construída sobre auxiliares do pré-processador.
- [Algorithm](Algorithm.md) — exemplos de templates tipados e declarações geradas.

## Headers de origem

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
