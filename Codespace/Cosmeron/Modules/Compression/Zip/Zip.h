#pragma once
#include "../Codec/Codec.h"

#define COMPRESSION_ZIP_OPEN_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipOpen)( \
      COMPRESSION_TYPE(TZipReader) *reader, \
      const void *data, \
      size_t size)
#define COMPRESSION_ZIP_CLOSE_PROTOTYPE \
  static inline void COMPRESSION_FUNC(ZipClose)( \
      COMPRESSION_TYPE(TZipReader) *reader)
#define COMPRESSION_ZIP_COUNT_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipCount)( \
      const COMPRESSION_TYPE(TZipReader) *reader, \
      size_t *outCount)
#define COMPRESSION_ZIP_ENTRY_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipEntry)( \
      const COMPRESSION_TYPE(TZipReader) *reader, \
      size_t index, \
      COMPRESSION_TYPE(TZipEntry) *outEntry)
#define COMPRESSION_ZIP_NAME_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipName)( \
      const COMPRESSION_TYPE(TZipReader) *reader, \
      size_t index, \
      char *destination, \
      size_t capacity, \
      size_t *outSize)
#define COMPRESSION_ZIP_FIND_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipFind)( \
      const COMPRESSION_TYPE(TZipReader) *reader, \
      const char *name, \
      size_t *outIndex)
#define COMPRESSION_ZIP_EXTRACT_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipExtract)( \
      const COMPRESSION_TYPE(TZipReader) *reader, \
      size_t index, \
      void *destination, \
      size_t capacity, \
      size_t *outSize)
#define COMPRESSION_ZIP_EXTRACT_ALLOC_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipExtractAlloc)( \
      const COMPRESSION_TYPE(TZipReader) *reader, \
      size_t index, \
      size_t maxOutput, \
      COMPRESSION_TYPE(TBuffer) *outBuffer)
#define COMPRESSION_ZIP_CREATE_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipCreate)( \
      COMPRESSION_TYPE(TZipWriter) *writer)
#define COMPRESSION_ZIP_ADD_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipAdd)( \
      COMPRESSION_TYPE(TZipWriter) *writer, \
      const char *name, \
      const void *data, \
      size_t size, \
      int level)
#define COMPRESSION_ZIP_FINISH_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(ZipFinish)( \
      COMPRESSION_TYPE(TZipWriter) *writer, \
      COMPRESSION_TYPE(TBuffer) *outBuffer)
#define COMPRESSION_ZIP_DISCARD_PROTOTYPE \
  static inline void COMPRESSION_FUNC(ZipDiscard)( \
      COMPRESSION_TYPE(TZipWriter) *writer)

COMPRESSION_ZIP_OPEN_PROTOTYPE;
COMPRESSION_ZIP_CLOSE_PROTOTYPE;
COMPRESSION_ZIP_COUNT_PROTOTYPE;
COMPRESSION_ZIP_ENTRY_PROTOTYPE;
COMPRESSION_ZIP_NAME_PROTOTYPE;
COMPRESSION_ZIP_FIND_PROTOTYPE;
COMPRESSION_ZIP_EXTRACT_PROTOTYPE;
COMPRESSION_ZIP_EXTRACT_ALLOC_PROTOTYPE;
COMPRESSION_ZIP_CREATE_PROTOTYPE;
COMPRESSION_ZIP_ADD_PROTOTYPE;
COMPRESSION_ZIP_FINISH_PROTOTYPE;
COMPRESSION_ZIP_DISCARD_PROTOTYPE;

#include "Impl/Zip.impl"
