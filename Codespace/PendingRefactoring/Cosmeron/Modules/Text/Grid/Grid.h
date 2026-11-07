#pragma once

#include "Attribute.h"
#include "Char.h"

#define TEXT_GRID_STRUCT(TYPE, SUFFIX)                                       \
  typedef struct {                                                            \
    TEXT_GRID_CHAR_TYPE(SUFFIX) chars;                                        \
    TEXT_GRID_ATTRIBUTE_TYPE(Grid) attributes;                               \
  } TEXT_GRID_TYPE(SUFFIX)

#define X(TYPE, SUFFIX) TEXT_GRID_STRUCT(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#define TEXT_GRID_IS_EMPTY_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline bool TEXT_GRID_FUNC(SUFFIX, IsEmpty)(                         \
      const TEXT_GRID_TYPE(SUFFIX) *grid)

#define TEXT_GRID_IS_VALID_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline bool TEXT_GRID_FUNC(SUFFIX, IsValid)(                         \
      const TEXT_GRID_TYPE(SUFFIX) *grid)

#define TEXT_GRID_CREATE_PROTOTYPE(TYPE, SUFFIX)                            \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Create)(                      \
      TEXT_GRID_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size)

#define TEXT_GRID_DESTROY_PROTOTYPE(TYPE, SUFFIX)                           \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Destroy)(                     \
      TEXT_GRID_TYPE(SUFFIX) *grid)

#define TEXT_GRID_RESIZE_PROTOTYPE(TYPE, SUFFIX)                            \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Resize)(                      \
      TEXT_GRID_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size)

#define TEXT_GRID_RECREATE_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Recreate)(                    \
      TEXT_GRID_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) size)

#define TEXT_GRID_CLONE_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Clone)(                       \
      TEXT_GRID_TYPE(SUFFIX) *destination,                                    \
      const TEXT_GRID_TYPE(SUFFIX) *source)

#define TEXT_GRID_CLEAR_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Clear)(                       \
      TEXT_GRID_TYPE(SUFFIX) *grid)

#define TEXT_GRID_CLEAR_WITH_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, ClearWith)(                   \
      TEXT_GRID_TYPE(SUFFIX) *grid, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TEXT_GRID_READ_CELL_PROTOTYPE(TYPE, SUFFIX)                         \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, ReadCell)(                    \
      const TEXT_GRID_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) position,        \
      TYPE *character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute)

#define TEXT_GRID_WRITE_CELL_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, WriteCell)(                   \
      TEXT_GRID_TYPE(SUFFIX) *grid, TDUAL_TYPE(uint16) position,              \
      TYPE character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TEXT_GRID_FILL_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Fill)(                        \
      TEXT_GRID_TYPE(SUFFIX) *grid, TYPE character,                           \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TEXT_GRID_FILL_REGION_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, FillRegion)(                  \
      TEXT_GRID_TYPE(SUFFIX) *grid, TQUAD_TYPE(uint16) region,                \
      TYPE character, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TEXT_GRID_READ_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Read)(                        \
      const TEXT_GRID_TYPE(SUFFIX) *source, TQUAD_TYPE(uint16) region,        \
      TEXT_GRID_TYPE(SUFFIX) *destination)

#define TEXT_GRID_WRITE_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Write)(                       \
      TEXT_GRID_TYPE(SUFFIX) *destination,                                    \
      TDUAL_TYPE(uint16) destinationPosition,                                 \
      const TEXT_GRID_TYPE(SUFFIX) *source)

#define TEXT_GRID_WRITE_REGION_PROTOTYPE(TYPE, SUFFIX)                      \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, WriteRegion)(                 \
      TEXT_GRID_TYPE(SUFFIX) *destination,                                    \
      TDUAL_TYPE(uint16) destinationPosition,                                 \
      const TEXT_GRID_TYPE(SUFFIX) *source, TQUAD_TYPE(uint16) sourceRegion)

#define TEXT_GRID_BLIT_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, Blit)(                        \
      TEXT_GRID_TYPE(SUFFIX) *destination,                                    \
      TDUAL_TYPE(uint16) destinationPosition,                                 \
      const TEXT_GRID_TYPE(SUFFIX) *source)

#define TEXT_GRID_BLIT_REGION_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS TEXT_GRID_FUNC(SUFFIX, BlitRegion)(                  \
      TEXT_GRID_TYPE(SUFFIX) *destination,                                    \
      TDUAL_TYPE(uint16) destinationPosition,                                 \
      const TEXT_GRID_TYPE(SUFFIX) *source, TQUAD_TYPE(uint16) sourceRegion)

#define TEXT_GRID_DECLARE_ALL(TYPE, SUFFIX)                                 \
  TEXT_GRID_IS_EMPTY_PROTOTYPE(TYPE, SUFFIX);                                \
  TEXT_GRID_IS_VALID_PROTOTYPE(TYPE, SUFFIX);                                \
  TEXT_GRID_CREATE_PROTOTYPE(TYPE, SUFFIX);                                  \
  TEXT_GRID_DESTROY_PROTOTYPE(TYPE, SUFFIX);                                 \
  TEXT_GRID_RESIZE_PROTOTYPE(TYPE, SUFFIX);                                  \
  TEXT_GRID_RECREATE_PROTOTYPE(TYPE, SUFFIX);                                \
  TEXT_GRID_CLONE_PROTOTYPE(TYPE, SUFFIX);                                   \
  TEXT_GRID_CLEAR_PROTOTYPE(TYPE, SUFFIX);                                   \
  TEXT_GRID_CLEAR_WITH_PROTOTYPE(TYPE, SUFFIX);                              \
  TEXT_GRID_READ_CELL_PROTOTYPE(TYPE, SUFFIX);                               \
  TEXT_GRID_WRITE_CELL_PROTOTYPE(TYPE, SUFFIX);                              \
  TEXT_GRID_FILL_PROTOTYPE(TYPE, SUFFIX);                                    \
  TEXT_GRID_FILL_REGION_PROTOTYPE(TYPE, SUFFIX);                             \
  TEXT_GRID_READ_PROTOTYPE(TYPE, SUFFIX);                                    \
  TEXT_GRID_WRITE_PROTOTYPE(TYPE, SUFFIX);                                   \
  TEXT_GRID_WRITE_REGION_PROTOTYPE(TYPE, SUFFIX);                            \
  TEXT_GRID_BLIT_PROTOTYPE(TYPE, SUFFIX);                                    \
  TEXT_GRID_BLIT_REGION_PROTOTYPE(TYPE, SUFFIX)

#define X(TYPE, SUFFIX) TEXT_GRID_DECLARE_ALL(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#include "Impl/Grid.impl"
