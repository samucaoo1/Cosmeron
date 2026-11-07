#pragma once
#include "../Common/Common.h"

typedef enum PARSING_TYPE(JSONKind) {
  PARSING_CONST(JSON_NULL), PARSING_CONST(JSON_BOOL), PARSING_CONST(JSON_NUMBER),
  PARSING_CONST(JSON_STRING), PARSING_CONST(JSON_ARRAY), PARSING_CONST(JSON_OBJECT)
} PARSING_TYPE(JSONKind);
typedef struct PARSING_TYPE(TJSONValue) {
  PARSING_TYPE(JSONKind) kind;
  char *text, *key; size_t textSize, keySize, count;
  bool boolean;
  struct PARSING_TYPE(TJSONValue) *parent, *first, *last, *next;
} PARSING_TYPE(TJSONValue);
typedef struct PARSING_TYPE(TJSONDocument) { PARSING_TYPE(TJSONValue) *root; } PARSING_TYPE(TJSONDocument);

/* Pull reader: raw, borrowed token spans; decoded strings use TokenWrite. */
typedef enum PARSING_TYPE(JSONTokenKind) {
  PARSING_CONST(JSON_TOKEN_OBJECT_BEGIN), PARSING_CONST(JSON_TOKEN_OBJECT_END),
  PARSING_CONST(JSON_TOKEN_ARRAY_BEGIN), PARSING_CONST(JSON_TOKEN_ARRAY_END),
  PARSING_CONST(JSON_TOKEN_KEY), PARSING_CONST(JSON_TOKEN_STRING),
  PARSING_CONST(JSON_TOKEN_NUMBER), PARSING_CONST(JSON_TOKEN_BOOL), PARSING_CONST(JSON_TOKEN_NULL)
} PARSING_TYPE(JSONTokenKind);
typedef struct PARSING_TYPE(TJSONToken) { PARSING_TYPE(JSONTokenKind) kind; PARSING_TYPE(TSlice) text; size_t depth; } PARSING_TYPE(TJSONToken);
typedef enum PARSING_INS(JSONReadState) {
  PARSING_INS(JSON_OBJECT_FIRST), PARSING_INS(JSON_OBJECT_KEY), PARSING_INS(JSON_OBJECT_VALUE), PARSING_INS(JSON_OBJECT_COMMA),
  PARSING_INS(JSON_ARRAY_FIRST), PARSING_INS(JSON_ARRAY_VALUE), PARSING_INS(JSON_ARRAY_COMMA)
} PARSING_INS(JSONReadState);
typedef struct PARSING_TYPE(TJSONReader) {
  PARSING_INS(Cursor) cursor; PARSING_INS(JSONReadState) stack[PARSING_CONST(DEFAULT_DEPTH)]; size_t depth; bool rootSeen;
} PARSING_TYPE(TJSONReader);

#define PARSING_JSON_FREE_PROTOTYPE \
  static inline void PARSING_FUNC(JSON_Free)( \
      PARSING_TYPE(TJSONDocument) *document)

#define PARSING_JSON_PARSE_WITH_LIMITS_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_ParseWithLimits)( \
      const char *source, \
      size_t sourceSize, \
      const PARSING_TYPE(TLimits) *limits, \
      PARSING_TYPE(TJSONDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_JSON_PARSE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Parse)( \
      const char *source, \
      size_t sourceSize, \
      PARSING_TYPE(TJSONDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_JSON_ROOT_PROTOTYPE \
  static inline const PARSING_TYPE(TJSONValue) *PARSING_FUNC(JSON_Root)( \
      const PARSING_TYPE(TJSONDocument) *document)

#define PARSING_JSON_SIZE_PROTOTYPE \
  static inline size_t PARSING_FUNC(JSON_Size)( \
      const PARSING_TYPE(TJSONValue) *value)

#define PARSING_JSON_GET_SLICE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_GetSlice)( \
      const PARSING_TYPE(TJSONValue) *object, \
      PARSING_TYPE(TSlice) key, \
      const PARSING_TYPE(TJSONValue) **outValue)

#define PARSING_JSON_GET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Get)( \
      const PARSING_TYPE(TJSONValue) *object, \
      const char *key, \
      const PARSING_TYPE(TJSONValue) **outValue)

#define PARSING_JSON_AT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_At)( \
      const PARSING_TYPE(TJSONValue) *array, \
      size_t index, \
      const PARSING_TYPE(TJSONValue) **outValue)

#define PARSING_JSON_STRING_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_String)( \
      const PARSING_TYPE(TJSONValue) *value, \
      PARSING_TYPE(TSlice) *outValue)

#define PARSING_JSON_I64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_I64)( \
      const PARSING_TYPE(TJSONValue) *value, \
      int64_t *outValue)

#define PARSING_JSON_U64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_U64)( \
      const PARSING_TYPE(TJSONValue) *value, \
      uint64_t *outValue)

#define PARSING_JSON_F64_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_F64)( \
      const PARSING_TYPE(TJSONValue) *value, \
      double *outValue)

#define PARSING_JSON_BOOL_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Bool)( \
      const PARSING_TYPE(TJSONValue) *value, \
      bool *outValue)

#define PARSING_JSON_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Write)( \
      const PARSING_TYPE(TJSONDocument) *document, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

#define PARSING_JSON_SET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Set)( \
      PARSING_TYPE(TJSONValue) *object, \
      const char *key, \
      const char *json, \
      size_t jsonSize, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_JSON_APPEND_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Append)( \
      PARSING_TYPE(TJSONValue) *array, \
      const char *json, \
      size_t jsonSize, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_JSON_REMOVE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Remove)( \
      PARSING_TYPE(TJSONValue) *object, \
      const char *key)

#define PARSING_JSON_READER_CREATE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_ReaderCreate)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TJSONReader) *outReader, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_JSON_NEXT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_Next)( \
      PARSING_TYPE(TJSONReader) *reader, \
      PARSING_TYPE(TJSONToken) *outToken, \
      bool *outEnd)

#define PARSING_JSON_TOKEN_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_TokenWrite)( \
      const PARSING_TYPE(TJSONToken) *token, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

PARSING_JSON_FREE_PROTOTYPE;
PARSING_JSON_PARSE_WITH_LIMITS_PROTOTYPE;
PARSING_JSON_PARSE_PROTOTYPE;
PARSING_JSON_ROOT_PROTOTYPE;
PARSING_JSON_SIZE_PROTOTYPE;
PARSING_JSON_GET_SLICE_PROTOTYPE;
PARSING_JSON_GET_PROTOTYPE;
PARSING_JSON_AT_PROTOTYPE;
PARSING_JSON_STRING_PROTOTYPE;
PARSING_JSON_I64_PROTOTYPE;
PARSING_JSON_U64_PROTOTYPE;
PARSING_JSON_F64_PROTOTYPE;
PARSING_JSON_BOOL_PROTOTYPE;
PARSING_JSON_WRITE_PROTOTYPE;
PARSING_JSON_SET_PROTOTYPE;
PARSING_JSON_APPEND_PROTOTYPE;
PARSING_JSON_REMOVE_PROTOTYPE;
PARSING_JSON_READER_CREATE_PROTOTYPE;
PARSING_JSON_NEXT_PROTOTYPE;
PARSING_JSON_TOKEN_WRITE_PROTOTYPE;

#define PARSING_JSON_GET_MUTABLE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_GetMutable)( \
      PARSING_TYPE(TJSONValue) *object, \
      const char *key, \
      PARSING_TYPE(TJSONValue) **outValue)
PARSING_JSON_GET_MUTABLE_PROTOTYPE;

#define PARSING_JSON_AT_MUTABLE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(JSON_AtMutable)( \
      PARSING_TYPE(TJSONValue) *array, \
      size_t index, \
      PARSING_TYPE(TJSONValue) **outValue)
PARSING_JSON_AT_MUTABLE_PROTOTYPE;

#include "Impl/JSON.impl"
