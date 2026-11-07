#define COSMERON_NAMESPACE Custom
#define COSMERON_NAMESPACE_CONST CUSTOM
#include "../../Cosmeron/Modules/File/File.h"
int main(void) {
  Custom_File_TFile file = {0};
  return Custom_File_Close(&file) == CUSTOM_STATUS_SUCCESS ? 0 : 1;
}
