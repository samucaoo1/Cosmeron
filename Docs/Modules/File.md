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
| `FILE_CONST(CREATE)` | Create if missing; preserve existing content |
| `FILE_CONST(TRUNCATE)` | Clear existing content; requires WRITE |
| `FILE_CONST(APPEND)` | Write at the end; requires WRITE |
| `FILE_CONST(EXCLUSIVE)` | Fail if the destination exists; requires CREATE |

At least READ or WRITE is required. Unknown flags and APPEND combined with TRUNCATE are rejected.

Seek origins are `FILE_CONST(SEEK_BEGIN)`, `FILE_CONST(SEEK_CURRENT)` and `FILE_CONST(SEEK_END)`.

## Metadata and entries

`TFileInfo` contains `kind`, `size`, `modified`, `accessed` and `available`. The instants reuse `CHRONOMETRY_TYPE(Instant)` and represent seconds/nanoseconds from the Unix epoch. Check INFO_MODIFIED and INFO_ACCESSED in available before consuming those fields. Access times can depend on filesystem policy.

Kinds are KIND_UNKNOWN, KIND_REGULAR, KIND_DIRECTORY, KIND_LINK and KIND_OTHER. `TFileEntry` contains only name and kind; use Info when full metadata is needed.

# API reference

## Path

### Path_Join

Joins two path components. An absolute right-hand path replaces the base. Rooted or drive-relative right-hand paths on Windows are rejected.

#### Syntax

```c
OPSTATUS FILE_FUNC(Path_Join)(
    const char *base, const char *part,
    char *output, size_t capacity, size_t *outRequired
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `base` | Base component. |
| `part` | Component to append. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Path_Normalize

Normalizes separators, duplicate separators and single-dot components without querying the filesystem. Parent components (`..`) remain intact to preserve symbolic-link traversal semantics.

#### Syntax

```c
OPSTATUS FILE_FUNC(Path_Normalize)(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Path_Parent

Returns the parent path. A root is its own parent; a relative leaf has `.` as its parent.

#### Syntax

```c
OPSTATUS FILE_FUNC(Path_Parent)(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Path_Name

Returns a borrowed pointer to the last component. A trailing separator produces an empty component. NULL input returns NULL.

#### Syntax

```c
const char * FILE_FUNC(Path_Name)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

The natural result described above.

### Path_Extension

Returns a borrowed pointer to the final extension, including its dot. Dotfiles without a second dot and `..` have no extension; the returned pointer then addresses the string terminator. NULL input returns NULL.

#### Syntax

```c
const char * FILE_FUNC(Path_Extension)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

The natural result described above.

### Path_IsAbsolute

Tests whether a path is absolute. Windows accepts drive-rooted and UNC paths; drive-relative and current-drive-rooted paths are not fully absolute.

#### Syntax

```c
bool FILE_FUNC(Path_IsAbsolute)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

The natural result described above.

### Path_Resolve

Resolves an existing path through the filesystem, following symbolic links. The Windows result uses the native extended path prefix (`\\?\`).

#### Syntax

```c
OPSTATUS FILE_FUNC(Path_Resolve)(
    const char *path, char *output,
    size_t capacity, size_t *outRequired
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `output` | Caller-owned output buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRequired` | Required bytes, including NUL; also set on OUT_OF_RANGE. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

## Stream

### Open

Opens a zero-initialized or closed file. Opening an already-owned object returns BUSY. CREATE without TRUNCATE preserves existing bytes.

#### Syntax

```c
OPSTATUS FILE_FUNC(Open)(
    FILE_TYPE(TFile) *file, const char *path, uint32_t flags
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `path` | Borrowed path string. |
| `flags` | Combination of open flags. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Close

Closes the owned descriptor and clears the object, including after a close error. Closing an already-closed object succeeds. A failed close must not be retried through a copied object.

#### Syntax

```c
OPSTATUS FILE_FUNC(Close)(
    FILE_TYPE(TFile) *file
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Read

Reads up to capacity bytes. Successful short reads are allowed. For a positive capacity, a successful zero-byte read indicates EOF. NULL buffer is accepted only with zero capacity.

#### Syntax

```c
OPSTATUS FILE_FUNC(Read)(
    FILE_TYPE(TFile) *file, void *buffer,
    size_t capacity, size_t *outRead
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `buffer` | Caller-provided byte buffer. |
| `capacity` | Available buffer size in bytes. |
| `outRead` | Bytes actually read. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Write

Writes up to size bytes. Successful short writes are allowed; callers must loop if they need every byte transferred. NULL buffer is accepted only with zero size.

#### Syntax

```c
OPSTATUS FILE_FUNC(Write)(
    FILE_TYPE(TFile) *file, const void *buffer,
    size_t size, size_t *outWritten
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `buffer` | Caller-provided byte buffer. |
| `size` | Byte count or requested file length. |
| `outWritten` | Bytes actually written. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Seek

Changes the byte position relative to the beginning, current position or end. The backend rejects positions it cannot represent.

#### Syntax

```c
OPSTATUS FILE_FUNC(Seek)(
    FILE_TYPE(TFile) *file, int64_t offset, FILE_TYPE(Seek) origin
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `offset` | Signed byte offset. |
| `origin` | Seek reference point. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Tell

Returns the current byte position.

#### Syntax

```c
OPSTATUS FILE_FUNC(Tell)(
    FILE_TYPE(TFile) *file, uint64_t *outPosition
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `outPosition` | Current byte position. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Size

Returns the size of the opened object without changing its byte position. Non-regular objects may not have a meaningful content size.

#### Syntax

```c
OPSTATUS FILE_FUNC(Size)(
    FILE_TYPE(TFile) *file, uint64_t *outSize
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `outSize` | Produced byte count. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Truncate

Sets the file length without changing the current byte position. Requires write access. Values outside the backend signed offset range return OUT_OF_RANGE.

#### Syntax

```c
OPSTATUS FILE_FUNC(Truncate)(
    FILE_TYPE(TFile) *file, uint64_t size
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |
| `size` | Byte count or requested file length. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Flush

Validates the open file. The descriptor backend has no library-side write buffer, so flushing is otherwise a no-op.

#### Syntax

```c
OPSTATUS FILE_FUNC(Flush)(
    FILE_TYPE(TFile) *file
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Sync

Requests synchronization of the opened descriptor with storage. This operation reports the backend result; it does not synchronize the parent directory.

#### Syntax

```c
OPSTATUS FILE_FUNC(Sync)(
    FILE_TYPE(TFile) *file
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `file` | Owned, zero-initialized file object. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

## Entry

### Info

Queries kind, byte size and modification/access instants. followLinks controls symbolic-link traversal. Windows reports reparse points as KIND_LINK when traversal is disabled. POSIX timestamps have second precision in this implementation.

#### Syntax

```c
OPSTATUS FILE_FUNC(Info)(
    const char *path, bool followLinks, FILE_TYPE(TFileInfo) *outInfo
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `followLinks` | Whether to inspect the link target. |
| `outInfo` | Produced metadata. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Exists

Checks whether a directory entry exists, including dangling symbolic links. Missing entries produce SUCCESS and false. Permission and I/O failures remain errors.

#### Syntax

```c
OPSTATUS FILE_FUNC(Exists)(
    const char *path, bool *outExists
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `outExists` | Whether the entry exists. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Copy

Copies regular-file bytes. replace=false uses exclusive destination creation. Opened-file identity is checked before truncation, preventing destruction through same-path or hard-link aliases. Permissions and timestamps are not copied. A transfer failure may leave a partial destination.

#### Syntax

```c
OPSTATUS FILE_FUNC(Copy)(
    const char *source, const char *destination, bool replace
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `source` | Source entry. |
| `destination` | Destination entry. |
| `replace` | Whether an existing destination may be replaced. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Move

Moves an entry. Replacement uses native rename on POSIX and MoveFileExW on Windows. Cross-volume regular-file movement falls back to copy then removal, retaining the source if copying fails. POSIX non-replacing moves support regular files via link/unlink; other kinds return NOT_SUPPORTED. If final removal fails, both names may remain. Cross-volume movement is not atomic.

#### Syntax

```c
OPSTATUS FILE_FUNC(Move)(
    const char *source, const char *destination, bool replace
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `source` | Source entry. |
| `destination` | Destination entry. |
| `replace` | Whether an existing destination may be replaced. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Remove

Removes a file entry. It does not recursively remove a directory. POSIX symbolic links are removed without deleting their targets; Windows directory reparse points use Directory_Remove.

#### Syntax

```c
OPSTATUS FILE_FUNC(Remove)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

## Content

### Load

Loads bytes into malloc-compatible owned memory. The reader grows its buffer until EOF, including if the file size changes. Empty files also return a freeable allocation. No NUL terminator is promised.

#### Syntax

```c
OPSTATUS FILE_FUNC(Load)(
    const char *path, void **outData, size_t *outSize
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `outData` | Produced owned byte buffer. |
| `outSize` | Produced byte count. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### LoadText

Loads bytes and appends a NUL terminator. outSize excludes the terminator. Embedded NUL bytes, original line endings and encodings remain unchanged. Returned memory is malloc-compatible.

#### Syntax

```c
OPSTATUS FILE_FUNC(LoadText)(
    const char *path, char **outText, size_t *outSize
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `outText` | Produced owned NUL-terminated buffer. |
| `outSize` | Produced byte count. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Save

Creates or truncates a file and writes all bytes. Failure may leave a partial file; use SaveAtomic for replacement without exposing intermediate content.

#### Syntax

```c
OPSTATUS FILE_FUNC(Save)(
    const char *path, const void *data, size_t size
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `data` | Borrowed content; NULL permitted only when size is zero. |
| `size` | Byte count or requested file length. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Append

Creates the file if absent and appends all bytes. Each backend write uses append mode; concurrent multi-write records may interleave.

#### Syntax

```c
OPSTATUS FILE_FUNC(Append)(
    const char *path, const void *data, size_t size
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `data` | Borrowed content; NULL permitted only when size is zero. |
| `size` | Byte count or requested file length. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### SaveAtomic

Creates an exclusive temporary file beside the target, writes all bytes, synchronizes and closes it, then replaces the target. Before replacement fails, the old target remains intact and cleanup of the owned temporary file is attempted. Replacement creates a new file identity and does not preserve target metadata. The parent directory is not synchronized, so success does not promise survival of a sudden power loss. Native filesystem replacement semantics apply; network filesystems and concurrent path changes may provide weaker guarantees.

#### Syntax

```c
OPSTATUS FILE_FUNC(SaveAtomic)(
    const char *path, const void *data, size_t size
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |
| `data` | Borrowed content; NULL permitted only when size is zero. |
| `size` | Byte count or requested file length. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

## Directory

### Directory_Create

Creates one directory. Existing entries return ALREADY_EXISTS.

#### Syntax

```c
OPSTATUS FILE_FUNC(Directory_Create)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Directory_CreateAll

Creates missing parent directories. Existing directories are accepted; a file blocking a component is an error. Creation is incremental and is not rolled back after failure.

#### Syntax

```c
OPSTATUS FILE_FUNC(Directory_CreateAll)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Directory_Open

Opens a zero-initialized or closed directory iterator. Opening an already-owned iterator returns BUSY.

#### Syntax

```c
OPSTATUS FILE_FUNC(Directory_Open)(
    FILE_TYPE(TDirectory) *directory, const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `directory` | Owned, zero-initialized iterator. |
| `path` | Borrowed path string. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Directory_Next

Produces the next entry, skipping `.` and `..`. SUCCESS with outFound=false means exhaustion; outEntry is then unchanged. The name is borrowed and valid only until the next Next call or Close. Ordering is unspecified. A POSIX entry disappearing during lookup is reported as KIND_UNKNOWN. Enumeration is not a snapshot; errors may consume an entry.

#### Syntax

```c
OPSTATUS FILE_FUNC(Directory_Next)(
    FILE_TYPE(TDirectory) *directory,
    FILE_TYPE(TFileEntry) *outEntry, bool *outFound
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `directory` | Owned, zero-initialized iterator. |
| `outEntry` | Produced borrowed entry; unchanged at exhaustion. |
| `outFound` | Whether an entry was produced. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Directory_Close

Closes the iterator, frees internal memory and clears the object. An already-closed iterator succeeds.

#### Syntax

```c
OPSTATUS FILE_FUNC(Directory_Close)(
    FILE_TYPE(TDirectory) *directory
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `directory` | Owned, zero-initialized iterator. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

### Directory_Remove

Removes an empty directory. Nonempty directories remain intact. Windows also uses this operation for directory reparse points.

#### Syntax

```c
OPSTATUS FILE_FUNC(Directory_Remove)(
    const char *path
);
```

#### Parameters

| Parameter | Meaning |
| --- | --- |
| `path` | Borrowed path string. |

#### Return value

`STATUS_CONST(SUCCESS)` on completion; otherwise an Error status. Outputs remain unchanged on failure except the documented path sizing output.

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
