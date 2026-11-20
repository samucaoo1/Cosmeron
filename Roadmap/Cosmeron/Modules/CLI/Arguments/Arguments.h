#pragma once
#include "../CLI.space"

/* Direct queries accept literal names and stop at --. They do not infer aliases. */

#define CLI_ARGUMENTS_CREATE_PROTOTYPE \
  static inline CLI_TYPE(TArguments) CLI_FUNC(Arguments_Create)( \
      int argc, \
      char **argv)

#define CLI_ARGUMENTS_COUNT_PROTOTYPE \
  static inline size_t CLI_FUNC(Arguments_Count)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name)

#define CLI_ARGUMENTS_HAS_PROTOTYPE \
  static inline bool CLI_FUNC(Arguments_Has)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name)

#define CLI_ARGUMENTS_STRING_AT_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_StringAt)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      size_t occurrence, \
      const char **outValue)

#define CLI_ARGUMENTS_STRING_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_String)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      const char **outValue)

#define CLI_ARGUMENTS_VIEW_AT_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_ViewAt)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      size_t occurrence, \
      TEXT_VIEW_TYPE(8) *outValue)

#define CLI_ARGUMENTS_VIEW_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_View)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      TEXT_VIEW_TYPE(8) *outValue)

#define CLI_ARGUMENTS_I64_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_I64)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      int64_t *outValue)

#define CLI_ARGUMENTS_U64_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_U64)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      uint64_t *outValue)

#define CLI_ARGUMENTS_F64_PROTOTYPE \
  static inline OPSTATUS CLI_FUNC(Arguments_F64)( \
      const CLI_TYPE(TArguments) *arguments, \
      const char *name, \
      double *outValue)

CLI_ARGUMENTS_CREATE_PROTOTYPE;
CLI_ARGUMENTS_COUNT_PROTOTYPE;
CLI_ARGUMENTS_HAS_PROTOTYPE;
CLI_ARGUMENTS_STRING_AT_PROTOTYPE;
CLI_ARGUMENTS_STRING_PROTOTYPE;
CLI_ARGUMENTS_VIEW_AT_PROTOTYPE;
CLI_ARGUMENTS_VIEW_PROTOTYPE;
CLI_ARGUMENTS_I64_PROTOTYPE;
CLI_ARGUMENTS_U64_PROTOTYPE;
CLI_ARGUMENTS_F64_PROTOTYPE;

#include "Impl/Arguments.impl"
