#define COSMERON_NAMESPACE Custom
#define COSMERON_NAMESPACE_CONST CUSTOM
#include "../../Cosmeron/Modules/CLI/CLI.h"
#include <assert.h>
int main(void) {
  Custom_CLI_TArguments args = Custom_CLI_Functions.Arguments_Create(0,NULL);
  assert(!Custom_CLI_Arguments_Has(&args,"--help"));
  return 0;
}
