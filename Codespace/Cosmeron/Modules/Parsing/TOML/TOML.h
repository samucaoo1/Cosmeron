#pragma once
#include "../JSON/JSON.h" /* Reuse the strict Unicode escape decoder and quoting. */

typedef enum PARSING_TYPE(TOMLKind) {
  PARSING_CONST(TOML_TABLE), PARSING_CONST(TOML_ARRAY), PARSING_CONST(TOML_STRING),
  PARSING_CONST(TOML_INTEGER), PARSING_CONST(TOML_FLOAT), PARSING_CONST(TOML_BOOL)
} PARSING_TYPE(TOMLKind);
typedef struct PARSING_TYPE(TTOMLValue) {
  PARSING_TYPE(TOMLKind) kind; char *key, *text; size_t keySize, textSize, count;
  bool declared, sealed;
  struct PARSING_TYPE(TTOMLValue) *first, *last, *next;
} PARSING_TYPE(TTOMLValue);
typedef struct PARSING_TYPE(TTOMLDocument) { PARSING_TYPE(TTOMLValue) *root; } PARSING_TYPE(TTOMLDocument);

#define PARSING_TOML_FREE_PROTOTYPE \
  static inline void PARSING_FUNC(TOML_Free)( \
      PARSING_TYPE(TTOMLDocument) *document)

#define PARSING_TOML_PARSE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_Parse)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TTOMLDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_TOML_GET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_Get)( \
      const PARSING_TYPE(TTOMLValue) *table, \
      const char *key, \
      const PARSING_TYPE(TTOMLValue) **outValue)

#define PARSING_TOML_AT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_At)( \
      const PARSING_TYPE(TTOMLValue) *array, \
      size_t index, \
      const PARSING_TYPE(TTOMLValue) **outValue)

#define PARSING_TOML_STRING_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_String)( \
      const PARSING_TYPE(TTOMLValue) *value, \
      PARSING_TYPE(TSlice) *outValue)

#define PARSING_TOML_I64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_I64)( \
      const PARSING_TYPE(TTOMLValue) *value, \
      int64_t *outValue)

#define PARSING_TOML_F64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_F64)( \
      const PARSING_TYPE(TTOMLValue) *value, \
      double *outValue)

#define PARSING_TOML_BOOL_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_Bool)( \
      const PARSING_TYPE(TTOMLValue) *value, \
      bool *outValue)

#define PARSING_TOML_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_Write)( \
      const PARSING_TYPE(TTOMLDocument) *document, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

#define PARSING_TOML_SET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_Set)( \
      PARSING_TYPE(TTOMLDocument) *document, \
      const char *key, \
      const char *literal, \
      size_t literalSize, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_TOML_REMOVE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(TOML_Remove)( \
      PARSING_TYPE(TTOMLDocument) *document, \
      const char *key)

PARSING_TOML_FREE_PROTOTYPE;
PARSING_TOML_PARSE_PROTOTYPE;
PARSING_TOML_GET_PROTOTYPE;
PARSING_TOML_AT_PROTOTYPE;
PARSING_TOML_STRING_PROTOTYPE;
PARSING_TOML_I64_PROTOTYPE;
PARSING_TOML_F64_PROTOTYPE;
PARSING_TOML_BOOL_PROTOTYPE;
PARSING_TOML_WRITE_PROTOTYPE;
PARSING_TOML_SET_PROTOTYPE;
PARSING_TOML_REMOVE_PROTOTYPE;

#include "Impl/TOML.impl"
