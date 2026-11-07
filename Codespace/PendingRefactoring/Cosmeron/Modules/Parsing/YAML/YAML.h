#pragma once
#include "../JSON/JSON.h"

typedef enum PARSING_TYPE(YAMLKind) { PARSING_CONST(YAML_SCALAR), PARSING_CONST(YAML_MAPPING), PARSING_CONST(YAML_SEQUENCE) } PARSING_TYPE(YAMLKind);
typedef struct PARSING_TYPE(TYAMLValue) {
  PARSING_TYPE(YAMLKind) kind;
  char *key, *text; size_t keySize, textSize, count;
  bool quoted; /* No implicit schema: unquoted scalars retain their spelling. */
  struct PARSING_TYPE(TYAMLValue) *first, *last, *next;
} PARSING_TYPE(TYAMLValue);
typedef struct PARSING_TYPE(TYAMLDocument) { PARSING_TYPE(TYAMLValue) *root; } PARSING_TYPE(TYAMLDocument);

#define PARSING_YAML_FREE_PROTOTYPE \
  static inline void PARSING_FUNC(YAML_Free)( \
      PARSING_TYPE(TYAMLDocument) *document)

#define PARSING_YAML_PARSE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_Parse)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TYAMLDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_YAML_GET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_Get)( \
      const PARSING_TYPE(TYAMLValue) *mapping, \
      const char *key, \
      const PARSING_TYPE(TYAMLValue) **outValue)

#define PARSING_YAML_AT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_At)( \
      const PARSING_TYPE(TYAMLValue) *sequence, \
      size_t index, \
      const PARSING_TYPE(TYAMLValue) **outValue)

#define PARSING_YAML_SCALAR_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_Scalar)( \
      const PARSING_TYPE(TYAMLValue) *value, \
      PARSING_TYPE(TSlice) *outValue)

#define PARSING_YAML_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_Write)( \
      const PARSING_TYPE(TYAMLDocument) *document, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

#define PARSING_YAML_SET_SCALAR_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_SetScalar)( \
      PARSING_TYPE(TYAMLValue) *value, \
      PARSING_TYPE(TSlice) text)

PARSING_YAML_FREE_PROTOTYPE;
PARSING_YAML_PARSE_PROTOTYPE;
PARSING_YAML_GET_PROTOTYPE;
PARSING_YAML_AT_PROTOTYPE;
PARSING_YAML_SCALAR_PROTOTYPE;
PARSING_YAML_WRITE_PROTOTYPE;
PARSING_YAML_SET_SCALAR_PROTOTYPE;

#define PARSING_YAML_GET_MUTABLE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_GetMutable)( \
      PARSING_TYPE(TYAMLValue) *object, \
      const char *key, \
      PARSING_TYPE(TYAMLValue) **outValue)
PARSING_YAML_GET_MUTABLE_PROTOTYPE;

#define PARSING_YAML_AT_MUTABLE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(YAML_AtMutable)( \
      PARSING_TYPE(TYAMLValue) *array, \
      size_t index, \
      PARSING_TYPE(TYAMLValue) **outValue)
PARSING_YAML_AT_MUTABLE_PROTOTYPE;

#include "Impl/YAML.impl"
