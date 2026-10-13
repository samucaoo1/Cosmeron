# CLI

The **CLI** module interprets `argc/argv`, validates declarative command schemas and generates help text. It is header-only C11, requires no additional linker flags and has no global initialization or shutdown.

```c
#include "Cosmeron/Modules/CLI/CLI.h"
```

# Overview

| Package | Header | Responsibility |
| --- | --- | --- |
| Arguments | `CLI/Arguments/Arguments.h` | Literal option queries and strict numeric conversions |
| Command | `CLI/Command/Command.h` | Flags, values, positionals, subcommands and validation |
| Help | `CLI/Help/Help.h` | Buffer-based help generation from a schema |

Package headers can be included independently. CLI reuses Parsing/Common for bounded numeric conversion and buffer output; it does not include any document-format package. Terminal remains responsible for drawing, colors and interactive input. CLI does not execute commands or tokenize shell strings.

## Naming

| Macro | Default C identifier |
| --- | --- |
| `CLI_FUNC(Arguments_Create)` | `CLI_Arguments_Create` |
| `CLI_TYPE(TArguments)` | `CLI_TArguments` |
| `CLI_TYPE(TCommand)` | `CLI_TCommand` |
| `CLI_CONST(FLAG)` | `CLI_FLAG` |
| `CLI_NS(Functions)` | `CLI_Functions` |

`COSMERON_NAMESPACE`, `COSMERON_NAMESPACE_CONST`, `CLI_NS` and `CLI_CNS` support custom namespaces. Each public function has a readable `CLI_*_PROTOTYPE` macro. Define `CLI_DISABLE_FUNCTION_TABLE` before inclusion to omit the optional module FunctionTable.

## Ownership and outputs

Arguments borrow `argv` and its strings. Command schemas borrow their tables and strings. Matches borrow schema pointers and argument values. Keep these sources alive while using the results. `argc` includes the program name; parsing starts at index 1.

Parsing uses caller-provided match storage and does not allocate. Outputs remain unchanged on failure, except that `Command_Parse` reports the required match count on `INSUFFICIENT_SPACE`, and `Help_Write` reports the output byte count on `INSUFFICIENT_SPACE`. Diagnostics may be updated on failure and are cleared on successful command parsing.

`Help_Write` temporarily allocates its generated text. Its size excludes the terminating NUL; capacity must be at least size + 1. A NULL buffer and zero capacity queries the size. An insufficient buffer remains unchanged. The caller controls output to stdout, File or Terminal.

# Usage

## Direct queries

```c
CLI_TYPE(TArguments) arguments = CLI_FUNC(Arguments_Create)(argc, argv);
uint64_t jobs = 1;
OPSTATUS status = CLI_FUNC(Arguments_U64)(&arguments, "--jobs", &jobs);
if (status != STATUS_CONST(SUCCESS) && status != STATUS_CONST(NOT_FOUND)) {
    /* Invalid syntax, missing value or overflow. */
}
```

Direct queries recognize exact long names, `--name=value`, exact short names and attached short values such as `-ofile`. They stop at `--`. They do not know aliases or which unrelated arguments are flags versus values. Use the declarative parser for short flag groups, unknown-option rejection or positional interpretation. Count/Has are literal queries rather than schema validation.

`Arguments_String` selects occurrence 0; `Arguments_StringAt` selects a zero-based occurrence. Missing options return `NOT_FOUND`; a missing value returns `INCOMPLETE_SEQUENCE`. An empty attached value is present and returned as an empty string.

## Declarative commands

```c
static const CLI_TYPE(TOption) options[] = {
    {.shortName = 'v', .longName = "verbose", .kind = CLI_CONST(FLAG)},
    {.shortName = 'o', .longName = "output", .kind = CLI_CONST(VALUE),
     .valueName = "file", .required = true}
};
static const CLI_TYPE(TPositional) positionals[] = {
    {.name = "input", .required = true}
};
static const CLI_TYPE(TCommand) command = {
    .name = "convert", .description = "Convert a document",
    .options = options, .optionCount = 2,
    .positionals = positionals, .positionalCount = 1
};

CLI_TYPE(TMatch) matches[8];
CLI_TYPE(TDiagnostic) diagnostic = {0};
size_t count = 0;
OPSTATUS status = CLI_FUNC(Command_Parse)(
    &command, argc, argv, matches, 8, &count, &diagnostic
);
```

Accepted forms include `-v`, `--verbose`, `-ofile`, `-o file`, `--output=file`, `--output file` and short flag groups such as `-ab`. A value-taking short option consumes the remainder of its group, or the next argv element. Values may start with `-`; only the exact separator `--` cannot be consumed as a separate value. There is no implicit `--help`, boolean negation or long-name abbreviation.

Options may be required or repeatable. Nonzero `exclusiveGroup` values identify mutually exclusive options within one command. Repeated non-repeatable options return `ALREADY_EXISTS`; conflicts return `INVALID_ARGUMENT`. Missing required options or positionals return `NOT_FOUND` with a diagnostic.

Optional positionals follow required positionals. Only the final positional may be variadic. `--` stops option interpretation, allowing positional values such as `-filename`. A command with subcommands cannot also declare positionals: the first non-option selects its subcommand. `requireCommand` makes that selection mandatory. Parent options apply before selection; child options apply afterward. Selected commands also appear as matches, with option and positional both NULL.

Schema validation rejects duplicate names, invalid option kinds, ambiguous positionals and cyclic/overdeep command trees. The maximum command depth is `CLI_CONST(MAX_COMMAND_DEPTH)`. Validation rescans argv instead of allocating occurrence tables, making it suitable for small to medium command schemas.

## Diagnostics

`TDiagnostic` contains `code`, `argumentIndex` and a borrowed `name`. Codes distinguish invalid schemas, unknown options/commands, missing values, required arguments, repetition, exclusivity, unexpected positionals and insufficient match capacity.

## Numeric conversions

I64/U64 are bounded decimal conversions; F64 accepts decimal fractions and exponents. Invalid suffixes, whitespace and overflow are rejected. U64 does not accept a sign. I64 accepts `+` or `-`. F64 adapts to the current locale's decimal separator without changing the process locale; callers must synchronize concurrent process-wide locale changes.

# API reference

## Function summary

| Package | Function | Return |
| --- | --- | --- |
| Arguments | `CLI_FUNC(Arguments_Create)` | `CLI_TYPE(TArguments)` |
| Arguments | `CLI_FUNC(Arguments_Count)` | `size_t` |
| Arguments | `CLI_FUNC(Arguments_Has)` | `bool` |
| Arguments | `CLI_FUNC(Arguments_StringAt)` | `OPSTATUS` |
| Arguments | `CLI_FUNC(Arguments_String)` | `OPSTATUS` |
| Arguments | `CLI_FUNC(Arguments_I64)` | `OPSTATUS` |
| Arguments | `CLI_FUNC(Arguments_U64)` | `OPSTATUS` |
| Arguments | `CLI_FUNC(Arguments_F64)` | `OPSTATUS` |
| Command | `CLI_FUNC(Command_Parse)` | `OPSTATUS` |
| Help | `CLI_FUNC(Help_Write)` | `OPSTATUS` |

Examples using `args` assume `CLI_TYPE(TArguments) args = CLI_FUNC(Arguments_Create)(argc, argv);` has already been declared in the same function. Examples using `command` refer to the `CLI_TYPE(TCommand)` example in the Usage section above.

# Arguments package

# Arguments_Create

Constrói uma visão emprestada dos argumentos argc/argv.

### Syntax

#### Macro form

```c
CLI_TYPE(TArguments) CLI_FUNC(Arguments_Create)(int argc, char **argv);
```

#### Direct form

```c
CLI_TArguments CLI_Arguments_Create(int argc, char **argv);
```

### Parameters

- `argc`: quantidade de argumentos, incluindo o nome do programa.
- `argv`: vetor de strings C passado ao programa; não é copiado.

### Return value

Retorna `CLI_TYPE(TArguments)` por valor.

### Remarks

`argc` inclui o nome do executável; opções começam em argv[1]. A função não copia strings nem aloca memória.

### Example

```c
CLI_TYPE(TArguments) args = CLI_FUNC(Arguments_Create)(argc, argv);
```

# Arguments_Count

Conta ocorrências literais da opção até o separador --.

### Syntax

#### Macro form

```c
size_t CLI_FUNC(Arguments_Count)(const CLI_TYPE(TArguments) *arguments, const char *name);
```

#### Direct form

```c
size_t CLI_Arguments_Count(const CLI_TArguments *arguments, const char *name);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.

### Return value

Retorna o número de ocorrências encontradas.

### Remarks

Não interpreta aliases nem grupos de flags; a contagem não substitui validação do schema.

### Example

```c
size_t matches = CLI_FUNC(Arguments_Count)(&args, "--verbose");
```

# Arguments_Has

Verifica se uma opção literal está presente.

### Syntax

#### Macro form

```c
bool CLI_FUNC(Arguments_Has)(const CLI_TYPE(TArguments) *arguments, const char *name);
```

#### Direct form

```c
bool CLI_Arguments_Has(const CLI_TArguments *arguments, const char *name);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.

### Return value

Retorna true quando a opção está presente.

### Remarks

Procura a forma nominal indicada, sem resolver aliases nem opções declarativas.

### Example

```c
bool verbose = CLI_FUNC(Arguments_Has)(&args, "--verbose");
```

# Arguments_StringAt

Obtém o valor de uma ocorrência específica, indexada a partir de zero.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Arguments_StringAt)(const CLI_TYPE(TArguments) *arguments, const char *name, size_t occurrence, const char **outValue);
```

#### Direct form

```c
OPSTATUS CLI_Arguments_StringAt(const CLI_TArguments *arguments, const char *name, size_t occurrence, const char **outValue);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.
- `occurrence`: índice zero-based entre ocorrências.
- `outValue`: recebe ponteiro para string emprestada ou valor numérico convertido em SUCCESS.

### Return value

`SUCCESS` quando encontrado; `NOT_FOUND` quando a opção não existe, `INCOMPLETE_SEQUENCE` para valor faltante.

### Remarks

Reconhece `--name=value`, `--name value` e valores anexados a opções curtas. Ponteiro devolvido é emprestado de argv. Valor vazio anexado é válido.

### Example

```c
const char *file = NULL;
OPSTATUS status = CLI_FUNC(Arguments_StringAt)(&args, "--output", 0, &file);
```

# Arguments_String

Obtém o valor da primeira ocorrência da opção.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Arguments_String)(const CLI_TYPE(TArguments) *arguments, const char *name, const char **outValue);
```

#### Direct form

```c
OPSTATUS CLI_Arguments_String(const CLI_TArguments *arguments, const char *name, const char **outValue);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.
- `outValue`: recebe ponteiro para string emprestada ou valor numérico convertido em SUCCESS.

### Return value

`SUCCESS`, `NOT_FOUND`, `INCOMPLETE_SEQUENCE` ou erro de argumento.

### Remarks

Equivale à consulta de ocorrência 0 por `Arguments_StringAt`. O resultado não é uma cópia de string.

### Example

```c
const char *output = NULL;
OPSTATUS status = CLI_FUNC(Arguments_String)(&args, "--output", &output);
```

# Arguments_I64

Interpreta valor decimal com sinal, dentro da faixa de int64_t.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Arguments_I64)(const CLI_TYPE(TArguments) *arguments, const char *name, int64_t *outValue);
```

#### Direct form

```c
OPSTATUS CLI_Arguments_I64(const CLI_TArguments *arguments, const char *name, int64_t *outValue);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.
- `outValue`: recebe ponteiro para string emprestada ou valor numérico convertido em SUCCESS.

### Return value

`SUCCESS`, `NOT_FOUND` ou status de sequência/conversão inválida ou fora de faixa.

### Remarks

Aceita `+` e `-`, mas rejeita espaços e sufixos inválidos. A opção precisa existir e conter valor.

### Example

```c
int64_t offset = 0;
OPSTATUS status = CLI_FUNC(Arguments_I64)(&args, "--offset", &offset);
```

# Arguments_U64

Interpreta valor decimal sem sinal dentro da faixa de uint64_t.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Arguments_U64)(const CLI_TYPE(TArguments) *arguments, const char *name, uint64_t *outValue);
```

#### Direct form

```c
OPSTATUS CLI_Arguments_U64(const CLI_TArguments *arguments, const char *name, uint64_t *outValue);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.
- `outValue`: recebe ponteiro para string emprestada ou valor numérico convertido em SUCCESS.

### Return value

`SUCCESS`, `NOT_FOUND` ou status de sequência/conversão inválida ou fora de faixa.

### Remarks

Rejeita valores com sinal, sufixos extras e overflow; não aceita valores prefixados em outras bases.

### Example

```c
uint64_t count = 0;
OPSTATUS status = CLI_FUNC(Arguments_U64)(&args, "--jobs", &count);
```

# Arguments_F64

Interpreta número decimal de ponto flutuante e notação exponencial.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Arguments_F64)(const CLI_TYPE(TArguments) *arguments, const char *name, double *outValue);
```

#### Direct form

```c
OPSTATUS CLI_Arguments_F64(const CLI_TArguments *arguments, const char *name, double *outValue);
```

### Parameters

- `arguments`: ponteiro para a visão CLI_TArguments retornada por Arguments_Create.
- `name`: nome literal de opção, por exemplo `--output`.
- `outValue`: recebe ponteiro para string emprestada ou valor numérico convertido em SUCCESS.

### Return value

`SUCCESS`, `NOT_FOUND` ou status de conversão inválida/fora de faixa.

### Remarks

Adapta a conversão ao separador decimal do locale corrente sem alterar o locale do processo. Alterações concorrentes de locale exigem sincronização externa.

### Example

```c
double ratio = 0;
OPSTATUS status = CLI_FUNC(Arguments_F64)(&args, "--ratio", &ratio);
```

# Command package

# Command_Parse

Valida um schema de comandos e interpreta opções, posicionais e subcomandos.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Command_Parse)(const CLI_TYPE(TCommand) *command, int argc, char **argv, CLI_TYPE(TMatch) *matches, size_t matchCapacity, size_t *outMatchCount, CLI_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS CLI_Command_Parse(const CLI_TCommand *command, int argc, char **argv, CLI_TMatch *matches, size_t matchCapacity, size_t *outMatchCount, CLI_TDiagnostic *diagnostic);
```

### Parameters

- `command`: ponteiro para schema declarativo CLI_TCommand; estruturas/strings permanecem emprestadas.
- `argc`: quantidade de argumentos, incluindo o nome do programa.
- `argv`: vetor de strings C passado ao programa; não é copiado.
- `matches`: array de saída para correspondências CLI_TMatch.
- `matchCapacity`: capacidade do array matches em número de elementos.
- `outMatchCount`: número de matches produzido, ou capacidade necessária em caso de INSUFFICIENT_SPACE.
- `diagnostic`: diagnóstico opcional com código, índice do argumento e nome.

### Return value

`SUCCESS` para parse válido. Pode retornar `NOT_FOUND` para obrigatórios faltantes, `ALREADY_EXISTS` para repetição, `INVALID_ARGUMENT` para conflitos/schema, e `INSUFFICIENT_SPACE` quando a tabela fornecida é pequena.

### Remarks

O parser não aloca armazenamento de matches. `TMatch` referencia opções e strings do schema/argv. Quando matchCapacity não basta, `outMatchCount` informa a necessidade; a diagnostic indica a natureza do problema.

### Example

```c
CLI_TYPE(TMatch) matches[8];
CLI_TYPE(TDiagnostic) diagnostic = {0};
size_t used = 0;
OPSTATUS status = CLI_FUNC(Command_Parse)(&command, argc, argv, matches, 8, &used, &diagnostic);
```

# Help package

# Help_Write

Gera texto de ajuda a partir de um schema, sem imprimir diretamente.

### Syntax

#### Macro form

```c
OPSTATUS CLI_FUNC(Help_Write)(const CLI_TYPE(TCommand) *command, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS CLI_Help_Write(const CLI_TCommand *command, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `command`: ponteiro para schema declarativo CLI_TCommand; estruturas/strings permanecem emprestadas.
- `buffer`: buffer destino de texto gerado.
- `capacity`: capacidade de buffer em bytes.
- `outSize`: quantidade de caracteres gerados, sem NUL terminador.

### Return value

`SUCCESS` na gravação, `INSUFFICIENT_SPACE` quando o buffer é pequeno ou ausente em consulta, e erros de schema/memória.

### Remarks

O tamanho retornado exclui NUL; `capacity` precisa comportar texto mais terminador. `buffer == NULL` e capacidade zero permitem consultar o tamanho. A implementação aloca um buffer temporário.

### Example

```c
size_t needed = 0;
OPSTATUS query = CLI_FUNC(Help_Write)(&command, NULL, 0, &needed);
/* Alocar needed + 1 bytes e chamar Help_Write novamente. */
```

# Naming and portability

Namespaces use `CLI_FUNC`, `CLI_TYPE`, and `CLI_CONST`. The umbrella defines an optional function table, which can be disabled by defining `CLI_DISABLE_FUNCTION_TABLE` before inclusion. The parser requires C11 but does not require a shell, CLI runtime initialization, extra link libraries, or Terminal graphics.

# See also

- [Parsing](Parsing.md) — document-format parsing and shared numeric parsing helpers.
- [Terminal](Terminal.md) — interaction, display and line I/O.
- [File](File.md) — persistence of parsed results.
- [Bit](Bit.md) — documentation reference format.
