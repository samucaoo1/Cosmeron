#include "../../Cosmeron/Modules/File/File.h"
#include "../../Cosmeron/Modules/Parsing/JSON/JSON.h"
int main(int argc,char **argv) {
  char *source=NULL,*output=NULL; size_t sourceSize=0,outputSize=0;
  PARSING_TYPE(TJSONDocument) document={0}; PARSING_TYPE(TDiagnostic) diagnostic={0};
  const PARSING_TYPE(TJSONValue) *value; uint64_t width; OPSTATUS status;
  if(argc!=3) { fprintf(stderr,"Usage: configuration input.json output.json\n"); return 1; }
  status=FILE_FUNC(LoadText)(argv[1],&source,&sourceSize);
  if(status!=STATUS_CONST(SUCCESS))return 1;
  status=PARSING_FUNC(JSON_Parse)(source,sourceSize,&document,&diagnostic);
  Memory_Free(source);
  if(status!=STATUS_CONST(SUCCESS)) { fprintf(stderr,"Invalid document at %zu:%zu\n",diagnostic.line,diagnostic.column); return 1; }
  status=PARSING_FUNC(JSON_Get)(document.root,"width",&value);
  if(status==STATUS_CONST(SUCCESS)) {
    status=PARSING_FUNC(JSON_U64)(value,&width);
    if(status==STATUS_CONST(SUCCESS))printf("Width: %llu\n",(unsigned long long)width);
  }
  status=PARSING_FUNC(JSON_Write)(&document,NULL,0,&outputSize);
  if(status==STATUS_CONST(INSUFFICIENT_SPACE)) {
    void *memory;
    status=MEMORY_FUNC(AllocBytes)(&memory,outputSize+1);
    if(status==STATUS_CONST(SUCCESS)) {
      output=(char *)memory;
      status=PARSING_FUNC(JSON_Write)(&document,output,outputSize+1,&outputSize);
      if(status==STATUS_CONST(SUCCESS))status=FILE_FUNC(SaveAtomic)(argv[2],output,outputSize);
    }
  }
  Memory_Free(output); PARSING_FUNC(JSON_Free)(&document);
  return status==STATUS_CONST(SUCCESS)?0:1;
}
