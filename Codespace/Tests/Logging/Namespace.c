#define COSMERON_NAMESPACE Custom
#define COSMERON_NAMESPACE_CONST CUSTOM
#include "../../Cosmeron/Modules/Logging/Logging.h"
#include <assert.h>
#include <string.h>

int main(void) {
  Custom_Logging_TLogger logger;
  assert(Custom_Logging_Functions.Logger_Init(&logger, "CustomApp", CUSTOM_LOGGING_LEVEL_DEBUG) == STATUS_CONST(SUCCESS));
  Custom_Logging_LogLevel level;
  assert(Custom_Logging_Logger_GetLevel(&logger, &level) == STATUS_CONST(SUCCESS));
  assert(level == CUSTOM_LOGGING_LEVEL_DEBUG);
  assert(strcmp(Custom_Logging_Level_Name(level), "DEBUG") == 0);
  Custom_Logging_Logger_Destroy(&logger);
  return 0;
}
