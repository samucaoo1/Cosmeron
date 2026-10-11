#pragma once
#include "../../../../Core/Namespace.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Renderer-independent primitives used by both cell UI and pixel GUI. */
#define COSMERON_MACRO_INTERNAL_UI_SHARED(NAME) GNS3(LIB_PREFIX(UI), Shared, NAME)
static inline uint64_t COSMERON_MACRO_INTERNAL_UI_SHARED(HashText)(uint64_t hash,
                                                                   const char *text) {
  const unsigned char *p = (const unsigned char *)text;
  if (!text)
    return hash;
  while (*p) {
    hash ^= *p++;
    hash *= UINT64_C(1099511628211);
  }
  return hash ? hash : UINT64_C(1);
}
static inline uint64_t COSMERON_MACRO_INTERNAL_UI_SHARED(FocusStep)(const uint64_t *ids,
                                                                    size_t count, uint64_t focus,
                                                                    bool forward) {
  size_t i;
  if (!count)
    return 0;
  for (i = 0; i < count; ++i)
    if (ids[i] == focus)
      break;
  if (i == count)
    return ids[forward ? 0 : count - 1];
  return ids[forward ? (i + 1) % count : (i + count - 1) % count];
}
static inline bool
COSMERON_MACRO_INTERNAL_UI_SHARED(PointerActivate)(uint64_t *focus, uint64_t *held, uint64_t id,
                                                   bool pressed, bool released, bool pressInside,
                                                   bool releaseInside, bool disabled) {
  if (disabled)
    return false;
  if (pressed && pressInside && !*held) {
    *held = id;
    *focus = id;
  }
  return released && releaseInside && *held == id;
}

static inline bool COSMERON_MACRO_INTERNAL_UI_SHARED(Boundary)(const char *text, size_t length,
                                                               size_t offset) {
  return offset <= length && (offset == length || ((unsigned char)text[offset] & 0xC0U) != 0x80U);
}
static inline size_t COSMERON_MACRO_INTERNAL_UI_SHARED(Previous)(const char *text, size_t offset) {
  if (offset) {
    --offset;
    while (offset && ((unsigned char)text[offset] & 0xC0U) == 0x80U)
      --offset;
  }
  return offset;
}
static inline size_t COSMERON_MACRO_INTERNAL_UI_SHARED(Next)(const char *text, size_t length,
                                                             size_t offset) {
  if (offset >= length)
    return length;
  ++offset;
  while (offset < length && ((unsigned char)text[offset] & 0xC0U) == 0x80U)
    ++offset;
  return offset;
}
static inline void COSMERON_MACRO_INTERNAL_UI_SHARED(DeleteRange)(char *buffer, size_t *length,
                                                                  size_t start, size_t end) {
  memmove(buffer + start, buffer + end, *length - end + 1);
  *length -= end - start;
}

static inline size_t COSMERON_MACRO_INTERNAL_UI_SHARED(LineStart)(const char *text, size_t offset) {
  while (offset && text[offset - 1] != '\n')
    --offset;
  return offset;
}
static inline size_t COSMERON_MACRO_INTERNAL_UI_SHARED(LineEnd)(const char *text, size_t length,
                                                                size_t offset) {
  while (offset < length && text[offset] != '\n')
    ++offset;
  return offset;
}
static inline size_t COSMERON_MACRO_INTERNAL_UI_SHARED(Vertical)(const char *text, size_t length,
                                                                 size_t caret, bool down) {
  size_t start = COSMERON_MACRO_INTERNAL_UI_SHARED(LineStart)(text, caret), p = start, column = 0;
  while (p < caret) {
    p = COSMERON_MACRO_INTERNAL_UI_SHARED(Next)(text, length, p);
    ++column;
  }
  if (down) {
    p = COSMERON_MACRO_INTERNAL_UI_SHARED(LineEnd)(text, length, caret);
    if (p == length)
      return caret;
    ++p;
  } else {
    if (!start)
      return caret;
    p = COSMERON_MACRO_INTERNAL_UI_SHARED(LineStart)(text, start - 1);
  }
  while (column && p < length && text[p] != '\n') {
    p = COSMERON_MACRO_INTERNAL_UI_SHARED(Next)(text, length, p);
    --column;
  }
  return p;
}
