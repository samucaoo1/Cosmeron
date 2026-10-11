#pragma once
#include "../File.space"
#include "../Entry/Entry.h"

#define FILE_LOAD_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Load)( \
      const char *path, void **outData, size_t *outSize)
#define FILE_LOAD_TEXT_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(LoadText)( \
      const char *path, char **outText, size_t *outSize)
#define FILE_SAVE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Save)( \
      const char *path, const void *data, size_t size)
#define FILE_APPEND_CONTENT_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Append)( \
      const char *path, const void *data, size_t size)
#define FILE_SAVE_ATOMIC_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(SaveAtomic)( \
      const char *path, const void *data, size_t size)

FILE_LOAD_PROTOTYPE;
FILE_LOAD_TEXT_PROTOTYPE;
FILE_SAVE_PROTOTYPE;
FILE_APPEND_CONTENT_PROTOTYPE;
FILE_SAVE_ATOMIC_PROTOTYPE;

#include "Impl/Content.impl"
