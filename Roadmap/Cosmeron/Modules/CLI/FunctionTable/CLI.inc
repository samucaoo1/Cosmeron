#pragma once
#ifndef CLI_DISABLE_FUNCTION_TABLE
#define CLI_FUNCTION_TABLE_STRUCT() \
  typedef struct CLI_TYPE(FunctionTable) { \
    CLI_TYPE(TArguments) (*Arguments_Create)(int argc, char **argv); \
    bool (*Arguments_Has)(const CLI_TYPE(TArguments) *arguments, const char *name); \
    OPSTATUS (*Command_Parse)(const CLI_TYPE(TCommand) *command, int argc, char **argv, \
        CLI_TYPE(TMatch) *matches, size_t capacity, size_t *outCount, CLI_TYPE(TDiagnostic) *diagnostic); \
    OPSTATUS (*Help_Write)(const CLI_TYPE(TCommand) *command, char *buffer, size_t capacity, size_t *outSize); \
  } CLI_TYPE(FunctionTable);
#define CLI_FUNCTION_TABLE_INSTANCE() \
  static const CLI_TYPE(FunctionTable) CLI_NS(Functions) = { \
    CLI_FUNC(Arguments_Create), CLI_FUNC(Arguments_Has), CLI_FUNC(Command_Parse), CLI_FUNC(Help_Write) \
  };
CLI_FUNCTION_TABLE_STRUCT()
CLI_FUNCTION_TABLE_INSTANCE()
#endif
