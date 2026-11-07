#define COSMERON_NAMESPACE Custom
#define COSMERON_NAMESPACE_CONST CUSTOM
#include "../../Cosmeron/Modules/Parsing/Parsing.h"
#include <assert.h>
int main(void) {
  Custom_Parsing_TJSONDocument document={0};
  assert(Custom_Parsing_Functions.JSON_Parse("{}",2,&document,NULL)==CUSTOM_STATUS_SUCCESS);
  Custom_Parsing_JSON_Free(&document);
  return 0;
}
