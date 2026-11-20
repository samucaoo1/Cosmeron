# Padrão arquitetural e de código do Cosmeron

Este documento descreve a arquitetura, as convenções de nomenclatura, as declarações e os padrões de implementação do ecossistema Cosmeron.

> **Filosofia central:** prefira construções nativas de C a macros do pré-processador. Use macros para vinculação de namespace, geração declarativa de código e processamento interno. Números mágicos e camadas obsoletas de compatibilidade são proibidos.

## 1. Camadas e hierarquia arquitetural

O Cosmeron organiza o código em quatro níveis declarativos:

```
DOMAIN
  └── MODULE
        ├── PACKAGE
        ├── STRUCT
        └── OBJECT
              ├── SPACE
              └── VARIANT
```

- **DOMAIN** agrupa módulos por área arquitetural ou diretório, como Core, Container, Text, Security e IO.
- **MODULE** é a unidade fundamental de compilação e semântica, como Memory, File, Bit e Text. Um módulo pode existir sem DOMAIN; o domínio é apenas organizacional.
- **PACKAGE** reúne funções livres, eventos, callbacks e subnamespaces estáticos. Não possui receptor `self`, estrutura de instância ou tipo prefixado com `T`.
- **STRUCT** é um contêiner POD puro, sem ponteiro `api`, FunctionTable ou receptor. Gera um tipo público com prefixo `T` e aceita inicializadores designados de C.
- **ISTRUCT** é um POD interno, nomeado sob `Module_Internal_`, sem prefixo `T`.
- **INTERNAL / INTERNAL_EX** declaram infraestrutura e auxiliares internos, normalmente como funções `static inline`, sem custo de FunctionTable.
- **OBJECT** representa uma instância com estado, campos, métodos vinculados a `self`, auxiliares de ciclo de vida e tipo público prefixado com `T`.
- **SPACE** cria um subnamespace dentro de PACKAGE ou OBJECT.
- **VARIANT** agrupa uma família plana de sobrecargas sob um prefixo comum.

### 1.1 Identidade do módulo

As duas formas são válidas:

```c
MODULE(DOMAIN(Core), Memory, ...);
MODULE(Memory, ...);
```

Em ambos os casos, os símbolos C canônicos seguem o namespace do módulo, com o prefixo de biblioteca configurado quando aplicável.

### 1.2 SPACE e VARIANT

`SPACE(Name, ...)` cria símbolos e tabelas aninhados. `VARIANT(Family, ...)` agrupa operações relacionadas: a operação-base usa o nome da família e as variantes acrescentam um sufixo.

## 2. Nomenclatura e uso de maiúsculas

Tudo deve permanecer no namespace do módulo/package sempre que C permitir. Exceções são limitadas a macros públicas de conveniência previamente aprovadas, `OPSTATUS` e aliases curtos de comparação terminados em `OUT`.

| Elemento | Convenção | Exemplo |
|---|---|---|
| Domain, Module, Package e Object | PascalCase | `Core`, `Memory`, `Repeat` |
| Tipo público | PascalCase com `T` | `TVector_uint32`, `TMemoryPool` |
| Enum de saída/tipo interno | PascalCase sem `T` | `ComparisonResult`, `SeekOrigin` |
| Função/método | PascalCase | `Memory_Copy`, `Vector_Push` |
| Constante/item de enum | SNAKE_CASE | `SUCCESS`, `OUT_OF_RANGE` |
| Macro do pré-processador | SNAKE_CASE | `MODULE`, `PACKAGE_CALL` |
| Macro interna | `COSMERON_MACRO_INTERNAL_*` | `COSMERON_MACRO_INTERNAL_PKG_CALL_0` |
| Parâmetro, variável ou campo | camelCase | `destination`, `bufferCapacity` |

### 2.1 Regra do prefixo T

Use `T` somente em tipos destinados ao uso direto pelo consumidor. Tipos de objetos gerados recebem o prefixo automaticamente. Enums de saída e tipos internos não usam `T`. Evite nomes genéricos ambíguos, como `TPool` e `TNode`; prefira nomes semânticos explícitos, como `TMemoryPool` e `TGraphNode`.

### 2.2 Macros de namespace

Prefira as macros semânticas `MODULE_TYPE`, `MODULE_TYPED`, `MODULE_ENUM`, `MODULE_CONST`, `MODULE_FUNC`, `MODULE_NS` e `DOMAIN_NS` a concatenações manuais. `NS_JOIN` realiza a junção plana em tempo constante de expansão.

Não crie arquivos `.space` nem macros de namespace locais em headers, como `FOO_NS`, `FOO_FUNC`, `FOO_TYPE`, `FOO_CONST`, `GNS2` ou `CNS2`. Use as macros canônicas de `Core/Module/Namespace.h`.

### 2.3 Organização de arquivos e Common.inc

Não crie headers sintéticos `Types.h`: tipos e enums pertencem ao header do recurso correspondente. Reserve `Common.inc` exclusivamente para infraestrutura interna compartilhada dentro do módulo, evitando duplicação entre packages. O arquivo não deve ser incluído diretamente por consumidores.

### 2.4 Configuração do namespace da biblioteca

`LIBRARY_NAMESPACE_CONFIG` ou flags de compilação podem configurar o prefixo dos símbolos:

- `BARE`: sem prefixo da biblioteca, por exemplo `Text_TString`;
- `SHORT`: prefixo curto, por exemplo `Cos_Text_TString`;
- `STANDARD`: prefixo padrão `Cosmeron_`;
- `FULL`: inclui também o domínio, por exemplo `Cosmeron_Core_Text_TString`.

## 3. Contratos de funções e métodos

Funções e métodos devem pertencer a blocos declarativos e seguir contratos explícitos:

| Contrato | Macro | Retorno | Significado |
|---|---|---|---|
| Operação sujeita a falha | `OPERATION(Name, Params)` | `OPSTATUS` | Ação que informa status |
| Predicado | `PREDICATE(Name, Params)` | `bool` | Consulta lógica |
| Função de valor | `FUNCTION(Ret, Name, Params)` | `Ret` | Produz um valor |
| Procedimento | `PROCEDURE(Name, Params)` | `void` | Ação sem falha reportável |
| Comparação de três vias | `COMPARISON(Name, Params)` | `CMPOUT` | Retorna LOWER, EQUAL ou HIGHER |

### 3.1 Receptor e variantes inline

Em OBJECT, métodos recebem `self`; métodos somente de leitura usam `CONST_*` e receptor constante. Definições inline usam o sufixo `_INLINE` e incorporam o corpo à declaração.

```c
OPERATION_INLINE(Clear, (), { self->count = 0; return STATUS(SUCCESS); })
PREDICATE_INLINE(IsEmpty, (size_t size), { return size == 0U; })
```

### 3.2 Erros e parâmetros

- Parâmetros `out` obrigatórios não podem ser nulos; nesse caso, retorne `STATUS(INVALID_ARGUMENT)`.
- Modifique um parâmetro `out` somente quando o retorno for `STATUS(SUCCESS)`.
- Ponteiros são emprestados por padrão; marque entradas somente de leitura com `const`.
- Nunca retorne inteiros arbitrários como status. Use constantes nomeadas, como `STATUS(SUCCESS)` e `STATUS(OUT_OF_RANGE)`.

## 4. Declarações em headers

Headers declaram recursos por meio de `MODULE(...)`, envolvendo PACKAGE, OBJECT, ENUM, SPACE, VARIANT ou templates.

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

O Cosmeron utiliza um mecanismo canônico de templates `.def` em modo duplo, com uma única fonte de verdade. O corpo declarativo é escrito uma única vez no arquivo `<Target>.def`.

### 5.1 Modos de uso

1. **Exportação de macro (recomendado):** incluir o `.def` sem parâmetros previamente definidos. O arquivo exporta `TEMPLATE_<Module>_<Target>`, permitindo instanciação sob demanda com `TEMPLATE_INSTANTIATE` ou em lote com `TEMPLATE_INSTANTIATE_TABLE`.
2. **Instanciação por inclusão direta:** definir previamente os parâmetros, como `SUFFIX` e `TYPE`, incluir o `.def` e deixar que ele instancie o template e remova os parâmetros com `#undef`.

### 5.2 Regras

1. O corpo do template deve existir em um único lugar; não duplique listas de membros ou campos.
2. Use `TEMPLATE_<Module>_<Target>(PARAMS...)` como nome da macro.
3. Tipos públicos gerados seguem `T<Target>_<suffix>`; tipos internos omitem o `T`.
4. A ordem dos parâmetros é homogênea: identificador `SUFFIX`, tipos de dados e, por fim, metadados como capacidade ou política.
5. Aridades comuns: `(SUFFIX, TYPE)`; `(SUFFIX, FIRST_TYPE, SECOND_TYPE)`; ou combinações com `CAPACITY`.
6. Linhas X-macro devem poder ser desempacotadas diretamente com `PP_UNWRAP ROW`.
7. STRUCTs POD e templates com métodos inline são autocontidos. Não exigem macros stub `_IMPLEMENT`; métodos não inline são implementados em `.impl` quando existirem.

### 5.3 Padrão recomendado para templates de módulo

Quando um template de módulo gera packages ou objetos parametrizados diretamente no namespace do módulo:

- Use `MODULE_TEMPLATE` no `.def` para distinguir definições-template de declarações fixas.
- Coloque algoritmos e constantes independentes de tipo/largura em `INTERNAL(...)` dentro de `Impl/Common.inc`, sem FunctionTable.
- Declare constantes numéricas internas por X-Macro e ENUM; evite `#define` numérico bruto e números mágicos.
- Agrupe famílias relacionadas com `VARIANT`.

## 6. Escopo das implementações (.impl / .c)

Arquivos de implementação usam escopo léxico por meio de `*_SCOPE` e `*_SCOPE_END()`. Não usam macros finalizadoras `PACKAGE_IMPLEMENT` ou `OBJECT_IMPLEMENT`.

### 6.1 Implementação de PACKAGE

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

### 6.2 Implementação de OBJECT

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

## 7. Callbacks, closures e eventos declarativos

### 7.1 Callbacks puros (TCALLBACK)

Callbacks são ponteiros de função puros e sem estado, sem custo adicional: ocupam exatamente `sizeof(void *)`. Podem ser declarados isoladamente ou em blocos variádicos dentro de MODULE, PACKAGE ou OBJECT.

Declarações disponíveis: `CALLBACK_OPERATION`, `CALLBACK_PREDICATE`, `CALLBACK_FUNCTION`, `CALLBACK_PROCEDURE` e `CALLBACK_COMPARISON`.

```c
MODULE(DOMAIN(Core), MyModule,
    CALLBACK(
        FUNCTION(int, Transform, (int x)),
        PROCEDURE(Log, (int code))
    )
);
```

Use `CALLBACK_CALL` ou `CALLBACK_INVOKE` para invocar, `CALLBACK_IS_VALID` para validar e `CALLBACK_EMPTY` para representar ausência (`NULL`).

### 7.2 Closures contextuais (TCLOSURE)

Closures combinam um ponteiro de função e um contexto opcional emprestado em uma estrutura POD de dois ponteiros. Há declarações correspondentes a operação, predicado, função, procedimento e comparação.

- `CLOSURE_FUNC(Name_Bind)(fn, ctx)` associa um alvo com estado que recebe `void *context` ao final.
- `CLOSURE_FUNC(Name_FromCallback)(cb)` promove um callback puro sem wrapper.
- `CLOSURE_CALL` escolhe a chamada direta quando o contexto é nulo ou a chamada contextual quando não é.
- `CLOSURE_ADAPT_OBJECT_METHOD` adapta métodos com receptor para assinaturas de closure.
- `CLOSURE_AS_CLOSURE` cria uma closure a partir de uma instância de objeto.

### 7.3 Eventos

`EVENT_PROCEDURE` e `EVENT_OPERATION` oferecem notificação observador 1-para-N sem alocação, usando TCLOSURE. Os slots são embutidos em estruturas ou packages. A emissão cria um snapshot local dos listeners ativos, tornando a iteração segura contra reentrância. `OBJECT_MUTATE` atualiza um campo e emite o evento quando há alteração.

## 8. FunctionTables e desativação em compilação

Packages e objects podem despachar por FunctionTable. Packages usam uma tabela singleton e `PACKAGE_CALL`; objects usam `obj->api` e `OBJECT_CALL`.

A tabela pode ser removida em compilação:

- `MODULE_DISABLE_FUNCTION_TABLE`: todas as tabelas;
- `PACKAGE_DISABLE_FUNCTION_TABLE`: packages;
- `OBJECT_DISABLE_FUNCTION_TABLE`: objects;
- flags específicas por módulo/alvo, avaliadas durante a expansão sem contaminar módulos seguintes;
- tag declarativa `NO_FUNCTION_TABLE` dentro da declaração.

Quando desativada, nenhuma estrutura ou instância de FunctionTable é emitida.

## 9. Enums e padrão de tabelas X-macro

Todo enum deve usar uma tabela X-macro desacoplada, no padrão Row-Tuple:

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

Enums podem ser declarados em MODULE, PACKAGE ou OBJECT, ou fora deles com `ENUM_DEF` e `ENUM_DEF_EX`. Presets de dispatch:

- `PLAIN` / `NAME_VALUE`: emite constante com nome e valor.
- `QUALIFIED`: inclui o nome do enum para evitar colisões.
- `NAME_ONLY`: emite apenas o nome e deixa C incrementar o valor.
- `QUALIFIED_NAME`: nome qualificado sem valor explícito.
- Dispatch customizado: macro `DISPATCH(TARGET, ENUM_NAME, ROW)`.

## 10. Gramática canônica de consumo

Consumidores devem usar a gramática unificada de `Namespace.h`:

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

## 11. Regras estritas de refatoração (sem retrocompatibilidade)

1. Exclua arquivos `.space`; nunca os crie, mantenha ou inclua.
2. Elimine macros boilerplate de namespace nos headers.
3. Não crie `Types.h` sintéticos; mantenha os tipos junto de seus recursos e reserve `Common.inc` para utilitários internos compartilhados.
4. Não crie macros blueprint que dupliquem declarações de templates.
5. Não mantenha camadas de retrocompatibilidade, wrappers ou typedefs legados.
6. Não use números mágicos; use status nomeados e enums X-macro.
7. Não faça detecção ad hoc de plataforma/compilador. Use exclusivamente `Core/Preprocessor/Detect.h`.
8. Identificadores, comentários, mensagens de commit e documentação do código devem permanecer em inglês.

## 12. Detecção centralizada de plataforma e ambiente

Todo código dependente do ambiente deve incluir `Core/Preprocessor/Detect.h` ou `Detect/Detect.h`. A detecção fornece constantes de compilação normalizadas em 0 ou 1:

- **Sistema operacional:** `OS_WINDOWS`, `OS_LINUX`, `OS_MAC`, `OS_IOS`, `OS_BSD`, `OS_FREEBSD`, `OS_OPENBSD`, `OS_NETBSD`, `OS_DRAGONFLY`, `OS_SOLARIS`, `OS_AIX`, `OS_HAIKU`, `OS_QNX`, `OS_HURD`, `OS_REDOX`, `OS_WASI`, `OS_EMSCRIPTEN`, `OS_POSIX`, `OS_UNIX`, `OS_ANDROID`, `OS_OHOS`.
- **Compilador:** `COMPILER_CLANG`, `COMPILER_GCC`, `COMPILER_MSVC`, `COMPILER_INTEL`, `COMPILER_KNOWN`.
- **Arquitetura:** `PROCESSOR_X86`, `PROCESSOR_X64`, `PROCESSOR_ARM`, `PROCESSOR_ARM64`, `PROCESSOR_RISCV`, `PROCESSOR_WASM`, `PROCESSOR_PPC`, `PROCESSOR_PPC64`, `PROCESSOR_MIPS`, `PROCESSOR_LOONGARCH64`, `PROCESSOR_S390X`, `PROCESSOR_KNOWN`.
- **Biblioteca C:** `LIBC_GLIBC`, `LIBC_MUSL`, `LIBC_BIONIC`, `LIBC_MSVC`, `LIBC_APPLE`, `LIBC_BSD`, `LIBC_UCLIBC`, `LIBC_NEWLIB`, `LIBC_WASI`, `LIBC_EMSCRIPTEN`, `LIBC_RELIBC`, `LIBC_KNOWN`.

## Arquivos de referência

- `Codespace/Cosmeron/Core/Module/Namespace.h`
- `Codespace/Cosmeron/Core/Preprocessor/Detect.h`
