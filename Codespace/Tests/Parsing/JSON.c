#include "../../Cosmeron/Modules/Parsing/JSON/JSON.h"
#include <assert.h>
static void rejected(const char *source) {
  PARSING_TYPE(TJSONDocument) document={0}; PARSING_TYPE(TDiagnostic) diagnostic={0};
  assert(PARSING_FUNC(JSON_Parse)(source,strlen(source),&document,&diagnostic)!=STATUS_CONST(SUCCESS));
  assert(!document.root);
}
int main(void) {
  const char *source="{\"number\":18446744073709551615,\"text\":\"A\\u0000\\uD83D\\uDE00\",\"items\":[true,null,-1.5e2]}";
  PARSING_TYPE(TJSONDocument) document={0}, roundtrip={0}; const PARSING_TYPE(TJSONValue) *value,*array;
  PARSING_TYPE(TSlice) text; uint64_t number; int64_t integer=8; double real; bool boolean;
  char output[1024], tiny[1]={'x'}; size_t size;
  assert(PARSING_FUNC(JSON_Parse)(source,strlen(source),&document,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_Get)(PARSING_FUNC(JSON_Root)(&document),"number",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_U64)(value,&number)==STATUS_CONST(SUCCESS) && number==UINT64_MAX);
  assert(PARSING_FUNC(JSON_I64)(value,&integer)==STATUS_CONST(OUT_OF_RANGE) && integer==8);
  assert(PARSING_FUNC(JSON_Get)(document.root,"text",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_String)(value,&text)==STATUS_CONST(SUCCESS) && text.size==6 && text.data[1]==0);
  assert(PARSING_FUNC(JSON_Get)(document.root,"items",&array)==STATUS_CONST(SUCCESS) && PARSING_FUNC(JSON_Size)(array)==3);
  assert(PARSING_FUNC(JSON_At)(array,0,&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_Bool)(value,&boolean)==STATUS_CONST(SUCCESS) && boolean);
  assert(PARSING_FUNC(JSON_At)(array,2,&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_F64)(value,&real)==STATUS_CONST(SUCCESS) && real==-150);
  assert(PARSING_FUNC(JSON_Write)(&document,tiny,1,&size)==STATUS_CONST(INSUFFICIENT_SPACE) && tiny[0]=='x');
  assert(PARSING_FUNC(JSON_Write)(&document,output,sizeof(output),&size)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_Parse)(output,size,&roundtrip,NULL)==STATUS_CONST(SUCCESS)); PARSING_FUNC(JSON_Free)(&roundtrip);
  assert(PARSING_FUNC(JSON_Set)(document.root,"number","42",2,NULL)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_Set)(document.root,"new","{}",2,NULL)==STATUS_CONST(SUCCESS));
  { PARSING_TYPE(TJSONValue) *mutableArray, *mutableItem;
    assert(PARSING_FUNC(JSON_GetMutable)(document.root,"items",&mutableArray)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_AtMutable)(mutableArray,0,&mutableItem)==STATUS_CONST(SUCCESS));
    assert(mutableItem->kind==PARSING_CONST(JSON_BOOL));
    assert(PARSING_FUNC(JSON_Append)(mutableArray,"\"end\"",5,NULL)==STATUS_CONST(SUCCESS));
  }
  assert(PARSING_FUNC(JSON_Remove)(document.root,"text")==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_Remove)(document.root,"missing")==STATUS_CONST(NOT_FOUND));
  assert(PARSING_FUNC(JSON_Get)(document.root,"number",&value)==STATUS_CONST(SUCCESS));
  assert(PARSING_FUNC(JSON_I64)(value,&integer)==STATUS_CONST(SUCCESS) && integer==42);
  assert(PARSING_FUNC(JSON_Parse)("{}",2,&document,NULL)==STATUS_CONST(INVALID_ARGUMENT));
  PARSING_FUNC(JSON_Free)(&document); PARSING_FUNC(JSON_Free)(&document);
  rejected(""); rejected("01"); rejected("+1"); rejected("1."); rejected("1e"); rejected("[1,]"); rejected("{\"x\":1,}");
  rejected("{\"x\":1,\"x\":2}"); rejected("\"\\uD800\""); rejected("\"\\uDC00\""); rejected("true false"); rejected("\"\n\"");
  { PARSING_TYPE(TLimits) limits={2,10,100}; PARSING_TYPE(TDiagnostic) diagnostic={0};
    assert(PARSING_FUNC(JSON_ParseWithLimits)("[[0]]",5,&limits,&document,&diagnostic)==STATUS_CONST(OUT_OF_RANGE));
    assert(diagnostic.code==PARSING_CONST(DIAGNOSTIC_DEPTH)); limits.maxDepth=64; limits.maxNodes=1;
    assert(PARSING_FUNC(JSON_ParseWithLimits)("[0]",3,&limits,&document,&diagnostic)==STATUS_CONST(OUT_OF_RANGE));
  }
  { PARSING_TYPE(TJSONReader) reader; PARSING_TYPE(TJSONToken) token; bool end; size_t count=0;
    assert(PARSING_FUNC(JSON_ReaderCreate)(source,strlen(source),&reader,NULL)==STATUS_CONST(SUCCESS));
    while (1) { assert(PARSING_FUNC(JSON_Next)(&reader,&token,&end)==STATUS_CONST(SUCCESS)); if(end)break; ++count;
      if(token.kind==PARSING_CONST(JSON_TOKEN_STRING)) { assert(PARSING_FUNC(JSON_TokenWrite)(&token,output,sizeof(output),&size)==STATUS_CONST(SUCCESS)); assert(size==6); }
    }
    assert(count==12);
  }
  { PARSING_TYPE(TJSONReader) reader; PARSING_TYPE(TJSONToken) token; bool end;
    assert(PARSING_FUNC(JSON_ReaderCreate)("[1,]",4,&reader,NULL)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_Next)(&reader,&token,&end)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_Next)(&reader,&token,&end)==STATUS_CONST(SUCCESS));
    assert(PARSING_FUNC(JSON_Next)(&reader,&token,&end)==STATUS_CONST(INVALID_SEQUENCE));
  }
  return 0;
}
