#pragma once

PANIC_STACKTRACE_PROTOTYPE {
  void *frames[32];
  int frameCount = backtrace(frames, 32);
  char **symbols = backtrace_symbols(frames, frameCount);

  if (!symbols) {
    for (int i = 0; i < frameCount; ++i)
      fprintf(stderr, "%02d  %p\n", i, frames[i]);
    return;
  }

  for (int i = 0; i < frameCount; ++i)
    fprintf(stderr, "%02d  %s\n", i, symbols[i]);

  free(symbols);
}
/* EOF */
