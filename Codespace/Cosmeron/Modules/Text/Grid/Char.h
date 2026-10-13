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

#define TEXT_GRID_CHAR_IS_EMPTY_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline bool TEXT_GRID_CHAR_FUNC(SUFFIX, IsEmpty)(                    \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *grid)

#define TEXT_GRID_CHAR_IS_VALID_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline bool TEXT_GRID_CHAR_FUNC(SUFFIX, IsValid)(                    \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *grid)

#define TEXT_GRID_CHAR_CREATE_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Create)(                 \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size)

#define TEXT_GRID_CHAR_DESTROY_PROTOTYPE(TYPE, SUFFIX)                      \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Destroy)(                \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid)

#define TEXT_GRID_CHAR_RESIZE_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Resize)(                 \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size)

#define TEXT_GRID_CHAR_RECREATE_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Recreate)(               \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size)

#define TEXT_GRID_CHAR_CLONE_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Clone)(                  \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination,                               \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source)

#define TEXT_GRID_CHAR_CLEAR_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Clear)(                  \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid)

#define TEXT_GRID_CHAR_READ_CELL_PROTOTYPE(TYPE, SUFFIX)                    \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, ReadCell)(               \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) position,   \
      TYPE *character)

#define TEXT_GRID_CHAR_WRITE_CELL_PROTOTYPE(TYPE, SUFFIX)                   \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, WriteCell)(              \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) position,         \
      TYPE character)

#define TEXT_GRID_CHAR_FILL_PROTOTYPE(TYPE, SUFFIX)                         \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Fill)(                   \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TYPE character)

#define TEXT_GRID_CHAR_FILL_REGION_PROTOTYPE(TYPE, SUFFIX)                  \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, FillRegion)(             \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *grid, TQUAD_TYPE(uint16) region,           \
      TYPE character)

#define TEXT_GRID_CHAR_READ_PROTOTYPE(TYPE, SUFFIX)                         \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Read)(                   \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source, TQUAD_TYPE(uint16) region,   \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination)

#define TEXT_GRID_CHAR_WRITE_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, Write)(                  \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination,                               \
      TDUAL_TYPE(uint16) destinationPosition,                                 \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source)

#define TEXT_GRID_CHAR_WRITE_REGION_PROTOTYPE(TYPE, SUFFIX)                 \
  static inline OPSTATUS TEXT_GRID_CHAR_FUNC(SUFFIX, WriteRegion)(            \
      TEXT_GRID_CHAR_TYPE(SUFFIX) *destination,                               \
      TDUAL_TYPE(uint16) destinationPosition,                                 \
      const TEXT_GRID_CHAR_TYPE(SUFFIX) *source,                              \
      TQUAD_TYPE(uint16) sourceRegion)

#define TEXT_GRID_CHAR_DECLARE_ALL(TYPE, SUFFIX)                             \
  TEXT_GRID_CHAR_IS_EMPTY_PROTOTYPE(TYPE, SUFFIX);                            \
  TEXT_GRID_CHAR_IS_VALID_PROTOTYPE(TYPE, SUFFIX);                            \
  TEXT_GRID_CHAR_CREATE_PROTOTYPE(TYPE, SUFFIX);                              \
  TEXT_GRID_CHAR_DESTROY_PROTOTYPE(TYPE, SUFFIX);                             \
  TEXT_GRID_CHAR_RESIZE_PROTOTYPE(TYPE, SUFFIX);                              \
  TEXT_GRID_CHAR_RECREATE_PROTOTYPE(TYPE, SUFFIX);                            \
  TEXT_GRID_CHAR_CLONE_PROTOTYPE(TYPE, SUFFIX);                               \
  TEXT_GRID_CHAR_CLEAR_PROTOTYPE(TYPE, SUFFIX);                               \
  TEXT_GRID_CHAR_READ_CELL_PROTOTYPE(TYPE, SUFFIX);                           \
  TEXT_GRID_CHAR_WRITE_CELL_PROTOTYPE(TYPE, SUFFIX);                          \
  TEXT_GRID_CHAR_FILL_PROTOTYPE(TYPE, SUFFIX);                                \
  TEXT_GRID_CHAR_FILL_REGION_PROTOTYPE(TYPE, SUFFIX);                         \
  TEXT_GRID_CHAR_READ_PROTOTYPE(TYPE, SUFFIX);                                \
  TEXT_GRID_CHAR_WRITE_PROTOTYPE(TYPE, SUFFIX);                               \
  TEXT_GRID_CHAR_WRITE_REGION_PROTOTYPE(TYPE, SUFFIX)

#define X(TYPE, SUFFIX) TEXT_GRID_CHAR_DECLARE_ALL(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#include "Impl/Char.impl"
