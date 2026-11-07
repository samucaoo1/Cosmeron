#pragma once
#include "../File.space"
#include "../Stream/Stream.h"

#define FILE_INFO_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Info)( \
      const char *path, bool followLinks, FILE_TYPE(TFileInfo) *outInfo)
#define FILE_EXISTS_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Exists)( \
      const char *path, bool *outExists)
#define FILE_COPY_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Copy)( \
      const char *source, const char *destination, bool replace)
#define FILE_MOVE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Move)( \
      const char *source, const char *destination, bool replace)
#define FILE_REMOVE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Remove)( \
      const char *path)

FILE_INFO_PROTOTYPE;
FILE_EXISTS_PROTOTYPE;
FILE_COPY_PROTOTYPE;
FILE_MOVE_PROTOTYPE;
FILE_REMOVE_PROTOTYPE;

#include "Impl/Entry.impl"
