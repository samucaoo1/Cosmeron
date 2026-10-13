# File

The **File** module provides native file I/O, whole-file content operations, path manipulation, metadata and directory enumeration. It is header-only, targets C11 and requires no additional linker flags.

```c
#include "Cosmeron/Modules/File/File.h"
```

# Overview

| Package | Header | Responsibility |
| --- | --- | --- |
| Path | `File/Path/Path.h` | Names, extensions, parents and resolution |
| Stream | `File/Stream/Stream.h` | Owned open files and byte I/O |
| Content | `File/Content/Content.h` | Load, save, append and atomic replacement |
| Entry | `File/Entry/Entry.h` | Metadata, existence, copy, move and removal |
| Directory | `File/Directory/Directory.h` | Creation and incremental enumeration |

Package headers can be included independently. Required package dependencies are included automatically. Mapping and Watch are reserved for later development and are not part of this implementation.

## Naming and namespace

| Macro form | Default direct form |
| --- | --- |
| `FILE_FUNC(Open)` | `File_Open` |
| `FILE_TYPE(TFile)` | `File_TFile` |
| `FILE_TYPE(SeekOrigin)` | `File_SeekOrigin` |
| `FILE_CONST(READ)` | `FILE_READ` |
| `FILE_FUNC(Path_Join)` | `File_Path_Join` |

`COSMERON_NAMESPACE` and `COSMERON_NAMESPACE_CONST` prefix symbols normally. `FILE_NS` and `FILE_CNS` may also be configured before inclusion.

## Ownership and outputs

Initialize `TFile` and `TDirectory` to `{0}` before opening. They own native resources and must not be copied as independent owners. Close them explicitly; there is no global Init/Free lifecycle. Operations on a shared object require external synchronization.

Paths and write buffers are borrowed. Content returned by Load/LoadText is owned by the caller and compatible with `free` and Cosmeron's malloc-compatible Memory wrappers. It is not an arena allocation.

Required output pointers must be non-NULL. Outputs change only on SUCCESS, with one explicit exception: path-buffer functions also set `outRequired` on OUT_OF_RANGE. This value includes the terminating NUL. A NULL output buffer with zero capacity queries the required size and returns OUT_OF_RANGE. Insufficient buffers remain unchanged. Path functions support overlapping input and output buffers.

## Open flags

| Constant | Meaning |
| --- | --- |
| `FILE_CONST(READ)` | Permit reads |
| `FILE_CONST(WRITE)` | Permit writes |
| `FILE_CONST(OPEN_CREATE)` | Create if missing; preserve existing content |
| `FILE_CONST(OPEN_TRUNCATE)` | Clear existing content; requires WRITE |
| `FILE_CONST(APPEND)` | Write at the end; requires WRITE |
| `FILE_CONST(EXCLUSIVE)` | Fail if the destination exists; requires OPEN_CREATE |


`OPEN_CREATE` and `OPEN_TRUNCATE` replace the original `CREATE`/`TRUNCATE` flag names to avoid Windows SDK macro collisions. Their bit values and behavior are unchanged.

At least READ or WRITE is required. Unknown flags and APPEND combined with OPEN_TRUNCATE are rejected.

Seek origins are `FILE_CONST(SEEK_BEGIN)`, `FILE_CONST(SEEK_CURRENT)` and `FILE_CONST(SEEK_END)`.

## Metadata and entries

`TFileInfo` contains `kind`, `size`, `modified`, `accessed` and `available`. The instants reuse `CHRONOMETRY_TYPE(Instant)` and represent seconds/nanoseconds from the Unix epoch. Check INFO_MODIFIED and INFO_ACCESSED in available before consuming those fields. Access times can depend on filesystem policy.

Kinds are KIND_UNKNOWN, KIND_REGULAR, KIND_DIRECTORY, KIND_LINK and KIND_OTHER. `TFileEntry` contains only name and kind; use Info when full metadata is needed.

# API reference

## Path

# Path_Join

Joins two path components. An absolute right-hand path replaces the base. Rooted or drive-relative right-hand paths on Windows are rejected.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Path_Join)(
    const char *base, const char *part,
    char *output, size_t capacity, size_t *outRequired
);
```

#### Direct form

```c
OPSTATUS File_Path_Join(
    const char *base, const char *part,
    char *output, size_t capacity, size_t *outRequired
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `base` | Base component. |
| `part` | Component to append. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. On `OUT_OF_RANGE`, `outRequired` is set to the required size, including NUL.

### Remarks

Mantém o controle de memória com o chamador. Ao dimensionar buffers, consulte `outRequired` (incluindo o NUL). Não assume que o caminho resultante já exista.

### Example

```c
char path[256];
size_t required = 0;
OPSTATUS status = FILE_FUNC(Path_Join)("data", "file.txt", path, sizeof path, &required);
```

# Path_Normalize

Normalizes separators, duplicate separators and single-dot components without querying the filesystem. Parent components (`..`) remain intact to preserve symbolic-link traversal semantics.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Path_Normalize)(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

#### Direct form

```c
OPSTATUS File_Path_Normalize(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. On `OUT_OF_RANGE`, `outRequired` is set to the required size, including NUL.

### Remarks

Normalização lexical, não canonização de links simbólicos. Segmentos `..` não são removidos deliberadamente.

### Example

```c
char path[256];
size_t required = 0;
OPSTATUS status = FILE_FUNC(Path_Normalize)("data//./file.txt", path, sizeof path, &required);
```

# Path_Parent

Returns the parent path. A root is its own parent; a relative leaf has `.` as its parent.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Path_Parent)(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

#### Direct form

```c
OPSTATUS File_Path_Parent(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. On `OUT_OF_RANGE`, `outRequired` is set to the required size, including NUL.

### Remarks

Sem acesso ao disco; para um arquivo relativo sem pais, retorna `.`. O resultado é escrito num buffer do chamador.

### Example

```c
char parent[256];
size_t required = 0;
OPSTATUS status = FILE_FUNC(Path_Parent)("data/file.txt", parent, sizeof parent, &required);
```

# Path_Name

Returns a borrowed pointer to the last component. A trailing separator produces an empty component. NULL input returns NULL.

### Syntax

#### Macro form

```c
const char * FILE_FUNC(Path_Name)(
    const char *path
);
```

#### Direct form

```c
const char * File_Path_Name(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

The natural result described above.

### Remarks

O ponteiro retornado aponta para a string de entrada e não deve ser liberado. Continua válido somente enquanto a entrada existir.

### Example

```c
const char *name = FILE_FUNC(Path_Name)("data/file.txt");
/* name aponta para "file.txt" na string de entrada. */
```

# Path_Extension

Returns a borrowed pointer to the final extension, including its dot. Dotfiles without a second dot and `..` have no extension; the returned pointer then addresses the string terminator. NULL input returns NULL.

### Syntax

#### Macro form

```c
const char * FILE_FUNC(Path_Extension)(
    const char *path
);
```

#### Direct form

```c
const char * File_Path_Extension(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

The natural result described above.

### Remarks

O ponteiro retornado é emprestado e conserva o ponto. Ausência de extensão retorna ponteiro para o NUL da string.

### Example

```c
const char *extension = FILE_FUNC(Path_Extension)("data/file.txt");
/* extension aponta para ".txt". */
```

# Path_IsAbsolute

Tests whether a path is absolute. Windows accepts drive-rooted and UNC paths; drive-relative and current-drive-rooted paths are not fully absolute.

### Syntax

#### Macro form

```c
bool FILE_FUNC(Path_IsAbsolute)(
    const char *path
);
```

#### Direct form

```c
bool File_Path_IsAbsolute(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

The natural result described above.

### Remarks

A classificação é dependente da plataforma. Não interpreta caminhos de unidade relativa do Windows como absolutos.

### Example

```c
bool absolute = FILE_FUNC(Path_IsAbsolute)("data/file.txt");
```

# Path_Resolve

Resolves an existing path through the filesystem, following symbolic links. The Windows result uses the native extended path prefix (`\\?\`).

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Path_Resolve)(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

#### Direct form

```c
OPSTATUS File_Path_Resolve(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. On `OUT_OF_RANGE`, `outRequired` is set to the required size, including NUL.

## Stream

### Remarks

Consulta o sistema de arquivos; pode falhar por inexistência/permissões. Em Windows, a forma nativa pode incluir o prefixo de caminho estendido.

### Example

```c
char resolved[4096];
size_t required = 0;
OPSTATUS status = FILE_FUNC(Path_Resolve)(".", resolved, sizeof resolved, &required);
```

# Open

Opens a zero-initialized or closed file. Opening an already-owned object returns BUSY. CREATE without TRUNCATE preserves existing bytes.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Open)(
    FILE_TYPE(TFile) *file, const char *path, uint32_t flags
);
```

#### Direct form

```c
OPSTATUS File_Open(
    File_TFile *file, const char *path, uint32_t flags
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `path` | Borrowed path string. |
| `flags` | Combination of open flags. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

`FILE_CONST(READ)` ou `FILE_CONST(WRITE)` é obrigatório; combine flags conforme a tabela acima. Um `TFile` aberto não pode ser aberto novamente sem Close.

### Example

```c
FILE_TYPE(TFile) file = {0};
OPSTATUS status = FILE_FUNC(Open)(&file, "input.bin", FILE_CONST(READ));
if (status == STATUS_CONST(SUCCESS)) FILE_FUNC(Close)(&file);
```

# Close

Closes the owned descriptor and clears the object, including after a close error. Closing an already-closed object succeeds. A failed close must not be retried through a copied object.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Close)(
    FILE_TYPE(TFile) *file
);
```

#### Direct form

```c
OPSTATUS File_Close(
    File_TFile *file
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Libera o descritor e limpa o objeto. Mesmo que Close falhe, não reutilize uma cópia do descritor.

### Example

```c
FILE_TYPE(TFile) file = {0};
/* Um objeto fechado pode receber Close novamente. */
OPSTATUS status = FILE_FUNC(Close)(&file);
```

# Read

Reads up to capacity bytes. Successful short reads are allowed. For a positive capacity, a successful zero-byte read indicates EOF. NULL buffer is accepted only with zero capacity.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Read)(
    FILE_TYPE(TFile) *file, void *buffer,
    size_t capacity, size_t *outRead
);
```

#### Direct form

```c
OPSTATUS File_Read(
    File_TFile *file, void *buffer,
    size_t capacity, size_t *outRead
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `buffer` | Caller-provided byte buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRead` | Bytes actually read. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Leituras curtas são válidas; uma leitura bem-sucedida de zero bytes com capacidade positiva indica EOF. Trate `outRead` em vez de assumir que todas as posições do buffer foram preenchidas.

### Example

```c
FILE_TYPE(TFile) file = {0};
if (FILE_FUNC(Open)(&file, "input.bin", FILE_CONST(READ)) == STATUS_CONST(SUCCESS)) {
    unsigned char bytes[128]; size_t n = 0;
    OPSTATUS status = FILE_FUNC(Read)(&file, bytes, sizeof bytes, &n);
    FILE_FUNC(Close)(&file);
}
```

# Write

Writes up to size bytes. Successful short writes are allowed; callers must loop if they need every byte transferred. NULL buffer is accepted only with zero size.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Write)(
    FILE_TYPE(TFile) *file, const void *buffer,
    size_t size, size_t *outWritten
);
```

#### Direct form

```c
OPSTATUS File_Write(
    File_TFile *file, const void *buffer,
    size_t size, size_t *outWritten
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `buffer` | Caller-provided byte buffer. |
| `size` | Byte count or requested file length. |
| `outWritten` | Bytes actually written. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Escritas curtas bem-sucedidas são possíveis, por isso repita até transmitir todos os bytes quando precisar de escrita integral. O buffer permanece do chamador.

### Example

```c
FILE_TYPE(TFile) file = {0};
if (FILE_FUNC(Open)(&file, "output.bin", FILE_CONST(WRITE) | FILE_CONST(OPEN_CREATE)) == STATUS_CONST(SUCCESS)) {
    const char message[] = "Hi"; size_t n = 0;
    OPSTATUS status = FILE_FUNC(Write)(&file, message, sizeof message - 1, &n);
    FILE_FUNC(Close)(&file);
}
```

# Seek

Changes the byte position relative to the beginning, current position or end. The backend rejects positions it cannot represent.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Seek)(
    FILE_TYPE(TFile) *file, int64_t offset, FILE_TYPE(Seek) origin
);
```

#### Direct form

```c
OPSTATUS File_Seek(
    File_TFile *file, int64_t offset, File_Seek origin
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `offset` | Signed byte offset. |
| `origin` | Seek reference point. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Offsets negativos são permitidos quando formam posição válida. Origem é um dos valores `SEEK_BEGIN`, `SEEK_CURRENT` ou `SEEK_END` do namespace File.

### Example

```c
/* file deve estar aberto. */
OPSTATUS status = FILE_FUNC(Seek)(&file, 0, FILE_CONST(SEEK_BEGIN));
```

# Tell

Returns the current byte position.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Tell)(
    FILE_TYPE(TFile) *file, uint64_t *outPosition
);
```

#### Direct form

```c
OPSTATUS File_Tell(
    File_TFile *file, uint64_t *outPosition
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `outPosition` | Current byte position. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

O resultado é o deslocamento em bytes da posição atual, sem modificar o cursor.

### Example

```c
uint64_t position = 0;
OPSTATUS status = FILE_FUNC(Tell)(&file, &position); /* file aberto */
```

# Size

Returns the size of the opened object without changing its byte position. Non-regular objects may not have a meaningful content size.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Size)(
    FILE_TYPE(TFile) *file, uint64_t *outSize
);
```

#### Direct form

```c
OPSTATUS File_Size(
    File_TFile *file, uint64_t *outSize
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `outSize` | Produced byte count. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Consulta o tamanho do objeto aberto e preserva a posição de leitura/escrita. Dispositivos e objetos não regulares podem não ter tamanho significativo.

### Example

```c
uint64_t size = 0;
OPSTATUS status = FILE_FUNC(Size)(&file, &size); /* file aberto */
```

# Truncate

Sets the file length without changing the current byte position. Requires write access. Values outside the backend signed offset range return OUT_OF_RANGE.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Truncate)(
    FILE_TYPE(TFile) *file, uint64_t size
);
```

#### Direct form

```c
OPSTATUS File_Truncate(
    File_TFile *file, uint64_t size
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `size` | Byte count or requested file length. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Requer acesso de escrita; altera comprimento, mas não reposiciona o cursor aberto. Valores maiores que o limite do backend são rejeitados.

### Example

```c
OPSTATUS status = FILE_FUNC(Truncate)(&file, 1024); /* file aberto para escrita */
```

# Flush

Validates the open file. The descriptor backend has no library-side write buffer, so flushing is otherwise a no-op.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Flush)(
    FILE_TYPE(TFile) *file
);
```

#### Direct form

```c
OPSTATUS File_Flush(
    File_TFile *file
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Os descritores nativos não mantêm um buffer de biblioteca. Para solicitar persistência física, use Sync.

### Example

```c
OPSTATUS status = FILE_FUNC(Flush)(&file); /* file aberto */
```

# Sync

Requests synchronization of the opened descriptor with storage. This operation reports the backend result; it does not synchronize the parent directory.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Sync)(
    FILE_TYPE(TFile) *file
);
```

#### Direct form

```c
OPSTATUS File_Sync(
    File_TFile *file
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

## Entry

### Remarks

Solicita sincronização de conteúdo/descritor, não necessariamente do diretório pai. Garantias finais dependem do SO e sistema de arquivos.

### Example

```c
OPSTATUS status = FILE_FUNC(Sync)(&file); /* file aberto */
```

# Info

Queries kind, byte size and modification/access instants. followLinks controls symbolic-link traversal. Windows reports reparse points as KIND_LINK when traversal is disabled. POSIX timestamps have second precision in this implementation.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Info)(
    const char *path, bool followLinks, FILE_TYPE(TFileInfo) *outInfo
);
```

#### Direct form

```c
OPSTATUS File_Info(
    const char *path, bool followLinks, File_TFileInfo *outInfo
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `followLinks` | Whether to inspect the link target. |
| `outInfo` | Produced metadata. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

`followLinks` decide se segue links. Instantes opcionais devem ser usados apenas após verificar as flags em `available`.

### Example

```c
FILE_TYPE(TFileInfo) info = {0};
OPSTATUS status = FILE_FUNC(Info)("input.bin", true, &info);
```

# Exists

Checks whether a directory entry exists, including dangling symbolic links. Missing entries produce SUCCESS and false. Permission and I/O failures remain errors.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Exists)(
    const char *path, bool *outExists
);
```

#### Direct form

```c
OPSTATUS File_Exists(
    const char *path, bool *outExists
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `outExists` | Whether the entry exists. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Uma entrada ausente corresponde a SUCCESS com `false`; erros de acesso continuam erros, e links simbólicos pendentes são considerados existentes.

### Example

```c
bool exists = false;
OPSTATUS status = FILE_FUNC(Exists)("input.bin", &exists);
```

# Copy

Copies regular-file bytes. replace=false uses exclusive destination creation. Opened-file identity is checked before truncation, preventing destruction through same-path or hard-link aliases. Permissions and timestamps are not copied. A transfer failure may leave a partial destination.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Copy)(
    const char *source, const char *destination, bool replace
);
```

#### Direct form

```c
OPSTATUS File_Copy(
    const char *source, const char *destination, bool replace
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `source` | Source entry. |
| `destination` | Destination entry. |
| `replace` | Whether an existing destination may be replaced. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Copia bytes de arquivos regulares. Não preserva automaticamente metadados e pode deixar destino parcial se falhar. Protege contra auto-sobrescrita por aliases de arquivo aberto.

### Example

```c
OPSTATUS status = FILE_FUNC(Copy)("input.bin", "copy.bin", false);
```

# Move

Moves an entry. Replacement uses native rename on POSIX and MoveFileExW on Windows. Cross-volume regular-file movement falls back to copy then removal, retaining the source if copying fails. POSIX non-replacing moves support regular files via link/unlink; other kinds return NOT_SUPPORTED. If final removal fails, both names may remain. Cross-volume movement is not atomic.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Move)(
    const char *source, const char *destination, bool replace
);
```

#### Direct form

```c
OPSTATUS File_Move(
    const char *source, const char *destination, bool replace
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `source` | Source entry. |
| `destination` | Destination entry. |
| `replace` | Whether an existing destination may be replaced. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Preferência por renomeação nativa. Movimentos entre volumes de arquivos regulares podem usar cópia + remoção; caso a cópia falhe, a origem é preservada.

### Example

```c
OPSTATUS status = FILE_FUNC(Move)("old.bin", "new.bin", false);
```

# Remove

Removes a file entry. It does not recursively remove a directory. POSIX symbolic links are removed without deleting their targets; Windows directory reparse points use Directory_Remove.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Remove)(
    const char *path
);
```

#### Direct form

```c
OPSTATUS File_Remove(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

## Content

### Remarks

Remove entradas sem recursão. Não use para apagar árvore de diretórios.

### Example

```c
OPSTATUS status = FILE_FUNC(Remove)("old.bin");
```

# Load

Loads bytes into malloc-compatible owned memory. The reader grows its buffer until EOF, including if the file size changes. Empty files also return a freeable allocation. No NUL terminator is promised.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Load)(
    const char *path, void **outData, size_t *outSize
);
```

#### Direct form

```c
OPSTATUS File_Load(
    const char *path, void **outData, size_t *outSize
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `outData` | Produced owned byte buffer. |
| `outSize` | Produced byte count. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Retorna alocação compatível com free inclusive para arquivos vazios. Não pressupõe terminador NUL.

### Example

```c
void *data = NULL; size_t size = 0;
OPSTATUS status = FILE_FUNC(Load)("input.bin", &data, &size);
/* Em SUCCESS, libere `data` com free(data) quando terminar. */
```

# LoadText

Loads bytes and appends a NUL terminator. outSize excludes the terminator. Embedded NUL bytes, original line endings and encodings remain unchanged. Returned memory is malloc-compatible.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(LoadText)(
    const char *path, char **outText, size_t *outSize
);
```

#### Direct form

```c
OPSTATUS File_LoadText(
    const char *path, char **outText, size_t *outSize
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `outText` | Produced owned NUL-terminated buffer. |
| `outSize` | Produced byte count. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Aloca espaço extra para NUL mas preserva NULs internos e os bytes originais. `outSize` não inclui o terminador acrescentado.

### Example

```c
char *text = NULL; size_t size = 0;
OPSTATUS status = FILE_FUNC(LoadText)("notes.txt", &text, &size);
/* Em SUCCESS, libere `text` com free(text) quando terminar. */
```

# Save

Creates or truncates a file and writes all bytes. Failure may leave a partial file; use SaveAtomic for replacement without exposing intermediate content.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Save)(
    const char *path, const void *data, size_t size
);
```

#### Direct form

```c
OPSTATUS File_Save(
    const char *path, const void *data, size_t size
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `data` | Borrowed content; NULL permitted only when size is zero. |
| `size` | Byte count or requested file length. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Pode truncar um arquivo existente antes de completar a escrita. Se precisar de substituição sem expor conteúdo intermediário, escolha SaveAtomic.

### Example

```c
const char contents[] = "hello";
OPSTATUS status = FILE_FUNC(Save)("notes.txt", contents, sizeof contents - 1);
```

# Append

Creates the file if absent and appends all bytes. Each backend write uses append mode; concurrent multi-write records may interleave.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Append)(
    const char *path, const void *data, size_t size
);
```

#### Direct form

```c
OPSTATUS File_Append(
    const char *path, const void *data, size_t size
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `data` | Borrowed content; NULL permitted only when size is zero. |
| `size` | Byte count or requested file length. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Cria arquivo se necessário. Registros com múltiplas operações Write podem intercalar com outros processos.

### Example

```c
const char contents[] = "next line\n";
OPSTATUS status = FILE_FUNC(Append)("notes.txt", contents, sizeof contents - 1);
```

# SaveAtomic

Creates an exclusive temporary file beside the target, writes all bytes, synchronizes and closes it, then replaces the target. Before replacement fails, the old target remains intact and cleanup of the owned temporary file is attempted. Replacement creates a new file identity and does not preserve target metadata. The parent directory is not synchronized, so success does not promise survival of a sudden power loss. Native filesystem replacement semantics apply; network filesystems and concurrent path changes may provide weaker guarantees.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(SaveAtomic)(
    const char *path, const void *data, size_t size
);
```

#### Direct form

```c
OPSTATUS File_SaveAtomic(
    const char *path, const void *data, size_t size
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `data` | Borrowed content; NULL permitted only when size is zero. |
| `size` | Byte count or requested file length. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

## Directory

### Remarks

A substituição é feita com arquivo temporário vizinho ao destino. Isso evita expor escrita parcial na troca; não promete atomicidade/durabilidade superior às garantias reais do backend.

### Example

```c
const char contents[] = "configuration";
OPSTATUS status = FILE_FUNC(SaveAtomic)("config.txt", contents, sizeof contents - 1);
```

# Directory_Create

Creates one directory. Existing entries return ALREADY_EXISTS.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Directory_Create)(
    const char *path
);
```

#### Direct form

```c
OPSTATUS File_Directory_Create(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Cria somente a última pasta. Não cria pais ausentes e retorna ALREADY_EXISTS para entrada prévia.

### Example

```c
OPSTATUS status = FILE_FUNC(Directory_Create)("newdir");
```

# Directory_CreateAll

Creates missing parent directories. Existing directories are accepted; a file blocking a component is an error. Creation is incremental and is not rolled back after failure.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Directory_CreateAll)(
    const char *path
);
```

#### Direct form

```c
OPSTATUS File_Directory_CreateAll(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Cria componentes intermediários conforme necessário. Se ocorrer erro após algumas criações, elas não são revertidas.

### Example

```c
OPSTATUS status = FILE_FUNC(Directory_CreateAll)("cache/output/reports");
```

# Directory_Open

Opens a zero-initialized or closed directory iterator. Opening an already-owned iterator returns BUSY.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Directory_Open)(
    FILE_TYPE(TDirectory) *directory, const char *path
);
```

#### Direct form

```c
OPSTATUS File_Directory_Open(
    File_TDirectory *directory, const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `directory` | Owned, zero-initialized iterator. |
| `path` | Borrowed path string. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

O objeto `TDirectory` deve começar zerado. É necessário fechar a iteração para liberar seus recursos.

### Example

```c
FILE_TYPE(TDirectory) directory = {0};
OPSTATUS status = FILE_FUNC(Directory_Open)(&directory, ".");
if (status == STATUS_CONST(SUCCESS)) FILE_FUNC(Directory_Close)(&directory);
```

# Directory_Next

Produces the next entry, skipping `.` and `..`. SUCCESS with outFound=false means exhaustion; outEntry is then unchanged. The name is borrowed and valid only until the next Next call or Close. Ordering is unspecified. A POSIX entry disappearing during lookup is reported as KIND_UNKNOWN. Enumeration is not a snapshot; errors may consume an entry.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Directory_Next)(
    FILE_TYPE(TDirectory) *directory,
    FILE_TYPE(TFileEntry) *outEntry, bool *outFound
);
```

#### Direct form

```c
OPSTATUS File_Directory_Next(
    File_TDirectory *directory,
    File_TFileEntry *outEntry, bool *outFound
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `directory` | Owned, zero-initialized iterator. |
| `outEntry` | Produced borrowed entry; unchanged at exhaustion. |
| `outFound` | Whether an entry was produced. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Retorna `outFound=false` na conclusão, sem erro. O nome retornado é emprestado e fica inválido na próxima iteração ou fechamento.

### Example

```c
/* directory já foi aberta. */
FILE_TYPE(TFileEntry) entry = {0}; bool found = false;
OPSTATUS status = FILE_FUNC(Directory_Next)(&directory, &entry, &found);
```

# Directory_Close

Closes the iterator, frees internal memory and clears the object. An already-closed iterator succeeds.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Directory_Close)(
    FILE_TYPE(TDirectory) *directory
);
```

#### Direct form

```c
OPSTATUS File_Directory_Close(
    File_TDirectory *directory
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `directory` | Owned, zero-initialized iterator. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Idempotente em objeto zerado/fechado; libera estado interno e reinicializa seus campos.

### Example

```c
FILE_TYPE(TDirectory) directory = {0};
OPSTATUS status = FILE_FUNC(Directory_Close)(&directory);
```

# Directory_Remove

Removes an empty directory. Nonempty directories remain intact. Windows also uses this operation for directory reparse points.

### Syntax

#### Macro form

```c
OPSTATUS FILE_FUNC(Directory_Remove)(
    const char *path
);
```

#### Direct form

```c
OPSTATUS File_Directory_Remove(
    const char *path
);
```

### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Side effects on the underlying file or filesystem may already have occurred before an I/O error.

### Remarks

Exige diretório vazio e não executa remoção recursiva.

### Example

```c
OPSTATUS status = FILE_FUNC(Directory_Remove)("emptydir");
```


# Examples

## Direct content operations

```c
#include "Cosmeron/Modules/File/File.h"

int main(void) {
    char *text = NULL;
    size_t size = 0;
    OPSTATUS status = File_SaveAtomic("config.txt", "volume=80\n", 10);
    if (status != STATUS_CONST(SUCCESS)) return 1;
    status = File_LoadText("config.txt", &text, &size);
    if (status != STATUS_CONST(SUCCESS)) return 1;
    /* Consume exactly size bytes; text[size] is NUL. */
    free(text);
    return 0;
}
```

## Incremental reading, using the namespace-independent form

```c
#include "Cosmeron/Modules/File/Stream/Stream.h"

int main(void) {
    FILE_TYPE(TFile) file = {0};
    unsigned char buffer[256];
    OPSTATUS status = FILE_FUNC(Open)(&file, "data.bin", FILE_CONST(READ));
    if (status != STATUS_CONST(SUCCESS)) return 1;
    for (;;) {
        size_t count;
        status = FILE_FUNC(Read)(&file, buffer, sizeof(buffer), &count);
        if (status != STATUS_CONST(SUCCESS) || count == 0) break;
        /* Consume count bytes from buffer. */
    }
    OPSTATUS closed = FILE_FUNC(Close)(&file);
    return status == STATUS_CONST(SUCCESS) && closed == STATUS_CONST(SUCCESS) ? 0 : 1;
}
```

# Requirements and portability

| Backend | Implementation | Extra linker flags |
| --- | --- | --- |
| POSIX (Linux/macOS/BSD) | Native descriptors, stat/lstat, directory APIs | None |
| Windows (MinGW/MSVC) | Wide CRT descriptors and Win32 filesystem APIs | None beyond the compiler's default runtime/system libraries |

Windows path input/output is UTF-8, converted strictly to/from UTF-16. POSIX paths preserve native bytes; callers may use UTF-8 but encoding is not imposed. No fixed PATH_MAX buffers are used. A Windows native extended path remains usable by the Windows backend.

Seek and truncation accept 64-bit API values, but reject values unrepresentable in native off_t on POSIX. Applications targeting large files on 32-bit POSIX should select the appropriate libc large-file configuration before including any system header.

The headers do not define libc feature macros. Standard POSIX declarations hidden by strict feature profiles are declared with their native signatures. No external process, shell command or global state is used at runtime.

Read/Write, Append, Copy and Move are not transactions. SaveAtomic replaces content through a sibling temporary file but does not preserve metadata or guarantee parent-directory crash durability. The module does not implement recursive deletion, permissions editing, file locking, mapping or watching yet.

# Validation

```sh
make -C Codespace/Tests/File run
```

The suite covers independent headers, custom namespace, multi-translation-unit linking, binary/text/empty content, incremental I/O, EOF, truncation, exclusive creation, Unicode names, path buffer sizing, directory iteration and failure output contracts. POSIX tests also cover hard-link self-copy protection and dangling symbolic links. CI includes GCC, Clang, macOS, MinGW, MSVC, sanitizers and 32-bit POSIX compilation.

# See also

- [Error](Error.md) — operation status.
- [Memory](Memory.md) — ownership and allocation helpers.
- [Chronometry](Chronometry.md) — instant representation.
- [Text](Text.md) — encoding and text processing.
