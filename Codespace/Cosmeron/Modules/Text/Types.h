#pragma once

#include "Text.space"

typedef enum TText_Error {
  TEXT_ERROR_NONE = 0,
  TEXT_ERROR_INVALID_ARGUMENT,
  TEXT_ERROR_INVALID_SEQUENCE,
  TEXT_ERROR_INCOMPLETE_SEQUENCE,
  TEXT_ERROR_INSUFFICIENT_SPACE
} TText_Error;

typedef struct TText_Decode_Result {
  TText_Char32 codepoint;
  size_t units;
  TText_Error error;
} TText_Decode_Result;

typedef struct TText_Encode_Result {
  size_t units;
  TText_Error error;
} TText_Encode_Result;

typedef struct TText_Measure_Result {
  size_t columns;
  size_t units;
  TText_Error error;
} TText_Measure_Result;
