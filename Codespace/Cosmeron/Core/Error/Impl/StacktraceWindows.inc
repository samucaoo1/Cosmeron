#pragma once

PANIC_STACKTRACE_PROTOTYPE {
  void *frames[32];
  USHORT frameCount =
      CaptureStackBackTrace(0, (DWORD)(sizeof(frames) / sizeof(frames[0])),
                            frames, NULL);

  if (frameCount == 0) {
    fprintf(stderr, "stacktrace unavailable\n");
    return;
  }

  for (USHORT i = 0; i < frameCount; ++i)
    fprintf(stderr, "%02u  %p\n", (unsigned)i, frames[i]);
}
/* EOF */
