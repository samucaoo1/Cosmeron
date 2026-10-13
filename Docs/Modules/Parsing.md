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

Reference signatures follow the public Parsing headers. Each example is a usage fragment; named reader, document, object, array, record, limits or value handles not declared locally must have been obtained from a preceding successful call.

## Function summary

| Package | Function | Return |
| --- | --- | --- |
| CSV | `PARSING_FUNC(CSV_ReaderCreate)` | `OPSTATUS` |
| CSV | `PARSING_FUNC(CSV_Next)` | `OPSTATUS` |
| CSV | `PARSING_FUNC(CSV_FieldWrite)` | `OPSTATUS` |
| CSV | `PARSING_FUNC(CSV_Free)` | `void` |
| CSV | `PARSING_FUNC(CSV_Parse)` | `OPSTATUS` |
| CSV | `PARSING_FUNC(CSV_Field)` | `OPSTATUS` |
| CSV | `PARSING_FUNC(CSV_Set)` | `OPSTATUS` |
| CSV | `PARSING_FUNC(CSV_Write)` | `OPSTATUS` |
| Common | `PARSING_FUNC(Slice_Create)` | `PARSING_TYPE(TSlice)` |
| Common | `PARSING_FUNC(Slice_Equal)` | `bool` |
| Common | `PARSING_FUNC(Slice_U64)` | `OPSTATUS` |
| Common | `PARSING_FUNC(Slice_I64)` | `OPSTATUS` |
| Common | `PARSING_FUNC(Slice_F64)` | `OPSTATUS` |
| Common | `PARSING_FUNC(Slice_Bool)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_ReaderCreate)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_Next)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_Free)` | `void` |
| INI | `PARSING_FUNC(INI_Parse)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_Get)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_Set)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_Remove)` | `OPSTATUS` |
| INI | `PARSING_FUNC(INI_Write)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Free)` | `void` |
| JSON | `PARSING_FUNC(JSON_ParseWithLimits)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Parse)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Root)` | `const PARSING_TYPE(TJSONValue) *` |
| JSON | `PARSING_FUNC(JSON_Size)` | `size_t` |
| JSON | `PARSING_FUNC(JSON_GetSlice)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Get)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_At)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_String)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_I64)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_U64)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_F64)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Bool)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Write)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Set)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Append)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Remove)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_ReaderCreate)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_Next)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_TokenWrite)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_GetMutable)` | `OPSTATUS` |
| JSON | `PARSING_FUNC(JSON_AtMutable)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_Free)` | `void` |
| TOML | `PARSING_FUNC(TOML_Parse)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_Get)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_At)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_String)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_I64)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_F64)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_Bool)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_Write)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_Set)` | `OPSTATUS` |
| TOML | `PARSING_FUNC(TOML_Remove)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_ReaderCreate)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_Next)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_TextWrite)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_Free)` | `void` |
| XML | `PARSING_FUNC(XML_Parse)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_Attribute)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_SetAttribute)` | `OPSTATUS` |
| XML | `PARSING_FUNC(XML_Write)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_Free)` | `void` |
| YAML | `PARSING_FUNC(YAML_Parse)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_Get)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_At)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_Scalar)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_Write)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_SetScalar)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_GetMutable)` | `OPSTATUS` |
| YAML | `PARSING_FUNC(YAML_AtMutable)` | `OPSTATUS` |

# Common package

# Slice_Create

Creates a non-owning sized text slice.

### Syntax

#### Macro form

```c
PARSING_TYPE(TSlice) PARSING_FUNC(Slice_Create)(const char *data, size_t size);
```

#### Direct form

```c
Parsing_TSlice Parsing_Slice_Create(const char *data, size_t size);
```

### Parameters

- `data`: pointer to source bytes.
- `size`: byte length of source or data.

### Return value

Returns a value or borrowed pointer of type `PARSING_TYPE(TSlice)`.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
PARSING_TYPE(TSlice) slice = PARSING_FUNC(Slice_Create)("123", 3);
```

# Slice_Equal

Compares two text slices byte for byte.

### Syntax

#### Macro form

```c
bool PARSING_FUNC(Slice_Equal)(PARSING_TYPE(TSlice) left, PARSING_TYPE(TSlice) right);
```

#### Direct form

```c
bool Parsing_Slice_Equal(Parsing_TSlice left, Parsing_TSlice right);
```

### Parameters

- `left`: first slice to compare.
- `right`: second slice to compare.

### Return value

`bool`: boolean query result.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
PARSING_TYPE(TSlice) a = PARSING_FUNC(Slice_Create)("yes", 3);
PARSING_TYPE(TSlice) b = PARSING_FUNC(Slice_Create)("yes", 3);
bool same = PARSING_FUNC(Slice_Equal)(a, b);
```

# Slice_U64

Converts a value to an unsigned 64-bit integer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(Slice_U64)(PARSING_TYPE(TSlice) text, uint64_t *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_Slice_U64(Parsing_TSlice text, uint64_t *outValue);
```

### Parameters

- `text`: text to interpret or store.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
uint64_t result = {0};
OPSTATUS status = PARSING_FUNC(Slice_U64)(PARSING_FUNC(Slice_Create)("42", 2), &result);
```

# Slice_I64

Converts a value to a signed 64-bit integer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(Slice_I64)(PARSING_TYPE(TSlice) text, int64_t *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_Slice_I64(Parsing_TSlice text, int64_t *outValue);
```

### Parameters

- `text`: text to interpret or store.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
int64_t result = {0};
OPSTATUS status = PARSING_FUNC(Slice_I64)(PARSING_FUNC(Slice_Create)("42", 2), &result);
```

# Slice_F64

Converts a numeric value to double.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(Slice_F64)(PARSING_TYPE(TSlice) text, double *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_Slice_F64(Parsing_TSlice text, double *outValue);
```

### Parameters

- `text`: text to interpret or store.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
double result = {0};
OPSTATUS status = PARSING_FUNC(Slice_F64)(PARSING_FUNC(Slice_Create)("42", 2), &result);
```

# Slice_Bool

Obtains a boolean value.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(Slice_Bool)(PARSING_TYPE(TSlice) text, bool *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_Slice_Bool(Parsing_TSlice text, bool *outValue);
```

### Parameters

- `text`: text to interpret or store.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
bool result = {0};
OPSTATUS status = PARSING_FUNC(Slice_Bool)(PARSING_FUNC(Slice_Create)("42", 2), &result);
```

# CSV package

# CSV_ReaderCreate

Initializes a reader that borrows the source text.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_ReaderCreate)(const char *source, size_t size, char delimiter, PARSING_TYPE(TCSVReader) *outReader, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_ReaderCreate(const char *source, size_t size, char delimiter, Parsing_TCSVReader *outReader, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `delimiter`: CSV field delimiter character.
- `outReader`: output reader initialized from the supplied source.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "a,b\n1,2\n";
PARSING_TYPE(TCSVReader) reader = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(CSV_ReaderCreate)(source, sizeof source - 1, ',', &reader, &diagnostic);
```

# CSV_Next

Advances to the next reader item or reports end-of-input.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_Next)(PARSING_TYPE(TCSVReader) *reader, PARSING_TYPE(TCSVField) *outField, bool *outEnd);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_Next(Parsing_TCSVReader *reader, Parsing_TCSVField *outField, bool *outEnd);
```

### Parameters

- `reader`: reader whose source remains alive.
- `outField`: next CSV field output.
- `outEnd`: set to true after end of input.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TCSVField) field = {0};
bool end = false;
OPSTATUS status = PARSING_FUNC(CSV_Next)(&reader, &field, &end);
```

# CSV_FieldWrite

Decodes a CSV field into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_FieldWrite)(const PARSING_TYPE(TCSVField) *field, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_FieldWrite(const Parsing_TCSVField *field, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `field`: CSV field descriptor.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it. Borrowed value pointers/slices are invalidated by relevant edits and document destruction. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(CSV_FieldWrite)(&field, buffer, sizeof buffer, &written);
```

# CSV_Free

Releases an owned document and resets the handle.

### Syntax

#### Macro form

```c
void PARSING_FUNC(CSV_Free)(PARSING_TYPE(TCSVDocument) *document);
```

#### Direct form

```c
void Parsing_CSV_Free(Parsing_TCSVDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

No return value; releases owned memory.

### Remarks

The document owns allocations; freeing is safe again after reset. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_FUNC(CSV_Free)(&document);
```

# CSV_Parse

Parses an input text into an owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_Parse)(const char *source, size_t size, char delimiter, PARSING_TYPE(TCSVDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_Parse(const char *source, size_t size, char delimiter, Parsing_TCSVDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `delimiter`: CSV field delimiter character.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "a,b\n1,2\n";
PARSING_TYPE(TCSVDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(CSV_Parse)(source, sizeof source - 1, ',', &document, &diagnostic);
```

# CSV_Field

Gets a field from a CSV record by index.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_Field)(const PARSING_TYPE(TCSVRecord) *record, size_t index, PARSING_TYPE(TSlice) *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_Field(const Parsing_TCSVRecord *record, size_t index, Parsing_TSlice *outValue);
```

### Parameters

- `record`: CSV record.
- `index`: zero-based index.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TSlice) result = {0};
OPSTATUS status = PARSING_FUNC(CSV_Field)(&record, 0, &result);
```

# CSV_Set

Changes a keyed value or a CSV field in the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_Set)(PARSING_TYPE(TCSVDocument) *document, size_t recordIndex, size_t fieldIndex, PARSING_TYPE(TSlice) value);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_Set(Parsing_TCSVDocument *document, size_t recordIndex, size_t fieldIndex, Parsing_TSlice value);
```

### Parameters

- `document`: previously parsed document.
- `recordIndex`: zero-based CSV record index.
- `fieldIndex`: zero-based CSV field index.
- `value`: value or slice to query/edit.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(CSV_Set)(&document, 0, 0, PARSING_FUNC(Slice_Create)("42", 2));
```

# CSV_Write

Serializes an owned document into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(CSV_Write)(const PARSING_TYPE(TCSVDocument) *document, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_CSV_Write(const Parsing_TCSVDocument *document, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `document`: previously parsed document.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it. CSV uses configurable ASCII delimiters and supports quoted multiline cells.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(CSV_Write)(&document, buffer, sizeof buffer, &written);
```

# INI package

# INI_ReaderCreate

Initializes a reader that borrows the source text.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_ReaderCreate)(const char *source, size_t size, PARSING_TYPE(TINIReader) *outReader, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_INI_ReaderCreate(const char *source, size_t size, Parsing_TINIReader *outReader, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outReader`: output reader initialized from the supplied source.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "name=value\n";
PARSING_TYPE(TINIReader) reader = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(INI_ReaderCreate)(source, sizeof source - 1, &reader, &diagnostic);
```

# INI_Next

Advances to the next reader item or reports end-of-input.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_Next)(PARSING_TYPE(TINIReader) *reader, PARSING_TYPE(TINIEntry) *outEntry, bool *outEnd);
```

#### Direct form

```c
OPSTATUS Parsing_INI_Next(Parsing_TINIReader *reader, Parsing_TINIEntry *outEntry, bool *outEnd);
```

### Parameters

- `reader`: reader whose source remains alive.
- `outEntry`: next INI entry output.
- `outEnd`: set to true after end of input.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TINIEntry) entry = {0};
bool end = false;
OPSTATUS status = PARSING_FUNC(INI_Next)(&reader, &entry, &end);
```

# INI_Free

Releases an owned document and resets the handle.

### Syntax

#### Macro form

```c
void PARSING_FUNC(INI_Free)(PARSING_TYPE(TINIDocument) *document);
```

#### Direct form

```c
void Parsing_INI_Free(Parsing_TINIDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

No return value; releases owned memory.

### Remarks

The document owns allocations; freeing is safe again after reset.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_FUNC(INI_Free)(&document);
```

# INI_Parse

Parses an input text into an owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_Parse)(const char *source, size_t size, PARSING_TYPE(TINIDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_INI_Parse(const char *source, size_t size, Parsing_TINIDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "name=value\n";
PARSING_TYPE(TINIDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(INI_Parse)(source, sizeof source - 1, &document, &diagnostic);
```

# INI_Get

Looks up a member by name.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_Get)(const PARSING_TYPE(TINIDocument) *document, const char *section, const char *key, PARSING_TYPE(TSlice) *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_INI_Get(const Parsing_TINIDocument *document, const char *section, const char *key, Parsing_TSlice *outValue);
```

### Parameters

- `document`: previously parsed document.
- `section`: INI section name.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TSlice) result = {0};
OPSTATUS status = PARSING_FUNC(INI_Get)(&document, "general", "name", &result);
```

# INI_Set

Changes a keyed value or a CSV field in the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_Set)(PARSING_TYPE(TINIDocument) *document, const char *section, const char *key, const char *value);
```

#### Direct form

```c
OPSTATUS Parsing_INI_Set(Parsing_TINIDocument *document, const char *section, const char *key, const char *value);
```

### Parameters

- `document`: previously parsed document.
- `section`: INI section name.
- `key`: C-string or sized slice key.
- `value`: value or slice to query/edit.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(INI_Set)(&document, "general", "name", value);
```

# INI_Remove

Removes a keyed value from the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_Remove)(PARSING_TYPE(TINIDocument) *document, const char *section, const char *key);
```

#### Direct form

```c
OPSTATUS Parsing_INI_Remove(Parsing_TINIDocument *document, const char *section, const char *key);
```

### Parameters

- `document`: previously parsed document.
- `section`: INI section name.
- `key`: C-string or sized slice key.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(INI_Remove)(&document, "general", "name");
```

# INI_Write

Serializes an owned document into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(INI_Write)(const PARSING_TYPE(TINIDocument) *document, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_INI_Write(const Parsing_TINIDocument *document, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `document`: previously parsed document.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(INI_Write)(&document, buffer, sizeof buffer, &written);
```

# JSON package

# JSON_Free

Releases an owned document and resets the handle.

### Syntax

#### Macro form

```c
void PARSING_FUNC(JSON_Free)(PARSING_TYPE(TJSONDocument) *document);
```

#### Direct form

```c
void Parsing_JSON_Free(Parsing_TJSONDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

No return value; releases owned memory.

### Remarks

The document owns allocations; freeing is safe again after reset.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_FUNC(JSON_Free)(&document);
```

# JSON_ParseWithLimits

Parses JSON using caller-provided input and structural limits.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_ParseWithLimits)(const char *source, size_t sourceSize, const PARSING_TYPE(TLimits) *limits, PARSING_TYPE(TJSONDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_ParseWithLimits(const char *source, size_t sourceSize, const Parsing_TLimits *limits, Parsing_TJSONDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `sourceSize`: length of input source in bytes.
- `limits`: input/depth/node limits for JSON.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function. Decoded duplicate object keys are rejected.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "{}";
PARSING_TYPE(TJSONDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(JSON_ParseWithLimits)(source, sizeof source - 1, &limits, &document, &diagnostic);
```

# JSON_Parse

Parses an input text into an owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Parse)(const char *source, size_t sourceSize, PARSING_TYPE(TJSONDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Parse(const char *source, size_t sourceSize, Parsing_TJSONDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `sourceSize`: length of input source in bytes.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function. Decoded duplicate object keys are rejected.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "{}";
PARSING_TYPE(TJSONDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(JSON_Parse)(source, sizeof source - 1, &document, &diagnostic);
```

# JSON_Root

Returns the borrowed JSON root value.

### Syntax

#### Macro form

```c
const PARSING_TYPE(TJSONValue) * PARSING_FUNC(JSON_Root)(const PARSING_TYPE(TJSONDocument) *document);
```

#### Direct form

```c
const Parsing_TJSONValue * Parsing_JSON_Root(const Parsing_TJSONDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

Returns a value or borrowed pointer of type `const PARSING_TYPE(TJSONValue) *`.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TJSONValue) *root = PARSING_FUNC(JSON_Root)(&document);
```

# JSON_Size

Returns the number of child values.

### Syntax

#### Macro form

```c
size_t PARSING_FUNC(JSON_Size)(const PARSING_TYPE(TJSONValue) *value);
```

#### Direct form

```c
size_t Parsing_JSON_Size(const Parsing_TJSONValue *value);
```

### Parameters

- `value`: value or slice to query/edit.

### Return value

`size_t`: number of items.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
size_t count = PARSING_FUNC(JSON_Size)(value);
```

# JSON_GetSlice

Looks up a JSON member using a size-bounded key.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_GetSlice)(const PARSING_TYPE(TJSONValue) *object, PARSING_TYPE(TSlice) key, const PARSING_TYPE(TJSONValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_GetSlice(const Parsing_TJSONValue *object, Parsing_TSlice key, const Parsing_TJSONValue **outValue);
```

### Parameters

- `object`: JSON/YAML object borrowed from an owned document.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TJSONValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(JSON_GetSlice)(object, PARSING_FUNC(Slice_Create)("name", 4), &found);
```

# JSON_Get

Looks up a member by name.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Get)(const PARSING_TYPE(TJSONValue) *object, const char *key, const PARSING_TYPE(TJSONValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Get(const Parsing_TJSONValue *object, const char *key, const Parsing_TJSONValue **outValue);
```

### Parameters

- `object`: JSON/YAML object borrowed from an owned document.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TJSONValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(JSON_Get)(object, "name", &found);
```

# JSON_At

Gets an indexed element from an array/sequence.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_At)(const PARSING_TYPE(TJSONValue) *array, size_t index, const PARSING_TYPE(TJSONValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_At(const Parsing_TJSONValue *array, size_t index, const Parsing_TJSONValue **outValue);
```

### Parameters

- `array`: JSON/TOML/YAML array borrowed from an owned document.
- `index`: zero-based index.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TJSONValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(JSON_At)(array, 0, &found);
```

# JSON_String

Obtains a string as a borrowed slice.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_String)(const PARSING_TYPE(TJSONValue) *value, PARSING_TYPE(TSlice) *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_String(const Parsing_TJSONValue *value, Parsing_TSlice *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TSlice) result = {0};
OPSTATUS status = PARSING_FUNC(JSON_String)(value, &result);
```

# JSON_I64

Converts a value to a signed 64-bit integer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_I64)(const PARSING_TYPE(TJSONValue) *value, int64_t *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_I64(const Parsing_TJSONValue *value, int64_t *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
int64_t result = {0};
OPSTATUS status = PARSING_FUNC(JSON_I64)(value, &result);
```

# JSON_U64

Converts a value to an unsigned 64-bit integer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_U64)(const PARSING_TYPE(TJSONValue) *value, uint64_t *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_U64(const Parsing_TJSONValue *value, uint64_t *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
uint64_t result = {0};
OPSTATUS status = PARSING_FUNC(JSON_U64)(value, &result);
```

# JSON_F64

Converts a numeric value to double.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_F64)(const PARSING_TYPE(TJSONValue) *value, double *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_F64(const Parsing_TJSONValue *value, double *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
double result = {0};
OPSTATUS status = PARSING_FUNC(JSON_F64)(value, &result);
```

# JSON_Bool

Obtains a boolean value.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Bool)(const PARSING_TYPE(TJSONValue) *value, bool *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Bool(const Parsing_TJSONValue *value, bool *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Validate pointer lifetimes and result statuses before accessing outputs. This function does not transfer ownership unless it explicitly returns an owned document.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
bool result = {0};
OPSTATUS status = PARSING_FUNC(JSON_Bool)(value, &result);
```

# JSON_Write

Serializes an owned document into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Write)(const PARSING_TYPE(TJSONDocument) *document, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Write(const Parsing_TJSONDocument *document, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `document`: previously parsed document.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(JSON_Write)(&document, buffer, sizeof buffer, &written);
```

# JSON_Set

Changes a keyed value or a CSV field in the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Set)(PARSING_TYPE(TJSONValue) *object, const char *key, const char *json, size_t jsonSize, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Set(Parsing_TJSONValue *object, const char *key, const char *json, size_t jsonSize, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `object`: JSON/YAML object borrowed from an owned document.
- `key`: C-string or sized slice key.
- `json`: encoded JSON value text.
- `jsonSize`: JSON text length in bytes.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(JSON_Set)(object, "name", "1", 1, &diagnostic);
```

# JSON_Append

Appends an encoded JSON value to an array.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Append)(PARSING_TYPE(TJSONValue) *array, const char *json, size_t jsonSize, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Append(Parsing_TJSONValue *array, const char *json, size_t jsonSize, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `array`: JSON/TOML/YAML array borrowed from an owned document.
- `json`: encoded JSON value text.
- `jsonSize`: JSON text length in bytes.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(JSON_Append)(array, "1", 1, &diagnostic);
```

# JSON_Remove

Removes a keyed value from the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Remove)(PARSING_TYPE(TJSONValue) *object, const char *key);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Remove(Parsing_TJSONValue *object, const char *key);
```

### Parameters

- `object`: JSON/YAML object borrowed from an owned document.
- `key`: C-string or sized slice key.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(JSON_Remove)(object, "name");
```

# JSON_ReaderCreate

Initializes a reader that borrows the source text.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_ReaderCreate)(const char *source, size_t size, PARSING_TYPE(TJSONReader) *outReader, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_ReaderCreate(const char *source, size_t size, Parsing_TJSONReader *outReader, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outReader`: output reader initialized from the supplied source.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "{}";
PARSING_TYPE(TJSONReader) reader = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(JSON_ReaderCreate)(source, sizeof source - 1, &reader, &diagnostic);
```

# JSON_Next

Advances to the next reader item or reports end-of-input.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_Next)(PARSING_TYPE(TJSONReader) *reader, PARSING_TYPE(TJSONToken) *outToken, bool *outEnd);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_Next(Parsing_TJSONReader *reader, Parsing_TJSONToken *outToken, bool *outEnd);
```

### Parameters

- `reader`: reader whose source remains alive.
- `outToken`: next JSON token output.
- `outEnd`: set to true after end of input.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TJSONToken) token = {0};
bool end = false;
OPSTATUS status = PARSING_FUNC(JSON_Next)(&reader, &token, &end);
```

# JSON_TokenWrite

Decodes the escaped content of a JSON token.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_TokenWrite)(const PARSING_TYPE(TJSONToken) *token, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_TokenWrite(const Parsing_TJSONToken *token, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `token`: value of the declared C type.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(JSON_TokenWrite)(&token, buffer, sizeof buffer, &written);
```

# JSON_GetMutable

Gets a mutable member for editing.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_GetMutable)(PARSING_TYPE(TJSONValue) *object, const char *key, PARSING_TYPE(TJSONValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_GetMutable(Parsing_TJSONValue *object, const char *key, Parsing_TJSONValue **outValue);
```

### Parameters

- `object`: JSON/YAML object borrowed from an owned document.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TJSONValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(JSON_GetMutable)(object, "name", &found);
```

# JSON_AtMutable

Gets a mutable indexed item for editing.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(JSON_AtMutable)(PARSING_TYPE(TJSONValue) *array, size_t index, PARSING_TYPE(TJSONValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_JSON_AtMutable(Parsing_TJSONValue *array, size_t index, Parsing_TJSONValue **outValue);
```

### Parameters

- `array`: JSON/TOML/YAML array borrowed from an owned document.
- `index`: zero-based index.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TJSONValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(JSON_AtMutable)(array, 0, &found);
```

# TOML package

# TOML_Free

Releases an owned document and resets the handle.

### Syntax

#### Macro form

```c
void PARSING_FUNC(TOML_Free)(PARSING_TYPE(TTOMLDocument) *document);
```

#### Direct form

```c
void Parsing_TOML_Free(Parsing_TTOMLDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

No return value; releases owned memory.

### Remarks

The document owns allocations; freeing is safe again after reset. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_FUNC(TOML_Free)(&document);
```

# TOML_Parse

Parses an input text into an owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_Parse)(const char *source, size_t size, PARSING_TYPE(TTOMLDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_Parse(const char *source, size_t size, Parsing_TTOMLDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "title = \"demo\"\n";
PARSING_TYPE(TTOMLDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(TOML_Parse)(source, sizeof source - 1, &document, &diagnostic);
```

# TOML_Get

Looks up a member by name.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_Get)(const PARSING_TYPE(TTOMLValue) *table, const char *key, const PARSING_TYPE(TTOMLValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_Get(const Parsing_TTOMLValue *table, const char *key, const Parsing_TTOMLValue **outValue);
```

### Parameters

- `table`: TOML table.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TTOMLValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(TOML_Get)(table, "name", &found);
```

# TOML_At

Gets an indexed element from an array/sequence.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_At)(const PARSING_TYPE(TTOMLValue) *array, size_t index, const PARSING_TYPE(TTOMLValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_At(const Parsing_TTOMLValue *array, size_t index, const Parsing_TTOMLValue **outValue);
```

### Parameters

- `array`: JSON/TOML/YAML array borrowed from an owned document.
- `index`: zero-based index.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TTOMLValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(TOML_At)(array, 0, &found);
```

# TOML_String

Obtains a string as a borrowed slice.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_String)(const PARSING_TYPE(TTOMLValue) *value, PARSING_TYPE(TSlice) *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_String(const Parsing_TTOMLValue *value, Parsing_TSlice *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TSlice) result = {0};
OPSTATUS status = PARSING_FUNC(TOML_String)(value, &result);
```

# TOML_I64

Converts a value to a signed 64-bit integer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_I64)(const PARSING_TYPE(TTOMLValue) *value, int64_t *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_I64(const Parsing_TTOMLValue *value, int64_t *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
int64_t result = {0};
OPSTATUS status = PARSING_FUNC(TOML_I64)(value, &result);
```

# TOML_F64

Converts a numeric value to double.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_F64)(const PARSING_TYPE(TTOMLValue) *value, double *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_F64(const Parsing_TTOMLValue *value, double *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
double result = {0};
OPSTATUS status = PARSING_FUNC(TOML_F64)(value, &result);
```

# TOML_Bool

Obtains a boolean value.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_Bool)(const PARSING_TYPE(TTOMLValue) *value, bool *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_Bool(const Parsing_TTOMLValue *value, bool *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
bool result = {0};
OPSTATUS status = PARSING_FUNC(TOML_Bool)(value, &result);
```

# TOML_Write

Serializes an owned document into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_Write)(const PARSING_TYPE(TTOMLDocument) *document, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_Write(const Parsing_TTOMLDocument *document, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `document`: previously parsed document.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(TOML_Write)(&document, buffer, sizeof buffer, &written);
```

# TOML_Set

Changes a keyed value or a CSV field in the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_Set)(PARSING_TYPE(TTOMLDocument) *document, const char *key, const char *literal, size_t literalSize, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_Set(Parsing_TTOMLDocument *document, const char *key, const char *literal, size_t literalSize, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `document`: previously parsed document.
- `key`: C-string or sized slice key.
- `literal`: TOML literal to store.
- `literalSize`: TOML literal byte length.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(TOML_Set)(&document, "name", "1", 1, &diagnostic);
```

# TOML_Remove

Removes a keyed value from the owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(TOML_Remove)(PARSING_TYPE(TTOMLDocument) *document, const char *key);
```

#### Direct form

```c
OPSTATUS Parsing_TOML_Remove(Parsing_TTOMLDocument *document, const char *key);
```

### Parameters

- `document`: previously parsed document.
- `key`: C-string or sized slice key.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Mutations may invalidate existing borrowed pointers and indices. TOML implements a configuration profile; dates/times and exact source formatting are unsupported.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(TOML_Remove)(&document, "name");
```

# XML package

# XML_ReaderCreate

Initializes a reader that borrows the source text.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_ReaderCreate)(const char *source, size_t size, PARSING_TYPE(TXMLReader) *outReader, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_XML_ReaderCreate(const char *source, size_t size, Parsing_TXMLReader *outReader, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outReader`: output reader initialized from the supplied source.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "\x3croot/\x3e";
PARSING_TYPE(TXMLReader) reader = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(XML_ReaderCreate)(source, sizeof source - 1, &reader, &diagnostic);
```

# XML_Next

Advances to the next reader item or reports end-of-input.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_Next)(PARSING_TYPE(TXMLReader) *reader, PARSING_TYPE(TXMLEvent) *outEvent, bool *outEnd);
```

#### Direct form

```c
OPSTATUS Parsing_XML_Next(Parsing_TXMLReader *reader, Parsing_TXMLEvent *outEvent, bool *outEnd);
```

### Parameters

- `reader`: reader whose source remains alive.
- `outEvent`: next XML event output.
- `outEnd`: set to true after end of input.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The reader borrows source bytes and performs no document allocation. Next returns SUCCESS and outEnd=true at end. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TXMLEvent) event = {0};
bool end = false;
OPSTATUS status = PARSING_FUNC(XML_Next)(&reader, &event, &end);
```

# XML_TextWrite

Writes the textual content of an XML event.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_TextWrite)(const PARSING_TYPE(TXMLEvent) *event, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_XML_TextWrite(const Parsing_TXMLEvent *event, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `event`: value of the declared C type.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(XML_TextWrite)(&event, buffer, sizeof buffer, &written);
```

# XML_Free

Releases an owned document and resets the handle.

### Syntax

#### Macro form

```c
void PARSING_FUNC(XML_Free)(PARSING_TYPE(TXMLDocument) *document);
```

#### Direct form

```c
void Parsing_XML_Free(Parsing_TXMLDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

No return value; releases owned memory.

### Remarks

The document owns allocations; freeing is safe again after reset. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_FUNC(XML_Free)(&document);
```

# XML_Parse

Parses an input text into an owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_Parse)(const char *source, size_t size, PARSING_TYPE(TXMLDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_XML_Parse(const char *source, size_t size, Parsing_TXMLDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "\x3croot/\x3e";
PARSING_TYPE(TXMLDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(XML_Parse)(source, sizeof source - 1, &document, &diagnostic);
```

# XML_Attribute

Finds an XML attribute by name.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_Attribute)(const PARSING_TYPE(TXMLDocument) *document, size_t elementIndex, const char *name, PARSING_TYPE(TSlice) *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_XML_Attribute(const Parsing_TXMLDocument *document, size_t elementIndex, const char *name, Parsing_TSlice *outValue);
```

### Parameters

- `document`: previously parsed document.
- `elementIndex`: zero-based XML element index.
- `name`: XML attribute name.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TSlice) result = {0};
OPSTATUS status = PARSING_FUNC(XML_Attribute)(&document, 0, "id", &result);
```

# XML_SetAttribute

Sets the named XML attribute.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_SetAttribute)(PARSING_TYPE(TXMLDocument) *document, size_t elementIndex, const char *name, PARSING_TYPE(TSlice) value);
```

#### Direct form

```c
OPSTATUS Parsing_XML_SetAttribute(Parsing_TXMLDocument *document, size_t elementIndex, const char *name, Parsing_TSlice value);
```

### Parameters

- `document`: previously parsed document.
- `elementIndex`: zero-based XML element index.
- `name`: XML attribute name.
- `value`: value or slice to query/edit.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. Mutations may invalidate existing borrowed pointers and indices. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(XML_SetAttribute)(&document, 0, "id", PARSING_FUNC(Slice_Create)("42", 2));
```

# XML_Write

Serializes an owned document into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(XML_Write)(const PARSING_TYPE(TXMLDocument) *document, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_XML_Write(const Parsing_TXMLDocument *document, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `document`: previously parsed document.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it. XML preserves an ordered mixed-content event stream, but does not support DTD.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(XML_Write)(&document, buffer, sizeof buffer, &written);
```

# YAML package

# YAML_Free

Releases an owned document and resets the handle.

### Syntax

#### Macro form

```c
void PARSING_FUNC(YAML_Free)(PARSING_TYPE(TYAMLDocument) *document);
```

#### Direct form

```c
void Parsing_YAML_Free(Parsing_TYAMLDocument *document);
```

### Parameters

- `document`: previously parsed document.

### Return value

No return value; releases owned memory.

### Remarks

The document owns allocations; freeing is safe again after reset. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_FUNC(YAML_Free)(&document);
```

# YAML_Parse

Parses an input text into an owned document.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_Parse)(const char *source, size_t size, PARSING_TYPE(TYAMLDocument) *outDocument, PARSING_TYPE(TDiagnostic) *diagnostic);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_Parse(const char *source, size_t size, Parsing_TYAMLDocument *outDocument, Parsing_TDiagnostic *diagnostic);
```

### Parameters

- `source`: input UTF-8 bytes (NULL only when size is zero).
- `size`: byte length of source or data.
- `outDocument`: zero-initialized output document acquiring ownership on success.
- `diagnostic`: optional diagnostic with position and error code.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The input must be UTF-8. The document owns its retained content after SUCCESS; release it using the corresponding Free function. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const char source[] = "title: demo\n";
PARSING_TYPE(TYAMLDocument) document = {0};
PARSING_TYPE(TDiagnostic) diagnostic = {0};
OPSTATUS status = PARSING_FUNC(YAML_Parse)(source, sizeof source - 1, &document, &diagnostic);
```

# YAML_Get

Looks up a member by name.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_Get)(const PARSING_TYPE(TYAMLValue) *mapping, const char *key, const PARSING_TYPE(TYAMLValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_Get(const Parsing_TYAMLValue *mapping, const char *key, const Parsing_TYAMLValue **outValue);
```

### Parameters

- `mapping`: YAML mapping.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TYAMLValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(YAML_Get)(mapping, "name", &found);
```

# YAML_At

Gets an indexed element from an array/sequence.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_At)(const PARSING_TYPE(TYAMLValue) *sequence, size_t index, const PARSING_TYPE(TYAMLValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_At(const Parsing_TYAMLValue *sequence, size_t index, const Parsing_TYAMLValue **outValue);
```

### Parameters

- `sequence`: YAML sequence.
- `index`: zero-based index.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
const PARSING_TYPE(TYAMLValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(YAML_At)(sequence, 0, &found);
```

# YAML_Scalar

Obtains a YAML scalar as a borrowed slice.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_Scalar)(const PARSING_TYPE(TYAMLValue) *value, PARSING_TYPE(TSlice) *outValue);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_Scalar(const Parsing_TYAMLValue *value, Parsing_TSlice *outValue);
```

### Parameters

- `value`: value or slice to query/edit.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TSlice) result = {0};
OPSTATUS status = PARSING_FUNC(YAML_Scalar)(value, &result);
```

# YAML_Write

Serializes an owned document into a caller buffer.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_Write)(const PARSING_TYPE(TYAMLDocument) *document, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_Write(const Parsing_TYAMLDocument *document, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `document`: previously parsed document.
- `buffer`: caller-allocated output bytes.
- `capacity`: output capacity including room for NUL.
- `outSize`: output size excluding the terminating NUL.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

The writer reports required outSize on INSUFFICIENT_SPACE. The size excludes the final NUL; caller capacity must include it. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
char buffer[512] = {0};
size_t written = 0;
OPSTATUS status = PARSING_FUNC(YAML_Write)(&document, buffer, sizeof buffer, &written);
```

# YAML_SetScalar

Updates a YAML scalar's text.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_SetScalar)(PARSING_TYPE(TYAMLValue) *value, PARSING_TYPE(TSlice) text);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_SetScalar(Parsing_TYAMLValue *value, Parsing_TSlice text);
```

### Parameters

- `value`: value or slice to query/edit.
- `text`: text to interpret or store.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. Mutations may invalidate existing borrowed pointers and indices. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
OPSTATUS status = PARSING_FUNC(YAML_SetScalar)(value, PARSING_FUNC(Slice_Create)("42", 2));
```

# YAML_GetMutable

Gets a mutable member for editing.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_GetMutable)(PARSING_TYPE(TYAMLValue) *object, const char *key, PARSING_TYPE(TYAMLValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_GetMutable(Parsing_TYAMLValue *object, const char *key, Parsing_TYAMLValue **outValue);
```

### Parameters

- `object`: JSON/YAML object borrowed from an owned document.
- `key`: C-string or sized slice key.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TYAMLValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(YAML_GetMutable)(object, "name", &found);
```

# YAML_AtMutable

Gets a mutable indexed item for editing.

### Syntax

#### Macro form

```c
OPSTATUS PARSING_FUNC(YAML_AtMutable)(PARSING_TYPE(TYAMLValue) *array, size_t index, PARSING_TYPE(TYAMLValue) **outValue);
```

#### Direct form

```c
OPSTATUS Parsing_YAML_AtMutable(Parsing_TYAMLValue *array, size_t index, Parsing_TYAMLValue **outValue);
```

### Parameters

- `array`: JSON/TOML/YAML array borrowed from an owned document.
- `index`: zero-based index.
- `outValue`: output converted value or borrowed pointer, according to its type.

### Return value

`OPSTATUS`: SUCCESS on completion, or format, argument, range, missing-value, unsupported-feature, allocation, or destination-space errors where applicable.

### Remarks

Borrowed value pointers/slices are invalidated by relevant edits and document destruction. YAML uses a block profile, without aliases or advanced flow forms.

### Example

```c
/* Fragment: other named objects below must be initialized beforehand. */
PARSING_TYPE(TYAMLValue) *found = NULL;
OPSTATUS status = PARSING_FUNC(YAML_AtMutable)(array, 0, &found);
```

# Limitations and cross-module references

- TOML, XML and YAML are intentionally implemented as restricted profiles, not complete language specifications.
- Input strings use explicit byte sizes; document edits must be made through documented functions to retain ownership safety.
- JSON, CSV, INI and XML readers borrow original source bytes; TOML/YAML expose owned document parsing.
- See [File](File.md) for file handling, [Text](Text.md) for UTF-8, [CLI](CLI.md) for command-line arguments, and [Bit](Bit.md) for reference layout.

# C11 smoke test (JSON)

This standalone test exercises document ownership, object lookup and checked typed conversion.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Modules/Parsing/JSON/JSON.h"
#include <assert.h>

int main(void) {
    const char source[] = "{\"answer\":42}";
    PARSING_TYPE(TJSONDocument) doc = {0};
    const PARSING_TYPE(TJSONValue) *value = NULL;
    int64_t answer = 0;
    assert(PARSING_FUNC(JSON_Parse)(source, sizeof source - 1,
                                    &doc, NULL) == STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_Get)(PARSING_FUNC(JSON_Root)(&doc),
                                  "answer", &value) == STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_I64)(value, &answer) == STATUS_CONST(SUCCESS));
    assert(answer == 42);
    PARSING_FUNC(JSON_Free)(&doc);
    return 0;
}
```
