#include "../../Cosmeron/Modules/CLI/CLI.h"
int main(int argc, char **argv) {
  static const CLI_TYPE(TOption) options[] = {
    {.shortName='h', .longName="help", .kind=CLI_CONST(FLAG), .help="Show help"},
    {.shortName='j', .longName="jobs", .kind=CLI_CONST(VALUE), .valueName="count", .help="Worker count"}
  };
  static const CLI_TYPE(TCommand) command = {.name="options", .description="CLI example", .options=options, .optionCount=2};
  CLI_TYPE(TMatch) matches[2]; CLI_TYPE(TDiagnostic) diagnostic={0}; size_t count=0,index;
  OPSTATUS status=CLI_FUNC(Command_Parse)(&command,argc,argv,matches,2,&count,&diagnostic);
  if(status!=STATUS_CONST(SUCCESS)) { fprintf(stderr,"Invalid argument at index %zu\n",diagnostic.argumentIndex); return 1; }
  for(index=0;index<count;++index) {
    if(matches[index].option==options) {
      char help[1024]; size_t size;
      status=CLI_FUNC(Help_Write)(&command,help,sizeof(help),&size);
      if(status!=STATUS_CONST(SUCCESS))return 1;
      fwrite(help,1,size,stdout);
    } else {
      uint64_t jobs;
      status=PARSING_FUNC(Slice_U64)(PARSING_FUNC(Slice_Create)(matches[index].value,strlen(matches[index].value)),&jobs);
      if(status!=STATUS_CONST(SUCCESS))return 1;
      printf("Jobs: %llu\n",(unsigned long long)jobs);
    }
  }
  return 0;
}
