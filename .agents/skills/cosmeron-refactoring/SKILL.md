---
name: cosmeron-refactoring
description: >-
  Diretrizes e procedimentos para a refatoração completa do ecossistema Cosmeron.
  Garante adoção rigorosa dos novos padrões arquiteturais sem criar camadas de
  retrocompatibilidade ou manter código obsoleto.
---

# Cosmeron Refactoring Skill

Esta skill rege a refatoração total da base de código do **Cosmeron**.

> ⚠️ **REGRA FUNDAMENTAL: ZERO RETROCOMPATIBILIDADE**  
> **Não preserve nem crie camadas, aliases, wrappers ou macros de compatibilidade antiga.**  
> O projeto está passando por uma modernização completa de padrões. Qualquer construção legada encontrada deve ser **removida e substituída diretamente** pela sua forma canônica atualizada.

---

## 1. Níveis Declarativos e Arquitetura de Namespace

O ecossistema é organizado em quatro níveis hierárquicos:

```
DOMAIN (Agrupamento Arquitetural/Pasta: Core, Container, Text, IO, Graphics)
  └── MODULE (Unidade Fundamental de Compilação e Semântica: Memory, File, Bit, Text)
        ├── PACKAGE (Conjuntos Estáticos de Funções Livres: Alloc, Operations)
        ├── STRUCT (Containers POD Puros de Dados sem self/vtable: Point, Range, Header)
        ├── ISTRUCT (Containers POD Internos sem prefixo T e sem vtable: HeapPageHeader, HeapSpan)
        └── OBJECT (Instâncias com Estado, Receptor 'self' e FunctionTable: Stream, Pool)
              ├── SPACE (Sub-Namespaces Aninhados / Sub-Tabelas: Transform, Codec)
              └── VARIANT (Famílias de Sobrecargas Planas: Alloc, AllocAndFill)
```

### A. Eliminação Total de Arquivos `.space` e Boilerplate Local
- **Proibido:** Criar, manter ou incluir arquivos `.space`. Todos os arquivos `.space` foram eliminados.
- **Proibido:** Criar blocos manuais de macros locais de conveniência em cabeçalhos:
  ```c
  /* TOTALMENTE PROIBIDO: */
  #define FOO_NS(NAME)     ...
  #define FOO_CNS(NAME)    ...
  #define FOO_INS(NAME)    ...
  #define FOO_CINS(NAME)   ...
  #define FOO_TYPE(NAME)   ...
  #define FOO_FUNC(NAME)   ...
  #define FOO_CONST(NAME)  ...
  #define FOO_MOD / FOO_CMOD
  #define GNS2..GNS5 / CNS2..CNS5
  ```
- **Obrigatório:** Preferir semânticos `MODULE_*` (`MODULE_TYPE`, `MODULE_TYPED`, `MODULE_ENUM`, `MODULE_CONST`, `MODULE_FUNC`, `MODULE_NS`, `DOMAIN_NS`) no lugar de `NS(...)` bruto. Usar `NS_JOIN(...)` em $O(1)$ para concatenação plana de tokens.

### B. Distribuição Canônica de Arquivos e Infraestrutura Comum (`Common.inc`)
- **Sem Padrão Artificial de `Types.h`:** Tipos e enums não devem ser segregados artificialmente em arquivos `Types.h`.
- **`Common.inc`:** É destinado **estritamente a infraestrutura interna compartilhada** do próprio módulo (helpers inline, cálculos de alinhamento, funções utilitárias internas como `Memory/Common.inc`), evitando que pacotes do mesmo módulo reimplementem código. A extensão `.inc` sinaliza explicitamente que é um arquivo de inclusão interna, nunca público para o consumidor.
- **Tipos Específicos:** Ficam diretamente no cabeçalho do recurso correspondente (`Outcome/Status.h` define `OPSTATUS` e enums de status; `Algorithm/Comparison.h` define `CMPOUT` e enums de comparação).

---

## 2. Declaração Declarativa no Cabeçalho (`.h`)

Os cabeçalhos declaram o módulo via `MODULE(...)`:

```c
#pragma once
#include "Core/Module/Module.h"

MODULE(DOMAIN(Core), Text,
    /* 1. Enum com tabela X-Macro desacoplada (suporta ENUM e ENUM_EX com dispatch PLAIN, QUALIFIED, NAME_ONLY ou custom) */
    ENUM(SeekOrigin, SEEK_TABLE),
    ENUM_EX(Policy, POLICY_TABLE, QUALIFIED),

    /* 1.5. Struct POD puro (Gera Text_TPoint sem vtable/api) */
    STRUCT(Point,
        FIELDS(
            int32_t x;
            int32_t y;
        )
    ),

    /* 1.6. Struct POD interno (Gera Text_Internal_BufferNode sem prefixo T e sem vtable/api) */
    ISTRUCT(BufferNode,
        FIELDS(
            void *next;
            size_t size;
        )
    ),

    /* 2. Objeto com estado (Gera automaticamente o tipo Text_TString) */
    OBJECT(String,
        FIELDS(
            char *data;
            size_t size;
            size_t capacity;
        ),
        OPERATION(Clear, ()),
        OPERATION(Reserve, (size_t newCapacity)),
        CONST_FUNCTION(size_t, Length, ()),
        CONST_PREDICATE(IsEmpty, ()),
        CONST_COMPARISON(Compare, (const MODULE_TYPE(Text, String) *other))
    ),

    /* 3. Pacote de funções livres */
    PACKAGE(CString,
        FUNCTION(size_t, Length, (const char *str)),
        PREDICATE(IsEmpty, (const char *str)),

        /* Sub-Namespace com sub-tabela no FunctionTable */
        SPACE(Transform,
            PROCEDURE(ToUpper, (char *dest, const char *src)),
            PROCEDURE(ToLower, (char *dest, const char *src))
        ),

        /* Família plana de sobrecarga */
        VARIANT(Alloc,
            FUNCTION(void *, (size_t size)),
            FUNCTION(void *, AndFill, (size_t size, char fillChar))
        )
    )
);
```

### Contratos de Retorno de Membros
- `OPERATION(Name, (params))` $\to$ `OPSTATUS`.
- `PREDICATE(Name, (params))` $\to$ `bool`.
- `FUNCTION(Ret, Name, (params))` $\to$ `Ret`.
- `PROCEDURE(Name, (params))` $\to$ `void`.
- `COMPARISON(Name, (params))` $\to$ `CMPOUT` (`ComparisonResult`: `LOWER`, `EQUAL`, `HIGHER`).
- Use prefixo `CONST_*` para leitores em `OBJECT`.
- Use sufixo `*_INLINE(..., { corpo })` quando a implementação estiver no cabeçalho.

---

## 3. Templates Declarativos (`TEMPLATE_OBJECT`, `TEMPLATE_PACKAGE` e `TEMPLATE_STRUCT`)

Templates são declarados dentro de `MODULE(...)`:

```c
MODULE(DOMAIN(Container), Container,
    TEMPLATE_OBJECT(Vector, (SUFFIX, TYPE),
        FIELDS(
            TYPE *items;
            size_t count;
            size_t capacity;
        ),
        OPERATION(Push, (TYPE item)),
        OPERATION(Pop, (TYPE *outItem)),
        CONST_FUNCTION(size_t, Count, ()),
        CONST_PREDICATE(IsEmpty, ())
    ),

    TEMPLATE_PACKAGE(BitOps, (SUFFIX, TYPE),
        OPERATION(Set, (TYPE *value, uint8_t bit)),
        PREDICATE(Check, (TYPE value, uint8_t bit))
    ),

    TEMPLATE_STRUCT(Pair, (SUFFIX, TYPE),
        FIELDS(
            TYPE first;
            TYPE second;
        )
    )
);
```

### Regras de Templates:
- **POD Puro Canônico é Regra Absoluta:** `STRUCT` e `TEMPLATE_STRUCT` são unicamente Plain Old Data. É expressamente proibido criar métodos artificiais (`Create`, `Extract`, `Unpack`, `Cast_*`) ou tabelas de funções (`FunctionTable`) para structs. Manipulação e instanciação usam 100% C11 nativo (designated initializers e acesso direto por ponto).
- **Zero Macros de Blueprint & Zero Gambiarras:** Proibido definir macros `#define TEMPLATE_<Name>(...)`, blocos monolíticos de código com `\` ou teias de inclusão manual `.inc`. A declaração vive exclusivamente dentro de `MODULE(...)`.
- **Protocolo de Extensão da DSL:** Se uma funcionalidade necessária (como pack/unpack ou serialização) não se encaixa de forma limpa na DSL declarativa existente, **NUNCA improvise soluções malucas**. Pare imediatamente, proponha uma extensão canônica para a DSL (`MODULE(...)`, `Declarative.impl`) e consulte o usuário.
- **Modos de Instanciação:**
  1. **Instanciação Direta:** `TEMPLATE_INSTANTIATE(Vector, uint32, uint32_t);`
  2. **Instanciação de Struct:** `TEMPLATE_STRUCT(Container, Pair, int, FIELDS(int first; int second;));`
  3. **Instanciação em Lote (X-Macro):** `TEMPLATE_INSTANTIATE_TABLE(BitOps, BIT_WIDTH_TABLE);`

---

## 4. Implementação no `.impl` / `.c` (Lexical Scope)

O arquivo `.impl` utiliza o modelo de escopo léxico, **sem macros finalizadoras obrigatórias** (`PACKAGE_IMPLEMENT` / `OBJECT_IMPLEMENT`):

```c
#pragma once
#include "String.h"

MODULE_SCOPE(Text, String)

OPERATION(Clear, ()) {
    if (!self) return STATUS(INVALID_ARGUMENT);
    self->size = 0U;
    return STATUS(SUCCESS);
}

CONST_FUNCTION(size_t, Length, ()) {
    return self ? self->size : 0U;
}

MODULE_SCOPE_END()

PACKAGE_SCOPE(Text, CString)

FUNCTION(size_t, Length, (const char *str)) {
    if (!str) return 0U;
    return strlen(str);
}

SPACE_SCOPE(Transform)
PROCEDURE(ToUpper, (char *dest, const char *src)) {
    /* ... */
}
SPACE_SCOPE_END()

PACKAGE_SCOPE_END()
```

---

## 5. Gramática Canônica de Consumo Cross-Module

Todo consumo externo utiliza a gramática unificada de `Namespace.h`, imune a prefixos:

```c
/* 1. Tipos de Objeto (Regra do 'T' automático): */
MODULE_TYPE(Text, String) str; /* Expande para: Text_TString */

/* 2. Tipos de Template: */
MODULE_TYPED(Container, Vector, uint32) vec; /* Expande para: Container_TVector_uint32 */

/* 3. Forward Declarations: */
FORWARD_OBJECT(Text, String);
FORWARD_TEMPLATE_OBJECT(Container, Vector, uint32);
FORWARD_ENUM(Text, SeekOrigin);

/* 4. Enums e Constantes: */
MODULE_ENUM(Text, SeekOrigin) origin = MODULE_CONST(Text, CUR);

/* 5. Identificadores Semânticos em O(1): */
MODULE_NS(Text, CString);               /* Expande para: Text_CString */
MODULE_FUNC(Text, CString, Length);     /* Expande para: Text_CString_Length */
DOMAIN_NS(Core, Algorithm, Repeat);     /* Expande para: Core_Algorithm_Repeat */

/* 6. Status Universal: */
OPSTATUS status = STATUS(SUCCESS);

/* 7. Invocação Despachada (MODULE_CALL em O(1)): */
MODULE_CALL(Text, String, Init, (&str));
MODULE_CALL(Text, String, Clear, (&str));
size_t len = MODULE_CALL(Text, CString, Length, ("abc"));
MODULE_CALL(Text, CString, Transform, ToUpper, (dest, src));
```

---

## 6. Sistema Declarativo de Eventos & Mutação Reativa

- Slots estáticos embutidos com zero alocação dinâmica (capacidade com decay para 4, 8, 12, 16 slots).
- Declaração por pares de tupla sem duplicação:
  ```c
  EVENT_PROCEDURE(Click, ((int, button), (int, x), (int, y)))
  EVENT_OPERATION(Process, ((int, code)))
  EVENT_PROCEDURE(Reset, ())
  ```
- Mutação Reativa: `OBJECT_MUTATE(self, ClassName, fieldName, newValue, EventName);`.

---

## 7. Callbacks de Primeira Classe (`TCALLBACK`)

- Callbacks tipados via `CALLBACK_OPERATION`, `CALLBACK_PREDICATE`, `CALLBACK_FUNCTION`, `CALLBACK_PROCEDURE`, `CALLBACK_COMPARISON`.
- Sem variantes `WithContext`. Unicamente `TCALLBACK(Name)`.
- Invocação: `CALLBACK_CALL(cb, args...)` ou `CALLBACK_INVOKE(cb, args...)`.
- Validação: `CALLBACK_IS_VALID(cb)`.
- Comparador Canônico: `TCALLBACK(Comparison)`.

---

## 8. Checklist de Refatoração Passo-a-Passo

Ao refatorar qualquer módulo:

1. **Exclua o arquivo `.space` e remova seus `#include`**.
2. **Remova qualquer macro boilerplate local (`FOO_NS`, `FOO_CNS`, etc.)**.
3. **Crie `Common.inc` apenas se houver infraestrutura interna comum a ser compartilhada** entre os pacotes do módulo (não crie `Types.h`).
4. **Declare o módulo com `MODULE(...)`** agrupando seus `OBJECT`, `PACKAGE`, `ENUM`, `SPACE`, `VARIANT` ou `TEMPLATE_*`.
5. **Implemente no `.impl` usando `MODULE_SCOPE`**, sem adicionar finalizadores `PACKAGE_IMPLEMENT`/`OBJECT_IMPLEMENT`.
6. **Substitua comparadores e callbacks brutos por `TCALLBACK(...)`**.
8. **Substitua qualquer checagem direta de SO, compilador ou arquitetura por `Detect.h`**: Proibido `#ifdef _WIN32`, `#ifdef __linux__`, `#if defined(__clang__)`, etc. Toda detecção é centralizada via `Codespace/Cosmeron/Core/Preprocessor/Detect.h`.
9. **Atualize e valide os testes correspondentes em `Codespace/Tests/`** (`-Wall -Wextra -Wpedantic -Werror`).
