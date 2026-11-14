#pragma once

#include "../../../Core/Condition/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../Tuple/TDual.h"
#include "../../Tuple/TQuad.h"
#include "../Impl/Common.inc"

#define TEXT_GRID_CHAR_STRUCT(TYPE, SUFFIX)                                  \
  typedef struct {                                                            \
    TYPE *data;                                                               \
    MODULE_TYPED(Tuple, Dual, UInt16) size;                                                  \
  } MODULE_TYPE(Text, Char, SUFFIX)

#define X(TYPE, SUFFIX) TEXT_GRID_CHAR_STRUCT(TYPE, SUFFIX);
TEXT_GRID_CHARACTER_TABLE(X)
#undef X

#define TEXT_GRID_CHAR_IS_EMPTY_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline bool MODULE_FUNC(Text, Char, SUFFIX, IsEmpty)(                    \
      const MODULE_TYPE(Text, Char, SUFFIX) *grid)

#define TEXT_GRID_CHAR_IS_VALID_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline bool MODULE_FUNC(Text, Char, SUFFIX, IsValid)(                    \
      const MODULE_TYPE(Text, Char, SUFFIX) *grid)

#define TEXT_GRID_CHAR_CREATE_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Create)(                 \
      MODULE_TYPE(Text, Char, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) size)

#define TEXT_GRID_CHAR_DESTROY_PROTOTYPE(TYPE, SUFFIX)                      \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Destroy)(                \
      MODULE_TYPE(Text, Char, SUFFIX) *grid)

#define TEXT_GRID_CHAR_RESIZE_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Resize)(                 \
      MODULE_TYPE(Text, Char, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) size)

#define TEXT_GRID_CHAR_RECREATE_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Recreate)(               \
      MODULE_TYPE(Text, Char, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) size)

#define TEXT_GRID_CHAR_CLONE_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Clone)(                  \
      MODULE_TYPE(Text, Char, SUFFIX) *destination,                               \
      const MODULE_TYPE(Text, Char, SUFFIX) *source)

#define TEXT_GRID_CHAR_CLEAR_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Clear)(                  \
      MODULE_TYPE(Text, Char, SUFFIX) *grid)

#define TEXT_GRID_CHAR_READ_CELL_PROTOTYPE(TYPE, SUFFIX)                    \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, ReadCell)(               \
      const MODULE_TYPE(Text, Char, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) position,   \
      TYPE *character)

#define TEXT_GRID_CHAR_WRITE_CELL_PROTOTYPE(TYPE, SUFFIX)                   \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, WriteCell)(              \
      MODULE_TYPE(Text, Char, SUFFIX) *grid, MODULE_TYPED(Tuple, Dual, UInt16) position,         \
      TYPE character)

#define TEXT_GRID_CHAR_FILL_PROTOTYPE(TYPE, SUFFIX)                         \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Fill)(                   \
      MODULE_TYPE(Text, Char, SUFFIX) *grid, TYPE character)

#define TEXT_GRID_CHAR_FILL_REGION_PROTOTYPE(TYPE, SUFFIX)                  \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, FillRegion)(             \
      MODULE_TYPE(Text, Char, SUFFIX) *grid, MODULE_TYPED(Tuple, Quad, UInt16) region,           \
      TYPE character)

#define TEXT_GRID_CHAR_READ_PROTOTYPE(TYPE, SUFFIX)                         \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Read)(                   \
      const MODULE_TYPE(Text, Char, SUFFIX) *source, MODULE_TYPED(Tuple, Quad, UInt16) region,   \
      MODULE_TYPE(Text, Char, SUFFIX) *destination)

#define TEXT_GRID_CHAR_WRITE_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, Write)(                  \
      MODULE_TYPE(Text, Char, SUFFIX) *destination,                               \
      MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,                                 \
      const MODULE_TYPE(Text, Char, SUFFIX) *source)

#define TEXT_GRID_CHAR_WRITE_REGION_PROTOTYPE(TYPE, SUFFIX)                 \
  static inline OPSTATUS MODULE_FUNC(Text, Char, SUFFIX, WriteRegion)(            \
      MODULE_TYPE(Text, Char, SUFFIX) *destination,                               \
      MODULE_TYPED(Tuple, Dual, UInt16) destinationPosition,                                 \
      const MODULE_TYPE(Text, Char, SUFFIX) *source,                              \
      MODULE_TYPED(Tuple, Quad, UInt16) sourceRegion)

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
