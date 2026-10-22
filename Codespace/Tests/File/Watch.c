#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../../Cosmeron/Modules/File/File.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

int main(void) {
  char root[128], filePath[256];
  size_t required;
#ifdef _WIN32
  unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
  unsigned long pid = (unsigned long)getpid();
#endif
  assert(snprintf(root, sizeof(root), "cosmeron-watch-test-%lu", pid) > 0);

  OK(File_Directory_Create(root));
  OK(File_Path_Join(root, "sample.txt", filePath, sizeof(filePath), &required));

  /* ------------------------------------------------------------
   * 1. Lifecycle and Object API
   * ------------------------------------------------------------ */
  TFileWatcher watcher = {0};
  OK(FILE_WATCH_FUNC(Init)(&watcher));
  assert(watcher.api != NULL);
  assert(!watcher.api->IsRunning(&watcher));
  assert(watcher.api->WatchCount(&watcher) == 0);

  OK(watcher.api->Start(&watcher));
  assert(watcher.api->IsRunning(&watcher));

  /* ------------------------------------------------------------
   * 2. Add Watch
   * ------------------------------------------------------------ */
  int32_t watchId = -1;
  OK(watcher.api->Add(&watcher, root, FILE_WATCH_CONST(EVENT_ALL), &watchId));
  assert(watchId >= 0);
  assert(watcher.api->WatchCount(&watcher) == 1);

  /* ------------------------------------------------------------
   * 3. Trigger filesystem event and poll
   * ------------------------------------------------------------ */
  OK(File_Save(filePath, "test payload", 12));

  TFileWatchEvent events[4];
  size_t eventCount = 0;
  OPSTATUS pollStatus = watcher.api->Poll(&watcher, events, 4, &eventCount, 500);

#if OS_LINUX
  OK(pollStatus);
  assert(eventCount > 0);
  assert(events[0].watchId == watchId);
  /* The event should report creation or modification */
  assert((events[0].events & (FILE_WATCH_CONST(EVENT_CREATED) |
                              FILE_WATCH_CONST(EVENT_MODIFIED))) != 0);
  assert(strcmp(events[0].name, "sample.txt") == 0);
#else
  (void)pollStatus;
#endif

  /* ------------------------------------------------------------
   * 4. Remove Watch and Stop
   * ------------------------------------------------------------ */
  OK(watcher.api->Remove(&watcher, watchId));
  assert(watcher.api->WatchCount(&watcher) == 0);

  OK(watcher.api->Stop(&watcher));
  assert(!watcher.api->IsRunning(&watcher));

  /* Cleanup */
  File_Remove(filePath);
  File_Directory_Remove(root);

  return 0;
}
