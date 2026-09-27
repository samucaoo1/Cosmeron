#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Error.space"

#define STATUS_TYPE(NAME) GNS2(LIB_PREFIX(Status), NAME)

#define STATUS_CONST_PREFIXED_RAW(PREFIX, NAME) PREFIX##_STATUS_##NAME
#define STATUS_CONST_PREFIXED(PREFIX, NAME)                                    \
  STATUS_CONST_PREFIXED_RAW(PREFIX, NAME)

#ifdef COSMERON_NAMESPACE_CONST
#define STATUS_CONST(NAME)                                                     \
  STATUS_CONST_PREFIXED(COSMERON_NAMESPACE_CONST, NAME)
#else
#define STATUS_CONST(NAME) STATUS_CONST_PREFIXED(COSMERON, NAME)
#endif

/* Transitional source-compatibility alias. New code uses STATUS_CONST. */
#define STATUS_NS(NAME) STATUS_CONST(NAME)

#define OPSTATUS STATUS_TYPE(Status)

#define STATUS_TABLE(X)                                                        \
  X(SUCCESS, "Success")                                                       \
  X(GENERIC_ERROR, "Generic error")                                           \
  X(INVALID_ARGUMENT, "Invalid argument")                                     \
  X(OUT_OF_RANGE, "Out of range")                                             \
  X(OUT_OF_MEMORY, "Out of memory")                                           \
  X(NOT_FOUND, "Not found")                                                   \
  X(ALREADY_EXISTS, "Already exists")                                         \
  X(NOT_SUPPORTED, "Not supported")                                           \
  X(NOT_AVAILABLE, "Not available")                                           \
  X(BUSY, "Busy")                                                             \
  X(WOULD_BLOCK, "Would block")                                               \
  X(TIMEOUT, "Timeout")                                                       \
  X(CANCELLED, "Cancelled")                                                   \
  X(ARITHMETIC_OVERFLOW, "Arithmetic overflow")                               \
  X(DIVISION_BY_ZERO, "Division by zero")

typedef enum STATUS_TYPE(Status) {
#define STATUS_ENUM_ITEM(NAME, MESSAGE) STATUS_CONST(NAME),
  STATUS_TABLE(STATUS_ENUM_ITEM)
#undef STATUS_ENUM_ITEM
} OPSTATUS;
/* EOF */
