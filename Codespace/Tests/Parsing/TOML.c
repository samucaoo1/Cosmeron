#include "../../Cosmeron/Modules/Parsing/TOML/TOML.h"
#include <assert.h>
static void rejected(const char *text, OPSTATUS expected) {
  PARSING_TYPE(TTOMLDocument) document={0};
  OPSTATUS status=PARSING_FUNC(TOML_Parse)(text,strlen(text),&document,NULL);
  assert(status==expected && !document.root);
}
static void extended(void) {
  const char *source =
    "negativeExponent=1e-2\n"
    "del=\"\\u007f\"\n"
    "basic=\"tab\t\\xE9\\U0001F600\\e\"\n"
    "multiline=\"\"\"\r\nfirst\r\nsecond\"\"\"\n"
    "joined=\"\"\"one \\\n  \n  two\"\"\"\n"
    "literal=\x27\x27\x27\nC:\\path\nfile\x27\x27\x27\n"
    "quotes=\"\"\"\"quoted\"\"\"\"\n"
    "inline={\n a.b=1, # comment\n a.c=2,\n}\n"
    "[[items]]\nname='first'\n[items.details]\nsize=1\n"
    "[[items.children]]\nname='child'\n"
    "[[items]]\nname='second'\n[items.details]\nsize=2\n";
  PARSING_TYPE(TTOMLDocument) document={0},copy={0};
  const PARSING_TYPE(TTOMLValue) *v,*table,*array;
  PARSING_TYPE(TSlice) text;
  int64_t number;
  char output[4096]; size_t size;
  assert(PARSING_FUNC(TOML_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  { double real;
    assert(PARSING_FUNC(TOML_Get)(document.root,"negativeExponent",&v)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(TOML_F64)(v,&real)==STATUS_CONST(SUCCESS) && real==0.01);
  }
  assert(PARSING_FUNC(TOML_Get)(document.root,"multiline",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_String)(v,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)("first\nsecond",12)));
  assert(PARSING_FUNC(TOML_Get)(document.root,"joined",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_String)(v,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)("one two",7)));
  assert(PARSING_FUNC(TOML_Get)(document.root,"basic",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_String)(v,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)("tab\t\xc3\xa9\xf0\x9f\x98\x80\x1b",11)));
  assert(PARSING_FUNC(TOML_Get)(document.root,"inline",&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(table,"a",&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(table,"c",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_I64)(v,&number)==STATUS_CONST(SUCCESS) && number==2);
  assert(PARSING_FUNC(TOML_Get)(document.root,"items",&array)==STATUS_CONST(SUCCESS) && array->count==2);
  assert(PARSING_FUNC(TOML_At)(array,0,&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(table,"children",&v)==STATUS_CONST(SUCCESS) && v->count==1);
  assert(PARSING_FUNC(TOML_At)(array,1,&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(table,"details",&table)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(table,"size",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_I64)(v,&number)==STATUS_CONST(SUCCESS) && number==2);
  assert(PARSING_FUNC(TOML_Write)(&document,output,sizeof(output),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Parse)(output,size,&copy,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(TOML_Get)(copy.root,"items",&array)==STATUS_CONST(SUCCESS) && array->count==2);
  PARSING_FUNC(TOML_Free)(&document); PARSING_FUNC(TOML_Free)(&copy);
  rejected("x=\"\\uD800\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x=\"\\U00110000\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x=\"\\/\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x=\"\"\"unterminated",STATUS_CONST(INCOMPLETE_SEQUENCE));
  rejected("\"\"\"key\"\"\"=1",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x=\"\"\"a\rb\"\"\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x={a.b=1,a.b=2}",STATUS_CONST(ALREADY_EXISTS));
  rejected("items=[]\n[[items]]\nx=1",STATUS_CONST(ALREADY_EXISTS));
  rejected("[items]\nx=1\n[[items]]",STATUS_CONST(ALREADY_EXISTS));
  rejected("[[items]]\nx=1\n[items]",STATUS_CONST(ALREADY_EXISTS));
}
int main(void) {
  extended();
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
  rejected("x=2026-10-11",STATUS_CONST(NOT_SUPPORTED));

  rejected("x=0xFG",STATUS_CONST(INVALID_SEQUENCE)); rejected("x=[1",STATUS_CONST(INCOMPLETE_SEQUENCE));
  return 0;
}
