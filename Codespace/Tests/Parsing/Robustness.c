#include "../../Cosmeron/Modules/Parsing/Parsing.h"
#include <assert.h>

/* Deterministic malformed-input exploration; every successful parse must survive
 * serialization and reparsing, and every failure must leave an empty owner. */
static void exercise(const char *source, size_t size) {
  PARSING_TYPE(TJSONDocument) json={0}, jsonCopy={0};
  PARSING_TYPE(TINIDocument) ini={0}, iniCopy={0};
  PARSING_TYPE(TCSVDocument) csv={0}, csvCopy={0};
  PARSING_TYPE(TTOMLDocument) toml={0}, tomlCopy={0};
  PARSING_TYPE(TYAMLDocument) yaml={0}, yamlCopy={0};
  PARSING_TYPE(TXMLDocument) xml={0}, xmlCopy={0};
  char output[4096]; size_t length; OPSTATUS status;
  status=PARSING_FUNC(JSON_Parse)(source,size,&json,NULL);
  if(status==STATUS_CONST(SUCCESS)) {
    assert(PARSING_FUNC(JSON_Write)(&json,output,sizeof(output),&length)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_Parse)(output,length,&jsonCopy,NULL)==STATUS_CONST(SUCCESS));
  } else assert(!json.root);
  PARSING_FUNC(JSON_Free)(&json); PARSING_FUNC(JSON_Free)(&jsonCopy);
  status=PARSING_FUNC(INI_Parse)(source,size,&ini,NULL);
  if(status==STATUS_CONST(SUCCESS)) {
    assert(PARSING_FUNC(INI_Write)(&ini,output,sizeof(output),&length)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(INI_Parse)(output,length,&iniCopy,NULL)==STATUS_CONST(SUCCESS));
  } else assert(!ini.first);
  PARSING_FUNC(INI_Free)(&ini); PARSING_FUNC(INI_Free)(&iniCopy);
  status=PARSING_FUNC(CSV_Parse)(source,size,',',&csv,NULL);
  if(status==STATUS_CONST(SUCCESS)) {
    assert(PARSING_FUNC(CSV_Write)(&csv,output,sizeof(output),&length)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(CSV_Parse)(output,length,',',&csvCopy,NULL)==STATUS_CONST(SUCCESS));
  } else assert(!csv.records.data);
  PARSING_FUNC(CSV_Free)(&csv); PARSING_FUNC(CSV_Free)(&csvCopy);
  status=PARSING_FUNC(TOML_Parse)(source,size,&toml,NULL);
  if(status==STATUS_CONST(SUCCESS)) {
    assert(PARSING_FUNC(TOML_Write)(&toml,output,sizeof(output),&length)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(TOML_Parse)(output,length,&tomlCopy,NULL)==STATUS_CONST(SUCCESS));
  } else assert(!toml.root);
  PARSING_FUNC(TOML_Free)(&toml); PARSING_FUNC(TOML_Free)(&tomlCopy);
  status=PARSING_FUNC(YAML_Parse)(source,size,&yaml,NULL);
  if(status==STATUS_CONST(SUCCESS)) {
    assert(PARSING_FUNC(YAML_Write)(&yaml,output,sizeof(output),&length)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(YAML_Parse)(output,length,&yamlCopy,NULL)==STATUS_CONST(SUCCESS));
  } else assert(!yaml.root);
  PARSING_FUNC(YAML_Free)(&yaml); PARSING_FUNC(YAML_Free)(&yamlCopy);
  status=PARSING_FUNC(XML_Parse)(source,size,&xml,NULL);
  if(status==STATUS_CONST(SUCCESS)) {
    assert(PARSING_FUNC(XML_Write)(&xml,output,sizeof(output),&length)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(XML_Parse)(output,length,&xmlCopy,NULL)==STATUS_CONST(SUCCESS));
  } else assert(!xml.events.data);
  PARSING_FUNC(XML_Free)(&xml); PARSING_FUNC(XML_Free)(&xmlCopy);
}
int main(void) {
  static const char alphabet[]="abcxyz0123456789{}[]<>=&;:'\"\\,.-+_#*!|> \t\r\n";
  uint32_t state=42; char source[128]; size_t i,j;
  exercise("",0); exercise("{}",2); exercise("[]",2); exercise("a: {}\nb: []",11);
  exercise("\xff",1); exercise("\xc0\xaf",2); exercise("x\0y",3);
  for(i=0;i<2500;++i) {
    size_t length=i%sizeof(source);
    for(j=0;j<length;++j) { state=state*1664525u+1013904223u; source[j]=alphabet[state%(sizeof(alphabet)-1)]; }
    exercise(source,length);
  }
  { PARSING_TYPE(TDiagnostic) diagnostic={0}; PARSING_TYPE(TJSONDocument) document={0};
    assert(PARSING_FUNC(JSON_Parse)("{\r\n\"x\":\r\n?}",12,&document,&diagnostic)==STATUS_CONST(INVALID_SEQUENCE));
    assert(diagnostic.line==3 && diagnostic.column==1);
  }
  return 0;
}
