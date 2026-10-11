#include "../../Cosmeron/Modules/Parsing/TOML/TOML.h"
#include <assert.h>
static void rejected(const char *text, OPSTATUS expected) {
  PARSING_TYPE(TTOMLDocument) document={0};
  OPSTATUS status=PARSING_FUNC(TOML_Parse)(text,strlen(text),&document,NULL);
  assert(status==expected && !document.root);
}
int main(void) {
  const char *source="name='Cosmeron'\ncount=1_024\nhex=0xFF\nmin=-9223372036854775808\nratio=1.5e2\nenabled=true\narray=[1, 'x', false,]\ninline={a=1,b=\"x\"}\n[video]\nwidth=1920\n[video.window]\nname='main'\n";
  PARSING_TYPE(TTOMLDocument) document={0}, copy={0}; const PARSING_TYPE(TTOMLValue) *value,*table;
  int64_t integer; double real; bool boolean; PARSING_TYPE(TSlice) text; char output[2048]; size_t size;
  assert(PARSING_FUNC(TOML_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(document.root,"name",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_String)(value,&text)==STATUS_CONST(SUCCESS) && text.size==8);
  assert(PARSING_FUNC(TOML_Get)(document.root,"count",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_I64)(value,&integer)==STATUS_CONST(SUCCESS) && integer==1024);
  assert(PARSING_FUNC(TOML_Get)(document.root,"hex",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_I64)(value,&integer)==STATUS_CONST(SUCCESS) && integer==255);
  assert(PARSING_FUNC(TOML_Get)(document.root,"min",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_I64)(value,&integer)==STATUS_CONST(SUCCESS) && integer==INT64_MIN);
  assert(PARSING_FUNC(TOML_Get)(document.root,"ratio",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_F64)(value,&real)==STATUS_CONST(SUCCESS) && real==150);
  assert(PARSING_FUNC(TOML_Get)(document.root,"enabled",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Bool)(value,&boolean)==STATUS_CONST(SUCCESS) && boolean);
  assert(PARSING_FUNC(TOML_Get)(document.root,"array",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_At)(value,1,&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(document.root,"video",&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(table,"window",&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Set)(&document,"count","42",2,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Set)(&document,"new","[1,2]",5,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Remove)(&document,"hex")==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Write)(&document,output,sizeof(output),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Parse)(output,size,&copy,NULL)==STATUS_CONST(SUCCESS));
  PARSING_FUNC(TOML_Free)(&document); PARSING_FUNC(TOML_Free)(&copy);
  rejected("x=01",STATUS_CONST(INVALID_SEQUENCE)); rejected("x=1__2",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x=1\nx=2",STATUS_CONST(ALREADY_EXISTS)); rejected("x=9223372036854775808",STATUS_CONST(OUT_OF_RANGE));
  rejected("x=2026-10-11",STATUS_CONST(NOT_SUPPORTED)); rejected("x=\"\"\"multiline\"\"\"",STATUS_CONST(NOT_SUPPORTED));
  rejected("[[items]]\nx=1",STATUS_CONST(NOT_SUPPORTED)); rejected("x={a.b=1}",STATUS_CONST(NOT_SUPPORTED));
  rejected("x=0xFG",STATUS_CONST(INVALID_SEQUENCE)); rejected("x=[1",STATUS_CONST(INCOMPLETE_SEQUENCE));
  return 0;
}
