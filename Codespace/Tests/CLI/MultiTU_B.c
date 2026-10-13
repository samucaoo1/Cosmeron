#include "../../Cosmeron/Modules/CLI/CLI.h"
int cli_b(void) { return CLI_FUNC(Arguments_Create)(0,NULL).argc; }
