#pragma once
#ifndef COMPRESSION_DISABLE_FUNCTION_TABLE
#define COMPRESSION_FUNCTION_TABLE_STRUCT() \
  typedef struct COMPRESSION_TYPE(FunctionTable) { \
    OPSTATUS (*Bound)(COMPRESSION_TYPE(Format), size_t, size_t *); \
    OPSTATUS (*Compress)(COMPRESSION_TYPE(Format), const void *, size_t, void *, size_t, int, size_t *); \
    OPSTATUS (*Decompress)(COMPRESSION_TYPE(Format), const void *, size_t, void *, size_t, size_t *); \
    OPSTATUS (*CompressAlloc)(COMPRESSION_TYPE(Format), const void *, size_t, int, COMPRESSION_TYPE(TBuffer) *); \
    OPSTATUS (*DecompressAlloc)(COMPRESSION_TYPE(Format), const void *, size_t, size_t, COMPRESSION_TYPE(TBuffer) *); \
    void (*BufferFree)(COMPRESSION_TYPE(TBuffer) *); \
    uint32_t (*CRC32)(uint32_t, const void *, size_t); \
    uint32_t (*Adler32)(uint32_t, const void *, size_t); \
    OPSTATUS (*ZipOpen)(COMPRESSION_TYPE(TZipReader) *, const void *, size_t); \
    void (*ZipClose)(COMPRESSION_TYPE(TZipReader) *); \
    OPSTATUS (*ZipCount)(const COMPRESSION_TYPE(TZipReader) *, size_t *); \
    OPSTATUS (*ZipEntry)(const COMPRESSION_TYPE(TZipReader) *, size_t, COMPRESSION_TYPE(TZipEntry) *); \
    OPSTATUS (*ZipName)(const COMPRESSION_TYPE(TZipReader) *, size_t, char *, size_t, size_t *); \
    OPSTATUS (*ZipFind)(const COMPRESSION_TYPE(TZipReader) *, const char *, size_t *); \
    OPSTATUS (*ZipExtract)(const COMPRESSION_TYPE(TZipReader) *, size_t, void *, size_t, size_t *); \
    OPSTATUS (*ZipExtractAlloc)(const COMPRESSION_TYPE(TZipReader) *, size_t, size_t, COMPRESSION_TYPE(TBuffer) *); \
    OPSTATUS (*ZipCreate)(COMPRESSION_TYPE(TZipWriter) *); \
    OPSTATUS (*ZipAdd)(COMPRESSION_TYPE(TZipWriter) *, const char *, const void *, size_t, int); \
    OPSTATUS (*ZipFinish)(COMPRESSION_TYPE(TZipWriter) *, COMPRESSION_TYPE(TBuffer) *); \
    OPSTATUS (*CompressFile)(COMPRESSION_TYPE(Format), const char *, const char *, int, size_t); \
    OPSTATUS (*DecompressFile)(COMPRESSION_TYPE(Format), const char *, const char *, size_t, size_t); \
    void (*ZipDiscard)(COMPRESSION_TYPE(TZipWriter) *); \
  } COMPRESSION_TYPE(FunctionTable);
#define COMPRESSION_FUNCTION_TABLE_INSTANCE() \
  static const COMPRESSION_TYPE(FunctionTable) COMPRESSION_NS(Functions) = { \
    COMPRESSION_FUNC(Bound), COMPRESSION_FUNC(Compress), COMPRESSION_FUNC(Decompress), \
    COMPRESSION_FUNC(CompressAlloc), COMPRESSION_FUNC(DecompressAlloc), COMPRESSION_FUNC(BufferFree), \
    COMPRESSION_FUNC(CRC32), COMPRESSION_FUNC(Adler32), \
    COMPRESSION_FUNC(ZipOpen), COMPRESSION_FUNC(ZipClose), COMPRESSION_FUNC(ZipCount), \
    COMPRESSION_FUNC(ZipEntry), COMPRESSION_FUNC(ZipName), COMPRESSION_FUNC(ZipFind), \
    COMPRESSION_FUNC(ZipExtract), COMPRESSION_FUNC(ZipExtractAlloc), COMPRESSION_FUNC(ZipCreate), \
    COMPRESSION_FUNC(ZipAdd), COMPRESSION_FUNC(ZipFinish), \
    COMPRESSION_FUNC(CompressFile), COMPRESSION_FUNC(DecompressFile), COMPRESSION_FUNC(ZipDiscard) \
  };
COMPRESSION_FUNCTION_TABLE_STRUCT()
COMPRESSION_FUNCTION_TABLE_INSTANCE()
#endif
