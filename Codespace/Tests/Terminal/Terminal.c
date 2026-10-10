#include <assert.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"
int main(void) {
  TERMINAL_TYPE(TTerminal) *standard=TERMINAL_FUNC(Standard)();
  TERMINAL_TYPE(THandle) *out=TERMINAL_FUNC(StandardHandle)(TERMINAL_CONST(STDOUT));
  TDUAL_TYPE(uint16) position={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  size_t count=0;
  assert(standard && out && out->stream==stdout);
  assert(TERMINAL_FUNC(StandardHandle)((TERMINAL_TYPE(StandardStream))123)==NULL);
  assert(TERMINAL_FUNC(WriteTo)(NULL,"x",1,&count)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(ReadLine)(NULL,1,&count)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(GetChar)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Print)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Scan("%s", (char[16]){0})==STATUS_CONST(NOT_SUPPORTED));
  assert(TERMINAL_FUNC(Scan("%n", (int[1]){0})==STATUS_CONST(NOT_SUPPORTED));
  assert(TERMINAL_FUNC(Style_Get)(&style)==STATUS_CONST(SUCCESS));
  assert((style.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND))!=0);
  assert(TERMINAL_FUNC(Style_Get)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Cursor_GetPosition)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Size_Get)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Key_Hit)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(InputMode_Get)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Title_Set)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(InputMode_Set)((TERMINAL_TYPE(InputMode))100)==STATUS_CONST(INVALID_ARGUMENT));
  (void)position;
  assert(TERMINAL_FUNC(Flush)()==STATUS_CONST(SUCCESS));
  return 0;
}
