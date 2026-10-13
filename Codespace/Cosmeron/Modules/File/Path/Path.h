#pragma once
#include "../File.space"

#define FILE_PATH_JOIN_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Path_Join)( \
      const char *base, const char *part, \
      char *output, size_t capacity, size_t *outRequired)
#define FILE_PATH_NORMALIZE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Path_Normalize)( \
      const char *path, char *output, \
      size_t capacity, size_t *outRequired)
#define FILE_PATH_PARENT_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Path_Parent)( \
      const char *path, char *output, \
      size_t capacity, size_t *outRequired)
#define FILE_PATH_NAME_PROTOTYPE \
  static inline const char * FILE_FUNC(Path_Name)( \
      const char *path)
#define FILE_PATH_EXTENSION_PROTOTYPE \
  static inline const char * FILE_FUNC(Path_Extension)( \
      const char *path)
#define FILE_PATH_IS_ABSOLUTE_PROTOTYPE \
  static inline bool FILE_FUNC(Path_IsAbsolute)( \
      const char *path)
#define FILE_PATH_RESOLVE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Path_Resolve)( \
      const char *path, char *output, \
      size_t capacity, size_t *outRequired)

FILE_PATH_JOIN_PROTOTYPE;
FILE_PATH_NORMALIZE_PROTOTYPE;
FILE_PATH_PARENT_PROTOTYPE;
FILE_PATH_NAME_PROTOTYPE;
FILE_PATH_EXTENSION_PROTOTYPE;
FILE_PATH_IS_ABSOLUTE_PROTOTYPE;
FILE_PATH_RESOLVE_PROTOTYPE;

#include "Impl/Path.impl"
