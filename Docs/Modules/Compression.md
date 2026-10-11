# Compression

The **Compression** module provides DEFLATE, zlib, gzip, ZIP, CRC32 and Adler32.
It targets C11, includes its compression engine and requires no `-lz`, `-lm`,
external executable or separately compiled implementation. Each translation
unit can include the header normally.

```c
#include "Cosmeron/Modules/Compression/Compression.h"
```

## Overview

| Package | Header | Responsibility |
| --- | --- | --- |
| Codec | `Compression/Codec/Codec.h` | Raw DEFLATE, zlib and gzip, caller buffers and owned results |
| Checksum | `Compression/Checksum/Checksum.h` | Incremental CRC32 and Adler32 |
| Zip | `Compression/Zip/Zip.h` | In-memory ZIP creation, listing and extraction |
| File | `Compression/File/File.h` | Bounded file transforms using the existing File module |

Package headers work independently. The umbrella includes the optional function
table; define `COMPRESSION_DISABLE_FUNCTION_TABLE` before including it to remove
the table's type, instance and bindings.

## Names and types

| Macro form | Direct form |
| --- | --- |
| `COMPRESSION_FUNC(Compress)` | `Compression_Compress` |
| `COMPRESSION_TYPE(Format)` | `Compression_Format` |
| `COMPRESSION_TYPE(TBuffer)` | `Compression_TBuffer` |
| `COMPRESSION_CONST(GZIP)` | `COMPRESSION_GZIP` |
| `COMPRESSION_NS(Functions)` | `Compression_Functions` |

`COSMERON_NAMESPACE`, `COSMERON_NAMESPACE_CONST`, `COMPRESSION_NS` and
`COMPRESSION_CNS` customize the public namespace. Vendor symbols have a fixed
private prefix and internal linkage; zlib compatibility aliases are disabled.

| Type | Fields / meaning |
| --- | --- |
| `TBuffer` | Owned `unsigned char *data` and byte count `size` |
| `TZipReader` | Owned opaque `native` reader; borrows archive bytes |
| `TZipWriter` | Owned opaque `native` writer; holds an archive in memory |
| `TZipEntry` | `compressedSize`, `size` (uint64), `crc32`, `directory`, `supported` |
| `Format` | `DEFLATE`, `ZLIB`, `GZIP` |

Raw DEFLATE has no header or checksum. zlib has its standard header and Adler32;
gzip has its header, CRC32 and size trailer. Checksums detect corruption and do
not authenticate data.

Levels range from `LEVEL_STORE` (0) through `LEVEL_BEST` (9), with `LEVEL_FAST`
(1) and `LEVEL_DEFAULT` (-1). Unsupported values return INVALID_ARGUMENT.

## Ownership and errors

Zero-initialize buffers and ZIP handles. Do not copy them as independent owners.
Allocation functions require an empty output buffer; release it with BufferFree,
which also clears its fields. A zero-length allocation may still own a non-NULL
pointer. ZIP readers borrow the original archive until ZipClose; keep its bytes
alive and unchanged. Close/discard/free accept NULL and repeated cleanup.

Required `out` pointers must be non-NULL. Outputs are committed only on SUCCESS.
The destination bytes of caller-buffer transforms and extraction may be partly
written on failure; their contents are usable only on SUCCESS. Source and
destination regions must not overlap. Shared ZIP handles require synchronization;
independent operations have no global lifecycle or LastError state.

| Status | Meaning |
| --- | --- |
| `SUCCESS` | Operation completed |
| `INVALID_ARGUMENT` | Invalid pointer, format, level, handle state or forbidden overlap |
| `INSUFFICIENT_SPACE` | Destination too small or decompression budget exceeded |
| `OUT_OF_RANGE` | Index, size or input-file budget exceeded |
| `OUT_OF_MEMORY` | Allocation failed |
| `INVALID_SEQUENCE` | Invalid data, checksum mismatch or forbidden trailing bytes |
| `INCOMPLETE_SEQUENCE` | Truncated codec input when distinguishable; otherwise INVALID_SEQUENCE |
| `NOT_FOUND` | ZIP name missing |
| `NOT_SUPPORTED` | ZIP method, encryption or feature unavailable |

File failures preserve the existing File statuses. Some vendor write failures
report GENERIC_ERROR because the engine does not retain the underlying
allocation cause. Discard a writer after a failed Add or Finish; it may contain
partially written archive data.

## Codec API

```c
OPSTATUS Compression_Bound(Compression_Format format,
    size_t size, size_t *outBound);
OPSTATUS Compression_Compress(Compression_Format format,
    const void *source, size_t size, void *destination, size_t capacity,
    int level, size_t *outSize);
OPSTATUS Compression_Decompress(Compression_Format format,
    const void *source, size_t size, void *destination, size_t capacity,
    size_t *outSize);
OPSTATUS Compression_CompressAlloc(Compression_Format format,
    const void *source, size_t size, int level, Compression_TBuffer *outBuffer);
OPSTATUS Compression_DecompressAlloc(Compression_Format format,
    const void *source, size_t size, size_t maxOutput,
    Compression_TBuffer *outBuffer);
void Compression_BufferFree(Compression_TBuffer *buffer);
```

**Bound** returns a conservative compressed capacity. It is not a query for
uncompressed size. **Compress** accepts NULL source only when size is zero.
**Decompress** requires an actual encoded stream; zero input returns
INCOMPLETE_SEQUENCE. Empty decoded data can use NULL destination and capacity 0.

**DecompressAlloc** grows its output geometrically, never above maxOutput,
and retries decoding after growth. This avoids allocating the whole budget
upfront, but is not an incremental streaming interface. Internal codec state
uses additional fixed working memory. A budget of zero permits only empty data.

Gzip decoding accepts optional extra fields, name, comment, header CRC and
concatenated members. Every member's CRC32 and size are checked, and the budget
applies to their combined output. Reserved flags, corrupt trailers and trailing
garbage are rejected. Raw DEFLATE and zlib require exactly one complete stream.
Preset zlib dictionaries are unsupported.

### Example: direct and function-table calls

```c
Compression_TBuffer packed = {0}, unpacked = {0};
OPSTATUS status = Compression_CompressAlloc(
    COMPRESSION_GZIP, text, textSize, COMPRESSION_LEVEL_DEFAULT, &packed);
if (status == STATUS_SUCCESS)
    status = Compression_Functions.DecompressAlloc(
        COMPRESSION_GZIP, packed.data, packed.size, maxOutput, &unpacked);
/* Use unpacked.data and unpacked.size only on SUCCESS. */
Compression_BufferFree(&unpacked);
Compression_BufferFree(&packed);
```

## Checksum API

```c
uint32_t Compression_CRC32(uint32_t previous, const void *data, size_t size);
uint32_t Compression_Adler32(uint32_t previous, const void *data, size_t size);
```

Start CRC32 at 0 and Adler32 at 1. Pass the previous result for subsequent
chunks. Nonzero size requires readable data. NULL data resets the checksum to
its initial value, matching the embedded engine.

## ZIP API

```c
OPSTATUS Compression_ZipOpen(Compression_TZipReader *reader,
    const void *data, size_t size);
void Compression_ZipClose(Compression_TZipReader *reader);
OPSTATUS Compression_ZipCount(const Compression_TZipReader *reader,
    size_t *outCount);
OPSTATUS Compression_ZipEntry(const Compression_TZipReader *reader,
    size_t index, Compression_TZipEntry *outEntry);
OPSTATUS Compression_ZipName(const Compression_TZipReader *reader,
    size_t index, char *destination, size_t capacity, size_t *outSize);
OPSTATUS Compression_ZipFind(const Compression_TZipReader *reader,
    const char *name, size_t *outIndex);
OPSTATUS Compression_ZipExtract(const Compression_TZipReader *reader,
    size_t index, void *destination, size_t capacity, size_t *outSize);
OPSTATUS Compression_ZipExtractAlloc(const Compression_TZipReader *reader,
    size_t index, size_t maxOutput, Compression_TBuffer *outBuffer);
OPSTATUS Compression_ZipCreate(Compression_TZipWriter *writer);
OPSTATUS Compression_ZipAdd(Compression_TZipWriter *writer, const char *name,
    const void *data, size_t size, int level);
OPSTATUS Compression_ZipFinish(Compression_TZipWriter *writer,
    Compression_TBuffer *outBuffer);
void Compression_ZipDiscard(Compression_TZipWriter *writer);
```

Open parses the archive directory; payload integrity is verified on extraction.
Count includes directory entries. Entry exposes the uncompressed size before
allocation; unsupported entries remain listable. Find is case-sensitive. Name
writes a NUL-terminated filename and returns its byte length excluding NUL;
capacity must include the terminator. ZIP names can use up to UINT16_MAX bytes.
Names read from external archives are untrusted bytes, including traversal names.

Extract writes into a caller buffer and verifies CRC32. ExtractAlloc checks the
entry size against maxOutput before allocating. Both support empty files and
directory entries. Unsupported compression/encryption returns NOT_SUPPORTED.
Extraction is to memory only; it does not interpret archive names as disk paths.

Create starts an in-memory writer. Add accepts portable relative paths, rejects
absolute paths, backslashes, colons, empty components, `.` and `..`, and accepts
trailing `/` for zero-sized directory entries. Duplicate names are permitted.
Finish transfers an owned archive buffer to the caller and clears the writer.
Discard releases an unfinished writer. Creation uses a deterministic DOS
1980-01-01 timestamp and emits UTF-8 filename flags without validating encoding.

Store and Deflate are supported, with ZIP64 handling inherited from miniz.
Multidisk archives and encrypted entries are unsupported. No filesystem tree
extraction, permissions restoration or symlink creation is provided.

## File API

```c
OPSTATUS Compression_CompressFile(Compression_Format format,
    const char *sourcePath, const char *destinationPath,
    int level, size_t maxInput);
OPSTATUS Compression_DecompressFile(Compression_Format format,
    const char *sourcePath, const char *destinationPath,
    size_t maxInput, size_t maxOutput);
```

These helpers use File_Open/Read/Size/Close and File_SaveAtomic. Input loading is
bounded, output allocation is bounded for decompression, and a failed transform
does not replace the destination. Source and destination may be the same path.
The helpers process regular, seekable files in memory. Concurrent source changes
are not a snapshot; growth beyond the measured size is rejected. Atomic save
has the existing File module's durability and platform guarantees.

## Limits and implementation

Codec buffers and their sizes/budgets are limited to UINT_MAX bytes, including
on 64-bit systems. Bound may reject a slightly smaller source when its worst-case
compressed size exceeds that limit. ZIP uses size_t-addressable memory and
uint64 entry sizes, with the engine's central-directory limits. Whole archives
are held in memory. maxOutput bounds output bytes, not all codec or ZIP metadata
allocations. There is no public streaming API yet.

RAR, 7z, TAR, bzip2, xz, Zstandard and LZ4 are not implemented. No format
autodetection or compression of directory trees is implied by this API.

The embedded engine is miniz **3.1.0**, commit
`174573d60290f447c13a2b1b3405de2b96e27d6c`, under the MIT license preserved in
`Compression/Vendor/LICENSE.txt`. `Tools/vendor_miniz.py` regenerates the vendor
include from that checkout, prefixes its identifiers, gives exported functions
internal linkage, disables stdio/time/zlib aliases, uses portable aligned reads,
sets valid deterministic ZIP dates and fixes temporary-compressor cleanup on
early heap-write failures. Upstream vendor code retains its original style.

## Validation

The Compression suite covers all public functions, every codec level, empty
inputs, exact and insufficient buffers, truncation, corrupt checksums,
concatenated gzip members, optional gzip headers, ZIP Store/Deflate, unsupported
methods, allocation-failure cleanup, random malformed inputs, namespace
customization, disabled tables, file budgets/atomic replacement and multiple
translation units. `interop.py` checks both directions with Python's zlib, gzip
and zipfile implementations. CI includes GCC, Clang, macOS, MinGW, MSVC,
sanitizers and 32-bit builds; platform results depend on those CI runs.

```sh
make -C Codespace/Tests/Compression run interop
```
