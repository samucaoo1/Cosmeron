#define CLI_DISABLE_FUNCTION_TABLE
#define PARSING_DISABLE_FUNCTION_TABLE
#define CONTAINER_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Modules/CLI/CLI.h"
#include "../../Cosmeron/Modules/Parsing/Parsing.h"
#ifdef CLI_FUNCTION_TABLE_STRUCT
#error CLI function table must not be emitted
#endif
#ifdef PARSING_FUNCTION_TABLE_STRUCT
#error Parsing function table must not be emitted
#endif
int main(void) { return 0; }
