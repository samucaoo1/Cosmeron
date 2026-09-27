#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Error.space"

#define STATUS_MOD Status
#define STATUS_CMOD STATUS

#define STATUS_NS(NAME) GNS2(LIB_PREFIX(STATUS_MOD), NAME)
#define STATUS_CNS(NAME) CNS2(LIB_PREFIX_CONST(STATUS_CMOD), NAME)
#define STATUS_INS(NAME) GNS3(LIB_PREFIX(STATUS_MOD), Internal, NAME)
#define STATUS_CINS(NAME) CNS3(LIB_PREFIX_CONST(STATUS_CMOD), INTERNAL, NAME)

#define STATUS_TYPE(NAME) STATUS_NS(NAME)
#define STATUS_CONST(NAME) STATUS_CNS(NAME)

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
