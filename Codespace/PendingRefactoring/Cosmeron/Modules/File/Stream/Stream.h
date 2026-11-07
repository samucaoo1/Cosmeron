#pragma once
#include "../File.space"

#define FILE_OPEN_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Open)( \
      FILE_TYPE(TFile) *file, const char *path, uint32_t flags)
#define FILE_CLOSE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Close)( \
      FILE_TYPE(TFile) *file)
#define FILE_READ_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Read)( \
      FILE_TYPE(TFile) *file, void *buffer, \
      size_t capacity, size_t *outRead)
#define FILE_WRITE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Write)( \
      FILE_TYPE(TFile) *file, const void *buffer, \
      size_t size, size_t *outWritten)
#define FILE_SEEK_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Seek)( \
      FILE_TYPE(TFile) *file, int64_t offset, FILE_TYPE(SeekOrigin) origin)
#define FILE_TELL_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Tell)( \
      FILE_TYPE(TFile) *file, uint64_t *outPosition)
#define FILE_SIZE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Size)( \
      FILE_TYPE(TFile) *file, uint64_t *outSize)
#define FILE_TRUNCATE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Truncate)( \
      FILE_TYPE(TFile) *file, uint64_t size)
#define FILE_FLUSH_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Flush)( \
      FILE_TYPE(TFile) *file)
#define FILE_SYNC_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Sync)( \
      FILE_TYPE(TFile) *file)

FILE_OPEN_PROTOTYPE;
FILE_CLOSE_PROTOTYPE;
FILE_READ_PROTOTYPE;
FILE_WRITE_PROTOTYPE;
FILE_SEEK_PROTOTYPE;
FILE_TELL_PROTOTYPE;
FILE_SIZE_PROTOTYPE;
FILE_TRUNCATE_PROTOTYPE;
FILE_FLUSH_PROTOTYPE;
FILE_SYNC_PROTOTYPE;

#include "Impl/Stream.impl"
