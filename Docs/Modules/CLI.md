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

Required output pointers must be non-NULL. Mutable getters produce borrowed mutable handles for the editing operations.

## Arguments

```c
static inline CLI_TYPE(TArguments) CLI_FUNC(Arguments_Create)(
      int argc,
      char **argv);

static inline size_t CLI_FUNC(Arguments_Count)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name);

static inline bool CLI_FUNC(Arguments_Has)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name);

static inline OPSTATUS CLI_FUNC(Arguments_StringAt)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name,
      size_t occurrence,
      const char **outValue);

static inline OPSTATUS CLI_FUNC(Arguments_String)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name,
      const char **outValue);

static inline OPSTATUS CLI_FUNC(Arguments_I64)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name,
      int64_t *outValue);

static inline OPSTATUS CLI_FUNC(Arguments_U64)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name,
      uint64_t *outValue);

static inline OPSTATUS CLI_FUNC(Arguments_F64)(
      const CLI_TYPE(TArguments) *arguments,
      const char *name,
      double *outValue);

```

## Command

```c
static inline OPSTATUS CLI_FUNC(Command_Parse)(
      const CLI_TYPE(TCommand) *command,
      int argc,
      char **argv,
      CLI_TYPE(TMatch) *matches,
      size_t matchCapacity,
      size_t *outMatchCount,
      CLI_TYPE(TDiagnostic) *diagnostic);

```

## Help

```c
static inline OPSTATUS CLI_FUNC(Help_Write)(
      const CLI_TYPE(TCommand) *command,
      char *buffer,
      size_t capacity,
      size_t *outSize);

```
