#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../Struct/TDual.h"
#include "../../Struct/TQuad.h"
#include "../Text.space"

#define TEXT_GRID_CHAR_STRUCT(TYPE, SUFFIX)                                  \
  typedef struct {                                                            \
    TYPE *data;                                                               \
    TDUAL_TYPE(uint16) size;                                                  \
  } TEXT_GRID_CHAR_TYPE(SUFFIX)

#define X(TYPE, SUFFIX) TEXT_GRID_CHAR_STRUCT(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#define TEXT_GRID_CHAR_PROTOTYPES(TYPE, SUFFIX)                               \
  static inline bool TEXT_GRID_CHAR_FUNC(SUFFIX, IsEmpty)(                    \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *grid);                               \
  static inline bool TEXT_GRID_CHAR_FUNC(SUFFIX, IsValid)(                    \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *grid);                               \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Create)(                 \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size);            \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Destroy)(                \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid);                                     \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Resize)(                 \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size);            \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Recreate)(               \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size);            \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Clone)(                  \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination,                               \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source);                             \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Clear)(                  \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid);                                     \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, ReadCell)(               \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) position,   \
      TYPE *character);                                                       \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, WriteCell)(              \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) position,         \
      TYPE character);                                                        \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Fill)(                   \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TYPE character);                     \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, FillRegion)(             \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TQUAD_TYPE(uint16) region,           \
      TYPE character);                                                        \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Read)(                   \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source, TQUAD_TYPE(uint16) region,   \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination);                              \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Write)(                  \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination,                               \
      TDUAL_TYPE(uint16) destinationPosition,                                \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source);                             \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, WriteRegion)(            \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination,                               \
      TDUAL_TYPE(uint16) destinationPosition,                                \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source,                              \
      TQUAD_TYPE(uint16) sourceRegion)

#define X(TYPE, SUFFIX) TEXT_GRID_CHAR_PROTOTYPES(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#include "Impl/Char.impl"
