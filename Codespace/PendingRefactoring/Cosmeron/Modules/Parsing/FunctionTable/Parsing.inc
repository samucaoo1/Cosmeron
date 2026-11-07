#pragma once
#ifndef PARSING_DISABLE_FUNCTION_TABLE
#define PARSING_FUNCTION_TABLE_STRUCT() \
  typedef struct PARSING_TYPE(FunctionTable) { \
    OPSTATUS (*JSON_Parse)(const char *, size_t, PARSING_TYPE(TJSONDocument) *, PARSING_TYPE(TDiagnostic) *); \
    OPSTATUS (*INI_Parse)(const char *, size_t, PARSING_TYPE(TINIDocument) *, PARSING_TYPE(TDiagnostic) *); \
    OPSTATUS (*CSV_Parse)(const char *, size_t, char, PARSING_TYPE(TCSVDocument) *, PARSING_TYPE(TDiagnostic) *); \
    OPSTATUS (*TOML_Parse)(const char *, size_t, PARSING_TYPE(TTOMLDocument) *, PARSING_TYPE(TDiagnostic) *); \
    OPSTATUS (*XML_Parse)(const char *, size_t, PARSING_TYPE(TXMLDocument) *, PARSING_TYPE(TDiagnostic) *); \
    OPSTATUS (*YAML_Parse)(const char *, size_t, PARSING_TYPE(TYAMLDocument) *, PARSING_TYPE(TDiagnostic) *); \
  } PARSING_TYPE(FunctionTable);
#define PARSING_FUNCTION_TABLE_INSTANCE() \
  static const PARSING_TYPE(FunctionTable) PARSING_NS(Functions) = { \
    PARSING_FUNC(JSON_Parse), PARSING_FUNC(INI_Parse), PARSING_FUNC(CSV_Parse), \
    PARSING_FUNC(TOML_Parse), PARSING_FUNC(XML_Parse), PARSING_FUNC(YAML_Parse) \
  };
PARSING_FUNCTION_TABLE_STRUCT()
PARSING_FUNCTION_TABLE_INSTANCE()
#endif
