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

### Config

Returns SQLite read/write/create defaults and a 3000 ms timeout.

#### Syntax

```c
DATABASE_TYPE(TConfig) DATABASE_FUNC(Config)(
      const char *path);
```

### Open

Opens a file or private in-memory SQLite database using defaults.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Open)(
      DATABASE_TYPE(TDatabase) *database,
      const char *path);
```

### OpenWith

Opens using caller-supplied backend, path, flags and timeout.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(OpenWith)(
      DATABASE_TYPE(TDatabase) *database,
      const DATABASE_TYPE(TConfig) *config);
```

### OpenWithDiagnostic

OpenWith with optional copied native diagnostics.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(OpenWithDiagnostic)(
      DATABASE_TYPE(TDatabase) *database,
      const DATABASE_TYPE(TConfig) *config,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

### Close

Closes an owned connection; BUSY preserves connections with outstanding statements.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Close)(
      DATABASE_TYPE(TDatabase) *database);
```

### Execute

Executes one instruction without result columns.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Execute)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql);
```

### ExecuteWithDiagnostic

Execute with optional copied native diagnostics.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(ExecuteWithDiagnostic)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

### Prepare

Creates an owned ready statement for one SQL instruction.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Prepare)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql,
      DATABASE_TYPE(TStatement) *statement);
```

### PrepareWithDiagnostic

Prepare with optional copied native diagnostics.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(PrepareWithDiagnostic)(
      DATABASE_TYPE(TDatabase) *database,
      const char *sql,
      DATABASE_TYPE(TStatement) *statement,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

### AffectedRows

Reads the native affected-row counter into outCount.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(AffectedRows)(
      const DATABASE_TYPE(TDatabase) *database,
      uint64_t *outCount);
```

### LastInsertId

Reads the native last inserted rowid into outId.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(LastInsertId)(
      const DATABASE_TYPE(TDatabase) *database,
      int64_t *outId);
```

### SetTimeout

Changes the native SQLite lock wait in milliseconds.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(SetTimeout)(
      DATABASE_TYPE(TDatabase) *database,
      uint32_t timeoutMs);
```


## Statement

### Statement_BindNull

Binds SQL NULL at a 1-based parameter index.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_BindNull)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index);
```

### Statement_BindInt64

Binds a signed 64-bit integer.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_BindInt64)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      int64_t value);
```

### Statement_BindDouble

Binds a floating-point value; NaN is rejected.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_BindDouble)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      double value);
```

### Statement_BindText

Copies length bytes of text; NULL with length 0 is empty TEXT.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_BindText)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      const char *text,
      size_t length);
```

### Statement_BindBlob

Copies size bytes; NULL with size 0 is empty BLOB.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_BindBlob)(
      DATABASE_TYPE(TStatement) *statement,
      size_t index,
      const void *data,
      size_t size);
```

### Statement_Next

Advances to one row or completion; writes false on failure.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_Next)(
      DATABASE_TYPE(TStatement) *statement,
      bool *outHasRow);
```

### Statement_NextWithDiagnostic

Next with optional copied native diagnostics.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_NextWithDiagnostic)(
      DATABASE_TYPE(TStatement) *statement,
      bool *outHasRow,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

### Statement_Execute

Executes a statement without result columns; completed statements do not rerun.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_Execute)(
      DATABASE_TYPE(TStatement) *statement);
```

### Statement_ExecuteWithDiagnostic

Statement_Execute with optional copied native diagnostics.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ExecuteWithDiagnostic)(
      DATABASE_TYPE(TStatement) *statement,
      DATABASE_TYPE(TDiagnostic) *outDiagnostic);
```

### Statement_Reset

Resets execution while retaining bindings; may report the previous error.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_Reset)(
      DATABASE_TYPE(TStatement) *statement);
```

### Statement_ClearBindings

Sets every parameter to NULL; requires the ready state.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ClearBindings)(
      DATABASE_TYPE(TStatement) *statement);
```

### Statement_Close

Destroys and clears the statement, even when reporting a prior error.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_Close)(
      DATABASE_TYPE(TStatement) *statement);
```

### Statement_ParameterCount

Returns the highest native binding index, or 0 for a closed/NULL statement.

#### Syntax

```c
size_t DATABASE_FUNC(Statement_ParameterCount)(
      const DATABASE_TYPE(TStatement) *statement);
```

### Statement_ColumnCount

Returns the result column count, or 0 for a closed/NULL statement.

#### Syntax

```c
size_t DATABASE_FUNC(Statement_ColumnCount)(
      const DATABASE_TYPE(TStatement) *statement);
```

### Statement_ColumnName

Returns a borrowed column name; does not require a current row.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnName)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      const char **outName);
```

### Statement_ColumnKind

Returns COLUMN_NULL, COLUMN_INTEGER, COLUMN_REAL, COLUMN_TEXT or COLUMN_BLOB.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnKind)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      DATABASE_TYPE(ColumnKind) *outKind);
```

### Statement_ColumnIsNull

Checks the current row column for SQL NULL through outIsNull.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnIsNull)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      bool *outIsNull);
```

### Statement_ColumnInt64

Reads INTEGER without coercing REAL, TEXT or NULL.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnInt64)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      int64_t *outValue);
```

### Statement_ColumnDouble

Reads REAL or INTEGER as double; integer precision may be lost.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnDouble)(
      const DATABASE_TYPE(TStatement) *statement,
      size_t column,
      double *outValue);
```

### Statement_ColumnText

Reads a borrowed TEXT pointer and byte length.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnText)(
      DATABASE_TYPE(TStatement) *statement,
      size_t column,
      const char **outText,
      size_t *outLength);
```

### Statement_ColumnBlob

Reads a borrowed BLOB pointer and byte size.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Statement_ColumnBlob)(
      DATABASE_TYPE(TStatement) *statement,
      size_t column,
      const void **outData,
      size_t *outSize);
```


## Transaction

### Transaction_Begin

Starts a deferred transaction.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Transaction_Begin)(
      DATABASE_TYPE(TDatabase) *database);
```

### Transaction_Commit

Commits the active transaction.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Transaction_Commit)(
      DATABASE_TYPE(TDatabase) *database);
```

### Transaction_Rollback

Rolls back the active transaction.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Transaction_Rollback)(
      DATABASE_TYPE(TDatabase) *database);
```

### Transaction_Savepoint

Creates a savepoint using an escaped nonempty name.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Transaction_Savepoint)(
      DATABASE_TYPE(TDatabase) *database,
      const char *name);
```

### Transaction_RollbackTo

Rolls back changes since a savepoint, preserving the savepoint.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Transaction_RollbackTo)(
      DATABASE_TYPE(TDatabase) *database,
      const char *name);
```

### Transaction_Release

Releases a savepoint; an outermost release commits its transaction.

#### Syntax

```c
OPSTATUS DATABASE_FUNC(Transaction_Release)(
      DATABASE_TYPE(TDatabase) *database,
      const char *name);
```

### Transaction_IsActive

Returns whether a valid connection is currently outside autocommit.

#### Syntax

```c
bool DATABASE_FUNC(Transaction_IsActive)(
      const DATABASE_TYPE(TDatabase) *database);
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
