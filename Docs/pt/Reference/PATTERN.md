# Padrão arquitetural e de código do Cosmeron

Este documento é a **especificação canônica única** para a arquitetura, nomenclatura, declarações e padrões de implementação de todo o ecossistema Cosmeron.

> **Filosofia central:** prefira construções nativas de C a macros do pré-processador. Use macros somente para vinculação de namespace, geração declarativa de código e pré-processamento interno. Números mágicos e camadas obsoletas de compatibilidade são estritamente proibidos.

---

## 1. Camadas e hierarquia arquitetural

O Cosmeron organiza o código em quatro níveis declarativos:

```
DOMAIN (Agrupamento arquitetural / de diretórios: Core, Container, Text, Security, IO)
  └── MODULE (Unidade fundamental de compilação e semântica: Memory, File, Bit, Text)
        ├── PACKAGE (Coleções estáticas de funções livres: Alloc, Operations, Swap)
        ├── STRUCT (Contêineres POD sem self/vtable: Point, Range, Header)
        └── OBJECT (Instâncias com estado, receptor 'self' e FunctionTable: Stream, Pool)
              ├── SPACE (Subnamespaces / subtabelas aninhados: Transform, Codec)
              └── VARIANT (Famílias planas de sobrecargas: Alloc, AllocAndFill, AllocZeroed)
```

### 1.1 O papel semântico de `MODULE`

- `MODULE` é a unidade fundamental de semântica e compilação da biblioteca (`Memory`, `Text`, `Algorithm`, `Sort`).
- Um módulo não precisa de um domínio para possuir identidade semântica completa. `DOMAIN` existe exclusivamente como agrupamento organizacional.
- As duas formas são igualmente válidas:
  - Com domínio organizacional: `MODULE(DOMAIN(DomainName), ModuleName, ...)`
  - Sem domínio: `MODULE(ModuleName, ...)`
- Em ambos os casos, os símbolos C canônicos emitidos seguem o formato `[Library_]ModuleName_Entity`.

### 1.2 Packages, structs e objects

- **`PACKAGE`**: coleção coesa e sem estado de funções livres, eventos, callbacks e subnamespaces. Não vincula um receptor `self`, não emite uma struct de instância nem um tipo com prefixo `T`.
- **`STRUCT`**: struct POD (Plain Old Data) pura. Emite um tipo com namespace e prefixo `T` (`ModuleName_TStructName`), mas não possui custo adicional: nenhum ponteiro `api`, FunctionTable ou receptor `self`. É compatível com inicializadores designados de C (`.field = val`), passagem em registradores pela ABI e `memcpy`.
- **`ISTRUCT`**: struct POD interna. Emite um tipo interno com namespace (`ModuleName_Internal_StructName`), sem o prefixo `T`, seguindo a convenção do Cosmeron para tipos internos. Não possui custo de FunctionTable.
- **`INTERNAL` / `INTERNAL_EX`**: escopo declarativo interno para rotinas auxiliares e infraestrutura do módulo. Emite funções `static inline` puras, sem custo de FunctionTable.
  - No nível do módulo: `INTERNAL(...)` emite `ModuleName_Internal_FuncName`.
  - No nível de componente: `INTERNAL_EX(Comp, ...)` ou `INTERNAL(Comp, ...)` emite `ModuleName_Comp_Internal_FuncName`.
  - Para invocar, use `MODULE_IFUNC(Mod, ...)` ou `MODULE_ICALL(Mod, ..., (args...))`.
- **`OBJECT`**: tipo com estado, campos estruturados, auxiliares de ciclo de vida (`Init`), métodos vinculados ao receptor (`self`) e geração automática de tipo de usuário com prefixo `T` (`ModuleName_TObjectName`).

### 1.3 Subnamespaces (`SPACE`) e famílias de sobrecarga (`VARIANT`)

- **`SPACE(SubName, ...)`**: declara um subnamespace aninhado dentro de um `PACKAGE` ou `OBJECT`. Emite símbolos aninhados (`Module_Package_SubName_Func`) e subtabelas aninhadas na `FunctionTable`.
- **`VARIANT(FamilyName, ...)`**: declara uma família plana de sobrecargas que compartilham um prefixo de nome, sem tokens vazios brutos. A função-base omite o subnome (`FUNCTION(RET, PARAMS)`), enquanto as variantes acrescentam um sufixo (`FUNCTION(RET, Suffix, PARAMS)`).

---

## 2. Nomenclatura e convenções de maiúsculas

Tudo no Cosmeron deve permanecer dentro do namespace do módulo/package sempre que C permitir. As únicas exceções são macros públicas de conveniência (que exigem aprovação prévia), `OPSTATUS` e aliases curtos de comparação `*OUT`.

### 2.1 Tabela de convenções

| Elemento | Convenção | Exemplo |
|---|---|---|
| Token de Domain / Module / Package / Object | `PascalCase` | `Core`, `Memory`, `Algorithm`, `Repeat` |
| Tipo de dados voltado ao usuário | `PascalCase` com `T` | `TVector_uint32`, `TMemoryPool`, `TString` |
| Enum de saída / tipo interno | `PascalCase` sem `T` | `ComparisonResult` (`CMPOUT`), `SeekOrigin` |
| Função / método | `PascalCase` | `Memory_Copy`, `Vector_Push`, `Repeat_Procedure` |
| Constante / item de enum | `SNAKE_CASE` | `SUCCESS`, `OUT_OF_RANGE`, `LOWER` |
| Macro do pré-processador | `SNAKE_CASE` | `MODULE`, `PACKAGE_CALL`, `MODULE_SCOPE` |
| Macro interna do pré-processador | `COSMERON_MACRO_INTERNAL_*` | `COSMERON_MACRO_INTERNAL_PKG_CALL_0` |
| Parâmetro / variável local / campo | `camelCase` | `destination`, `count`, `bufferCapacity` |

### 2.2 Regra do prefixo `T`

- Use o prefixo `T` **somente** para tipos destinados ao uso direto pelo usuário (por exemplo, `TMemoryPool`, `TVector_uint32`).
- Tipos de objetos gerados recebem automaticamente o prefixo `T` (`OBJECT(String)` emite `Text_TString`).
- Enums de saída e tipos internos **não** usam `T` (por exemplo, `SeekOrigin`, `ComparisonResult`).
- Evite nomes genéricos que possam colidir, como `TPool` ou `TNode`. Qualifique explicitamente o domínio semântico (`TMemoryPool`, `TGraphNode`).

### 2.3 Proibição estrita de boilerplate local de namespace em headers e macros semânticas O(1)

- **Prefira `MODULE_*` a `NS` bruto**: use sempre macros semânticas (`MODULE_TYPE`, `MODULE_TYPED`, `MODULE_ENUM`, `MODULE_CONST`, `MODULE_FUNC`, `MODULE_NS`, `DOMAIN_NS`) em interações públicas e voltadas ao consumidor.
- **Junção plana O(1) (`NS_JOIN`)**: concatenação rápida, não recursiva e de substituição única, com separador underscore (`_`), em O(1) etapas do pré-processador.
  - `MODULE_NS(Modulo, Pacote)`: emite `[LIB_]Modulo_Pacote` em O(1).
  - `MODULE_FUNC(Modulo, Pacote, Func)`: emite `[LIB_]Modulo_Pacote_Func` em O(1).
  - `DOMAIN_NS(Domain, ...)`: emite `Domain_...` em O(1).
- **Nenhum arquivo `.space`**: todos os arquivos `.space` legados foram removidos permanentemente.
- **Nenhuma macro manual de namespace no nível do header**. Nunca defina:
  ```c
  /* PROIBIDO: */
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
- Use as macros canônicas `MODULE_*` ou `NS(...)` / `CNS(...)` de `Core/Module/Namespace.h`.

### 2.4 Organização de arquivos e `Common.inc`

- **Nenhum `Types.h` sintético**: tipos e enums ficam no header do recurso correspondente.
- **`Common.inc`**: reservado **estritamente** à infraestrutura interna compartilhada de um módulo (helpers inline, cálculos e tipos internos compartilhados, como `Memory/Common.inc)), evitando duplicação de código entre packages na mesma pasta do módulo. A extensão `.inc` deixa explícito que se trata de uma inclusão interna, nunca destinada à inclusão direta por usuários.

### 2.5 Configuração do namespace da biblioteca (`LIBRARY_NAMESPACE_CONFIG`)

Os símbolos do Cosmeron podem receber uma marca ou ser configurados globalmente por `LIBRARY_NAMESPACE_CONFIG` ou por flags do compilador (`-DCOSMERON_NAMESPACE=...`, `-DCOSMERON_NAMESPACE_MODE=...`):

- `BARE`: `Text_TString`, `Text_String_Init`, `Status_SUCCESS` (sem prefixo da biblioteca).
- `SHORT`: `Cos_Text_TString`, `Cos_Text_String_Init`.
- `STANDARD`: `Cosmeron_Text_TString`, `Cosmeron_Text_String_Init` (padrão).
- `FULL`: `Cosmeron_Core_Text_TString` (inclui o domínio no símbolo).

Quando não configurada, `LIB_PREFIX(...)` é resolvida de forma limpa por meio de `NS(...)`.

---

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


---

## Complemento de especificação: detalhes de templates, closures e enums

### Arquivos de definição de templates em modo duplo

O arquivo `.def` é a única fonte de verdade do template. O corpo declarativo deve ser escrito uma única vez, usando a DSL canônica (`TEMPLATE_STRUCT`, `TEMPLATE_PACKAGE` ou `TEMPLATE_OBJECT`). Há dois modos de uso:

1. **Exportação de macro (principal e recomendado):** o arquivo é incluído sem parâmetros previamente definidos e exporta `TEMPLATE_<Module>_<Target>`. A macro pode ser instanciada sob demanda por `TEMPLATE_INSTANTIATE(Module, Target, Suffix, ConcreteType, ...)` ou em lote por `TEMPLATE_INSTANTIATE_TABLE(Module, Target, TYPE_TABLE)`.
2. **Instanciação por inclusão direta:** o consumidor define os parâmetros (por exemplo, `SUFFIX` e `TYPE`) antes de incluir o arquivo. O próprio `.def` instancia o template e executa `#undef` nos parâmetros, restaurando um ambiente limpo.

Exemplo de definição:

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

A ordem dos parâmetros é uniforme: primeiro o identificador `SUFFIX`, depois os tipos de dados e, por último, metadados estáticos, como capacidade ou política. Exemplos de aridade: `(SUFFIX, TYPE)`, `(SUFFIX, FIRST_TYPE, SECOND_TYPE)`, `(SUFFIX, TYPE, CAPACITY)` e `(SUFFIX, KEY_TYPE, VALUE_TYPE, CAPACITY)`. Tabelas X-macro devem ser desempacotadas diretamente com `PP_UNWRAP ROW`, sem macros intermediárias de ligação.

Structs POD e templates cujos métodos são inline são autocontidos. O mecanismo de templates não exige macros stub `_IMPLEMENT`; arquivos `.impl` só são necessários quando houver membros não inline.

### Padrão de referência para templates de módulo

Quando um template de módulo gera packages ou objects parametrizados diretamente no namespace do módulo:

1. Use `MODULE_TEMPLATE` no `.def`, em vez de uma declaração fixa `MODULE`, para identificar explicitamente uma definição-template.
2. Coloque algoritmos independentes de tipo e largura em `INTERNAL(...)`, dentro de `Impl/Common.inc`. Essas funções auxiliares não devem gerar FunctionTable.
3. Declare constantes numéricas internas por tabela X-macro e `ENUM`; evite `#define` numérico bruto e números mágicos. Máscaras de 64 bits devem ser constantes nomeadas dentro dos auxiliares `INTERNAL`.
4. Agrupe famílias relacionadas em blocos `VARIANT`, por exemplo operações de máscara, contagem de bits, logaritmo, índice e rotação.

### Layout e despacho de closures

Uma closure contextual combina um ponteiro de função e um contexto emprestado opcional em uma estrutura POD de dois ponteiros (`sizeof == 2 * sizeof(void *)`). O membro de função pode compartilhar uma união com o callback puro correspondente.

- `CLOSURE_FUNC(Name_Bind)(fn, ctx)` associa uma função que recebe `void *context` como último argumento.
- `CLOSURE_FUNC(Name_FromCallback)(cb)` converte um callback puro sem criar wrapper.
- `CLOSURE_CALL(cl, args...)` verifica o contexto: se for nulo, chama o callback diretamente; caso contrário, remove o sentinela de contexto nulo e chama a função contextual com o contexto ao final dos argumentos.
- `CLOSURE_ADAPT_OBJECT_METHOD` adapta um método que recebe `CLASS *self` para a assinatura de closure.
- `CLOSURE_AS_CLOSURE(METHOD_FUNC, self)` cria uma closure pronta a partir de uma instância de object.

### Eventos declarativos

`EVENT_PROCEDURE` e `EVENT_OPERATION` implementam notificação observador de um para muitos sem alocação dinâmica, usando `TCLOSURE`. Os slots são embutidos em structs ou packages. A emissão cria um snapshot local dos listeners ativos antes da chamada, protegendo a iteração contra reentrância. `OBJECT_MUTATE(self, ClassName, fieldName, newValue, EventName)` atualiza um campo e emite o evento quando o valor realmente muda.

### Despacho de enums

O padrão Row-Tuple permite extrair colunas de cada linha X-macro. Os presets de dispatch são:

- `PLAIN` ou `NAME_VALUE`: emite constante qualificada com nome e valor explícito.
- `QUALIFIED`: inclui também o nome do enum, evitando colisões entre itens homônimos.
- `NAME_ONLY`: emite apenas o nome e deixa o compilador C incrementar os valores.
- `QUALIFIED_NAME`: emite o nome qualificado, sem valor explícito.
- Dispatch personalizado: recebe `DISPATCH(TARGET, ENUM_NAME, ROW)` e pode transformar a linha, por exemplo em máscaras de bits.

Declarações fora de `MODULE` usam `ENUM_DEF`, `ENUM_DEF_EX` ou `ENUM_DEF_DIRECT`, recebendo o namespace-alvo, o nome do enum, a tabela e, na forma estendida, o preset de dispatch (ou uma macro `ROW_MACRO(ROW)` direta em $O(1)$ para tabelas grandes sem limite de recursão do pré-processador).


### Exemplos complementares de referência

#### Tabela de tipos para instanciação em lote

```c
#define STRUCT_TYPE_TABLE(X) \\
  X((uint8, uint8_t))        \\
  X((uint16, uint16_t))      \\
  X((uint32, uint32_t))
```

Cada linha fornece o sufixo e o tipo C correspondente. O desempacotamento deve permanecer compatível com `PP_UNWRAP ROW`.

#### Estrutura POD de closure

```c
typedef struct TClosure_Name {
  union {
    TCallback_Name callback;
    Name_Function function;
  };
  void *context;
} TClosure_Name;
```

O ponteiro de contexto é emprestado: a closure não assume a propriedade do objeto apontado.

#### Declaração variádica de callbacks

```c
MODULE(DOMAIN(Core), MyModule,
  CALLBACK(
    FUNCTION(int, Transform, (int x)),
    PROCEDURE(Log, (int code))
  )
);
```

#### Declaração variádica de closures

```c
MODULE(DOMAIN(Core), MyModule,
  CLOSURE(
    FUNCTION(int, Compute, (int x)),
    PREDICATE(Validate, (int x))
  )
);
```

#### Declarações independentes de enum

```c
ENUM_DEF(MODULE_NS(File, Stream), Seek, SEEK_TABLE);
ENUM_DEF_EX(MODULE_NS(File, Stream), Seek, SEEK_TABLE, QUALIFIED);
ENUM_DEF_DIRECT(MODULE_NS(Chrono, Timezone), ZoneId, LARGE_TABLE, ROW_DISP);
```
