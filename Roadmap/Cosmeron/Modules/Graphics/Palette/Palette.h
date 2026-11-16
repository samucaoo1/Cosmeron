#pragma once
#include "../../../Core/Module/Object.h"
#include "Palette.space"

#if defined(PALETTE_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

OBJECT(TPalette,
  FIELDS(
    uint32_t *colors;
    size_t count;
    size_t capacity;
    bool owns;
  ),
  METHOD(OPSTATUS, Create, (size_t capacity)),
  METHOD(void, Destroy, ()),
  METHOD(OPSTATUS, SetColor, (size_t index, uint32_t color)),
  METHOD(OPSTATUS, GetColor, (size_t index, uint32_t *outColor)),
  METHOD(OPSTATUS, AddColor, (uint32_t color, size_t *outIndex)),
  METHOD(OPSTATUS, FindNearest, (uint32_t color, size_t *outIndex)),
  METHOD(OPSTATUS, LoadANSI16, ()),
  METHOD(OPSTATUS, LoadXTerm256, ()),
  METHOD(size_t, Count, ())
);

#define PALETTE_CREATE_PROTOTYPE OBJECT_PROTOTYPE(TPalette, Create, (size_t capacity))
#define PALETTE_DESTROY_PROTOTYPE OBJECT_PROTOTYPE(TPalette, Destroy, ())
#define PALETTE_SET_COLOR_PROTOTYPE OBJECT_PROTOTYPE(TPalette, SetColor, (size_t index, uint32_t color))
#define PALETTE_GET_COLOR_PROTOTYPE OBJECT_PROTOTYPE(TPalette, GetColor, (size_t index, uint32_t *outColor))
#define PALETTE_ADD_COLOR_PROTOTYPE OBJECT_PROTOTYPE(TPalette, AddColor, (uint32_t color, size_t *outIndex))
#define PALETTE_FIND_NEAREST_PROTOTYPE OBJECT_PROTOTYPE(TPalette, FindNearest, (uint32_t color, size_t *outIndex))
#define PALETTE_LOAD_ANSI16_PROTOTYPE OBJECT_PROTOTYPE(TPalette, LoadANSI16, ())
#define PALETTE_LOAD_XTERM256_PROTOTYPE OBJECT_PROTOTYPE(TPalette, LoadXTerm256, ())
#define PALETTE_COUNT_PROTOTYPE OBJECT_PROTOTYPE(TPalette, Count, ())

#include "Impl/Palette.impl"
