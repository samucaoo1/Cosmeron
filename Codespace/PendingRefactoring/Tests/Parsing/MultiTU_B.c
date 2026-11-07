#include "../../Cosmeron/Modules/Parsing/Parsing.h"
int parsing_b(void) {
  PARSING_TYPE(TCSVDocument) document={0};
  OPSTATUS status=PARSING_NS(Functions).CSV_Parse("x",1,',',&document,NULL);
  PARSING_FUNC(CSV_Free)(&document); return status==STATUS_CONST(SUCCESS)?0:1;
}
