#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/Terminal/Terminal.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

int main(void) {
  /* ------------------------------------------------------------
   * 1. Lifecycle and Creation
   * ------------------------------------------------------------ */
  TTerminalScreen screen = {0};
  OK(TERMINAL_SCREEN_FUNC(Init)(&screen));
  assert(screen.api != NULL);
  assert(!screen.api->IsInitialized(&screen));

  TDUAL_TYPE(uint16) size = {.col = 40, .row = 10};
  OK(screen.api->Create(&screen, size, false, false));
  assert(screen.api->IsInitialized(&screen));
  assert(screen.api->Size(&screen).col == 40);
  assert(screen.api->Size(&screen).row == 10);
  assert(!screen.api->IsAlternate(&screen));
  assert(!screen.api->IsRaw(&screen));

  /* ------------------------------------------------------------
   * 2. Cell Writes and Reads
   * ------------------------------------------------------------ */
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cellAttr;
  memset(&cellAttr, 0, sizeof(cellAttr));
  cellAttr.flags = TEXT_GRID_ATTRIBUTE_CONST(BOLD);
  cellAttr.foreground = TEXT_GRID_COLOR_RGB(0, 255, 0);

  OK(screen.api->SetCell(&screen, 5, 2, 'X', cellAttr));

  uint32_t outCp = 0;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) readAttr;
  OK(screen.api->GetCell(&screen, 5, 2, &outCp, &readAttr));
  assert(outCp == 'X');
  assert((readAttr.flags & TEXT_GRID_ATTRIBUTE_CONST(BOLD)) != 0);
  assert(readAttr.foreground == TEXT_GRID_COLOR_RGB(0, 255, 0));

  /* Out of range check */
  assert(screen.api->SetCell(&screen, 100, 2, 'Z', cellAttr) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(screen.api->GetCell(&screen, 5, 100, &outCp, &readAttr) ==
         STATUS_CONST(OUT_OF_RANGE));

  /* ------------------------------------------------------------
   * 3. String Writes
   * ------------------------------------------------------------ */
  OK(screen.api->WriteString(&screen, 10, 4, "Cosmeron Terminal", cellAttr));

  OK(screen.api->GetCell(&screen, 10, 4, &outCp, &readAttr));
  assert(outCp == 'C');
  OK(screen.api->GetCell(&screen, 11, 4, &outCp, &readAttr));
  assert(outCp == 'o');
  OK(screen.api->GetCell(&screen, 12, 4, &outCp, &readAttr));
  assert(outCp == 's');

  /* ------------------------------------------------------------
   * 4. Clear and ClearWith
   * ------------------------------------------------------------ */
  OK(screen.api->ClearWith(&screen, cellAttr, '.'));
  OK(screen.api->GetCell(&screen, 0, 0, &outCp, &readAttr));
  assert(outCp == '.');
  OK(screen.api->GetCell(&screen, 39, 9, &outCp, &readAttr));
  assert(outCp == '.');

  OK(screen.api->Clear(&screen));

  /* ------------------------------------------------------------
   * 5. Resize and Present
   * ------------------------------------------------------------ */
  TDUAL_TYPE(uint16) newSize = {.col = 60, .row = 15};
  OK(screen.api->Resize(&screen, newSize));
  assert(screen.api->Size(&screen).col == 60);
  assert(screen.api->Size(&screen).row == 15);

  OK(screen.api->WriteString(&screen, 0, 0, "Test Frame", cellAttr));
  OK(screen.api->Present(&screen));

  /* ------------------------------------------------------------
   * 6. Cleanup
   * ------------------------------------------------------------ */
  OK(screen.api->Destroy(&screen));
  assert(!screen.api->IsInitialized(&screen));

  return 0;
}
