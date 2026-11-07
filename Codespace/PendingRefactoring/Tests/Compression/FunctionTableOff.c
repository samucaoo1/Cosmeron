#define COMPRESSION_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Modules/Compression/Compression.h"
/* Names must remain available for the application when table emission is off. */
struct Compression_FunctionTable { int value; };
int Compression_Functions;
int main(void) { return Compression_Functions; }
