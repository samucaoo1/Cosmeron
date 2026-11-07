#pragma once
#include "../File.space"
#include "../Path/Path.h"
#include "../Entry/Entry.h"

#define FILE_DIRECTORY_CREATE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Directory_Create)( \
      const char *path)
#define FILE_DIRECTORY_CREATE_ALL_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Directory_CreateAll)( \
      const char *path)
#define FILE_DIRECTORY_OPEN_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Directory_Open)( \
      FILE_TYPE(TDirectory) *directory, const char *path)
#define FILE_DIRECTORY_NEXT_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Directory_Next)( \
      FILE_TYPE(TDirectory) *directory, \
      FILE_TYPE(TFileEntry) *outEntry, bool *outFound)
#define FILE_DIRECTORY_CLOSE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Directory_Close)( \
      FILE_TYPE(TDirectory) *directory)
#define FILE_DIRECTORY_REMOVE_PROTOTYPE \
  static inline OPSTATUS FILE_FUNC(Directory_Remove)( \
      const char *path)

FILE_DIRECTORY_CREATE_PROTOTYPE;
FILE_DIRECTORY_CREATE_ALL_PROTOTYPE;
FILE_DIRECTORY_OPEN_PROTOTYPE;
FILE_DIRECTORY_NEXT_PROTOTYPE;
FILE_DIRECTORY_CLOSE_PROTOTYPE;
FILE_DIRECTORY_REMOVE_PROTOTYPE;

#include "Impl/Directory.impl"
