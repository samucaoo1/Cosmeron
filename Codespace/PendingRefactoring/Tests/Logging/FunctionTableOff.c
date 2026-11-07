#define LOGGING_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Modules/Logging/Logging.h"

#ifdef LOGGING_FUNCTION_TABLE_STRUCT
#error Logging function table struct must not be emitted when disabled
#endif

int main(void) {
  Logging_TLogger logger;
  Logging_Logger_Init(&logger, "Test", LOGGING_LEVEL_INFO);
  Logging_Logger_Destroy(&logger);
  return 0;
}
