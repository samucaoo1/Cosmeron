#pragma once
#include "../Checksum/Checksum.h"

#define COMPRESSION_BOUND_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(Bound)( \
      COMPRESSION_TYPE(Format) format, \
      size_t size, \
      size_t *outBound)
#define COMPRESSION_COMPRESS_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(Compress)( \
      COMPRESSION_TYPE(Format) format, \
      const void *source, \
      size_t size, \
      void *destination, \
      size_t capacity, \
      int level, \
      size_t *outSize)
#define COMPRESSION_DECOMPRESS_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(Decompress)( \
      COMPRESSION_TYPE(Format) format, \
      const void *source, \
      size_t size, \
      void *destination, \
      size_t capacity, \
      size_t *outSize)
#define COMPRESSION_COMPRESS_ALLOC_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(CompressAlloc)( \
      COMPRESSION_TYPE(Format) format, \
      const void *source, \
      size_t size, \
      int level, \
      COMPRESSION_TYPE(TBuffer) *outBuffer)
#define COMPRESSION_DECOMPRESS_ALLOC_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(DecompressAlloc)( \
      COMPRESSION_TYPE(Format) format, \
      const void *source, \
      size_t size, \
      size_t maxOutput, \
      COMPRESSION_TYPE(TBuffer) *outBuffer)
#define COMPRESSION_BUFFER_FREE_PROTOTYPE \
  static inline void COMPRESSION_FUNC(BufferFree)( \
      COMPRESSION_TYPE(TBuffer) *buffer)

COMPRESSION_BOUND_PROTOTYPE;
COMPRESSION_COMPRESS_PROTOTYPE;
COMPRESSION_DECOMPRESS_PROTOTYPE;
COMPRESSION_COMPRESS_ALLOC_PROTOTYPE;
COMPRESSION_DECOMPRESS_ALLOC_PROTOTYPE;
COMPRESSION_BUFFER_FREE_PROTOTYPE;

#include "Impl/Codec.impl"
