#include "../../Cosmeron/Modules/Parsing/Parsing.h"
int parsing_a(void) {
  PARSING_TYPE(TJSONDocument) document={0};
  OPSTATUS status=PARSING_NS(Functions).JSON_Parse("[]",2,&document,NULL);
  PARSING_FUNC(JSON_Free)(&document); return status==STATUS_CONST(SUCCESS)?0:1;
}
