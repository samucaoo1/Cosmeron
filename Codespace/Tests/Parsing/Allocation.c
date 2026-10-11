#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static size_t allocationBudget, liveAllocations;
static void *test_malloc(size_t size) {
  void *memory;
  if (!allocationBudget) return NULL;
  --allocationBudget; memory=malloc(size); if(memory)++liveAllocations; return memory;
}
static void *test_realloc(void *pointer,size_t size) {
  void *memory;
  if (!allocationBudget) return NULL;
  --allocationBudget; memory=realloc(pointer,size); if(memory && !pointer)++liveAllocations; return memory;
}
static void test_free(void *memory) { if(memory) { assert(liveAllocations); --liveAllocations; free(memory); } }
#define malloc test_malloc
#define realloc test_realloc
#define free test_free
#include "../../Cosmeron/Modules/Parsing/Parsing.h"
#include "../../Cosmeron/Modules/CLI/CLI.h"
#undef malloc
#undef realloc
#undef free
int main(void) {
  size_t budget; bool allSucceeded=false;
  const char *json="{\"x\":[1,2,\"a\\u0000b\"],\"y\":{\"z\":true}}";
  const char *ini="global=yes\n[a]\nx=1\ny=2";
  const char *csv="a,b,c\n1,\"a\"\"b\",3";
  const char *toml="x=[1,2,3]\ntext=\"\"\"\nhello \\\n world\\U0001F600\"\"\"\n[[items]]\nx={a.b=1,a.c=2,}\n[[items]]\nx=2";
  const char *yaml="x: [one, {a: [\"\\U0001F600\\0\\L\", two]},]\na:\n  b: c";
  const char *xml="<a x='a&amp;b'><b>text</b><![CDATA[other]]></a>";
  for(budget=0;budget<200;++budget) {
    PARSING_TYPE(TJSONDocument) j={0}; PARSING_TYPE(TINIDocument) i={0}; PARSING_TYPE(TCSVDocument) c={0};
    PARSING_TYPE(TTOMLDocument) t={0}; PARSING_TYPE(TYAMLDocument) y={0}; PARSING_TYPE(TXMLDocument) x={0};
    OPSTATUS js,is,cs,ts,ys,xs;
    allocationBudget=budget; js=PARSING_FUNC(JSON_Parse)(json,strlen(json),&j,NULL); PARSING_FUNC(JSON_Free)(&j); assert(!liveAllocations);
    allocationBudget=budget; is=PARSING_FUNC(INI_Parse)(ini,strlen(ini),&i,NULL); PARSING_FUNC(INI_Free)(&i); assert(!liveAllocations);
    allocationBudget=budget; cs=PARSING_FUNC(CSV_Parse)(csv,strlen(csv),',',&c,NULL); PARSING_FUNC(CSV_Free)(&c); assert(!liveAllocations);
    allocationBudget=budget; ts=PARSING_FUNC(TOML_Parse)(toml,strlen(toml),&t,NULL); PARSING_FUNC(TOML_Free)(&t); assert(!liveAllocations);
    allocationBudget=budget; ys=PARSING_FUNC(YAML_Parse)(yaml,strlen(yaml),&y,NULL); PARSING_FUNC(YAML_Free)(&y); assert(!liveAllocations);
    allocationBudget=budget; xs=PARSING_FUNC(XML_Parse)(xml,strlen(xml),&x,NULL); PARSING_FUNC(XML_Free)(&x); assert(!liveAllocations);
    assert(js==STATUS_CONST(SUCCESS)||js==STATUS_CONST(OUT_OF_MEMORY));
    assert(is==STATUS_CONST(SUCCESS)||is==STATUS_CONST(OUT_OF_MEMORY));
    assert(cs==STATUS_CONST(SUCCESS)||cs==STATUS_CONST(OUT_OF_MEMORY));
    assert(ts==STATUS_CONST(SUCCESS)||ts==STATUS_CONST(OUT_OF_MEMORY));
    assert(ys==STATUS_CONST(SUCCESS)||ys==STATUS_CONST(OUT_OF_MEMORY));
    assert(xs==STATUS_CONST(SUCCESS)||xs==STATUS_CONST(OUT_OF_MEMORY));
    if(js==STATUS_CONST(SUCCESS)&&is==STATUS_CONST(SUCCESS)&&cs==STATUS_CONST(SUCCESS)&&ts==STATUS_CONST(SUCCESS)&&ys==STATUS_CONST(SUCCESS)&&xs==STATUS_CONST(SUCCESS)) {allSucceeded=true;break;}
  }
  assert(allSucceeded);
  /* Pull readers and declarative CLI parsing work even when allocation fails. */
  allocationBudget=0;
  { PARSING_TYPE(TJSONReader) reader; PARSING_TYPE(TJSONToken) token; bool end;
    assert(PARSING_FUNC(JSON_ReaderCreate)(json,strlen(json),&reader,NULL)==STATUS_CONST(SUCCESS));
    do { assert(PARSING_FUNC(JSON_Next)(&reader,&token,&end)==STATUS_CONST(SUCCESS)); }while(!end);
  }
  { PARSING_TYPE(TXMLReader) reader; PARSING_TYPE(TXMLEvent) token; bool end;
    assert(PARSING_FUNC(XML_ReaderCreate)(xml,strlen(xml),&reader,NULL)==STATUS_CONST(SUCCESS));
    do { assert(PARSING_FUNC(XML_Next)(&reader,&token,&end)==STATUS_CONST(SUCCESS)); }while(!end);
  }
  { CLI_TYPE(TCommand) command={.name="tool"}; char *argv[]={"tool"}; size_t count=1;
    assert(CLI_FUNC(Command_Parse)(&command,1,argv,NULL,0,&count,NULL)==STATUS_CONST(SUCCESS)&&count==0);
  }
  assert(!liveAllocations); return 0;
}
