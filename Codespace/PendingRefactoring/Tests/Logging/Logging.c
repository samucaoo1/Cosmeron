#include "../../Cosmeron/Modules/Logging/Logging.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static int s_customCallbackCount = 0;
static OPSTATUS custom_sink_writer(Logging_TSink *sink, const Logging_TRecord *record) {
  (void)sink;
  (void)record;
  s_customCallbackCount++;
  return STATUS_CONST(SUCCESS);
}

static void test_levels(void) {
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_TRACE), "TRACE") == 0);
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_DEBUG), "DEBUG") == 0);
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_INFO), "INFO") == 0);
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_WARN), "WARN") == 0);
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_ERROR), "ERROR") == 0);
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_FATAL), "FATAL") == 0);
  assert(strcmp(Logging_Level_Name(LOGGING_LEVEL_OFF), "OFF") == 0);

  Logging_LogLevel parsed;
  assert(Logging_Level_FromName("info", &parsed) == STATUS_CONST(SUCCESS));
  assert(parsed == LOGGING_LEVEL_INFO);
  assert(Logging_Level_FromName("DEBUG", &parsed) == STATUS_CONST(SUCCESS));
  assert(parsed == LOGGING_LEVEL_DEBUG);
  assert(Logging_Level_FromName("warn", &parsed) == STATUS_CONST(SUCCESS));
  assert(parsed == LOGGING_LEVEL_WARN);
  assert(Logging_Level_FromName("ERROR", &parsed) == STATUS_CONST(SUCCESS));
  assert(parsed == LOGGING_LEVEL_ERROR);
  assert(Logging_Level_FromName("invalid", &parsed) == STATUS_CONST(INVALID_ARGUMENT));

  assert(strlen(Logging_Level_Color(LOGGING_LEVEL_INFO)) > 0);
  assert(strlen(Logging_Level_ResetColor()) > 0);
}

static void test_record_and_formatting(void) {
  Logging_TRecord record;
  assert(Logging_Record_Init(&record, LOGGING_LEVEL_INFO, "Network", "Connected to host", "net.c", 42, "net_connect") == STATUS_CONST(SUCCESS));
  assert(record.level == LOGGING_LEVEL_INFO);
  assert(strcmp(record.tag, "Network") == 0);
  assert(strcmp(record.message, "Connected to host") == 0);
  assert(strcmp(record.file, "net.c") == 0);
  assert(record.line == 42);
  assert(strcmp(record.function, "net_connect") == 0);

  char textBuf[512];
  size_t written = 0;
  assert(Logging_Format_Text(&record, false, textBuf, sizeof(textBuf), &written) == STATUS_CONST(SUCCESS));
  assert(strstr(textBuf, "[INFO]") != NULL);
  assert(strstr(textBuf, "[Network]") != NULL);
  assert(strstr(textBuf, "Connected to host") != NULL);
  assert(strstr(textBuf, "(net.c:42)") != NULL);

  char jsonBuf[512];
  written = 0;
  assert(Logging_Format_JSON(&record, jsonBuf, sizeof(jsonBuf), &written) == STATUS_CONST(SUCCESS));
  assert(strstr(jsonBuf, "\"level\":\"INFO\"") != NULL);
  assert(strstr(jsonBuf, "\"tag\":\"Network\"") != NULL);
  assert(strstr(jsonBuf, "\"message\":\"Connected to host\"") != NULL);
  assert(strstr(jsonBuf, "\"file\":\"net.c\"") != NULL);
  assert(strstr(jsonBuf, "\"line\":42") != NULL);
}

static void test_buffer_sink(void) {
  char logBuffer[1024];
  Logging_TSink sink;
  assert(Logging_Sink_BufferInit(&sink, LOGGING_LEVEL_DEBUG, logBuffer, sizeof(logBuffer), false) == STATUS_CONST(SUCCESS));

  Logging_TLogger logger;
  assert(Logging_Logger_Init(&logger, "TestModule", LOGGING_LEVEL_DEBUG) == STATUS_CONST(SUCCESS));
  assert(Logging_Logger_AddSink(&logger, &sink) == STATUS_CONST(SUCCESS));

  assert(Logging_Logger_Log(&logger, LOGGING_LEVEL_INFO, "Hello world", "test.c", 10, "main") == STATUS_CONST(SUCCESS));
  assert(strstr(logBuffer, "Hello world") != NULL);
  assert(strstr(logBuffer, "[TestModule]") != NULL);

  /* Message below level (TRACE < DEBUG) should not be logged */
  size_t lenBefore = strlen(logBuffer);
  assert(Logging_Logger_Log(&logger, LOGGING_LEVEL_TRACE, "Trace detail", "test.c", 11, "main") == STATUS_CONST(SUCCESS));
  assert(strlen(logBuffer) == lenBefore);

  Logging_Logger_Destroy(&logger);
}

static void test_custom_sink(void) {
  s_customCallbackCount = 0;
  Logging_TSink customSink;
  assert(Logging_Sink_CustomInit(&customSink, LOGGING_LEVEL_WARN, custom_sink_writer, NULL) == STATUS_CONST(SUCCESS));

  Logging_TLogger logger;
  assert(Logging_Logger_Init(&logger, "Custom", LOGGING_LEVEL_DEBUG) == STATUS_CONST(SUCCESS));
  assert(Logging_Logger_AddSink(&logger, &customSink) == STATUS_CONST(SUCCESS));

  /* INFO < WARN (sink threshold): custom callback should not be invoked */
  assert(Logging_Logger_Log(&logger, LOGGING_LEVEL_INFO, "Info msg", __FILE__, __LINE__, __func__) == STATUS_CONST(SUCCESS));
  assert(s_customCallbackCount == 0);

  /* WARN >= WARN: should be invoked */
  assert(Logging_Logger_Log(&logger, LOGGING_LEVEL_WARN, "Warn msg", __FILE__, __LINE__, __func__) == STATUS_CONST(SUCCESS));
  assert(s_customCallbackCount == 1);

  /* ERROR >= WARN: should be invoked */
  assert(Logging_Logger_Log(&logger, LOGGING_LEVEL_ERROR, "Error msg", __FILE__, __LINE__, __func__) == STATUS_CONST(SUCCESS));
  assert(s_customCallbackCount == 2);

  Logging_Logger_Destroy(&logger);
}

static void test_formatted_and_macros(void) {
  char buffer[1024];
  Logging_TSink sink;
  assert(Logging_Sink_BufferInit(&sink, LOGGING_LEVEL_TRACE, buffer, sizeof(buffer), false) == STATUS_CONST(SUCCESS));

  Logging_TLogger logger;
  assert(Logging_Logger_Init(&logger, "FmtApp", LOGGING_LEVEL_TRACE) == STATUS_CONST(SUCCESS));
  assert(Logging_Logger_AddSink(&logger, &sink) == STATUS_CONST(SUCCESS));

  assert(Logging_Logger_LogFormat(&logger, LOGGING_LEVEL_INFO, __FILE__, __LINE__, __func__, "Value = %d, str = %s", 123, "ok") == STATUS_CONST(SUCCESS));
  assert(strstr(buffer, "Value = 123, str = ok") != NULL);

  LOG_WARN(&logger, "Macro warning message");
  assert(strstr(buffer, "Macro warning message") != NULL);

  LOG_ERROR(&logger, "Macro error message");
  assert(strstr(buffer, "Macro error message") != NULL);

  Logging_Logger_Destroy(&logger);
}

static void test_default_logger(void) {
  Logging_TLogger logger;
  assert(Logging_Logger_Init(&logger, "Global", LOGGING_LEVEL_INFO) == STATUS_CONST(SUCCESS));
  Logging_SetDefault(&logger);
  assert(Logging_GetDefault() == &logger);
  Logging_SetDefault(NULL);
  assert(Logging_GetDefault() == NULL);
  Logging_Logger_Destroy(&logger);
}

int main(void) {
  test_levels();
  test_record_and_formatting();
  test_buffer_sink();
  test_custom_sink();
  test_formatted_and_macros();
  test_default_logger();
  printf("All Logging tests passed successfully!\n");
  return 0;
}
