#include "../../Cosmeron/Modules/Parsing/CSV/CSV.h"
#include <assert.h>
int main(void) {
  const char *source="name,description,empty\r\na,\"line1\nline2 \"\"quoted\"\"\",\r\n";
  PARSING_TYPE(TCSVDocument) document={0}, copy={0}; PARSING_TYPE(TSlice) field; char text[512]; size_t size;
  assert(PARSING_FUNC(CSV_Parse)(source,strlen(source),',',&document,NULL)==STATUS_CONST(SUCCESS));
  assert(document.records.size==2 && document.records.data[1].count==3);
  assert(PARSING_FUNC(CSV_Field)(document.records.data+1,1,&field)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(field,PARSING_FUNC(Slice_Create)("line1\nline2 \"quoted\"",20)));
  assert(PARSING_FUNC(CSV_Set)(&document,1,0,PARSING_FUNC(Slice_Create)("a,b",3))==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(CSV_Write)(&document,text,sizeof(text),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(CSV_Parse)(text,size,',',&copy,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(CSV_Field)(copy.records.data+1,0,&field)==STATUS_CONST(SUCCESS) && field.size==3);
  PARSING_FUNC(CSV_Free)(&document); PARSING_FUNC(CSV_Free)(&copy);
  assert(PARSING_FUNC(CSV_Parse)("a,",2,',',&document,NULL)==STATUS_CONST(SUCCESS));
  assert(document.records.size==1 && document.records.data[0].count==2 && !document.records.data[0].fields[1].size);
  PARSING_FUNC(CSV_Free)(&document);
  assert(PARSING_FUNC(CSV_Parse)("",0,',',&document,NULL)==STATUS_CONST(SUCCESS) && !document.records.size); PARSING_FUNC(CSV_Free)(&document);
  assert(PARSING_FUNC(CSV_Parse)("a\tb\n",4,'\t',&document,NULL)==STATUS_CONST(SUCCESS) && document.records.data[0].count==2); PARSING_FUNC(CSV_Free)(&document);
  assert(PARSING_FUNC(CSV_Parse)("\"broken",7,',',&document,NULL)==STATUS_CONST(INCOMPLETE_SEQUENCE));
  assert(PARSING_FUNC(CSV_Parse)("\"x\"y",4,',',&document,NULL)==STATUS_CONST(INVALID_SEQUENCE));
  { PARSING_TYPE(TCSVReader) reader; PARSING_TYPE(TCSVField) token; bool end; size_t count=0;
    assert(PARSING_FUNC(CSV_ReaderCreate)(source,strlen(source),',',&reader,NULL)==STATUS_CONST(SUCCESS));
    do { assert(PARSING_FUNC(CSV_Next)(&reader,&token,&end)==STATUS_CONST(SUCCESS)); if(!end) { ++count; assert(PARSING_FUNC(CSV_FieldWrite)(&token,text,sizeof(text),&size)==STATUS_CONST(SUCCESS)); } } while(!end);
    assert(count==6);
  }
  return 0;
}
