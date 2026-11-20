#pragma once

/* ==========================================================================
 * Container Shared Instantiation Tables (Row-Tuple Pattern)
 * ========================================================================== */

#define CONTAINER_ORDERED_TYPE_TABLE(X)                                        \
  X((Boolean, bool))                                                           \
  X((Int8, int8_t))                                                            \
  X((Int16, int16_t))                                                          \
  X((Int32, int32_t))                                                          \
  X((Int64, int64_t))                                                          \
  X((UInt8, uint8_t))                                                          \
  X((UInt16, uint16_t))                                                        \
  X((UInt32, uint32_t))                                                        \
  X((UInt64, uint64_t))                                                        \
  X((Char, char))                                                              \
  X((Char8, uint8_t))                                                          \
  X((Char16, uint16_t))                                                        \
  X((Char32, uint32_t))                                                        \
  X((Float32, float))                                                          \
  X((Float64, double))                                                         \
  X((Float, float))                                                            \
  X((Double, double))                                                          \
  X((LongDouble, long double))                                                 \
  X((Int, int))                                                                \
  X((UInt, unsigned int))                                                      \
  X((Size, size_t))                                                            \
  X((PtrDiff, ptrdiff_t))                                                      \
  X((IntPtr, intptr_t))                                                        \
  X((UIntPtr, uintptr_t))

#define CONTAINER_MAP_TYPE_TABLE(X)                                            \
  X((Int32_Int32, int32_t, int32_t))                                           \
  X((Int32_Float64, int32_t, double))                                          \
  X((Size_Size, size_t, size_t))

/* EOF */
