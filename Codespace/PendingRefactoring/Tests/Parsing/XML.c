#include "../../Cosmeron/Modules/Parsing/XML/XML.h"
#include <assert.h>
static void rejected(const char *text,OPSTATUS expected) {
  PARSING_TYPE(TXMLDocument) document={0};
  OPSTATUS status=PARSING_FUNC(XML_Parse)(text,strlen(text),&document,NULL);
  assert(status==expected && !document.events.data);
}
int main(void) {
  const char *source="<?xml version='1.0' encoding='UTF-8'?><root a='A&amp;B' n='&#x1F600;'><child/>before<![CDATA[<raw>]]>after<!--comment--><?target value?></root>";
  PARSING_TYPE(TXMLDocument) document={0},copy={0}; PARSING_TYPE(TSlice) value; char output[2048]; size_t size;
  assert(PARSING_FUNC(XML_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(XML_Attribute)(&document,1,"a",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(value,PARSING_FUNC(Slice_Create)("A&B",3)));
  assert(PARSING_FUNC(XML_Attribute)(&document,1,"n",&value)==STATUS_CONST(SUCCESS) && value.size==4);
  assert(PARSING_FUNC(XML_SetAttribute)(&document,1,"a",PARSING_FUNC(Slice_Create)("x\ny",3))==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(XML_SetAttribute)(&document,1,"new",PARSING_FUNC(Slice_Create)("< & >",5))==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(XML_Write)(&document,output,sizeof(output),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(XML_Parse)(output,size,&copy,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(XML_Attribute)(&copy,1,"a",&value)==STATUS_CONST(SUCCESS) && value.size==3 && value.data[1]=='\n');
  PARSING_FUNC(XML_Free)(&document); PARSING_FUNC(XML_Free)(&copy);
  rejected("<a><b></a>",STATUS_CONST(INVALID_SEQUENCE)); rejected("<a x='1' x='2'/>",STATUS_CONST(ALREADY_EXISTS));
  rejected("<a/><b/>",STATUS_CONST(INVALID_SEQUENCE)); rejected("<!DOCTYPE a><a/>",STATUS_CONST(NOT_SUPPORTED));
  rejected("<a>&external;</a>",STATUS_CONST(NOT_SUPPORTED)); rejected("<a>&#0;</a>",STATUS_CONST(INVALID_SEQUENCE));
  rejected("<a>]]></a>",STATUS_CONST(INVALID_SEQUENCE)); rejected("<a><!--a--b--></a>",STATUS_CONST(INVALID_SEQUENCE));
  rejected("<?xml version='1.1'?><a/>",STATUS_CONST(NOT_SUPPORTED)); rejected("<a x='<'/>",STATUS_CONST(INVALID_SEQUENCE));
  { PARSING_TYPE(TXMLReader) reader; PARSING_TYPE(TXMLEvent) event; bool end; size_t count=0;
    assert(PARSING_FUNC(XML_ReaderCreate)(source,strlen(source),&reader,NULL)==STATUS_CONST(SUCCESS));
    do { assert(PARSING_FUNC(XML_Next)(&reader,&event,&end)==STATUS_CONST(SUCCESS)); if(!end) { ++count; assert(PARSING_FUNC(XML_TextWrite)(&event,output,sizeof(output),&size)==STATUS_CONST(SUCCESS)); } }while(!end);
    assert(count==12);
  }
  return 0;
}
