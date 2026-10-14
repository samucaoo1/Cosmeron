#include "../../Cosmeron/Modules/CLI/CLI.h"
#include <assert.h>

static void direct(void) {
  char *argv[] = {"tool", "--jobs=42", "-oresult", "--tag", "a", "--tag=b", "-v", "--", "--jobs=9"};
  CLI_TYPE(TArguments) arguments = CLI_FUNC(Arguments_Create)(9, argv);
  const char *value = NULL; uint64_t number = 7; int64_t signedNumber; double real;
  assert(CLI_FUNC(Arguments_Has)(&arguments, "-v"));
  assert(CLI_FUNC(Arguments_Count)(&arguments, "--tag") == 2);
  assert(CLI_FUNC(Arguments_StringAt)(&arguments, "--tag", 1, &value) == STATUS_CONST(SUCCESS) && !strcmp(value, "b"));
  assert(CLI_FUNC(Arguments_String)(&arguments, "-o", &value) == STATUS_CONST(SUCCESS) && !strcmp(value, "result"));
  Text_TView8 viewVal;
  assert(CLI_FUNC(Arguments_View)(&arguments, "-o", &viewVal) == STATUS_CONST(SUCCESS) && Text_View8_EqualCStr(viewVal, "result"));
  assert(CLI_FUNC(Arguments_ViewAt)(&arguments, "--tag", 0, &viewVal) == STATUS_CONST(SUCCESS) && Text_View8_EqualCStr(viewVal, "a"));
  assert(CLI_FUNC(Arguments_U64)(&arguments, "--jobs", &number) == STATUS_CONST(SUCCESS) && number == 42);
  assert(CLI_FUNC(Arguments_I64)(&arguments, "--jobs", &signedNumber) == STATUS_CONST(SUCCESS) && signedNumber == 42);
  assert(CLI_FUNC(Arguments_F64)(&arguments, "--jobs", &real) == STATUS_CONST(SUCCESS) && real == 42);
  assert(CLI_FUNC(Arguments_U64)(&arguments, "--missing", &number) == STATUS_CONST(NOT_FOUND) && number == 42);
  assert(CLI_FUNC(Arguments_String)(&arguments, "--missing", &value) == STATUS_CONST(NOT_FOUND));
  assert(CLI_FUNC(Arguments_U64)(&arguments, "--jobs", NULL) == STATUS_CONST(INVALID_ARGUMENT));
  { char *bad[] = {"tool", "--jobs=12abc"}; arguments = CLI_FUNC(Arguments_Create)(2, bad);
    assert(CLI_FUNC(Arguments_U64)(&arguments, "--jobs", &number) == STATUS_CONST(INVALID_SEQUENCE) && number == 42);
  }
  { char *bad[] = {"tool", "--jobs"}; arguments = CLI_FUNC(Arguments_Create)(2, bad);
    assert(CLI_FUNC(Arguments_String)(&arguments, "--jobs", &value) == STATUS_CONST(INCOMPLETE_SEQUENCE));
  }
}
static void declarative(void) {
  const CLI_TYPE(TOption) options[] = {
    {.shortName='v', .longName="verbose", .kind=CLI_CONST(FLAG), .help="Details"},
    {.shortName='a', .longName="all", .kind=CLI_CONST(FLAG), .exclusiveGroup=1},
    {.shortName='b', .longName="brief", .kind=CLI_CONST(FLAG), .exclusiveGroup=1},
    {.shortName='o', .longName="output", .kind=CLI_CONST(VALUE), .required=true, .valueName="file"},
    {.longName="tag", .kind=CLI_CONST(VALUE), .repeatable=true}
  };
  const CLI_TYPE(TPositional) positionals[] = {{.name="input", .required=true}, {.name="extra", .variadic=true}};
  const CLI_TYPE(TCommand) command = {.name="tool", .description="Convert files", .options=options, .optionCount=5, .positionals=positionals, .positionalCount=2};
  char *argv[] = {"tool", "-va", "-oresult", "--tag=a", "--tag", "b", "input", "--", "-negative"};
  CLI_TYPE(TMatch) matches[8], unchanged[8]; CLI_TYPE(TDiagnostic) diagnostic = {0}; size_t count = 99, size;
  char help[512], tiny[2] = {'x','y'};
  assert(CLI_FUNC(Command_Parse)(&command, 9, argv, matches, 8, &count, &diagnostic) == STATUS_CONST(SUCCESS));
  assert(count == 7 && matches[0].option == options && matches[1].option == options+1);
  assert(matches[2].argumentIndex == 2 && !strcmp(matches[2].value,"result"));
  assert(matches[6].positional == positionals+1 && !strcmp(matches[6].value,"-negative"));
  memcpy(unchanged, matches, sizeof(matches));
  assert(CLI_FUNC(Command_Parse)(&command, 9, argv, matches, 1, &count, &diagnostic) == STATUS_CONST(INSUFFICIENT_SPACE));
  assert(count == 7 && !memcmp(matches, unchanged, sizeof(matches)));
  assert(CLI_FUNC(Help_Write)(&command, NULL, 0, &size) == STATUS_CONST(INSUFFICIENT_SPACE));
  assert(CLI_FUNC(Help_Write)(&command, help, sizeof(help), &size) == STATUS_CONST(SUCCESS));
  assert(strstr(help,"--output file") && strstr(help,"<input>"));
  assert(CLI_FUNC(Help_Write)(&command, tiny, sizeof(tiny), &size) == STATUS_CONST(INSUFFICIENT_SPACE) && tiny[0]=='x' && tiny[1]=='y');
  { char *bad[] = {"tool", "-ab", "-ox", "in"}; count=99;
    assert(CLI_FUNC(Command_Parse)(&command,4,bad,matches,8,&count,&diagnostic)==STATUS_CONST(INVALID_ARGUMENT));
    assert(count==99 && diagnostic.code==CLI_CONST(DIAGNOSTIC_EXCLUSIVE));
  }
  { char *bad[] = {"tool", "-vv", "-ox", "in"};
    assert(CLI_FUNC(Command_Parse)(&command,4,bad,matches,8,&count,&diagnostic)==STATUS_CONST(ALREADY_EXISTS));
  }
  { char *bad[] = {"tool", "--unknown"};
    assert(CLI_FUNC(Command_Parse)(&command,2,bad,matches,8,&count,&diagnostic)==STATUS_CONST(NOT_FOUND));
    assert(diagnostic.code==CLI_CONST(DIAGNOSTIC_UNKNOWN));
  }
  { char *bad[] = {"tool", "-o"};
    assert(CLI_FUNC(Command_Parse)(&command,2,bad,matches,8,&count,&diagnostic)==STATUS_CONST(INCOMPLETE_SEQUENCE));
  }
  { char *bad[] = {"tool", "in"};
    assert(CLI_FUNC(Command_Parse)(&command,2,bad,matches,8,&count,&diagnostic)==STATUS_CONST(NOT_FOUND));
    assert(diagnostic.code==CLI_CONST(DIAGNOSTIC_REQUIRED));
  }
  { char *bad[] = {"tool", "--verbose=yes", "-ox", "in"};
    assert(CLI_FUNC(Command_Parse)(&command,4,bad,matches,8,&count,&diagnostic)==STATUS_CONST(INVALID_ARGUMENT));
  }
}
static void subcommands(void) {
  CLI_TYPE(TCommand) child = {.name="convert"};
  CLI_TYPE(TCommand) root = {.name="tool", .commands=&child, .commandCount=1, .requireCommand=true};
  char *argv[] = {"tool", "convert"}; CLI_TYPE(TMatch) matches[2]; size_t count = 0;
  assert(CLI_FUNC(Command_Parse)(&root,2,argv,matches,2,&count,NULL)==STATUS_CONST(SUCCESS));
  assert(count==1 && matches[0].command==&child);
  assert(CLI_FUNC(Command_Parse)(&root,1,argv,matches,2,&count,NULL)==STATUS_CONST(NOT_FOUND));
  root.commands=&root;
  assert(CLI_FUNC(Command_Parse)(&root,2,argv,matches,2,&count,NULL)==STATUS_CONST(INVALID_ARGUMENT));
}
int main(void) { direct(); declarative(); subcommands(); return 0; }
