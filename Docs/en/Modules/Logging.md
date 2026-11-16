# Logging

The **Logging** module provides structured logging with configurable log levels, multiple output sinks, ANSI color formatting, JSON serialization, and diagnostic metadata (timestamp, tag, file, line, and function). It is header-only C11, requires zero external linker flags, and follows standard Cosmeron contracts.

```c
#include "Cosmeron/Modules/Logging/Logging.h"
```

# Overview

| Package | Header | Responsibility |
| --- | --- | --- |
| Diagnostics | `Logging/Diagnostics/Diagnostics.h` | Log records, levels, level names, colors and metadata |
| Format | `Logging/Format/Format.h` | Plain text and JSON log record formatting |
| Sink | `Logging/Sink/Sink.h` | Console (stdout/stderr), file, memory buffer, and custom sinks |
| Logger | `Logging/Logger/Logger.h` | Multi-sink dispatch, filtering, formatted logging and default logger |

## Naming and Conventions

| Macro | Default C identifier |
| --- | --- |
| `LOGGING_FUNC(Logger_Init)` | `Logging_Logger_Init` |
| `LOGGING_TYPE(TLogger)` | `Logging_TLogger` |
| `LOGGING_TYPE(TSink)` | `Logging_TSink` |
| `LOGGING_TYPE(TRecord)` | `Logging_TRecord` |
| `LOGGING_CONST(LEVEL_INFO)` | `LOGGING_LEVEL_INFO` |

Define `LOGGING_DISABLE_FUNCTION_TABLE` before inclusion to disable the optional module FunctionTable instance.

# Usage

## Basic Console Logging

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Modules/Logging/Logging.h"
#include <assert.h>

int main(void) {
    Logging_TSink sink;
    Logging_TLogger logger;

    assert(Logging_Sink_ConsoleInit(&sink, LOGGING_LEVEL_INFO, false, false) == STATUS_CONST(SUCCESS));
    assert(Logging_Logger_Init(&logger, "App", LOGGING_LEVEL_INFO) == STATUS_CONST(SUCCESS));
    assert(Logging_Logger_AddSink(&logger, &sink) == STATUS_CONST(SUCCESS));

    LOG_INFO(&logger, "Application initialized");
    Logging_Logger_Destroy(&logger);
    return 0;
}
```

# Reference

# Logger_Init

Initializes a logger with a given name and minimum log level threshold.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Logger_Init)(LOGGING_TYPE(TLogger) *logger, const char *name, LOGGING_TYPE(LogLevel) level);
```

#### Direct form

```c
OPSTATUS Logging_Logger_Init(Logging_TLogger *logger, const char *name, Logging_LogLevel level);
```

### Parameters

- `logger`: Pointer to the `Logging_TLogger` structure to initialize.
- `name`: Module or application name tag (e.g. `"Network"`).
- `level`: Minimum logging level threshold.

### Return value

Returns `STATUS_CONST(SUCCESS)` on success, or `STATUS_CONST(INVALID_ARGUMENT)` if `logger` is NULL.

### Remarks

Initializes sink count to 0 and prepares the logger for dispatch.

### Example

```c
Logging_TLogger logger;
Logging_Logger_Init(&logger, "Server", LOGGING_LEVEL_DEBUG);
```

# Logger_Destroy

Destroys a logger and clears all registered sinks.

### Syntax

#### Macro form

```c
void LOGGING_FUNC(Logger_Destroy)(LOGGING_TYPE(TLogger) *logger);
```

#### Direct form

```c
void Logging_Logger_Destroy(Logging_TLogger *logger);
```

### Parameters

- `logger`: Pointer to the `Logging_TLogger` to destroy.

### Return value

None.

### Remarks

Does not free the memory of external sinks attached to the logger.

### Example

```c
Logging_Logger_Destroy(&logger);
```

# Logger_AddSink

Attaches an output sink to a logger.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Logger_AddSink)(LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(TSink) *sink);
```

#### Direct form

```c
OPSTATUS Logging_Logger_AddSink(Logging_TLogger *logger, Logging_TSink *sink);
```

### Parameters

- `logger`: Pointer to the logger.
- `sink`: Pointer to the initialized `Logging_TSink`.

### Return value

Returns `STATUS_CONST(SUCCESS)` on success, `STATUS_CONST(INVALID_ARGUMENT)` if pointers are NULL, or `STATUS_CONST(OUT_OF_RANGE)` if maximum sink capacity is reached.

### Remarks

A single logger can route records to up to 16 concurrent sinks.

### Example

```c
Logging_Logger_AddSink(&logger, &consoleSink);
```

# Logger_Log

Logs a message string with diagnostic location details.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Logger_Log)(LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) level, const char *message, const char *file, int line, const char *function);
```

#### Direct form

```c
OPSTATUS Logging_Logger_Log(Logging_TLogger *logger, Logging_LogLevel level, const char *message, const char *file, int line, const char *function);
```

### Parameters

- `logger`: Target logger instance.
- `level`: Log level severity of the event.
- `message`: Text message to log.
- `file`: Source file path where the log event occurred.
- `line`: Source line number.
- `function`: Calling function name.

### Return value

Returns `STATUS_CONST(SUCCESS)` if handled or filtered out, or error status code on failure.

### Remarks

If `level` is below the logger threshold, the call returns `SUCCESS` immediately without formatting.

### Example

```c
Logging_Logger_Log(&logger, LOGGING_LEVEL_WARN, "Disk almost full", __FILE__, __LINE__, __func__);
```

# Logger_LogFormat

Formats and logs a variadic message.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Logger_LogFormat)(LOGGING_TYPE(TLogger) *logger, LOGGING_TYPE(LogLevel) level, const char *file, int line, const char *function, const char *format, ...);
```

#### Direct form

```c
OPSTATUS Logging_Logger_LogFormat(Logging_TLogger *logger, Logging_LogLevel level, const char *file, int line, const char *function, const char *format, ...);
```

### Parameters

- `logger`: Target logger instance.
- `level`: Severity level.
- `file`: Source file.
- `line`: Line number.
- `function`: Function name.
- `format`: `printf`-style format string followed by format arguments.

### Return value

Returns `STATUS_CONST(SUCCESS)` on success, or error status code on formatting failure.

### Remarks

Formats into an internal stack buffer before record dispatch.

### Example

```c
Logging_Logger_LogFormat(&logger, LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, "HTTP %d from %s", 404, host);
```

# Format_Text

Formats a record into standard human-readable text.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Format_Text)(const LOGGING_TYPE(TRecord) *record, bool useColors, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Logging_Format_Text(const Logging_TRecord *record, bool useColors, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `record`: Log record containing severity, timestamp, tag, and message.
- `useColors`: Whether to include ANSI color escape codes for terminal display.
- `buffer`: Output buffer for formatted string.
- `capacity`: Maximum byte capacity of `buffer`.
- `outSize`: Optional output pointer for written length excluding NUL.

### Return value

Returns `STATUS_CONST(SUCCESS)` on success, or `STATUS_CONST(OUT_OF_RANGE)` if truncated.

### Remarks

Generates format `[LEVEL] [TAG] message (file:line)`.

### Example

```c
char buf[256];
Logging_Format_Text(&record, true, buf, sizeof(buf), NULL);
```

# Format_JSON

Formats a record into a structured JSON string.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Format_JSON)(const LOGGING_TYPE(TRecord) *record, char *buffer, size_t capacity, size_t *outSize);
```

#### Direct form

```c
OPSTATUS Logging_Format_JSON(const Logging_TRecord *record, char *buffer, size_t capacity, size_t *outSize);
```

### Parameters

- `record`: Log record to format.
- `buffer`: Destination string buffer.
- `capacity`: Maximum buffer capacity.
- `outSize`: Optional pointer to store required size.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `STATUS_CONST(OUT_OF_RANGE)` if buffer is too small.

### Remarks

Produces a valid JSON object with `timestamp`, `level`, `tag`, `message`, `file`, and `line`.

### Example

```c
char json[512];
Logging_Format_JSON(&record, json, sizeof(json), NULL);
```

# Sink_ConsoleInit

Initializes a standard console sink targeting stdout and stderr.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Sink_ConsoleInit)(LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel, bool useColors, bool jsonFormat);
```

#### Direct form

```c
OPSTATUS Logging_Sink_ConsoleInit(Logging_TSink *sink, Logging_LogLevel minLevel, bool useColors, bool jsonFormat);
```

### Parameters

- `sink`: Pointer to uninitialized sink.
- `minLevel`: Minimum log level threshold for this sink.
- `useColors`: Enables ANSI colors on terminal.
- `jsonFormat`: Emits JSON lines instead of plain text.

### Return value

Returns `STATUS_CONST(SUCCESS)` on success.

### Remarks

Routes `WARN`, `ERROR` and `FATAL` records to `stderr` and other levels to `stdout`.

### Example

```c
Logging_TSink consoleSink;
Logging_Sink_ConsoleInit(&consoleSink, LOGGING_LEVEL_DEBUG, true, false);
```

# Sink_BufferInit

Initializes an in-memory buffer sink.

### Syntax

#### Macro form

```c
OPSTATUS LOGGING_FUNC(Sink_BufferInit)(LOGGING_TYPE(TSink) *sink, LOGGING_TYPE(LogLevel) minLevel, char *buffer, size_t capacity, bool jsonFormat);
```

#### Direct form

```c
OPSTATUS Logging_Sink_BufferInit(Logging_TSink *sink, Logging_LogLevel minLevel, char *buffer, size_t capacity, bool jsonFormat);
```

### Parameters

- `sink`: Pointer to sink.
- `minLevel`: Minimum log level threshold.
- `buffer`: Destination memory buffer.
- `capacity`: Maximum buffer capacity.
- `jsonFormat`: Enables JSON line formatting.

### Return value

Returns `STATUS_CONST(SUCCESS)` on success.

### Remarks

Appends log records sequentially into memory, suitable for testing and diagnostics inspection.

### Example

```c
char memory[2048];
Logging_TSink bufSink;
Logging_Sink_BufferInit(&bufSink, LOGGING_LEVEL_INFO, memory, sizeof(memory), false);
```
