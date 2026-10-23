#include "../../Cosmeron/Modules/Logging/Logging.h"

int log_b(void) {
  Logging_TLogger logger;
  Logging_Logger_Init(&logger, "B", LOGGING_LEVEL_WARN);
  Logging_Logger_Destroy(&logger);
  return 2;
}
