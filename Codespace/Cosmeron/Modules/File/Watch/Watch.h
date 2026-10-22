#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Module/Object.h"
#include "../File.space"

#if defined(FILE_DISABLE_FUNCTION_TABLE) ||                                    \
    defined(FILE_WATCH_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

#define FILE_WATCH_CMOD WATCH
#define FILE_WATCH_NS(NAME) GNS2(FILE_NS(Watch), NAME)
#define FILE_WATCH_CNS(NAME) CNS3(LIB_PREFIX_CONST(FILE_CMOD), FILE_WATCH_CMOD, NAME)
#define FILE_WATCH_INS(NAME) GNS3(FILE_NS(Watch), Internal, NAME)

#define FILE_WATCH_TYPE(NAME) FILE_WATCH_NS(NAME)
#define FILE_WATCH_FUNC(NAME) GNS2(FILE_WATCH_TYPE(TFileWatcher), NAME)
#define FILE_WATCH_CONST(NAME) FILE_WATCH_CNS(NAME)

#define FILE_WATCH_MAX_PATH_LEN 256U

/* Watch event bitmask flags */
typedef enum FILE_WATCH_TYPE(Event) {
  FILE_WATCH_CONST(EVENT_NONE) = 0U,
  FILE_WATCH_CONST(EVENT_CREATED) = 1U << 0,
  FILE_WATCH_CONST(EVENT_MODIFIED) = 1U << 1,
  FILE_WATCH_CONST(EVENT_DELETED) = 1U << 2,
  FILE_WATCH_CONST(EVENT_RENAMED) = 1U << 3,
  FILE_WATCH_CONST(EVENT_ALL) = 0x0FU
} FILE_WATCH_TYPE(Event);

/* Event report structure */
typedef struct FILE_WATCH_TYPE(EventData) {
  uint32_t events;
  int32_t watchId;
  char name[FILE_WATCH_MAX_PATH_LEN];
} FILE_WATCH_TYPE(EventData);

typedef FILE_WATCH_TYPE(EventData) TFileWatchEvent;

/* Object-oriented Filesystem Watcher structure definition */
OBJECT(FILE_WATCH_TYPE(TFileWatcher),
       FIELDS(intptr_t handle; bool isRunning; uint32_t watchCount;),
       METHOD(OPSTATUS, Start, ()), METHOD(OPSTATUS, Stop, ()),
       METHOD(OPSTATUS, Add,
              (const char *path, uint32_t eventMask, int32_t *outWatchId)),
       METHOD(OPSTATUS, Remove, (int32_t watchId)),
       METHOD(OPSTATUS, Poll,
              (FILE_WATCH_TYPE(EventData) * outEvents, size_t maxEvents,
               size_t *outCount, int32_t timeoutMs)),
       CONST_METHOD(bool, IsRunning, ()),
       CONST_METHOD(uint32_t, WatchCount, ()));

typedef FILE_WATCH_TYPE(TFileWatcher) TFileWatcher;

#include "Impl/Watch.impl"
