#pragma once
#include "../Common/Common.h"
#include "../../Container/Array/Vector.h"

typedef struct PARSING_TYPE(TCSVField) { PARSING_TYPE(TSlice) text; bool quoted, recordEnd; size_t recordIndex, fieldIndex; } PARSING_TYPE(TCSVField);
typedef struct PARSING_TYPE(TCSVReader) { PARSING_INS(Cursor) cursor; char delimiter; size_t recordIndex, fieldIndex; bool pendingEmpty; } PARSING_TYPE(TCSVReader);
typedef struct PARSING_TYPE(TCSVRecord) { PARSING_TYPE(TSlice) *fields; size_t count; } PARSING_TYPE(TCSVRecord);
/* Reuse Container's vector implementation for the owned record store. */
FLAT_VECTOR_IMPLEMENT_ALL(PARSING_TYPE(TCSVRecord), PARSING_INS(CSVRecords))
typedef struct PARSING_TYPE(TCSVDocument) { FLAT_VECTOR_TYPE(PARSING_INS(CSVRecords)) records; char delimiter; } PARSING_TYPE(TCSVDocument);

#define PARSING_CSV_READER_CREATE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_ReaderCreate)( \
      const char *source, \
      size_t size, \
      char delimiter, \
      PARSING_TYPE(TCSVReader) *outReader, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_CSV_NEXT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_Next)( \
      PARSING_TYPE(TCSVReader) *reader, \
      PARSING_TYPE(TCSVField) *outField, \
      bool *outEnd)

#define PARSING_CSV_FIELD_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_FieldWrite)( \
      const PARSING_TYPE(TCSVField) *field, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

#define PARSING_CSV_FREE_PROTOTYPE \
  static inline void PARSING_FUNC(CSV_Free)( \
      PARSING_TYPE(TCSVDocument) *document)

#define PARSING_CSV_PARSE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_Parse)( \
      const char *source, \
      size_t size, \
      char delimiter, \
      PARSING_TYPE(TCSVDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_CSV_FIELD_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_Field)( \
      const PARSING_TYPE(TCSVRecord) *record, \
      size_t index, \
      PARSING_TYPE(TSlice) *outValue)

#define PARSING_CSV_SET_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_Set)( \
      PARSING_TYPE(TCSVDocument) *document, \
      size_t recordIndex, \
      size_t fieldIndex, \
      PARSING_TYPE(TSlice) value)

#define PARSING_CSV_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(CSV_Write)( \
      const PARSING_TYPE(TCSVDocument) *document, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

PARSING_CSV_READER_CREATE_PROTOTYPE;
PARSING_CSV_NEXT_PROTOTYPE;
PARSING_CSV_FIELD_WRITE_PROTOTYPE;
PARSING_CSV_FREE_PROTOTYPE;
PARSING_CSV_PARSE_PROTOTYPE;
PARSING_CSV_FIELD_PROTOTYPE;
PARSING_CSV_SET_PROTOTYPE;
PARSING_CSV_WRITE_PROTOTYPE;

#include "Impl/CSV.impl"
