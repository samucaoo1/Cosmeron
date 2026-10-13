#include "../../Cosmeron/Modules/Parsing/YAML/YAML.h"
#include <assert.h>
static void rejected(const char *text,OPSTATUS expected) {
  PARSING_TYPE(TYAMLDocument) document={0};
  OPSTATUS status=PARSING_FUNC(YAML_Parse)(text,strlen(text),&document,NULL);
  assert(status==expected && !document.root);
}
static void extended(void) {
  const char *source="del: \"\\x7f\"\nitems: [1, {name: 'it''s', values: [true, \"a,b # c\",]}, []]\n"
    "escapes: \"\\0\\a\\v\\e\\ \\_\\N\\L\\P\\xE9\\U0001F600\"\n"
    "quoted: {\"a:b\":\"value\", empty: }\n";
  const char expected[]="\0\a\v\x1b \xc2\xa0\xc2\x85\xe2\x80\xa8\xe2\x80\xa9\xc3\xa9\xf0\x9f\x98\x80";
  PARSING_TYPE(TYAMLDocument) document={0},copy={0};
  const PARSING_TYPE(TYAMLValue) *v,*items;
  PARSING_TYPE(TSlice) text; char output[4096]; size_t size;
  assert(PARSING_FUNC(YAML_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Get)(document.root,"items",&items)==STATUS_CONST(SUCCESS) && items->count==3);
  assert(PARSING_FUNC(YAML_At)(items,1,&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Get)(v,"values",&v)==STATUS_CONST(SUCCESS) && v->count==2);
  assert(PARSING_FUNC(YAML_At)(v,1,&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Scalar)(v,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)("a,b # c",7)));
  assert(PARSING_FUNC(YAML_Get)(document.root,"escapes",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Scalar)(v,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)(expected,sizeof(expected)-1)));
  assert(PARSING_FUNC(YAML_Write)(&document,output,sizeof(output),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Parse)(output,size,&copy,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Get)(copy.root,"escapes",&v)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Scalar)(v,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)(expected,sizeof(expected)-1)));
  PARSING_FUNC(YAML_Free)(&document); PARSING_FUNC(YAML_Free)(&copy);
  assert(PARSING_FUNC(YAML_Parse)("[1, {a: 2}]",11,&document,NULL)==STATUS_CONST(SUCCESS));
  PARSING_FUNC(YAML_Free)(&document);
  rejected("{a: 1, a: 2}",STATUS_CONST(ALREADY_EXISTS));
  rejected("[1,,2]",STATUS_CONST(INVALID_SEQUENCE));
  rejected("[1,2",STATUS_CONST(INCOMPLETE_SEQUENCE));
  rejected("{a: 1]",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x: \"\\uD800\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x: \"\\UFFFFFFFF\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x: \"\\xGG\"",STATUS_CONST(INVALID_SEQUENCE));
  rejected("x: \"\\q\"",STATUS_CONST(INVALID_SEQUENCE));
  { char deep[160]; size_t i; PARSING_TYPE(TDiagnostic) diagnostic={0};
    for(i=0;i<70;++i) deep[i]='[';
    for(i=70;i<140;++i) deep[i]=']';
    assert(PARSING_FUNC(YAML_Parse)(deep,140,&document,&diagnostic)==STATUS_CONST(OUT_OF_RANGE));
    assert(!document.root && diagnostic.code==PARSING_CONST(DIAGNOSTIC_DEPTH));
  }
}
int main(void) {
  extended();
  const char *source="---\nname: Cosmeron # comment\nvideo:\n  width: 1920\n  modes:\n    - normal\n    - 'it''s bold'\n    - \"Unicode \\u00e1\"\nempty: []\nmap: {}\nurl: https://example.com/#anchor\n";
  PARSING_TYPE(TYAMLDocument) document={0},copy={0}; const PARSING_TYPE(TYAMLValue) *value,*video; PARSING_TYPE(TSlice) text;
  char output[2048]; size_t size;
  assert(PARSING_FUNC(YAML_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Get)(document.root,"video",&video)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Get)(video,"modes",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_At)(value,1,&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Scalar)(value,&text)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(text,PARSING_FUNC(Slice_Create)("it's bold",9)));
  { PARSING_TYPE(TYAMLValue) *mutableVideo,*mutableModes,*mutableItem;
    assert(PARSING_FUNC(YAML_GetMutable)(document.root,"video",&mutableVideo)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(YAML_GetMutable)(mutableVideo,"modes",&mutableModes)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(YAML_AtMutable)(mutableModes,1,&mutableItem)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(YAML_SetScalar)(mutableItem,PARSING_FUNC(Slice_Create)("new\nvalue",9))==STATUS_CONST(SUCCESS));
  }
  assert(PARSING_FUNC(YAML_Write)(&document,output,sizeof(output),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(YAML_Parse)(output,size,&copy,NULL)==STATUS_CONST(SUCCESS));
  PARSING_FUNC(YAML_Free)(&document); PARSING_FUNC(YAML_Free)(&copy);
  rejected("x: 1\nx: 2",STATUS_CONST(ALREADY_EXISTS)); rejected("x: &anchor value",STATUS_CONST(NOT_SUPPORTED));
  rejected("x: *alias",STATUS_CONST(NOT_SUPPORTED)); rejected("x: |\n  text",STATUS_CONST(NOT_SUPPORTED));
   rejected("x: 1\n---\nx: 2",STATUS_CONST(NOT_SUPPORTED));
  rejected("x:\n\ty: 1",STATUS_CONST(INVALID_SEQUENCE)); rejected("- key: value",STATUS_CONST(NOT_SUPPORTED));
  assert(PARSING_FUNC(YAML_Parse)("",0,&document,NULL)==STATUS_CONST(SUCCESS)); PARSING_FUNC(YAML_Free)(&document);
  assert(PARSING_FUNC(YAML_Parse)("true",4,&document,NULL)==STATUS_CONST(SUCCESS)); PARSING_FUNC(YAML_Free)(&document);
  return 0;
}
