#pragma once
#include "../Parsing.space"

/* Strict decimal grammar, independent of the process decimal separator. */

#define PARSING_SLICE_CREATE_PROTOTYPE \
  static inline PARSING_TYPE(TSlice) PARSING_FUNC(Slice_Create)( \
      const char *data, \
      size_t size)

#define PARSING_SLICE_EQUAL_PROTOTYPE \
  static inline bool PARSING_FUNC(Slice_Equal)( \
      PARSING_TYPE(TSlice) left, \
      PARSING_TYPE(TSlice) right)

#define PARSING_SLICE_U64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(Slice_U64)( \
      PARSING_TYPE(TSlice) text, \
      uint64_t *outValue)

#define PARSING_SLICE_I64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(Slice_I64)( \
      PARSING_TYPE(TSlice) text, \
      int64_t *outValue)

#define PARSING_SLICE_F64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(Slice_F64)( \
      PARSING_TYPE(TSlice) text, \
      double *outValue)

#define PARSING_SLICE_BOOL_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(Slice_Bool)( \
      PARSING_TYPE(TSlice) text, \
      bool *outValue)

PARSING_SLICE_CREATE_PROTOTYPE;
PARSING_SLICE_EQUAL_PROTOTYPE;
PARSING_SLICE_U64_PROTOTYPE;
PARSING_SLICE_I64_PROTOTYPE;
PARSING_SLICE_F64_PROTOTYPE;
PARSING_SLICE_BOOL_PROTOTYPE;

#include "Impl/Common.impl"
