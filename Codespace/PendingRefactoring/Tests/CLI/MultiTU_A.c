#include "../../Cosmeron/Modules/CLI/CLI.h"
int cli_a(void) { return CLI_FUNC(Arguments_Create)(0,NULL).argc; }
