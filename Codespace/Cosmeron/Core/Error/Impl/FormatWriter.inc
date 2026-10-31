#pragma once

#include "../Error.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  char *buffer;
  size_t capacity;
  size_t written;
  bool overflow;
} ERROR_INS(FormatWriter);

static inline void ERROR_INS(FormatWriter_Init)(ERROR_INS(FormatWriter) *w,
                                                char *buffer, size_t capacity) {
  w->buffer = buffer;
  w->capacity = capacity;
  w->written = 0;
  w->overflow = false;
  if (buffer != NULL && capacity > 0) {
    buffer[0] = '\0';
  }
}

static inline void ERROR_INS(FormatWriter_AppendChar)(ERROR_INS(FormatWriter) *w,
                                                      char c) {
  if (w->buffer != NULL && w->written + 1 < w->capacity) {
    w->buffer[w->written] = c;
    w->written++;
    w->buffer[w->written] = '\0';
  } else {
    w->overflow = true;
  }
}

static inline void ERROR_INS(FormatWriter_AppendStr)(ERROR_INS(FormatWriter) *w,
                                                     const char *str) {
  if (str == NULL) {
    return;
  }
  while (*str != '\0') {
    ERROR_INS(FormatWriter_AppendChar)(w, *str++);
  }
}

static inline void ERROR_INS(FormatWriter_AppendUint)(ERROR_INS(FormatWriter) *w,
                                                      uint64_t val) {
  char temp[32];
  size_t idx = 0;
  if (val == 0) {
    ERROR_INS(FormatWriter_AppendChar)(w, '0');
    return;
  }
  while (val > 0) {
    temp[idx++] = (char)('0' + (val % 10));
    val /= 10;
  }
  while (idx > 0) {
    ERROR_INS(FormatWriter_AppendChar)(w, temp[--idx]);
  }
}

static inline void ERROR_INS(FormatWriter_AppendHex)(ERROR_INS(FormatWriter) *w,
                                                     uintptr_t val) {
  ERROR_INS(FormatWriter_AppendStr)(w, "0x");
  const char hexChars[] = "0123456789abcdef";
  int shift = (int)((sizeof(uintptr_t) * 8) - 4);
  for (; shift >= 0; shift -= 4) {
    uint8_t nibble = (uint8_t)((val >> shift) & 0x0F);
    ERROR_INS(FormatWriter_AppendChar)(w, hexChars[nibble]);
  }
}

/* EOF */
