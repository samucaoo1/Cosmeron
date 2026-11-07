#include "../../Cosmeron/Modules/Logging/Logging.h"

int log_a(void) {
  Logging_TLogger logger;
  Logging_Logger_Init(&logger, "A", LOGGING_LEVEL_INFO);
  Logging_Logger_Destroy(&logger);
  return 1;
}
