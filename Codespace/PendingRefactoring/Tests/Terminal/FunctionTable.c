#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"

int main(void) {
  const struct Terminal_FunctionTable *ft = Terminal_GetFunctionTable();
  assert(ft != NULL);
  assert(ft->Standard != NULL);
  assert(ft->StandardHandle != NULL);
  assert(ft->Write != NULL);
  assert(ft->WriteLine != NULL);
  assert(ft->Clear != NULL);
  assert(ft->Cursor_SetPosition != NULL);
  assert(ft->Cursor_GetPosition != NULL);
  assert(ft->Style_Set != NULL);
  assert(ft->Style_Get != NULL);
  assert(ft->EnterAlternateScreen != NULL);
  assert(ft->LeaveAlternateScreen != NULL);
  assert(ft->EnterRawMode != NULL);
  assert(ft->LeaveRawMode != NULL);
  assert(ft->SaveState != NULL);
  assert(ft->RestoreState != NULL);

  assert(ft->Standard() == Terminal_Standard());
  return 0;
}
