# Parsing

The **Parsing** module reads, queries, edits and serializes structured text. It is header-only C11 and requires no additional linker flags. Each format has its own representation; XML preserves the ordered mixed-content event stream rather than forcing it into a JSON-like object tree.

```c
#include "Cosmeron/Modules/Parsing/Parsing.h"
/* Or include only one package: */
#include "Cosmeron/Modules/Parsing/JSON/JSON.h"
```

# Overview

| Package | Initial implementation |
| --- | --- |
| Common | Sized text slices, strict conversions, diagnostics and limits |
| INI | Sections and textual keys/values; reader, document, edit and write |
| JSON | Strict grammar, Unicode escapes, owned document, edit/write and pull reader |
| CSV | Configurable ASCII delimiter, quoted multiline fields; TSV uses tab |
| TOML | Configuration profile: tables, dotted keys, arrays, inline tables and scalars |
| XML | UTF-8 XML 1.0 profile without DTD; reader and ordered document |
| YAML | Single-document block profile with mappings, sequences and text scalars |

**TOML, YAML and XML are documented profiles, not claims of complete standards conformance.** Unsupported features are rejected rather than skipped. Incremental chunk input, custom arenas, multiline YAML flow syntax/aliases, TOML date/time values and exact source-format preservation remain future extensions. There is no automatic cross-format conversion in this release.

## Dependencies and namespaces

Common reuses Core/Error, Core/Memory and Text/Encoding/UTF8. CSV and XML reuse Container/Array/Vector for their owned stores. TOML and YAML reuse JSON's escape decoder and string quoting without sharing its public document model. No third-party parser code is vendored.

| Macro | Default C identifier |
| --- | --- |
| `PARSING_FUNC(JSON_Parse)` | `Parsing_JSON_Parse` |
| `PARSING_TYPE(TJSONDocument)` | `Parsing_TJSONDocument` |
| `PARSING_TYPE(TSlice)` | `Parsing_TSlice` |
| `PARSING_CONST(JSON_OBJECT)` | `PARSING_JSON_OBJECT` |

Namespace overrides follow `COSMERON_NAMESPACE`, `COSMERON_NAMESPACE_CONST`, `PARSING_NS` and `PARSING_CNS`. Public signatures use `PARSING_*_PROTOTYPE`; fixed functions do not introduce redundant implementation aliases. Define `PARSING_DISABLE_FUNCTION_TABLE` to omit the module FunctionTable. Container's independent `CONTAINER_DISABLE_FUNCTION_TABLE` switch remains supported.

## Ownership and outputs

Initialize every document to `{0}` before Parse. Parse requires an empty document, copies the information it retains and publishes the owner only on success. After success the source buffer can be freed. Documents must not be shallow-copied as independent owners. Use the corresponding Free function; Free resets the document and may be called again.

Value pointers, record fields and document slices borrow storage from their document. Treat exposed representation fields as read-only and use editing functions; directly modifying links, counts or ownership pointers violates the contract. Replacing/removing a value invalidates its borrowed pointers. Vector insertion may invalidate event/record addresses and shift indices.

Readers borrow the entire source buffer and retain no heap allocations. Reader diagnostics are borrowed too: keep the diagnostic object alive while advancing the reader. JSON, INI, CSV and XML expose pull readers. TOML/YAML currently expose owned documents only.

Required outputs change only on success. Exceptions are explicit: all Write/TokenWrite/FieldWrite/TextWrite operations report `outSize` on `INSUFFICIENT_SPACE`, and optional diagnostics report failure details. Buffer size excludes the terminating NUL, so capacity must be size + 1. NULL buffer with zero capacity queries the size. Insufficient buffers remain unchanged. Serialized output contains its own byte count; use that count when strings can contain NUL.

Document serialization normalizes syntax and does not preserve source comments/spacing, except that XML comments and processing instructions are retained as document events. Writers currently construct a temporary output buffer before copying it to the caller's buffer.

## Diagnostics and limits

`TDiagnostic` reports a namespaced code, a zero-based byte offset and one-based line/column. Columns count bytes; CRLF counts as one line break. Codes distinguish syntax, encoding, duplicate keys, depth, size/node limits and unsupported features. A diagnostic is cleared on successful Parse. Reader end-of-input returns SUCCESS with `outEnd = true` and leaves the entry/token output unchanged.

Inputs are explicit pointer/size pairs, validated as UTF-8. A leading UTF-8 BOM is tolerated. NULL source is valid only for size 0. The default limits are 64 levels, 100,000 values/events and 16 MiB of source. JSON_ParseWithLimits can lower/adjust node and byte budgets; depth must be within 1..64. Byte limits bound the source, not total heap usage. Editing enforces structural depth but does not maintain an aggregate document node/byte budget.

# Usage

## JSON document

```c
const char source[] = "{\"width\":1920,\"enabled\":true}";
PARSING_TYPE(TJSONDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(JSON_Parse)(
    source, sizeof(source) - 1, &document, &diagnostic
);
if (status == STATUS_CONST(SUCCESS)) {
    const PARSING_TYPE(TJSONValue) *value;
    uint64_t width;
    status = PARSING_FUNC(JSON_Get)(
        PARSING_FUNC(JSON_Root)(&document), "width", &value
    );
    if (status == STATUS_CONST(SUCCESS))
        status = PARSING_FUNC(JSON_U64)(value, &width);
}
PARSING_FUNC(JSON_Free)(&document);
```

Objects reject duplicate decoded keys. Numbers retain their exact lexical form; integer/float conversion is explicit and bounded. Strings carry size and can contain escaped NUL. JSON_GetSlice supports keys containing NUL. Set/Append accept JSON source for the new value; replacement is transactional. Set uses a C-string key; Remove removes an object member.

The pull reader returns begin/end container tokens, keys and scalar tokens. String/key spans contain the raw escaped content, without enclosing quotes; JSON_TokenWrite decodes them. The allocation-free reader validates grammar and Unicode escapes but does not maintain an object-key table to reject duplicates. Use the owned parser when duplicate-key validation is needed. Fully consume the reader to validate the entire source.

## INI profile

Section headers use `[section]`; entries use `key=value`. Surrounding whitespace is trimmed. Lines beginning with `;` or `#` are comments; section headers may have a trailing comment. Values are literal text: quotes have no special interpretation, and embedded `;`/`#` remain part of the value. Empty values are supported. Empty section names, malformed headers and duplicate section/key pairs are rejected by the document parser. The reader emits each entry with its current section and leaves duplicate policy to the caller.

INI_Set inserts/replaces a value. Its C-string inputs must be representable without a newline or trimming, and keys/sections must not contain structural delimiters. INI_Write groups sections and emits global keys first. Empty sections without entries are not retained.

## CSV/TSV

The reader emits one field at a time, with recordIndex, fieldIndex and recordEnd. Its text span excludes enclosing quotes but retains doubled quote escapes; CSV_FieldWrite decodes those escapes. The document stores decoded fields and records in a Container vector. CSV_Field queries a decoded field; CSV_Set replaces one existing field.

Delimiter may be any non-NUL ASCII character except quote, CR or LF. Use comma for CSV, tab for TSV or semicolon for a local dialect. Quoted fields support embedded CR/LF and doubled quotes. Quotes inside unquoted fields and characters after a closing quote are rejected. CRLF, CR and LF record endings are accepted; output uses CRLF. Empty input has zero records, a blank line is one empty field, and a trailing delimiter creates a final empty field. Header rows remain ordinary records, and record widths may differ.

## TOML configuration profile

Supported: bare/quoted keys, dotted assignment keys, explicit/dotted tables, single-line and multiline basic/literal strings, decimal/hex/octal/binary integers, underscores between digits, decimal floats, booleans, arrays (including multiline arrays/trailing commas), nested arrays, arrays of tables and inline tables with dotted keys (including TOML 1.1 multiline/trailing commas). Integers are bounded to signed 64-bit. Scalar spelling is retained; TOML_F64 converts finite decimal floats. `inf` and `nan` are retained as float literals, with numeric conversion returning NOT_SUPPORTED.

Basic strings support literal tabs and all TOML 1.1 escapes (`\u`, `\U`, `\x`, `\e` included). Multiline strings normalize CRLF to LF, trim the initial newline and implement line-ending backslash folding. Quoted keys remain single-line. Invalid Unicode scalar escapes and escaped slash are syntax errors. Datetime values remain unsupported; this is not a complete TOML 1.0/1.1 parser.

TOML_Get navigates one literal key at a time, without interpreting dots inside its argument. TOML_At indexes arrays. TOML_Set/Remove edit a root-table member; Set takes one TOML value literal. Serialization emits root assignments and nested inline tables, preserving the data rather than its original section layout.

## XML profile

Supported: UTF-8 XML 1.0 declarations, Unicode XML names, elements, quoted attributes, matched closing tags, empty elements, mixed text, predefined/numeric character references, comments, CDATA and processing instructions. Input line endings and attribute whitespace are normalized; character references preserve explicitly escaped attribute whitespace. Exactly one root element is required.

DTD declarations and non-predefined named entities return NOT_SUPPORTED; external resources are never fetched. Names containing namespace prefixes and xmlns attributes are preserved lexically, without resolving namespace URIs or validating prefix bindings. There is no DTD/schema validation, XPath or UTF-16 decoding.

XML_Next exposes raw borrowed events. XML_TextWrite decodes raw event text. XML_Parse stores decoded events in their original order; do not call XML_TextWrite on those already decoded document events. XML_Attribute looks up a qualified attribute name using its element's BEGIN event index. XML_SetAttribute replaces/inserts a decoded attribute, and may shift subsequent event indices. XML_Write escapes decoded text and may emit CDATA as escaped text; both preserve the same character data.

## YAML block profile

Supported: one optional leading `---`, block mappings with textual keys, block sequences, nested indentation, single-line plain/single-quoted/double-quoted scalars, doubled apostrophes, all single-line YAML double-quoted escapes, comments and single-line flow `{}`/`[]` collections (nested values, textual keys and trailing commas). Indentation uses spaces. Duplicate textual keys are rejected.

This representation has **no implicit scalar schema**: `true`, `null`, dates and numbers retain their spelling as unquoted scalars. Use Common conversions explicitly when your application requires them. Empty mapping/sequence values become the plain scalar `null`. Quoted status is retained so serialization preserves string intent. YAML_SetScalar writes a quoted textual scalar.

Unsupported: multiline flow collections, implicit flow mapping entries/pairs, inline mapping entries after a sequence dash, multiline quoted/block scalars, complex/empty keys, tags, anchors, aliases, directives, end markers and multi-document streams. These features return an error rather than being silently dropped. YAML_Write emits normalized block indentation and quoted keys.

# File integration

File owns the I/O operations; Parsing owns interpretation. See `Codespace/Examples/Parsing/Configuration.c` for loading a JSON file, freeing its source buffer, querying the owned document and serializing back through File_SaveAtomic.

# API reference

Required output pointers must be non-NULL. Mutable getters produce borrowed mutable handles for the editing operations.

## CSV

```c
static inline OPSTATUS PARSING_FUNC(CSV_ReaderCreate)(
      const char *source,
      size_t size,
      char delimiter,
      PARSING_TYPE(TCSVReader) *outReader,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(CSV_Next)(
      PARSING_TYPE(TCSVReader) *reader,
      PARSING_TYPE(TCSVField) *outField,
      bool *outEnd);

static inline OPSTATUS PARSING_FUNC(CSV_FieldWrite)(
      const PARSING_TYPE(TCSVField) *field,
      char *buffer,
      size_t capacity,
      size_t *outSize);

static inline void PARSING_FUNC(CSV_Free)(
      PARSING_TYPE(TCSVDocument) *document);

static inline OPSTATUS PARSING_FUNC(CSV_Parse)(
      const char *source,
      size_t size,
      char delimiter,
      PARSING_TYPE(TCSVDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(CSV_Field)(
      const PARSING_TYPE(TCSVRecord) *record,
      size_t index,
      PARSING_TYPE(TSlice) *outValue);

static inline OPSTATUS PARSING_FUNC(CSV_Set)(
      PARSING_TYPE(TCSVDocument) *document,
      size_t recordIndex,
      size_t fieldIndex,
      PARSING_TYPE(TSlice) value);

static inline OPSTATUS PARSING_FUNC(CSV_Write)(
      const PARSING_TYPE(TCSVDocument) *document,
      char *buffer,
      size_t capacity,
      size_t *outSize);

```

## Common

```c
static inline PARSING_TYPE(TSlice) PARSING_FUNC(Slice_Create)(
      const char *data,
      size_t size);

static inline bool PARSING_FUNC(Slice_Equal)(
      PARSING_TYPE(TSlice) left,
      PARSING_TYPE(TSlice) right);

static inline OPSTATUS PARSING_FUNC(Slice_U64)(
      PARSING_TYPE(TSlice) text,
      uint64_t *outValue);

static inline OPSTATUS PARSING_FUNC(Slice_I64)(
      PARSING_TYPE(TSlice) text,
      int64_t *outValue);

static inline OPSTATUS PARSING_FUNC(Slice_F64)(
      PARSING_TYPE(TSlice) text,
      double *outValue);

static inline OPSTATUS PARSING_FUNC(Slice_Bool)(
      PARSING_TYPE(TSlice) text,
      bool *outValue);

```

## INI

```c
static inline OPSTATUS PARSING_FUNC(INI_ReaderCreate)(
      const char *source,
      size_t size,
      PARSING_TYPE(TINIReader) *outReader,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(INI_Next)(
      PARSING_TYPE(TINIReader) *reader,
      PARSING_TYPE(TINIEntry) *outEntry,
      bool *outEnd);

static inline void PARSING_FUNC(INI_Free)(
      PARSING_TYPE(TINIDocument) *document);

static inline OPSTATUS PARSING_FUNC(INI_Parse)(
      const char *source,
      size_t size,
      PARSING_TYPE(TINIDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(INI_Get)(
      const PARSING_TYPE(TINIDocument) *document,
      const char *section,
      const char *key,
      PARSING_TYPE(TSlice) *outValue);

static inline OPSTATUS PARSING_FUNC(INI_Set)(
      PARSING_TYPE(TINIDocument) *document,
      const char *section,
      const char *key,
      const char *value);

static inline OPSTATUS PARSING_FUNC(INI_Remove)(
      PARSING_TYPE(TINIDocument) *document,
      const char *section,
      const char *key);

static inline OPSTATUS PARSING_FUNC(INI_Write)(
      const PARSING_TYPE(TINIDocument) *document,
      char *buffer,
      size_t capacity,
      size_t *outSize);

```

## JSON

```c
static inline void PARSING_FUNC(JSON_Free)(
      PARSING_TYPE(TJSONDocument) *document);

static inline OPSTATUS PARSING_FUNC(JSON_ParseWithLimits)(
      const char *source,
      size_t sourceSize,
      const PARSING_TYPE(TLimits) *limits,
      PARSING_TYPE(TJSONDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(JSON_Parse)(
      const char *source,
      size_t sourceSize,
      PARSING_TYPE(TJSONDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline const PARSING_TYPE(TJSONValue) *PARSING_FUNC(JSON_Root)(
      const PARSING_TYPE(TJSONDocument) *document);

static inline size_t PARSING_FUNC(JSON_Size)(
      const PARSING_TYPE(TJSONValue) *value);

static inline OPSTATUS PARSING_FUNC(JSON_GetSlice)(
      const PARSING_TYPE(TJSONValue) *object,
      PARSING_TYPE(TSlice) key,
      const PARSING_TYPE(TJSONValue) **outValue);

static inline OPSTATUS PARSING_FUNC(JSON_Get)(
      const PARSING_TYPE(TJSONValue) *object,
      const char *key,
      const PARSING_TYPE(TJSONValue) **outValue);

static inline OPSTATUS PARSING_FUNC(JSON_At)(
      const PARSING_TYPE(TJSONValue) *array,
      size_t index,
      const PARSING_TYPE(TJSONValue) **outValue);

static inline OPSTATUS PARSING_FUNC(JSON_String)(
      const PARSING_TYPE(TJSONValue) *value,
      PARSING_TYPE(TSlice) *outValue);

static inline OPSTATUS PARSING_FUNC(JSON_I64)(
      const PARSING_TYPE(TJSONValue) *value,
      int64_t *outValue);

static inline OPSTATUS PARSING_FUNC(JSON_U64)(
      const PARSING_TYPE(TJSONValue) *value,
      uint64_t *outValue);

static inline OPSTATUS PARSING_FUNC(JSON_F64)(
      const PARSING_TYPE(TJSONValue) *value,
      double *outValue);

static inline OPSTATUS PARSING_FUNC(JSON_Bool)(
      const PARSING_TYPE(TJSONValue) *value,
      bool *outValue);

static inline OPSTATUS PARSING_FUNC(JSON_Write)(
      const PARSING_TYPE(TJSONDocument) *document,
      char *buffer,
      size_t capacity,
      size_t *outSize);

static inline OPSTATUS PARSING_FUNC(JSON_Set)(
      PARSING_TYPE(TJSONValue) *object,
      const char *key,
      const char *json,
      size_t jsonSize,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(JSON_Append)(
      PARSING_TYPE(TJSONValue) *array,
      const char *json,
      size_t jsonSize,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(JSON_Remove)(
      PARSING_TYPE(TJSONValue) *object,
      const char *key);

static inline OPSTATUS PARSING_FUNC(JSON_ReaderCreate)(
      const char *source,
      size_t size,
      PARSING_TYPE(TJSONReader) *outReader,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(JSON_Next)(
      PARSING_TYPE(TJSONReader) *reader,
      PARSING_TYPE(TJSONToken) *outToken,
      bool *outEnd);

static inline OPSTATUS PARSING_FUNC(JSON_TokenWrite)(
      const PARSING_TYPE(TJSONToken) *token,
      char *buffer,
      size_t capacity,
      size_t *outSize);

static inline OPSTATUS PARSING_FUNC(JSON_GetMutable)(
      PARSING_TYPE(TJSONValue) *object,
      const char *key,
      PARSING_TYPE(TJSONValue) **outValue);

static inline OPSTATUS PARSING_FUNC(JSON_AtMutable)(
      PARSING_TYPE(TJSONValue) *array,
      size_t index,
      PARSING_TYPE(TJSONValue) **outValue);

```

## TOML

```c
static inline void PARSING_FUNC(TOML_Free)(
      PARSING_TYPE(TTOMLDocument) *document);

static inline OPSTATUS PARSING_FUNC(TOML_Parse)(
      const char *source,
      size_t size,
      PARSING_TYPE(TTOMLDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(TOML_Get)(
      const PARSING_TYPE(TTOMLValue) *table,
      const char *key,
      const PARSING_TYPE(TTOMLValue) **outValue);

static inline OPSTATUS PARSING_FUNC(TOML_At)(
      const PARSING_TYPE(TTOMLValue) *array,
      size_t index,
      const PARSING_TYPE(TTOMLValue) **outValue);

static inline OPSTATUS PARSING_FUNC(TOML_String)(
      const PARSING_TYPE(TTOMLValue) *value,
      PARSING_TYPE(TSlice) *outValue);

static inline OPSTATUS PARSING_FUNC(TOML_I64)(
      const PARSING_TYPE(TTOMLValue) *value,
      int64_t *outValue);

static inline OPSTATUS PARSING_FUNC(TOML_F64)(
      const PARSING_TYPE(TTOMLValue) *value,
      double *outValue);

static inline OPSTATUS PARSING_FUNC(TOML_Bool)(
      const PARSING_TYPE(TTOMLValue) *value,
      bool *outValue);

static inline OPSTATUS PARSING_FUNC(TOML_Write)(
      const PARSING_TYPE(TTOMLDocument) *document,
      char *buffer,
      size_t capacity,
      size_t *outSize);

static inline OPSTATUS PARSING_FUNC(TOML_Set)(
      PARSING_TYPE(TTOMLDocument) *document,
      const char *key,
      const char *literal,
      size_t literalSize,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(TOML_Remove)(
      PARSING_TYPE(TTOMLDocument) *document,
      const char *key);

```

## XML

```c
static inline OPSTATUS PARSING_FUNC(XML_ReaderCreate)(
      const char *source,
      size_t size,
      PARSING_TYPE(TXMLReader) *outReader,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(XML_Next)(
      PARSING_TYPE(TXMLReader) *reader,
      PARSING_TYPE(TXMLEvent) *outEvent,
      bool *outEnd);

static inline OPSTATUS PARSING_FUNC(XML_TextWrite)(
      const PARSING_TYPE(TXMLEvent) *event,
      char *buffer,
      size_t capacity,
      size_t *outSize);

static inline void PARSING_FUNC(XML_Free)(
      PARSING_TYPE(TXMLDocument) *document);

static inline OPSTATUS PARSING_FUNC(XML_Parse)(
      const char *source,
      size_t size,
      PARSING_TYPE(TXMLDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(XML_Attribute)(
      const PARSING_TYPE(TXMLDocument) *document,
      size_t elementIndex,
      const char *name,
      PARSING_TYPE(TSlice) *outValue);

static inline OPSTATUS PARSING_FUNC(XML_SetAttribute)(
      PARSING_TYPE(TXMLDocument) *document,
      size_t elementIndex,
      const char *name,
      PARSING_TYPE(TSlice) value);

static inline OPSTATUS PARSING_FUNC(XML_Write)(
      const PARSING_TYPE(TXMLDocument) *document,
      char *buffer,
      size_t capacity,
      size_t *outSize);

```

## YAML

```c
static inline void PARSING_FUNC(YAML_Free)(
      PARSING_TYPE(TYAMLDocument) *document);

static inline OPSTATUS PARSING_FUNC(YAML_Parse)(
      const char *source,
      size_t size,
      PARSING_TYPE(TYAMLDocument) *outDocument,
      PARSING_TYPE(TDiagnostic) *diagnostic);

static inline OPSTATUS PARSING_FUNC(YAML_Get)(
      const PARSING_TYPE(TYAMLValue) *mapping,
      const char *key,
      const PARSING_TYPE(TYAMLValue) **outValue);

static inline OPSTATUS PARSING_FUNC(YAML_At)(
      const PARSING_TYPE(TYAMLValue) *sequence,
      size_t index,
      const PARSING_TYPE(TYAMLValue) **outValue);

static inline OPSTATUS PARSING_FUNC(YAML_Scalar)(
      const PARSING_TYPE(TYAMLValue) *value,
      PARSING_TYPE(TSlice) *outValue);

static inline OPSTATUS PARSING_FUNC(YAML_Write)(
      const PARSING_TYPE(TYAMLDocument) *document,
      char *buffer,
      size_t capacity,
      size_t *outSize);

static inline OPSTATUS PARSING_FUNC(YAML_SetScalar)(
      PARSING_TYPE(TYAMLValue) *value,
      PARSING_TYPE(TSlice) text);

static inline OPSTATUS PARSING_FUNC(YAML_GetMutable)(
      PARSING_TYPE(TYAMLValue) *object,
      const char *key,
      PARSING_TYPE(TYAMLValue) **outValue);

static inline OPSTATUS PARSING_FUNC(YAML_AtMutable)(
      PARSING_TYPE(TYAMLValue) *array,
      size_t index,
      PARSING_TYPE(TYAMLValue) **outValue);

```
