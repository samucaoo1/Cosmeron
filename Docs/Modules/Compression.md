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


---

# API reference

The following reference expands the package-level overview above. Every exported callable is shown under its macro-qualified form and its default direct name; signatures are those declared in the corresponding headers. Examples use input pointers and handles prepared by the caller when indicated in comments.

## Function summary

| Package | Function | Return type |
| --- | --- | --- |
| Codec | `COMPRESSION_FUNC(Bound)` | `OPSTATUS` |
| Codec | `COMPRESSION_FUNC(Compress)` | `OPSTATUS` |
| Codec | `COMPRESSION_FUNC(Decompress)` | `OPSTATUS` |
| Codec | `COMPRESSION_FUNC(CompressAlloc)` | `OPSTATUS` |
| Codec | `COMPRESSION_FUNC(DecompressAlloc)` | `OPSTATUS` |
| Codec | `COMPRESSION_FUNC(BufferFree)` | `void` |
| Checksum | `COMPRESSION_FUNC(CRC32)` | `uint32_t` |
| Checksum | `COMPRESSION_FUNC(Adler32)` | `uint32_t` |
| Zip | `COMPRESSION_FUNC(ZipOpen)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipClose)` | `void` |
| Zip | `COMPRESSION_FUNC(ZipCount)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipEntry)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipName)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipFind)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipExtract)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipExtractAlloc)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipCreate)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipAdd)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipFinish)` | `OPSTATUS` |
| Zip | `COMPRESSION_FUNC(ZipDiscard)` | `void` |
| File | `COMPRESSION_FUNC(CompressFile)` | `OPSTATUS` |
| File | `COMPRESSION_FUNC(DecompressFile)` | `OPSTATUS` |

# Codec package

# Bound

Calcula um limite conservador para o tamanho codificado.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(Bound)(COMPRESSION_TYPE(Format) format, size_t size, size_t *outBound);
```

#### Direct form

```c
OPSTATUS Compression_Bound(Compression_Format format, size_t size, size_t *outBound);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).
- `outBound`: recebe a capacidade limite em bytes, apenas em SUCCESS.

### Return value

`OPSTATUS`. `OUT_OF_RANGE` caso o tamanho ou limite exceda `UINT_MAX`.

### Remarks

Um limite para alocar o destino de Compress, não o tamanho descompactado.

### Example

```c
size_t bound = 0;
OPSTATUS status = COMPRESSION_FUNC(Bound)(COMPRESSION_CONST(ZLIB), 2048, &bound);
```

# Compress

Codifica dados em um buffer fornecido pelo chamador.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(Compress)(COMPRESSION_TYPE(Format) format, const void *source, size_t size, void *destination, size_t capacity, int level, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Compression_Compress(Compression_Format format, const void *source, size_t size, void *destination, size_t capacity, int level, size_t *outSize);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `source`: ponteiro para dados da origem; NULL só quando size é zero.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).
- `destination`: buffer de destino do chamador.
- `capacity`: capacidade do destino em bytes.
- `level`: nível de compressão, de `LEVEL_DEFAULT` (-1) a `LEVEL_BEST` (9).
- `outSize`: recebe quantidade de bytes gravados, apenas em SUCCESS.

### Return value

`OPSTATUS`. `INSUFFICIENT_SPACE` se o destino não comportar a saída; `INVALID_ARGUMENT` para dados/nível/formato inválidos.

### Remarks

A origem e o destino não podem se sobrepor. O resultado é válido apenas em SUCCESS; bytes parcialmente escritos em erro não são utilizáveis.

### Example

```c
unsigned char dst[1024];
size_t written = 0;
OPSTATUS status = COMPRESSION_FUNC(Compress)(COMPRESSION_CONST(ZLIB), "hello", 5, dst, sizeof dst, COMPRESSION_CONST(LEVEL_DEFAULT), &written);
```

# Decompress

Decodifica um fluxo DEFLATE/zlib/gzip para buffer do chamador.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(Decompress)(COMPRESSION_TYPE(Format) format, const void *source, size_t size, void *destination, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Compression_Decompress(Compression_Format format, const void *source, size_t size, void *destination, size_t capacity, size_t *outSize);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `source`: ponteiro para dados da origem; NULL só quando size é zero.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).
- `destination`: buffer de destino do chamador.
- `capacity`: capacidade do destino em bytes.
- `outSize`: recebe quantidade de bytes gravados, apenas em SUCCESS.

### Return value

`OPSTATUS`. `INVALID_SEQUENCE` para corrupção, `INCOMPLETE_SEQUENCE` para truncamento e `INSUFFICIENT_SPACE` para destino curto.

### Remarks

Gzip valida CRC e ISIZE de todos os membros concatenados. Raw DEFLATE e zlib exigem um único fluxo completo, sem bytes extras. Entrada vazia é incompleta.

### Example

```c
unsigned char plain[1024];
size_t written = 0;
/* packed / packedSize representam fluxo obtido previamente. */
OPSTATUS status = COMPRESSION_FUNC(Decompress)(COMPRESSION_CONST(GZIP), packed, packedSize, plain, sizeof plain, &written);
```

# CompressAlloc

Aloca um TBuffer contendo os bytes codificados.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(CompressAlloc)(COMPRESSION_TYPE(Format) format, const void *source, size_t size, int level, COMPRESSION_TYPE(TBuffer) *outBuffer);
```

#### Direct form

```c
OPSTATUS Compression_CompressAlloc(Compression_Format format, const void *source, size_t size, int level, Compression_TBuffer *outBuffer);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `source`: ponteiro para dados da origem; NULL só quando size é zero.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).
- `level`: nível de compressão, de `LEVEL_DEFAULT` (-1) a `LEVEL_BEST` (9).
- `outBuffer`: ponteiro para `TBuffer` zerado, que recebe a alocação em SUCCESS.

### Return value

`OPSTATUS`. `OUT_OF_MEMORY` se o buffer não puder ser alocado; propaga os erros de Bound e Compress.

### Remarks

`outBuffer` deve estar zerado e não possuir alocação. Após SUCCESS, BufferFree é obrigatório.

### Example

```c
COMPRESSION_TYPE(TBuffer) packed = {0};
OPSTATUS status = COMPRESSION_FUNC(CompressAlloc)(COMPRESSION_CONST(GZIP), "abc", 3, COMPRESSION_CONST(LEVEL_DEFAULT), &packed);
COMPRESSION_FUNC(BufferFree)(&packed);
```

# DecompressAlloc

Decodifica para um TBuffer alocado, respeitando um orçamento de saída.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(DecompressAlloc)(COMPRESSION_TYPE(Format) format, const void *source, size_t size, size_t maxOutput, COMPRESSION_TYPE(TBuffer) *outBuffer);
```

#### Direct form

```c
OPSTATUS Compression_DecompressAlloc(Compression_Format format, const void *source, size_t size, size_t maxOutput, Compression_TBuffer *outBuffer);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `source`: ponteiro para dados da origem; NULL só quando size é zero.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).
- `maxOutput`: quantidade máxima permitida de bytes descompactados.
- `outBuffer`: ponteiro para `TBuffer` zerado, que recebe a alocação em SUCCESS.

### Return value

`OPSTATUS`. `INSUFFICIENT_SPACE` quando os dados decodificados excedem o limite; `OUT_OF_MEMORY` em falha de alocação.

### Remarks

Cresce a alocação por tentativas, sem exceder `maxOutput`. `maxOutput == 0` admite somente saída vazia; não é uma interface de streaming.

### Example

```c
COMPRESSION_TYPE(TBuffer) plain = {0};
OPSTATUS status = COMPRESSION_FUNC(DecompressAlloc)(COMPRESSION_CONST(ZLIB), packed, packedSize, 1024 * 1024, &plain);
COMPRESSION_FUNC(BufferFree)(&plain);
```

# BufferFree

Libera a memória pertencente a um TBuffer.

### Syntax

#### Macro form

```c
void COMPRESSION_FUNC(BufferFree)(COMPRESSION_TYPE(TBuffer) *buffer);
```

#### Direct form

```c
void Compression_BufferFree(Compression_TBuffer *buffer);
```

### Parameters

- `buffer`: buffer do chamador, liberado e zerado por BufferFree.

### Return value

Não retorna status.

### Remarks

Aceita ponteiro NULL, é idempotente e zera `data` e `size`. Não libera a própria struct.

### Example

```c
COMPRESSION_TYPE(TBuffer) buffer = {0};
COMPRESSION_FUNC(BufferFree)(&buffer);
```


# Checksum package

# CRC32

Calcula ou atualiza CRC32 incremental.

### Syntax

#### Macro form

```c
uint32_t COMPRESSION_FUNC(CRC32)(uint32_t previous, const void *data, size_t size);
```

#### Direct form

```c
uint32_t Compression_CRC32(uint32_t previous, const void *data, size_t size);
```

### Parameters

- `previous`: valor anterior do checksum para atualização incremental.
- `data`: buffer de dados legíveis ou bytes do arquivo ZIP, conforme a operação.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).

### Return value

Retorna o CRC32 de 32 bits, sem OPSTATUS.

### Remarks

Comece com zero; passe o resultado anterior para continuar. Um ponteiro de dados NULL reinicializa o checksum conforme o motor incorporado. Integridade não implica autenticidade.

### Example

```c
uint32_t crc = COMPRESSION_FUNC(CRC32)(0, "hel", 3);
crc = COMPRESSION_FUNC(CRC32)(crc, "lo", 2);
```

# Adler32

Calcula ou atualiza Adler32 incremental.

### Syntax

#### Macro form

```c
uint32_t COMPRESSION_FUNC(Adler32)(uint32_t previous, const void *data, size_t size);
```

#### Direct form

```c
uint32_t Compression_Adler32(uint32_t previous, const void *data, size_t size);
```

### Parameters

- `previous`: valor anterior do checksum para atualização incremental.
- `data`: buffer de dados legíveis ou bytes do arquivo ZIP, conforme a operação.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).

### Return value

Retorna o Adler32 de 32 bits, sem OPSTATUS.

### Remarks

Comece com 1 e continue com o resultado anterior. Dados com tamanho não nulo devem ser legíveis; NULL reinicializa segundo o motor incorporado.

### Example

```c
uint32_t adler = COMPRESSION_FUNC(Adler32)(1, "hel", 3);
adler = COMPRESSION_FUNC(Adler32)(adler, "lo", 2);
```


# Zip package

# ZipOpen

Abre um ZIP que já está inteiro na memória.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipOpen)(COMPRESSION_TYPE(TZipReader) *reader, const void *data, size_t size);
```

#### Direct form

```c
OPSTATUS Compression_ZipOpen(Compression_TZipReader *reader, const void *data, size_t size);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `data`: buffer de dados legíveis ou bytes do arquivo ZIP, conforme a operação.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).

### Return value

`OPSTATUS`. Retorna SUCCESS ou erro de parsing, formato não suportado ou memória.

### Remarks

O reader obtém seu estado nativo, mas **empresta** os bytes da entrada; mantenha o arquivo em memória e inalterado até ZipClose. A entrada não pode ser NULL.

### Example

```c
COMPRESSION_TYPE(TZipReader) reader = {0};
/* archive / archiveSize: bytes do ZIP mantidos pelo chamador. */
OPSTATUS status = COMPRESSION_FUNC(ZipOpen)(&reader, archive, archiveSize);
COMPRESSION_FUNC(ZipClose)(&reader);
```

# ZipClose

Fecha um leitor ZIP e libera seu estado.

### Syntax

#### Macro form

```c
void COMPRESSION_FUNC(ZipClose)(COMPRESSION_TYPE(TZipReader) *reader);
```

#### Direct form

```c
void Compression_ZipClose(Compression_TZipReader *reader);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.

### Return value

Não retorna status.

### Remarks

Não libera o buffer de ZIP emprestado. Aceita NULL e chamadas repetidas.

### Example

```c
COMPRESSION_TYPE(TZipReader) reader = {0};
COMPRESSION_FUNC(ZipClose)(&reader);
```

# ZipCount

Retorna a quantidade de entradas de um ZIP aberto.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipCount)(const COMPRESSION_TYPE(TZipReader) *reader, size_t *outCount);
```

#### Direct form

```c
OPSTATUS Compression_ZipCount(const Compression_TZipReader *reader, size_t *outCount);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `outCount`: parâmetro conforme a assinatura pública.

### Return value

`OPSTATUS`. SUCCESS ou INVALID_ARGUMENT se o reader não está aberto ou saída ausente.

### Remarks

Inclui arquivos e diretórios; outCount é definido em SUCCESS.

### Example

```c
size_t count = 0;
OPSTATUS status = COMPRESSION_FUNC(ZipCount)(&reader, &count);
```

# ZipEntry

Consulta metadados de uma entrada sem descompactar o conteúdo.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipEntry)(const COMPRESSION_TYPE(TZipReader) *reader, size_t index, COMPRESSION_TYPE(TZipEntry) *outEntry);
```

#### Direct form

```c
OPSTATUS Compression_ZipEntry(const Compression_TZipReader *reader, size_t index, Compression_TZipEntry *outEntry);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `index`: índice da entrada ZIP, baseado em zero.
- `outEntry`: recebe os metadados da entrada, apenas em SUCCESS.

### Return value

`OPSTATUS`. `OUT_OF_RANGE` para índice inválido; outros erros são propagados da biblioteca ZIP.

### Remarks

O resultado inclui tamanho compactado, tamanho original, CRC32 e indicadores `directory` e `supported`. Entradas não suportadas ainda podem ser listadas.

### Example

```c
COMPRESSION_TYPE(TZipEntry) entry = {0};
OPSTATUS status = COMPRESSION_FUNC(ZipEntry)(&reader, 0, &entry);
```

# ZipName

Copia o nome de uma entrada para um buffer C.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipName)(const COMPRESSION_TYPE(TZipReader) *reader, size_t index, char *destination, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Compression_ZipName(const Compression_TZipReader *reader, size_t index, char *destination, size_t capacity, size_t *outSize);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `index`: índice da entrada ZIP, baseado em zero.
- `destination`: buffer de destino do chamador.
- `capacity`: capacidade do destino em bytes.
- `outSize`: recebe quantidade de bytes gravados, apenas em SUCCESS.

### Return value

`OPSTATUS`. `INSUFFICIENT_SPACE` para buffer curto, `OUT_OF_RANGE` para índice inválido.

### Remarks

`outSize` recebe o número de bytes **sem** o terminador NUL; a capacidade precisa incluir o NUL. Não converta automaticamente nomes não confiáveis em caminhos de escrita no disco.

### Example

```c
char name[256];
size_t nameSize = 0;
OPSTATUS status = COMPRESSION_FUNC(ZipName)(&reader, 0, name, sizeof name, &nameSize);
```

# ZipFind

Procura uma entrada ZIP pelo nome.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipFind)(const COMPRESSION_TYPE(TZipReader) *reader, const char *name, size_t *outIndex);
```

#### Direct form

```c
OPSTATUS Compression_ZipFind(const Compression_TZipReader *reader, const char *name, size_t *outIndex);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `name`: nome da entrada ZIP, terminada em NUL.
- `outIndex`: recebe o índice encontrado.

### Return value

`OPSTATUS`. `NOT_FOUND` quando não existe entrada com o nome, ou erro de argumento/reader.

### Remarks

Comparação de nomes é sensível a maiúsculas/minúsculas. A busca não normaliza nem valida caminhos.

### Example

```c
size_t index = 0;
OPSTATUS status = COMPRESSION_FUNC(ZipFind)(&reader, "docs/readme.txt", &index);
```

# ZipExtract

Extrai uma entrada ZIP para buffer fornecido.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipExtract)(const COMPRESSION_TYPE(TZipReader) *reader, size_t index, void *destination, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Compression_ZipExtract(const Compression_TZipReader *reader, size_t index, void *destination, size_t capacity, size_t *outSize);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `index`: índice da entrada ZIP, baseado em zero.
- `destination`: buffer de destino do chamador.
- `capacity`: capacidade do destino em bytes.
- `outSize`: recebe quantidade de bytes gravados, apenas em SUCCESS.

### Return value

`OPSTATUS`. `NOT_SUPPORTED` para método/encriptação indisponíveis, `INSUFFICIENT_SPACE` para destino curto, ou erro de conteúdo.

### Remarks

Verifica integridade CRC32. O destino não pode se sobrepor aos bytes do ZIP emprestados pelo reader. Bytes escritos em caso de erro não devem ser usados.

### Example

```c
unsigned char bytes[4096];
size_t written = 0;
OPSTATUS status = COMPRESSION_FUNC(ZipExtract)(&reader, 0, bytes, sizeof bytes, &written);
```

# ZipExtractAlloc

Extrai uma entrada ZIP para buffer alocado.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipExtractAlloc)(const COMPRESSION_TYPE(TZipReader) *reader, size_t index, size_t maxOutput, COMPRESSION_TYPE(TBuffer) *outBuffer);
```

#### Direct form

```c
OPSTATUS Compression_ZipExtractAlloc(const Compression_TZipReader *reader, size_t index, size_t maxOutput, Compression_TBuffer *outBuffer);
```

### Parameters

- `reader`: leitor ZIP inicializado por ZipOpen, exceto em ZipOpen e ZipClose.
- `index`: índice da entrada ZIP, baseado em zero.
- `maxOutput`: quantidade máxima permitida de bytes descompactados.
- `outBuffer`: ponteiro para `TBuffer` zerado, que recebe a alocação em SUCCESS.

### Return value

`OPSTATUS`. `INSUFFICIENT_SPACE` se o tamanho anunciado excede maxOutput; `OUT_OF_MEMORY` e outros erros podem ocorrer.

### Remarks

Verifica previamente o tamanho anunciado contra `maxOutput` e valida o conteúdo ao extrair. Requer saída zerada; libere-a com BufferFree após SUCCESS.

### Example

```c
COMPRESSION_TYPE(TBuffer) content = {0};
OPSTATUS status = COMPRESSION_FUNC(ZipExtractAlloc)(&reader, 0, 16 * 1024 * 1024, &content);
COMPRESSION_FUNC(BufferFree)(&content);
```

# ZipCreate

Inicia criação de um ZIP em memória.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipCreate)(COMPRESSION_TYPE(TZipWriter) *writer);
```

#### Direct form

```c
OPSTATUS Compression_ZipCreate(Compression_TZipWriter *writer);
```

### Parameters

- `writer`: writer ZIP ativo, exceto em ZipCreate e ZipDiscard.

### Return value

`OPSTATUS`. SUCCESS, INVALID_ARGUMENT ou erro na alocação/inicialização.

### Remarks

O writer precisa estar zerado e sem arquivo ativo. Finalize com ZipFinish ou descarte com ZipDiscard.

### Example

```c
COMPRESSION_TYPE(TZipWriter) writer = {0};
OPSTATUS status = COMPRESSION_FUNC(ZipCreate)(&writer);
COMPRESSION_FUNC(ZipDiscard)(&writer);
```

# ZipAdd

Adiciona arquivo ou diretório a um ZIP em formação.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipAdd)(COMPRESSION_TYPE(TZipWriter) *writer, const char *name, const void *data, size_t size, int level);
```

#### Direct form

```c
OPSTATUS Compression_ZipAdd(Compression_TZipWriter *writer, const char *name, const void *data, size_t size, int level);
```

### Parameters

- `writer`: writer ZIP ativo, exceto em ZipCreate e ZipDiscard.
- `name`: nome da entrada ZIP, terminada em NUL.
- `data`: buffer de dados legíveis ou bytes do arquivo ZIP, conforme a operação.
- `size`: tamanho de entrada em bytes (ou bytes de um arquivo/entrada ZIP, conforme a função).
- `level`: nível de compressão, de `LEVEL_DEFAULT` (-1) a `LEVEL_BEST` (9).

### Return value

`OPSTATUS`. `INVALID_ARGUMENT` para nome/dados/nível inválidos, além de erros internos ZIP.

### Remarks

Nomes devem ser relativos e portáteis: sem caminho absoluto, backslash, dois-pontos, componente vazio, `.` ou `..`. Diretórios terminados em `/` devem ter tamanho zero. Duplicatas são permitidas. Após falha, descarte o writer.

### Example

```c
COMPRESSION_TYPE(TZipWriter) writer = {0};
OPSTATUS status = COMPRESSION_FUNC(ZipCreate)(&writer);
if (status == STATUS_CONST(SUCCESS)) {
    status = COMPRESSION_FUNC(ZipAdd)(&writer, "hello.txt", "hello", 5, COMPRESSION_CONST(LEVEL_DEFAULT));
}
COMPRESSION_FUNC(ZipDiscard)(&writer);
```

# ZipFinish

Finaliza o ZIP em memória e transfere a propriedade do arquivo ao chamador.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(ZipFinish)(COMPRESSION_TYPE(TZipWriter) *writer, COMPRESSION_TYPE(TBuffer) *outBuffer);
```

#### Direct form

```c
OPSTATUS Compression_ZipFinish(Compression_TZipWriter *writer, Compression_TBuffer *outBuffer);
```

### Parameters

- `writer`: writer ZIP ativo, exceto em ZipCreate e ZipDiscard.
- `outBuffer`: ponteiro para `TBuffer` zerado, que recebe a alocação em SUCCESS.

### Return value

`OPSTATUS`. SUCCESS ou erro de estado, escrita, memória ou engine ZIP.

### Remarks

Em SUCCESS o writer é limpo e o arquivo gerado fica em outBuffer. Requer buffer zerado. Após erro, descarte o writer.

### Example

```c
COMPRESSION_TYPE(TBuffer) archive = {0};
OPSTATUS status = COMPRESSION_FUNC(ZipFinish)(&writer, &archive);
/* Use archive somente em SUCCESS. */
COMPRESSION_FUNC(BufferFree)(&archive);
```

# ZipDiscard

Descarta um ZIP ainda em construção.

### Syntax

#### Macro form

```c
void COMPRESSION_FUNC(ZipDiscard)(COMPRESSION_TYPE(TZipWriter) *writer);
```

#### Direct form

```c
void Compression_ZipDiscard(Compression_TZipWriter *writer);
```

### Parameters

- `writer`: writer ZIP ativo, exceto em ZipCreate e ZipDiscard.

### Return value

Não retorna status.

### Remarks

Libera o estado e zera o handle; aceita NULL e chamada repetida. Não libera um TBuffer já transferido por ZipFinish.

### Example

```c
COMPRESSION_FUNC(ZipDiscard)(&writer);
```


# File package

# CompressFile

Comprime um arquivo para outro caminho com salvamento atômico.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(CompressFile)(COMPRESSION_TYPE(Format) format, const char *sourcePath, const char *destinationPath, int level, size_t maxInput);
```

#### Direct form

```c
OPSTATUS Compression_CompressFile(Compression_Format format, const char *sourcePath, const char *destinationPath, int level, size_t maxInput);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `sourcePath`: caminho do arquivo fonte, string terminada em NUL.
- `destinationPath`: caminho do arquivo destino, string terminada em NUL.
- `level`: nível de compressão, de `LEVEL_DEFAULT` (-1) a `LEVEL_BEST` (9).
- `maxInput`: limite de bytes permitidos para ler da fonte.

### Return value

`OPSTATUS`. Propaga erros de File, codec, orçamento e salvamento.

### Remarks

Lê arquivo regular/seekable, recusa crescimento detectado além do tamanho inicial e limita o input. Fonte e destino podem ser o mesmo caminho. Não cria árvore de diretórios.

### Example

```c
OPSTATUS status = COMPRESSION_FUNC(CompressFile)(COMPRESSION_CONST(GZIP), "input.bin", "input.gz", COMPRESSION_CONST(LEVEL_DEFAULT), 16 * 1024 * 1024);
```

# DecompressFile

Descomprime um arquivo com limite de entrada e saída e gravação atômica.

### Syntax

#### Macro form

```c
OPSTATUS COMPRESSION_FUNC(DecompressFile)(COMPRESSION_TYPE(Format) format, const char *sourcePath, const char *destinationPath, size_t maxInput, size_t maxOutput);
```

#### Direct form

```c
OPSTATUS Compression_DecompressFile(Compression_Format format, const char *sourcePath, const char *destinationPath, size_t maxInput, size_t maxOutput);
```

### Parameters

- `format`: formato `COMPRESSION_CONST(DEFLATE)`, `COMPRESSION_CONST(ZLIB)` ou `COMPRESSION_CONST(GZIP)`.
- `sourcePath`: caminho do arquivo fonte, string terminada em NUL.
- `destinationPath`: caminho do arquivo destino, string terminada em NUL.
- `maxInput`: limite de bytes permitidos para ler da fonte.
- `maxOutput`: quantidade máxima permitida de bytes descompactados.

### Return value

`OPSTATUS`. Propaga erros de File e codec, inclusive limite de saída excedido.

### Remarks

O arquivo inteiro é processado em memória. Caso a decodificação falhe, o destino não é substituído. Fonte e destino podem ter o mesmo caminho.

### Example

```c
OPSTATUS status = COMPRESSION_FUNC(DecompressFile)(COMPRESSION_CONST(GZIP), "input.gz", "output.bin", 16 * 1024 * 1024, 64 * 1024 * 1024);
```

# See also

- [File](File.md) — arquivo, diretórios e salvamento atômico.
- [Error](Error.md) — `STATUS_CONST` e tratamento de `OPSTATUS`.
- [Bit](Bit.md) — estilo da referência de API.
