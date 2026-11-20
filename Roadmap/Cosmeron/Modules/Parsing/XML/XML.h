#pragma once
#include "../Common/Common.h"
#include "../../Container/Array/Vector.h"

typedef enum PARSING_TYPE(XMLKind) {
  PARSING_CONST(XML_BEGIN), PARSING_CONST(XML_ATTRIBUTE), PARSING_CONST(XML_TEXT),
  PARSING_CONST(XML_END), PARSING_CONST(XML_COMMENT), PARSING_CONST(XML_CDATA), PARSING_CONST(XML_PI)
} PARSING_TYPE(XMLKind);
typedef struct PARSING_TYPE(TXMLEvent) { PARSING_TYPE(XMLKind) kind; PARSING_TYPE(TSlice) name, text; size_t depth; } PARSING_TYPE(TXMLEvent);
typedef struct PARSING_TYPE(TXMLReader) {
  PARSING_INS(Cursor) cursor;
  PARSING_TYPE(TSlice) stack[PARSING_CONST(DEFAULT_DEPTH)];
  size_t depth, attributeStart; bool inTag, rootSeen, rootClosed;
} PARSING_TYPE(TXMLReader);
FLAT_VECTOR_IMPLEMENT_ALL(PARSING_TYPE(TXMLEvent), PARSING_INS(XMLEvents))
typedef struct PARSING_TYPE(TXMLDocument) { FLAT_VECTOR_TYPE(PARSING_INS(XMLEvents)) events; } PARSING_TYPE(TXMLDocument);

#define PARSING_XML_READER_CREATE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_ReaderCreate)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TXMLReader) *outReader, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_XML_NEXT_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_Next)( \
      PARSING_TYPE(TXMLReader) *reader, \
      PARSING_TYPE(TXMLEvent) *outEvent, \
      bool *outEnd)

#define PARSING_XML_TEXT_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_TextWrite)( \
      const PARSING_TYPE(TXMLEvent) *event, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

#define PARSING_XML_FREE_PROTOTYPE \
  static inline void PARSING_FUNC(XML_Free)( \
      PARSING_TYPE(TXMLDocument) *document)

#define PARSING_XML_PARSE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_Parse)( \
      const char *source, \
      size_t size, \
      PARSING_TYPE(TXMLDocument) *outDocument, \
      PARSING_TYPE(TDiagnostic) *diagnostic)

#define PARSING_XML_ATTRIBUTE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_Attribute)( \
      const PARSING_TYPE(TXMLDocument) *document, \
      size_t elementIndex, \
      const char *name, \
      PARSING_TYPE(TSlice) *outValue)

#define PARSING_XML_SET_ATTRIBUTE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_SetAttribute)( \
      PARSING_TYPE(TXMLDocument) *document, \
      size_t elementIndex, \
      const char *name, \
      PARSING_TYPE(TSlice) value)

#define PARSING_XML_WRITE_PROTOTYPE \
  static inline OPSTATUS PARSING_FUNC(XML_Write)( \
      const PARSING_TYPE(TXMLDocument) *document, \
      char *buffer, \
      size_t capacity, \
      size_t *outSize)

PARSING_XML_READER_CREATE_PROTOTYPE;
PARSING_XML_NEXT_PROTOTYPE;
PARSING_XML_TEXT_WRITE_PROTOTYPE;
PARSING_XML_FREE_PROTOTYPE;
PARSING_XML_PARSE_PROTOTYPE;
PARSING_XML_ATTRIBUTE_PROTOTYPE;
PARSING_XML_SET_ATTRIBUTE_PROTOTYPE;
PARSING_XML_WRITE_PROTOTYPE;

#include "Impl/XML.impl"
