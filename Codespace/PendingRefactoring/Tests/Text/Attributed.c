#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../Cosmeron/Modules/Text/Text.h"

#define OK(x) assert((x) == STATUS_CONST(SUCCESS))

int main(void) {
  /* ------------------------------------------------------------
   * 1. Lifecycle & Creation
   * ------------------------------------------------------------ */
  TAttributedText attr = {0};
  OK(TEXT_ATTRIBUTED_FUNC(Init)(&attr));
  assert(attr.api != NULL);

  OK(attr.api->Create(&attr, 16));
  assert(attr.api->Length(&attr) == 0U);
  assert(attr.api->SpanCount(&attr) == 0U);
  assert(strcmp(attr.api->Text(&attr), "") == 0);

  /* ------------------------------------------------------------
   * 2. SetText & AppendText
   * ------------------------------------------------------------ */
  OK(attr.api->SetText(&attr, "Hello World", 11));
  assert(attr.api->Length(&attr) == 11U);
  assert(strcmp(attr.api->Text(&attr), "Hello World") == 0);

  OK(attr.api->AppendText(&attr, "!", 1));
  assert(attr.api->Length(&attr) == 12U);
  assert(strcmp(attr.api->Text(&attr), "Hello World!") == 0);

  /* ------------------------------------------------------------
   * 3. Spans & Attribute Queries
   * ------------------------------------------------------------ */
  /* Span 1: "Hello" (indices 0..4) is Bold + Red Foreground */
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell1;
  memset(&cell1, 0, sizeof(cell1));
  cell1.flags = TEXT_GRID_ATTRIBUTE_CONST(BOLD);
  cell1.foreground = TEXT_GRID_COLOR_RGB(255, 0, 0);
  OK(attr.api->AddSpan(&attr, 0, 5, cell1));

  /* Span 2: "World" (indices 6..10) is Italic + Single Underline */
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell2;
  memset(&cell2, 0, sizeof(cell2));
  cell2.flags = TEXT_GRID_ATTRIBUTE_CONST(ITALIC);
  cell2.underline = TEXT_GRID_UNDERLINE_CONST(SINGLE);
  OK(attr.api->AddSpan(&attr, 6, 5, cell2));

  assert(attr.api->SpanCount(&attr) == 2U);

  /* Query index 2 ('l' in "Hello") */
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) queryAttr;
  OK(attr.api->GetAttributeAt(&attr, 2, &queryAttr));
  assert((queryAttr.flags & TEXT_GRID_ATTRIBUTE_CONST(BOLD)) != 0);
  assert(queryAttr.foreground == TEXT_GRID_COLOR_RGB(255, 0, 0));
  assert(queryAttr.underline == 0);

  /* Query index 5 (' ' space between words) */
  OK(attr.api->GetAttributeAt(&attr, 5, &queryAttr));
  assert(queryAttr.flags == 0);
  assert(queryAttr.foreground == 0);
  assert(queryAttr.underline == 0);

  /* Query index 8 ('r' in "World") */
  OK(attr.api->GetAttributeAt(&attr, 8, &queryAttr));
  assert((queryAttr.flags & TEXT_GRID_ATTRIBUTE_CONST(ITALIC)) != 0);
  assert(queryAttr.underline == TEXT_GRID_UNDERLINE_CONST(SINGLE));
  assert(queryAttr.foreground == 0);

  /* Out of range index */
  assert(attr.api->GetAttributeAt(&attr, 99, &queryAttr) ==
         STATUS_CONST(OUT_OF_RANGE));

  /* ------------------------------------------------------------
   * 4. ANSI Sequence Rendering
   * ------------------------------------------------------------ */
  char ansi[256];
  size_t written = 0;
  OK(attr.api->RenderAnsi(&attr, ansi, sizeof(ansi), &written));
  assert(written > 0);
  assert(strstr(ansi, "\033[1m") != NULL);   /* Bold */
  assert(strstr(ansi, "\033[3m") != NULL);   /* Italic */
  assert(strstr(ansi, "\033[4m") != NULL);   /* Underline */
  assert(strstr(ansi, "\033[0m") != NULL);   /* Reset */
  assert(strstr(ansi, "Hello") != NULL);
  assert(strstr(ansi, "World") != NULL);

  /* ------------------------------------------------------------
   * 5. Clear & Destroy
   * ------------------------------------------------------------ */
  OK(attr.api->ClearSpans(&attr));
  assert(attr.api->SpanCount(&attr) == 0U);

  OK(attr.api->Clear(&attr));
  assert(attr.api->Length(&attr) == 0U);

  OK(attr.api->Destroy(&attr));
  assert(attr.api->Length(&attr) == 0U);
  assert(attr.api->SpanCount(&attr) == 0U);

  return 0;
}
