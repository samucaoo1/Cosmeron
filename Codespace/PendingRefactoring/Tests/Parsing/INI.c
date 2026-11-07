#include "../../Cosmeron/Modules/Parsing/INI/INI.h"
#include <assert.h>
int main(void) {
  const char *source="# comment\r\nglobal=yes\r\n[video] ; comment\r\nwidth = 1920\r\npath=C:\\x;literal#value\r\n";
  PARSING_TYPE(TINIDocument) document={0}, copy={0}; PARSING_TYPE(TSlice) value; char text[512]; size_t size; uint64_t width;
  assert(PARSING_FUNC(INI_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Get)(&document,"video","width",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_U64)(value,&width)==STATUS_CONST(SUCCESS) && width==1920);
  assert(PARSING_FUNC(INI_Get)(&document,"video","path",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(Slice_Equal)(value,PARSING_FUNC(Slice_Create)("C:\\x;literal#value",18)));
  assert(PARSING_FUNC(INI_Set)(&document,"video","width","640")==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Set)(&document,"","later","global")==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Set)(&document,"audio","volume","80")==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Set)(&document,"bad]","x","y")==STATUS_CONST(INVALID_ARGUMENT));
  assert(PARSING_FUNC(INI_Set)(&document,"","x"," padded ")==STATUS_CONST(INVALID_ARGUMENT));
  assert(PARSING_FUNC(INI_Write)(&document,text,sizeof(text),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Parse)(text,size,&copy,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Get)(&copy,"","later",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Remove)(&document,"video","path")==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(INI_Remove)(&document,"video","path")==STATUS_CONST(NOT_FOUND));
  PARSING_FUNC(INI_Free)(&document); PARSING_FUNC(INI_Free)(&copy);
  assert(PARSING_FUNC(INI_Parse)("x=1\nx=2",7,&document,NULL)==STATUS_CONST(ALREADY_EXISTS));
  assert(PARSING_FUNC(INI_Parse)("[broken",7,&document,NULL)==STATUS_CONST(INVALID_SEQUENCE));
  assert(PARSING_FUNC(INI_Parse)("=x",2,&document,NULL)==STATUS_CONST(INVALID_SEQUENCE));
  { PARSING_TYPE(TINIReader) reader; PARSING_TYPE(TINIEntry) entry; bool end; size_t count=0;
    assert(PARSING_FUNC(INI_ReaderCreate)(source,strlen(source),&reader,NULL)==STATUS_CONST(SUCCESS));
    do { assert(PARSING_FUNC(INI_Next)(&reader,&entry,&end)==STATUS_CONST(SUCCESS)); if(!end)++count; } while(!end);
    assert(count==3);
  }
  return 0;
}
