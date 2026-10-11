#include "../../Cosmeron/Modules/Parsing/YAML/YAML.h"
#include <assert.h>
static void rejected(const char *text,OPSTATUS expected) {
  PARSING_TYPE(TYAMLDocument) document={0};
  OPSTATUS status=PARSING_FUNC(YAML_Parse)(text,strlen(text),&document,NULL);
  assert(status==expected && !document.root);
}
int main(void) {
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
  rejected("x: [1,2]",STATUS_CONST(NOT_SUPPORTED)); rejected("x: 1\n---\nx: 2",STATUS_CONST(NOT_SUPPORTED));
  rejected("x:\n\ty: 1",STATUS_CONST(INVALID_SEQUENCE)); rejected("- key: value",STATUS_CONST(NOT_SUPPORTED));
  assert(PARSING_FUNC(YAML_Parse)("",0,&document,NULL)==STATUS_CONST(SUCCESS)); PARSING_FUNC(YAML_Free)(&document);
  assert(PARSING_FUNC(YAML_Parse)("true",4,&document,NULL)==STATUS_CONST(SUCCESS)); PARSING_FUNC(YAML_Free)(&document);
  return 0;
}
