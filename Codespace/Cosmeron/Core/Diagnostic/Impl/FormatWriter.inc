#pragma once

#include "../Diagnostic.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  char *buffer;
  size_t capacity;
  size_t written;
  bool overflow;
} DIAGNOSTIC_INS(FormatWriter);

static inline void DIAGNOSTIC_INS(FormatWriter_Init)(DIAGNOSTIC_INS(FormatWriter) *w,
                                                char *buffer, size_t capacity) {
  w->buffer = buffer;
  w->capacity = capacity;
  w->written = 0;
  w->overflow = false;
  if (buffer != NULL && capacity > 0) {
    buffer[0] = '\0';
  }
}

static inline void DIAGNOSTIC_INS(FormatWriter_AppendChar)(DIAGNOSTIC_INS(FormatWriter) *w,
                                                      char c) {
  if (w->buffer != NULL && w->written + 1 < w->capacity) {
    w->buffer[w->written] = c;
    w->written++;
    w->buffer[w->written] = '\0';
  } else {
    w->overflow = true;
  }
}

static inline void DIAGNOSTIC_INS(FormatWriter_AppendStr)(DIAGNOSTIC_INS(FormatWriter) *w,
                                                     const char *str) {
  if (str == NULL) {
    return;
  }
  while (*str != '\0') {
    DIAGNOSTIC_INS(FormatWriter_AppendChar)(w, *str++);
  }
}

static inline void DIAGNOSTIC_INS(FormatWriter_AppendUint)(DIAGNOSTIC_INS(FormatWriter) *w,
                                                      uint64_t val) {
  char temp[32];
  size_t idx = 0;
  if (val == 0) {
    DIAGNOSTIC_INS(FormatWriter_AppendChar)(w, '0');
    return;
  }
  while (val > 0) {
    temp[idx++] = (char)('0' + (val % 10));
    val /= 10;
  }
  while (idx > 0) {
    DIAGNOSTIC_INS(FormatWriter_AppendChar)(w, temp[--idx]);
  }
}

static inline void DIAGNOSTIC_INS(FormatWriter_AppendHex)(DIAGNOSTIC_INS(FormatWriter) *w,
                                                     uintptr_t val) {
  static const char hexChars[] = "0123456789abcdef";
  char temp[sizeof(uintptr_t) * 2 + 3];
  size_t idx = 0;

  DIAGNOSTIC_INS(FormatWriter_AppendStr)(w, "0x");
  if (val == 0) {
    DIAGNOSTIC_INS(FormatWriter_AppendChar)(w, '0');
    return;
  }
  while (val > 0) {
    temp[idx++] = hexChars[val & 0xFU];
    val >>= 4;
  }
  while (idx > 0) {
    DIAGNOSTIC_INS(FormatWriter_AppendChar)(w, temp[--idx]);
  }
}
/* EOF */

