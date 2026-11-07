#pragma once
#include "../Codec/Codec.h"
#include "../../File/File.h"
#define COMPRESSION_COMPRESS_FILE_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(CompressFile)( \
      COMPRESSION_TYPE(Format) format, \
      const char *sourcePath, const char *destinationPath, \
      int level, size_t maxInput)
#define COMPRESSION_DECOMPRESS_FILE_PROTOTYPE \
  static inline OPSTATUS COMPRESSION_FUNC(DecompressFile)( \
      COMPRESSION_TYPE(Format) format, \
      const char *sourcePath, const char *destinationPath, \
      size_t maxInput, size_t maxOutput)
COMPRESSION_COMPRESS_FILE_PROTOTYPE;
COMPRESSION_DECOMPRESS_FILE_PROTOTYPE;
#include "Impl/File.impl"
