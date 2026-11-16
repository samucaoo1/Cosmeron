# Database

The **Database** module embeds SQLite for local SQL databases in files or memory.
It provides prepared statements, copied parameters, checked column access and
explicit transactions. It targets C11 and uses the normal Cosmeron namespaces,
prototype macros and `OPSTATUS` contracts.

```c
#include "Cosmeron/Modules/Database/Database.h"
```

# Overview

| Package | Header | Responsibility |
| --- | --- | --- |
| Connection | `Database/Connection/Connection.h` | Config, open/close, execute, prepare and connection counters |
| Statement | `Database/Statement/Statement.h` | Parameter binding, execution, iteration and typed columns |
| Transaction | `Database/Transaction/Transaction.h` | Begin, commit, rollback and savepoints |
| SQLite | `Database/SQLite/SQLite.h` | Embedded SQLite 3.53.4 engine |

Package headers are independently includable. Only SQLite is currently implemented.
Migration, Backup, PostgreSQL and MySQL are future packages and are not exposed as
working backends. The module does not translate SQL dialects or provide an ORM.

## Compilation: one shared engine

In **exactly one C translation unit**, define the implementation switch before
its first project or system header:

```c
#define DATABASE_SQLITE_IMPLEMENTATION
#include "Cosmeron/Modules/Database/Database.h"
```

Other C files include the header normally. For example:

```c
/* engine.c */
#define DATABASE_SQLITE_IMPLEMENTATION
#include "Cosmeron/Modules/Database/SQLite/SQLite.h"
```

```sh
cc -std=c11 -I Codespace app.c engine.c -o app
```

All engine source is included from an `.inc` file. No `-lsqlite3`, `-lm` or
extension-loader link flags are needed. Native mutexes remain enabled. The
zero-extra-link-flags targets are modern Linux/glibc >= 2.34, macOS and
Windows/MinGW/MSVC. Older glibc, some BSD configurations and static libc linking
are outside that guarantee. An external SQLite library is not required.

The emission switch prevents duplicate global engines and supports handles
crossing translation units. Omitting it from every C file causes undefined
backend symbols at link time; defining it in multiple C files causes duplicate
symbols. Set it before the first include so SQLite can select its platform
feature declarations correctly. The definition applies to that one C file,
not globally to every file in the compiler invocation.

See [backend provenance and configuration](../../Codespace/Cosmeron/Modules/Database/SQLite/Internal/README.md).

## Naming

| Macro form | Default direct form |
| --- | --- |
| `DATABASE_TYPE(TDatabase)` | `Database_TDatabase` |
| `DATABASE_TYPE(TStatement)` | `Database_TStatement` |
| `DATABASE_TYPE(TConfig)` | `Database_TConfig` |
| `DATABASE_TYPE(TDiagnostic)` | `Database_TDiagnostic` |
| `DATABASE_FUNC(Open)` | `Database_Open` |
| `DATABASE_FUNC(Statement_Next)` | `Database_Statement_Next` |
| `DATABASE_CONST(OPEN_READ_ONLY)` | `DATABASE_OPEN_READ_ONLY` |

`COSMERON_NAMESPACE`, `COSMERON_NAMESPACE_CONST`, `DATABASE_NS` and
`DATABASE_CNS` configure the public identifiers. The embedded engine uses its
own fixed private namespace so differently prefixed API wrappers can share it.
There is no public vendor handle, global Init/Free or implicit LastError API.

## Ownership and lifecycle

Initialize connections and statements to `{0}`. They own their handles and must
not be copied into independent owners. Their storage must remain valid until
closed. Synchronize shared objects externally, including iteration, getters,
reset and close. SQLite's native serialized calls do not synchronize sequences
of wrapper operations or access to public handle fields.

Close every statement before its connection. Closing a connection with any
unfinalized statement returns BUSY and leaves the connection open and usable.
Both Close functions accept an already closed object and return SUCCESS; a NULL
object pointer is INVALID_ARGUMENT.

`Statement_Close` always clears a valid native statement even if it returns the
previous execution error from SQLite finalization. `Statement_Reset` similarly
resets to the ready state even when it reports a previous execution error.
Bindings survive Reset; ClearBindings replaces all bindings with SQL NULL.

Ordinary output parameters change only on SUCCESS. `Statement_Next` is an
explicit exception: a non-NULL `outHasRow` becomes false on failure as well as
on end-of-results. Optional diagnostic outputs may change on a native failure.

## Opening and configuration

```c
Database_TDatabase database = {0};
OPSTATUS status = Database_Open(&database, "data.db");
```

`Open` uses SQLite, read/write access, creation if missing and a 3000 ms lock
wait. `":memory:"` opens a separate private in-memory database per connection.
Paths are borrowed only during opening. Empty paths are rejected; URI filename
interpretation is disabled.

```c
DATABASE_TYPE(TConfig) config = DATABASE_FUNC(Config)("data.db");
config.flags = DATABASE_CONST(OPEN_READ_ONLY);
config.timeoutMs = 0;
OPSTATUS status = DATABASE_FUNC(OpenWith)(&database, &config);
```

| TConfig field | Meaning |
| --- | --- |
| `backend` | Currently `DATABASE_CONST(BACKEND_SQLITE)` only |
| `path` | UTF-8 SQLite filename or `":memory:"` |
| `flags` | Explicit opening flags |
| `timeoutMs` | Native SQLite lock wait; 0 means no wait, maximum INT_MAX |

Exactly one of OPEN_READ_ONLY and OPEN_READ_WRITE is required. OPEN_CREATE
requires OPEN_READ_WRITE. Unknown flags are rejected. Opening an already open
object returns BUSY without replacing it. A failed opening leaves it closed.
`Config(path)` supplies usable defaults; zero-initializing TConfig alone does
not supply the required path or flags.

Foreign key checking is enabled by default. SQLite native SQL, file locking and
WAL remain available. Loadable extensions and optional SQL math functions are
not enabled. Double-quoted identifiers are accepted, but double-quoted strings
are disabled; use single quotes for SQL string literals.

## Statements and values

Prepare and Execute accept **one nonempty SQL instruction**. Trailing whitespace,
comments and empty semicolon instructions are accepted; an additional SQL
instruction is rejected rather than silently ignored. SQLite performs some
PRAGMA work at prepare time: preparation is not a sandbox for untrusted SQL.

Execute/Statement_Execute accept instructions without result columns. SELECT,
PRAGMA queries and INSERT/UPDATE/DELETE RETURNING require Prepare + Next and are
rejected by Execute before stepping. Prepare does not copy the supplied SQL
for the caller; SQLite keeps the required internal representation.

Parameter indices start at **1**; column indices start at **0**. Native SQLite
placeholder forms such as `?`, `?1` and `:name` are supported; the public Bind
functions select them by native parameter index. ParameterCount reports the
highest native index, which may include holes when using `?NNN`.

Bind copies text and blob buffers. A NULL buffer with size 0 binds an empty
TEXT/BLOB, not SQL NULL. Use BindNull explicitly. Lengths count bytes and may
include embedded NUL bytes in text. Lengths above INT_MAX are OUT_OF_RANGE;
actual SQLite limits can be smaller. BindDouble rejects NaN, which SQLite would
otherwise silently turn into SQL NULL.

After execution or iteration, Reset is required before rebinding. A failed
step requires Reset before retrying, including BUSY. Repeated Next or Execute
after completion does not restart the instruction or repeat mutations.

| Next outcome | Status | outHasRow |
| --- | --- | --- |
| A row is available | SUCCESS | true |
| No more rows / instruction completed | SUCCESS | false |
| Failure | Non-success OPSTATUS | false |

Do not treat the first row of a RETURNING query as confirmation of completed
execution. Iterate until Next returns SUCCESS with false, and check failures
from Reset/Close when abandoning execution.

Columns require a current row, except ColumnName and ColumnCount, which work
after preparation. ColumnInt64 accepts INTEGER only. ColumnDouble accepts REAL
or INTEGER; converting a large integer to double can lose precision.
ColumnText accepts TEXT only, ColumnBlob BLOB only, and NULL is explicitly
checked through ColumnIsNull/ColumnKind. Failed reads preserve their outputs.

Text/blob views are borrowed until the next Next, Reset or Close on their
statement. They must not be freed. Text also has a terminating NUL; its explicit
length excludes that terminator and remains authoritative for embedded NULs.
An empty BLOB may yield a NULL pointer with size 0. ColumnName is borrowed until
reprepare or Close; copy it before stepping when retaining it for longer.
Use Cosmeron's Memory utilities to retain data beyond these lifetimes.

## Errors and diagnostics

Recoverable errors map to the existing Error module, including OUT_OF_MEMORY,
OUT_OF_RANGE, BUSY, CANCELLED and INVALID_ARGUMENT. Native constraint, syntax,
I/O, read-only, corrupt-file and other SQLite failures map to GENERIC_ERROR
unless a more specific existing status applies.

The WithDiagnostic variants accept an optional TDiagnostic pointer. It receives
the native/extended SQLite code, a copied message and a truncation flag on native
failure, and a zero code/empty message on success. Input/state validation
failures do not guarantee a diagnostic and normally leave it unchanged. Always
inspect OPSTATUS first. Messages are limited to 255 bytes plus NUL; truncation
can split a UTF-8 character. The diagnostic owns its message and remains valid
after closing the database.

AffectedRows uses SQLite's connection-local changes counter for the most recent
INSERT/UPDATE/DELETE, excluding trigger work and cascades. DDL does not reset
that native counter. LastInsertId returns SQLite's connection-local last inserted
rowid; it is not a portable server-generated-ID abstraction or a counter of
successful committed inserts. Synchronize counter reads with their writes.

## Transactions

Begin starts a deferred transaction. Commit and Rollback return the engine's
status; attempting either without an active transaction fails. Transaction
activity comes from SQLite's autocommit state, so direct SQL transactions and
savepoints remain observable. A failed commit can leave the transaction active.

Savepoint/RollbackTo/Release quote and escape identifiers. Nonempty UTF-8 names
may include spaces or quotes without becoming SQL code. RollbackTo preserves
the named savepoint; Release removes it. An outermost savepoint starts a
transaction and releasing it commits that transaction. Nested Begin is not a
replacement for savepoints. Closing a connection with an uncommitted transaction
rolls it back; explicitly check Commit when persistence matters.

# API reference

The syntax below uses the public macros. Each function's signature comes from
its corresponding DATABASE_*_PROTOTYPE macro in the package header.

## Connection

# Config

Returns SQLite read/write/create defaults and a 3000 ms timeout.

### Syntax

#### Macro form

```c
DATABASE_TYPE(TConfig) DATABASE_FUNC(Config)(
      const char *path);
```

#### Direct form

```c
Database_TConfig Database_Config(
      const char *path);
```

### Parameters

- `path`: caminho UTF-8 do banco, incluindo `:memory:` para banco privado em memória.

### Return value

Retorna a configuração pronta para abrir SQLite com leitura/escrita/criação e timeout padrão de 3000 ms.

### Remarks

Returns SQLite read/write/create defaults and a 3000 ms timeout.

### Example

```c
DATABASE_TYPE(TConfig) config = DATABASE_FUNC(Config)(":memory:");
```

# Open

Opens a file or private in-memory SQLite database using defaults.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Open)(
      DATABASE_TYPE(TDatabase) *database,
      const char *path);
```

#### Direct form

```c
OPSTATUS Database_Open(
      Database_TDatabase *database,
      const char *path);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `path`: caminho UTF-8 do banco, incluindo `:memory:` para banco privado em memória.

### Return value

`OPSTATUS`: SUCCESS na abertura; INVALID_ARGUMENT para caminho/objeto inválidos, BUSY se já aberto, ou erros de backend.

### Remarks

Opens a file or private in-memory SQLite database using defaults.

### Example

```c
DATABASE_TYPE(TDatabase) database = {0};
OPSTATUS status = DATABASE_FUNC(Open)(&database, ":memory:");
if (status == STATUS_CONST(SUCCESS)) (void)DATABASE_FUNC(Close)(&database);
```

# OpenWith

Opens using caller-supplied backend, path, flags and timeout.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(OpenWith)(
      DATABASE_TYPE(TDatabase) *database,
      const DATABASE_TYPE(TConfig) *config);
```

#### Direct form

```c
OPSTATUS Database_OpenWith(
      Database_TDatabase *database,
      const Database_TConfig *config);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `config`: configuração com backend, nome, flags e timeout válida durante OpenWith.

### Return value

`OPSTATUS`: sucesso, argumento inválido, BUSY ou erro do SQLite.

### Remarks

Opens using caller-supplied backend, path, flags and timeout.

### Example

```c
/* database: conexão válida para esta operação. */
OPSTATUS status = DATABASE_FUNC(OpenWith)(&database, &config);
```

# OpenWithDiagnostic

OpenWith with optional copied native diagnostics.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(OpenWithDiagnostic)(
      DATABASE_TYPE(TDatabase) *database,
      const DATABASE_TYPE(TConfig) *config,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

#### Direct form

```c
OPSTATUS Database_OpenWithDiagnostic(
      Database_TDatabase *database,
      const Database_TConfig *config,
      Database_TDiagnostic *outDiagnostic);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `config`: configuração com backend, nome, flags e timeout válida durante OpenWith.
- `outDiagnostic`: diagnóstico opcional atualizado em caso de falha nativa.

### Return value

`OPSTATUS`: mesma semântica de OpenWith; outDiagnostic é opcional.

### Remarks

OpenWith with optional copied native diagnostics.

### Example

```c
/* database: conexão válida para esta operação. */
DATABASE_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = DATABASE_FUNC(OpenWithDiagnostic)(&database, &config, &diagnostic);
```

# Close

Closes an owned connection; BUSY preserves connections with outstanding statements.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Close)(
      DATABASE_TYPE(TDatabase) *database);
```

#### Direct form

```c
OPSTATUS Database_Close(
      Database_TDatabase *database);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.

### Return value

`OPSTATUS`: SUCCESS ao fechar ou já fechado; BUSY quando existem statements não finalizados.

### Remarks

Closes an owned connection; BUSY preserves connections with outstanding statements.

### Example

```c
/* database: conexão válida para esta operação. */
OPSTATUS status = DATABASE_FUNC(Close)(&database);
```

# Execute

Executes one instruction without result columns.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Execute)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql);
```

#### Direct form

```c
OPSTATUS Database_Execute(
      Database_TDatabase *database,
      const char *sql);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `sql`: texto contendo exatamente uma instrução SQL (com comentários/trailing whitespace opcionais).

### Return value

`OPSTATUS`: SUCCESS se a instrução sem colunas foi concluída; erro do SQLite em falha.

### Remarks

Executes one instruction without result columns.

### Example

```c
/* database: conexão válida para esta operação. */
OPSTATUS status = DATABASE_FUNC(Execute)(&database, "CREATE TABLE IF NOT EXISTS example (id INTEGER)");
```

# ExecuteWithDiagnostic

Execute with optional copied native diagnostics.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(ExecuteWithDiagnostic)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

#### Direct form

```c
OPSTATUS Database_ExecuteWithDiagnostic(
      Database_TDatabase *database,
      const char *sql,
      Database_TDiagnostic *outDiagnostic);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `sql`: texto contendo exatamente uma instrução SQL (com comentários/trailing whitespace opcionais).
- `outDiagnostic`: diagnóstico opcional atualizado em caso de falha nativa.

### Return value

`OPSTATUS`: mesma semântica de Execute, com diagnóstico opcional.

### Remarks

Execute with optional copied native diagnostics.

### Example

```c
/* database: conexão válida para esta operação. */
DATABASE_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = DATABASE_FUNC(ExecuteWithDiagnostic)(&database, "CREATE TABLE IF NOT EXISTS example (id INTEGER)", &diagnostic);
```

# Prepare

Creates an owned ready statement for one SQL instruction.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Prepare)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql,
      DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
OPSTATUS Database_Prepare(
      Database_TDatabase *database,
      const char *sql,
      Database_TStatement *statement);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `sql`: texto contendo exatamente uma instrução SQL (com comentários/trailing whitespace opcionais).
- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`OPSTATUS`: SUCCESS com statement preparado; erro de SQL ou conexão quando inválido.

### Remarks

Creates an owned ready statement for one SQL instruction.

### Example

```c
/* database: conexão válida para esta operação. */
OPSTATUS status = DATABASE_FUNC(Prepare)(&database, "CREATE TABLE IF NOT EXISTS example (id INTEGER)", &statement);
```

# PrepareWithDiagnostic

Prepare with optional copied native diagnostics.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(PrepareWithDiagnostic)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql,
      DATABASE_TYPE(TStatement) *statement,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

#### Direct form

```c
OPSTATUS Database_PrepareWithDiagnostic(
      Database_TDatabase *database,
      const char *sql,
      Database_TStatement *statement,
      Database_TDiagnostic *outDiagnostic);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `sql`: texto contendo exatamente uma instrução SQL (com comentários/trailing whitespace opcionais).
- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `outDiagnostic`: diagnóstico opcional atualizado em caso de falha nativa.

### Return value

`OPSTATUS`: mesma semântica de Prepare, com diagnóstico opcional.

### Remarks

Prepare with optional copied native diagnostics.

### Example

```c
/* database: conexão válida para esta operação. */
DATABASE_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = DATABASE_FUNC(PrepareWithDiagnostic)(&database, "CREATE TABLE IF NOT EXISTS example (id INTEGER)", &statement, &diagnostic);
```

# AffectedRows

Reads the native affected-row counter into outCount.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(AffectedRows)(
      const DATABASE_TYPE(TDatabase) *database,
      uint64_t *outCount);
```

#### Direct form

```c
OPSTATUS Database_AffectedRows(
      const Database_TDatabase *database,
      uint64_t *outCount);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `outCount`: recebe quantidade consultada, apenas quando a operação for bem-sucedida.

### Return value

`OPSTATUS`: SUCCESS com contador escrito em outCount.

### Remarks

Reads the native affected-row counter into outCount.

### Example

```c
/* database: conexão válida para esta operação. */
uint64_t count = 0;
OPSTATUS status = DATABASE_FUNC(AffectedRows)(&database, &count);
```

# LastInsertId

Reads the native last inserted rowid into outId.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(LastInsertId)(
      const DATABASE_TYPE(TDatabase) *database,
      int64_t *outId);
```

#### Direct form

```c
OPSTATUS Database_LastInsertId(
      const Database_TDatabase *database,
      int64_t *outId);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `outId`: recebe rowid da última inserção reconhecida pela conexão.

### Return value

`OPSTATUS`: SUCCESS com rowid no outId.

### Remarks

Reads the native last inserted rowid into outId.

### Example

```c
/* database: conexão válida para esta operação. */
int64_t id = 0;
OPSTATUS status = DATABASE_FUNC(LastInsertId)(&database, &id);
```

# SetTimeout

Changes the native SQLite lock wait in milliseconds.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(SetTimeout)(
      DATABASE_TYPE(TDatabase) *database,
      uint32_t timeoutMs);
```

#### Direct form

```c
OPSTATUS Database_SetTimeout(
      Database_TDatabase *database,
      uint32_t timeoutMs);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `timeoutMs`: tempo máximo de espera no bloqueio, em milissegundos.

### Return value

`OPSTATUS`: SUCCESS após configurar timeout; erro em valor ou conexão inválidos.

### Remarks

Changes the native SQLite lock wait in milliseconds.

### Example

```c
/* database: conexão válida para esta operação. */
OPSTATUS status = DATABASE_FUNC(SetTimeout)(&database, 1000);
```

# Statement_BindNull

Associa valor SQL NULL ao parâmetro indicado.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_BindNull)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index);
```

#### Direct form

```c
OPSTATUS Database_Statement_BindNull(
      Database_TStatement *statement,
      size_t index);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `index`: índice de parâmetro SQL baseado em 1.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Binds SQL NULL at a 1-based parameter index. Os valores vinculados são copiados ou armazenados conforme a semântica de SQLite; não modifique a definição do statement durante a execução.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_BindNull)(&statement, 1);
```

# Statement_BindInt64

Associa inteiro assinado de 64 bits ao parâmetro.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_BindInt64)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      int64_t value);
```

#### Direct form

```c
OPSTATUS Database_Statement_BindInt64(
      Database_TStatement *statement,
      size_t index,
      int64_t value);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `index`: índice de parâmetro SQL baseado em 1.
- `value`: valor numérico a vincular ao parâmetro.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Binds a signed 64-bit integer. Os valores vinculados são copiados ou armazenados conforme a semântica de SQLite; não modifique a definição do statement durante a execução.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_BindInt64)(&statement, 1, 42);
```

# Statement_BindDouble

Associa número real, rejeitando NaN.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_BindDouble)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      double value);
```

#### Direct form

```c
OPSTATUS Database_Statement_BindDouble(
      Database_TStatement *statement,
      size_t index,
      double value);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `index`: índice de parâmetro SQL baseado em 1.
- `value`: valor numérico a vincular ao parâmetro.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Binds a floating-point value; NaN is rejected. Os valores vinculados são copiados ou armazenados conforme a semântica de SQLite; não modifique a definição do statement durante a execução.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_BindDouble)(&statement, 1, 42);
```

# Statement_BindText

Copia texto para a propriedade do statement durante o bind.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_BindText)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      const char *text,
      size_t length);
```

#### Direct form

```c
OPSTATUS Database_Statement_BindText(
      Database_TStatement *statement,
      size_t index,
      const char *text,
      size_t length);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `index`: índice de parâmetro SQL baseado em 1.
- `text`: sequência de bytes de texto de entrada.
- `length`: quantidade de bytes do texto (não presume terminação NUL).

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Copies length bytes of text; NULL with length 0 is empty TEXT. Os valores vinculados são copiados ou armazenados conforme a semântica de SQLite; não modifique a definição do statement durante a execução.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_BindText)(&statement, 1, "hello", 5);
```

# Statement_BindBlob

Copia bytes binários para o statement durante o bind.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_BindBlob)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      const void *data,
      size_t size);
```

#### Direct form

```c
OPSTATUS Database_Statement_BindBlob(
      Database_TStatement *statement,
      size_t index,
      const void *data,
      size_t size);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `index`: índice de parâmetro SQL baseado em 1.
- `data`: sequência binária de entrada.
- `size`: número de bytes da sequência.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Copies size bytes; NULL with size 0 is empty BLOB. Os valores vinculados são copiados ou armazenados conforme a semântica de SQLite; não modifique a definição do statement durante a execução.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_BindBlob)(&statement, 1, "abc", 3);
```

# Statement_Next

Advances to one row or completion; writes false on failure.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_Next)(
      DATABASE_TYPE(TStatement) *statement,
      bool *outHasRow);
```

#### Direct form

```c
OPSTATUS Database_Statement_Next(
      Database_TStatement *statement,
      bool *outHasRow);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `outHasRow`: recebe verdadeiro quando a iteração chega a uma linha; falso no fim ou erro.

### Return value

`OPSTATUS`: SUCCESS com outHasRow true/false. Em erro, outHasRow também fica false.

### Remarks

Advances to one row or completion; writes false on failure.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
bool hasRow = false;
OPSTATUS status = DATABASE_FUNC(Statement_Next)(&statement, &hasRow);
```

# Statement_NextWithDiagnostic

Next with optional copied native diagnostics.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_NextWithDiagnostic)(
      DATABASE_TYPE(TStatement) *statement,
      bool *outHasRow,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

#### Direct form

```c
OPSTATUS Database_Statement_NextWithDiagnostic(
      Database_TStatement *statement,
      bool *outHasRow,
      Database_TDiagnostic *outDiagnostic);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `outHasRow`: recebe verdadeiro quando a iteração chega a uma linha; falso no fim ou erro.
- `outDiagnostic`: diagnóstico opcional atualizado em caso de falha nativa.

### Return value

`OPSTATUS`: como Next, mas com diagnóstico opcional; outHasRow=false em erro.

### Remarks

Next with optional copied native diagnostics.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
bool hasRow = false;
DATABASE_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = DATABASE_FUNC(Statement_NextWithDiagnostic)(&statement, &hasRow, &diagnostic);
```

# Statement_Execute

Executes a statement without result columns; completed statements do not rerun.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_Execute)(
      DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
OPSTATUS Database_Statement_Execute(
      Database_TStatement *statement);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Executes a statement without result columns; completed statements do not rerun.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_Execute)(&statement);
```

# Statement_ExecuteWithDiagnostic

Statement_Execute with optional copied native diagnostics.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ExecuteWithDiagnostic)(
      DATABASE_TYPE(TStatement) *statement,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

#### Direct form

```c
OPSTATUS Database_Statement_ExecuteWithDiagnostic(
      Database_TStatement *statement,
      Database_TDiagnostic *outDiagnostic);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `outDiagnostic`: diagnóstico opcional atualizado em caso de falha nativa.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Statement_Execute with optional copied native diagnostics.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
DATABASE_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = DATABASE_FUNC(Statement_ExecuteWithDiagnostic)(&statement, &diagnostic);
```

# Statement_Reset

Resets execution while retaining bindings; may report the previous error.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_Reset)(
      DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
OPSTATUS Database_Statement_Reset(
      Database_TStatement *statement);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`OPSTATUS`: restaura estado pronto para executar, conservando binds, mesmo quando reporta erro anterior.

### Remarks

Resets execution while retaining bindings; may report the previous error.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_Reset)(&statement);
```

# Statement_ClearBindings

Substitui valores de parâmetros por SQL NULL, sem destruir statement.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ClearBindings)(
      DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
OPSTATUS Database_Statement_ClearBindings(
      Database_TStatement *statement);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Sets every parameter to NULL; requires the ready state.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_ClearBindings)(&statement);
```

# Statement_Close

Destroys and clears the statement, even when reporting a prior error.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_Close)(
      DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
OPSTATUS Database_Statement_Close(
      Database_TStatement *statement);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`OPSTATUS`: libera handle mesmo quando reporta erro anterior do statement.

### Remarks

Destroys and clears the statement, even when reporting a prior error.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
OPSTATUS status = DATABASE_FUNC(Statement_Close)(&statement);
```

# Statement_ParameterCount

Returns the highest native binding index, or 0 for a closed/NULL statement.

### Syntax

#### Macro form

```c
size_t DATABASE_FUNC(Statement_ParameterCount)(
      const DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
size_t Database_Statement_ParameterCount(
      const Database_TStatement *statement);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`size_t`: maior índice de parâmetro vinculado; retorna zero para handle fechado ou nulo.

### Remarks

Returns the highest native binding index, or 0 for a closed/NULL statement.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
size_t number = DATABASE_FUNC(Statement_ParameterCount)(&statement);
```

# Statement_ColumnCount

Returns the result column count, or 0 for a closed/NULL statement.

### Syntax

#### Macro form

```c
size_t DATABASE_FUNC(Statement_ColumnCount)(
      const DATABASE_TYPE(TStatement) *statement);
```

#### Direct form

```c
size_t Database_Statement_ColumnCount(
      const Database_TStatement *statement);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).

### Return value

`size_t`: quantidade de colunas, inclusive sem linha atual; zero para handle fechado/nulo.

### Remarks

Returns the result column count, or 0 for a closed/NULL statement. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
size_t number = DATABASE_FUNC(Statement_ColumnCount)(&statement);
```

# Statement_ColumnName

Returns a borrowed column name; does not require a current row.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnName)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      const char **outName);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnName(
      const Database_TStatement *statement,
      size_t column,
      const char **outName);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outName`: recebe nome emprestado da coluna; não deve ser liberado.

### Return value

`OPSTATUS`: sucesso com nome emprestado ou índice/handle inválidos.

### Remarks

Returns a borrowed column name; does not require a current row. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
const char *columnName = NULL;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnName)(&statement, 0, &columnName);
```

# Statement_ColumnKind

Consulta a classe SQLite da coluna da linha atual.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnKind)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      DATABASE_TYPE(ColumnKind) *outKind);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnKind(
      const Database_TStatement *statement,
      size_t column,
      Database_ColumnKind *outKind);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outKind`: recebe tipo SQLite da coluna atual.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Returns COLUMN_NULL, COLUMN_INTEGER, COLUMN_REAL, COLUMN_TEXT or COLUMN_BLOB. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
DATABASE_TYPE(ColumnKind) kind;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnKind)(&statement, 0, &kind);
```

# Statement_ColumnIsNull

Consulta se uma coluna da linha atual contém SQL NULL.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnIsNull)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      bool *outIsNull);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnIsNull(
      const Database_TStatement *statement,
      size_t column,
      bool *outIsNull);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outIsNull`: recebe se a coluna representa SQL NULL.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Checks the current row column for SQL NULL through outIsNull. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
bool isNull = false;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnIsNull)(&statement, 0, &isNull);
```

# Statement_ColumnInt64

Lê INTEGER em int64_t sem converter texto ou real.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnInt64)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      int64_t *outValue);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnInt64(
      const Database_TStatement *statement,
      size_t column,
      int64_t *outValue);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outValue`: recebe um valor numericamente convertido do tipo da assinatura.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Reads INTEGER without coercing REAL, TEXT or NULL. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
int64_t value = 0;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnInt64)(&statement, 0, &value);
```

# Statement_ColumnDouble

Lê REAL/INTEGER como double, sujeito a perda de precisão.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnDouble)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      double *outValue);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnDouble(
      const Database_TStatement *statement,
      size_t column,
      double *outValue);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outValue`: recebe um valor numericamente convertido do tipo da assinatura.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Reads REAL or INTEGER as double; integer precision may be lost. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
double value = 0;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnDouble)(&statement, 0, &value);
```

# Statement_ColumnText

Reads a borrowed TEXT pointer and byte length.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnText)(
      DATABASE_TYPE(TStatement) *statement,
      size_t column,
      const char **outText,
      size_t *outLength);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnText(
      Database_TStatement *statement,
      size_t column,
      const char **outText,
      size_t *outLength);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outText`: recebe ponteiro emprestado para TEXT da linha atual.
- `outLength`: recebe número de bytes de TEXT sem NUL implícito.

### Return value

`OPSTATUS`: sucesso com ponteiro e comprimento do valor textual na linha; erros de tipo e estado.

### Remarks

Reads a borrowed TEXT pointer and byte length. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
const char *text = NULL;
size_t length = 0;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnText)(&statement, 0, &text, &length);
```

# Statement_ColumnBlob

Reads a borrowed BLOB pointer and byte size.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnBlob)(
      DATABASE_TYPE(TStatement) *statement,
      size_t column,
      const void **outData,
      size_t *outSize);
```

#### Direct form

```c
OPSTATUS Database_Statement_ColumnBlob(
      Database_TStatement *statement,
      size_t column,
      const void **outData,
      size_t *outSize);
```

### Parameters

- `statement`: ponteiro para statement aberto (ou objeto zerado em Prepare).
- `column`: índice da coluna, baseado em 0.
- `outData`: recebe ponteiro emprestado para BLOB da linha atual.
- `outSize`: recebe número de bytes do BLOB.

### Return value

`OPSTATUS`: sucesso com ponteiro e comprimento do valor binário na linha; erros de tipo e estado.

### Remarks

Reads a borrowed BLOB pointer and byte size. Ponteiros retornados para texto/bytes são emprestados e ficam invalidados quando o statement avançar, reiniciar ou fechar.

### Example

```c
/* statement: statement previamente preparado; colunas exigem linha atual. */
const void *data = NULL;
size_t size = 0;
OPSTATUS status = DATABASE_FUNC(Statement_ColumnBlob)(&statement, 0, &data, &size);
```

# Transaction_Begin

Inicia transação diferida.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Transaction_Begin)(
      DATABASE_TYPE(TDatabase) *database);
```

#### Direct form

```c
OPSTATUS Database_Transaction_Begin(
      Database_TDatabase *database);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Starts a deferred transaction. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
OPSTATUS status = DATABASE_FUNC(Transaction_Begin)(&database);
```

# Transaction_Commit

Confirma a transação ativa.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Transaction_Commit)(
      DATABASE_TYPE(TDatabase) *database);
```

#### Direct form

```c
OPSTATUS Database_Transaction_Commit(
      Database_TDatabase *database);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Commits the active transaction. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
OPSTATUS status = DATABASE_FUNC(Transaction_Commit)(&database);
```

# Transaction_Rollback

Reverte a transação ativa.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Transaction_Rollback)(
      DATABASE_TYPE(TDatabase) *database);
```

#### Direct form

```c
OPSTATUS Database_Transaction_Rollback(
      Database_TDatabase *database);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Rolls back the active transaction. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
OPSTATUS status = DATABASE_FUNC(Transaction_Rollback)(&database);
```

# Transaction_Savepoint

Cria savepoint identificado por nome.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Transaction_Savepoint)(
      DATABASE_TYPE(TDatabase) *database,
      const char *name);
```

#### Direct form

```c
OPSTATUS Database_Transaction_Savepoint(
      Database_TDatabase *database,
      const char *name);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `name`: nome SQL de savepoint, não vazio; a biblioteca realiza escape do identificador.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Creates a savepoint using an escaped nonempty name. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
OPSTATUS status = DATABASE_FUNC(Transaction_Savepoint)(&database, "before_update");
```

# Transaction_RollbackTo

Desfaz alterações desde savepoint sem liberar sua definição.

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Transaction_RollbackTo)(
      DATABASE_TYPE(TDatabase) *database,
      const char *name);
```

#### Direct form

```c
OPSTATUS Database_Transaction_RollbackTo(
      Database_TDatabase *database,
      const char *name);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `name`: nome SQL de savepoint, não vazio; a biblioteca realiza escape do identificador.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Rolls back changes since a savepoint, preserving the savepoint. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
OPSTATUS status = DATABASE_FUNC(Transaction_RollbackTo)(&database, "before_update");
```

# Transaction_Release

Libera savepoint (ou confirma transação externa).

### Syntax

#### Macro form

```c
OPSTATUS DATABASE_FUNC(Transaction_Release)(
      DATABASE_TYPE(TDatabase) *database,
      const char *name);
```

#### Direct form

```c
OPSTATUS Database_Transaction_Release(
      Database_TDatabase *database,
      const char *name);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.
- `name`: nome SQL de savepoint, não vazio; a biblioteca realiza escape do identificador.

### Return value

`OPSTATUS`: SUCCESS quando executada, podendo indicar INVALID_ARGUMENT, NOT_AVAILABLE, OUT_OF_RANGE, estado inadequado ou erro nativo quando aplicável.

### Remarks

Releases a savepoint; an outermost release commits its transaction. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
OPSTATUS status = DATABASE_FUNC(Transaction_Release)(&database, "before_update");
```

# Transaction_IsActive

Returns whether a valid connection is currently outside autocommit.

### Syntax

#### Macro form

```c
bool DATABASE_FUNC(Transaction_IsActive)(
      const DATABASE_TYPE(TDatabase) *database);
```

#### Direct form

```c
bool Database_Transaction_IsActive(
      const Database_TDatabase *database);
```

### Parameters

- `database`: ponteiro para conexão inicializada ou aberta, segundo a operação.

### Return value

`bool`: true se a conexão está numa transação ativa, false caso contrário.

### Remarks

Returns whether a valid connection is currently outside autocommit. A operação depende do estado transacional corrente; SQL executado diretamente também pode afetar esse estado.

### Example

```c
/* database: conexão válida; transação aberta quando necessária. */
bool active = DATABASE_FUNC(Transaction_IsActive)(&database);
```


# Example and tests

[ReadWrite.c](../../Codespace/Examples/Database/ReadWrite.c) contains a checked
in-memory create/insert/query example with cleanup and both macro API forms.

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Werror Codespace/Examples/Database/ReadWrite.c -o database-example
./database-example
make -C Codespace/Tests/Database run
```

Tests cover text/blob ownership, embedded NULs, empty/NULL distinction, integer
limits, SQL validation, repeat-execution prevention, diagnostics, recoverable
errors, transaction/savepoint behavior, read-only access, file persistence,
locking, custom namespaces and handles crossing translation units. The CI
matrix includes GCC/Clang, macOS, MinGW/MSVC, sanitizers and Linux 32-bit builds.
