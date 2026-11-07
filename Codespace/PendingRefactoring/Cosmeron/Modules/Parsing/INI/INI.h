#pragma once
#include "../Common/Common.h"

typedef struct PARSING_TYPE(TINIEntry) { PARSING_TYPE(TSlice) section, key, value; } PARSING_TYPE(TINIEntry);
typedef struct PARSING_TYPE(TINIReader) { PARSING_INS(Cursor) cursor; PARSING_TYPE(TSlice) section; } PARSING_TYPE(TINIReader);
typedef struct PARSING_INS(ININode) {
  PARSING_TYPE(TINIEntry) entry; struct PARSING_INS(ININode) *next;
} PARSING_INS(ININode);
typedef struct PARSING_TYPE(TINIDocument) { PARSING_INS(ININode) *first, *last; size_t count; } PARSING_TYPE(TINIDocument);

#define PARSING_INI_READER_CREATE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_ReaderCreate)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TINIReader) *outReader, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_INI_NEXT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_Next)( \
      PARSING_TYPE(TINIReader) *reader, \
      PARSING_TYPE(TINIEntry) *outEntry, \
      bool *outEnd)

#define PARSING_INI_FREE_PROTOTYPE \
  static inline void PARSING_FUNC(INI_Free)( \
      PARSING_TYPE(TINIDocument) *document)

#define PARSING_INI_PARSE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_Parse)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TINIDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_INI_GET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_Get)( \
      const PARSING_TYPE(TINIDocument) *document, \
      const char *section, \
      const char *key, \
      PARSING_TYPE(TSlice) *outValue)

#define PARSING_INI_SET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_Set)( \
      PARSING_TYPE(TINIDocument) *document, \
      const char *section, \
      const char *key, \
      const char *value)

#define PARSING_INI_REMOVE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_Remove)( \
      PARSING_TYPE(TINIDocument) *document, \
      const char *section, \
      const char *key)

#define PARSING_INI_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(INI_Write)( \
      const PARSING_TYPE(TINIDocument) *document, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

PARSING_INI_READER_CREATE_PROTOTYPE;
PARSING_INI_NEXT_PROTOTYPE;
PARSING_INI_FREE_PROTOTYPE;
PARSING_INI_PARSE_PROTOTYPE;
PARSING_INI_GET_PROTOTYPE;
PARSING_INI_SET_PROTOTYPE;
PARSING_INI_REMOVE_PROTOTYPE;
PARSING_INI_WRITE_PROTOTYPE;

#include "Impl/INI.impl"
