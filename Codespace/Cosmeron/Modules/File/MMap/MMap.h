#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Module/Object.h"
#include "../File.space"
#include "../Stream/Stream.h"

#if defined(FILE_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(FILE_MMAP_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

#ifdef MMAP
#undef MMAP
#endif

#define FILE_MMAP_CMOD MMAP
#define FILE_MMAP_NS(NAME) GNS2(FILE_NS(MMap), NAME)
#define FILE_MMAP_CNS(NAME) CNS3(LIB_PREFIX_CONST(FILE_CMOD), FILE_MMAP_CMOD, NAME)
#define FILE_MMAP_INS(NAME) GNS3(FILE_NS(MMap), Internal, NAME)

#define FILE_MMAP_TYPE(NAME) FILE_MMAP_NS(NAME)
#define FILE_MMAP_FUNC(NAME) GNS2(FILE_MMAP_TYPE(TMemoryMappedFile), NAME)
#define FILE_MMAP_CONST(NAME) FILE_MMAP_CNS(NAME)

/* Protection and mapping access flags */
typedef enum FILE_MMAP_TYPE(Protection) {
  FILE_MMAP_CONST(READ) = 1u << 0,
  FILE_MMAP_CONST(WRITE) = 1u << 1,
  FILE_MMAP_CONST(READ_WRITE) = (1u << 0) | (1u << 1)
} FILE_MMAP_TYPE(Protection);

typedef enum FILE_MMAP_TYPE(SyncMode) {
  FILE_MMAP_CONST(SYNC_ASYNC) = 1u << 0,
  FILE_MMAP_CONST(SYNC_SYNC) = 1u << 1
} FILE_MMAP_TYPE(SyncMode);

/* Object-oriented Memory-Mapped File structure definition */
OBJECT(FILE_MMAP_TYPE(TMemoryMappedFile),
       FIELDS(void *data; size_t size; uint64_t offset; uint32_t flags;
              intptr_t fileDescriptor; intptr_t mapHandle; bool isMapped;),
       METHOD(OPSTATUS, Open,
              (const char *path, uint32_t protection, uint64_t offset,
               size_t length)),
       METHOD(OPSTATUS, OpenFile,
              (FILE_TYPE(TFile) * file, uint32_t protection, uint64_t offset,
               size_t length)),
       METHOD(OPSTATUS, Close, ()),
       METHOD(OPSTATUS, Sync, (uint32_t syncMode)),
       METHOD(OPSTATUS, SyncRange,
              (size_t offset, size_t length, uint32_t syncMode)),
       METHOD(OPSTATUS, Lock, ()),
       METHOD(OPSTATUS, LockRange, (size_t offset, size_t length)),
       METHOD(OPSTATUS, Unlock, ()),
       METHOD(OPSTATUS, UnlockRange, (size_t offset, size_t length)),
       CONST_METHOD(void *, Data, ()),
       CONST_METHOD(size_t, Size, ()),
       CONST_METHOD(uint64_t, Offset, ()),
       CONST_METHOD(bool, IsMapped, ()),
       CONST_METHOD(bool, IsWritable, ()));

typedef FILE_MMAP_TYPE(TMemoryMappedFile) TMemoryMappedFile;

#include "Impl/MMap.impl"

/* EOF */
