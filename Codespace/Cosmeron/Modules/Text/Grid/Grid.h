#pragma once

#include "Attribute.h"
#include "Char.h"

#define TEXT_GRID_STRUCT(TYPE, SUFFIX)                                       \
  typedef struct {                                                            \
    MODULE_TYPE(Text, Char, SUFFIX) chars;                                        \
    MODULE_TYPE(Text, Attribute, Grid) attributes;                               \
  } MODULE_TYPE(Text, Grid, SUFFIX)

#define X(TYPE, SUFFIX) TEXT_GRID_STRUCT(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#define TEXT_GRID_IS_EMPTY_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline bool MODULE_FUNC(Text, Grid, SUFFIX, IsEmpty)(                         \
      const MODULE_TYPE(Text, Grid, SUFFIX) *grid)

#define TEXT_GRID_IS_VALID_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline bool MODULE_FUNC(Text, Grid, SUFFIX, IsValid)(                         \
      const MODULE_TYPE(Text, Grid, SUFFIX) *grid)

#define TEXT_GRID_CREATE_PROTOTYPE(TYPE, SUFFIX)                            \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Create)(                      \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) size)

#define TEXT_GRID_DESTROY_PROTOTYPE(TYPE, SUFFIX)                           \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Destroy)(                     \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid)

#define TEXT_GRID_RESIZE_PROTOTYPE(TYPE, SUFFIX)                            \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Resize)(                      \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) size)

#define TEXT_GRID_RECREATE_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Recreate)(                    \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) size)

#define TEXT_GRID_CLONE_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Clone)(                       \
      MODULE_TYPE(Text, Grid, SUFFIX) *destination,                                    \
      const MODULE_TYPE(Text, Grid, SUFFIX) *source)

#define TEXT_GRID_CLEAR_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Clear)(                       \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid)

#define TEXT_GRID_CLEAR_WITH_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, ClearWith)(                   \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPE(Text, Attribute, Cell) attribute)

#define TEXT_GRID_READ_CELL_PROTOTYPE(TYPE, SUFFIX)                         \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, ReadCell)(                    \
      const MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) position,        \
      TYPE *character, MODULE_TYPE(Text, Attribute, Cell) *attribute)

#define TEXT_GRID_WRITE_CELL_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, WriteCell)(                   \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) position,              \
      TYPE character, MODULE_TYPE(Text, Attribute, Cell) attribute)

#define TEXT_GRID_FILL_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Fill)(                        \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, TYPE character,                           \
      MODULE_TYPE(Text, Attribute, Cell) attribute)

#define TEXT_GRID_FILL_REGION_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, FillRegion)(                  \
      MODULE_TYPE(Text, Grid, SUFFIX) *grid, MODULE_TYPED(Tuple, Quad, UInt16) region,                \
      TYPE character, MODULE_TYPE(Text, Attribute, Cell) attribute)

#define TEXT_GRID_READ_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Read)(                        \
      const MODULE_TYPE(Text, Grid, SUFFIX) *source, MODULE_TYPED(Tuple, Quad, UInt16) region,        \
      MODULE_TYPE(Text, Grid, SUFFIX) *destination)

#define TEXT_GRID_WRITE_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Write)(                       \
      MODULE_TYPE(Text, Grid, SUFFIX) *destination,                                    \
      MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,                                 \
      const MODULE_TYPE(Text, Grid, SUFFIX) *source)

#define TEXT_GRID_WRITE_REGION_PROTOTYPE(TYPE, SUFFIX)                      \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, WriteRegion)(                 \
      MODULE_TYPE(Text, Grid, SUFFIX) *destination,                                    \
      MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,                                 \
      const MODULE_TYPE(Text, Grid, SUFFIX) *source, MODULE_TYPED(Tuple, Quad, UInt16) sourceRegion)

#define TEXT_GRID_BLIT_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, Blit)(                        \
      MODULE_TYPE(Text, Grid, SUFFIX) *destination,                                    \
      MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,                                 \
      const MODULE_TYPE(Text, Grid, SUFFIX) *source)

#define TEXT_GRID_BLIT_REGION_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Text, Grid, SUFFIX, BlitRegion)(                  \
      MODULE_TYPE(Text, Grid, SUFFIX) *destination,                                    \
      MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,                                 \
      const MODULE_TYPE(Text, Grid, SUFFIX) *source, MODULE_TYPED(Tuple, Quad, UInt16) sourceRegion)

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
