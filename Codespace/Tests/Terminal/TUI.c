#include <assert.h>
#include <string.h>

#include "../../Cosmeron/Modules/Graphics/UI/UI.h"

#define OK STATUS_CONST(SUCCESS)

static TQUAD_TYPE(uint16) make_rect(uint16_t l, uint16_t t, uint16_t r, uint16_t b) {
  TQUAD_TYPE(uint16) q = {0};
  q.left = l;
  q.top = t;
  q.right = r;
  q.bottom = b;
  return q;
}

static void test_layout_helpers(void) {
  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == OK);

  /* Root layout */
  assert(UI_FUNC(Layout)(&ui, (TDUAL_TYPE(uint16)){.col = 80, .row = 24}) == OK);
  assert(ui.layouts[0].region.right == 79);
  assert(ui.layouts[0].region.bottom == 23);

  /* Horizontal split (50/50 with 1 gap) */
  TQUAD_TYPE(uint16) parent = make_rect(0, 0, 79, 23);
  TQUAD_TYPE(uint16) top, bottom;
  assert(UI_FUNC(Layout_SplitHorizontal)(parent, 1, 1, 1, &top, &bottom) == OK);
  assert(top.top == 0);
  assert(bottom.bottom == 23);
  assert(top.bottom + 2 == bottom.top);

  /* Vertical split (1:3 with 1 gap) */
  TQUAD_TYPE(uint16) left, right;
  assert(UI_FUNC(Layout_SplitVertical)(parent, 1, 3, 1, &left, &right) == OK);
  assert(left.left == 0);
  assert(right.right == 79);
  assert(left.right + 2 == right.left);

  /* Grid 3x2 */
  TQUAD_TYPE(uint16) cell;
  assert(UI_FUNC(Layout_Grid)(parent, 3, 2, 0, 0, 1, 1, &cell) == OK);
  assert(cell.left == 0);
  assert(cell.top == 0);
  assert(UI_FUNC(Layout_Grid)(parent, 3, 2, 2, 1, 1, 1, &cell) == OK);
  assert(cell.right == 79);
  assert(cell.bottom == 23);

  /* Center */
  TQUAD_TYPE(uint16) centered;
  assert(UI_FUNC(Layout_Center)(parent, (TDUAL_TYPE(uint16)){.col = 40, .row = 10}, &centered) == OK);
  assert(centered.left == 20);
  assert(centered.right == 59);
  assert(centered.top == 7);
  assert(centered.bottom == 16);
}

static void test_event_dispatch(void) {
  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == OK);

  /* Dispatch TERMINAL_TYPE(TEvent) */
  TERMINAL_TYPE(TEvent) termEvent = {0};
  termEvent.kind = TERMINAL_CONST(EVENT_KEY);
  termEvent.key = 'Z';
  assert(UI_FUNC(Dispatch)(&ui, &termEvent) == OK);
  assert(ui.keyCount == 1);
  assert(ui.keys[0] == 'Z');

  /* Tab dispatches focus next */
  termEvent.key = '\t';
  ui.focusIds[0] = 10;
  ui.focusIds[1] = 20;
  ui.focusCount = 2;
  ui.focus = 10;
  assert(UI_FUNC(Dispatch)(&ui, &termEvent) == OK);
  assert(ui.focus == 20);

  /* Dispatch INPUT_TYPE(TEvent) */
  INPUT_TYPE(TEvent) inEvent = {0};
  inEvent.type = INPUT_CONST(EVENT_MOUSE_POSITION);
  inEvent.data.motion.x = 15.0f;
  inEvent.data.motion.y = 8.0f;
  assert(UI_FUNC(DispatchInput)(&ui, &inEvent) == OK);
  assert(ui.pointerKnown);
  assert(ui.pointer.x == 15);
  assert(ui.pointer.y == 8);

  /* Mouse button click */
  inEvent.type = INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS);
  inEvent.data.mouseButton = INPUT_CONST(MOUSE_BUTTON_LEFT);
  assert(UI_FUNC(DispatchInput)(&ui, &inEvent) == OK);
  assert(ui.pointerPressed);

  /* Mouse wheel scroll */
  inEvent.type = INPUT_CONST(EVENT_MOUSE_SCROLL);
  inEvent.data.motion.y = 3.0f;
  assert(UI_FUNC(DispatchInput)(&ui, &inEvent) == OK);
  assert(ui.scroll == 3);

  /* Key down from Input module */
  inEvent.type = INPUT_CONST(EVENT_KEY_PRESS);
  inEvent.data.key = INPUT_CONST(KEY_DOWN);
  assert(UI_FUNC(DispatchInput)(&ui, &inEvent) == OK);
}

static void test_focus_management(void) {
  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == OK);

  assert(UI_FUNC(Focus_Set)(&ui, 42) == OK);
  assert(UI_FUNC(IsFocused)(&ui, 42));
  assert(!UI_FUNC(IsFocused)(&ui, 43));

  assert(UI_FUNC(Focus_Clear)(&ui) == OK);
  assert(ui.focus == 0);
  assert(!UI_FUNC(IsFocused)(&ui, 42));
}

static void test_scrollbar_widget(void) {
  TERMINAL_TYPE(TCanvas) canvas = {0};
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas, (TDUAL_TYPE(uint16)){.col = 40, .row = 20}) == OK);

  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == OK);

  int64_t vVal = 20;
  int64_t hVal = 50;

  assert(UI_FUNC(Begin)(&ui, &canvas) == OK);
  /* Vertical scrollbar */
  assert(UI_FUNC(Scrollbar)(&ui, make_rect(38, 1, 38, 18), 0, 100, 10, true, &vVal) == OK);
  /* Horizontal scrollbar */
  assert(UI_FUNC(Scrollbar)(&ui, make_rect(2, 18, 36, 18), 0, 100, 10, false, &hVal) == OK);
  assert(UI_FUNC(End)(&ui) == OK);

  /* Test keyboard interaction on focused vertical scrollbar */
  UI_TYPE(TScrollbar) sb = {0};
  sb.id = 12345;
  sb.region = make_rect(10, 2, 10, 15);
  sb.minimum = 0;
  sb.maximum = 100;
  sb.pageSize = 10;
  sb.vertical = true;
  sb.value = 50;

  assert(UI_FUNC(Focus_Set)(&ui, 12345) == OK);
  assert(UI_FUNC(Key)(&ui, TERMINAL_CONST(KEY_DOWN)) == OK);
  assert(UI_FUNC(Begin)(&ui, &canvas) == OK);
  assert(UI_FUNC(ScrollbarEx)(&ui, &sb) == OK);
  assert(sb.value == 51);
  assert(UI_FUNC(End)(&ui) == OK);

  TERMINAL_FUNC(Canvas_Free)(&canvas);
}

static OPSTATUS custom_component_draw(TComponent *self, UI_TYPE(TContext) *context) {
  return UI_FUNC(Label)(context, self->region, "Custom Component");
}

static void test_component_object(void) {
  TComponent comp = {0};
  TQUAD_TYPE(uint16) r = make_rect(5, 5, 25, 7);
  assert(UI_COMPONENT_FUNC(Create)(&comp, r, "TestComp") == OK);
  assert(comp.api != NULL);
  assert(comp.api->IsVisible(&comp));
  assert(!comp.api->IsDisabled(&comp));
  assert(comp.api->Region(&comp).left == 5);

  comp.api->SetDisabled(&comp, true);
  assert(comp.api->IsDisabled(&comp));
  comp.api->SetDisabled(&comp, false);

  comp.api->SetCallbacks(&comp, custom_component_draw, NULL);

  TERMINAL_TYPE(TCanvas) canvas = {0};
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas, (TDUAL_TYPE(uint16)){.col = 40, .row = 20}) == OK);
  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == OK);

  assert(ui.api->Begin(&ui, &canvas) == OK);
  assert(comp.api->Draw(&comp, &ui) == OK);
  assert(ui.api->End(&ui) == OK);

  assert(comp.api->Destroy(&comp) == OK);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}

static void test_render_and_present(void) {
  TERMINAL_TYPE(TCanvas) canvas = {0};
  TERMINAL_TYPE(TCanvas) targetCanvas = {0};
  TTerminalScreen screen = {0};

  assert(TERMINAL_FUNC(Canvas_Create)(&canvas, (TDUAL_TYPE(uint16)){.col = 30, .row = 10}) == OK);
  assert(TERMINAL_FUNC(Canvas_Create)(&targetCanvas, (TDUAL_TYPE(uint16)){.col = 30, .row = 10}) == OK);
  assert(TERMINAL_SCREEN_FUNC(Init)(&screen) == OK);
  assert(screen.api->Create(&screen, (TDUAL_TYPE(uint16)){.col = 30, .row = 10}, false, false) == OK);

  UI_TYPE(TContext) ui = {0};
  assert(UI_FUNC(Init)(&ui) == OK);

  assert(UI_FUNC(Begin)(&ui, &canvas) == OK);
  assert(UI_FUNC(Label)(&ui, make_rect(2, 2, 20, 2), "Render Test") == OK);
  assert(UI_FUNC(End)(&ui) == OK);

  /* Render to canvas */
  assert(UI_FUNC(Render)(&ui, &targetCanvas) == OK);

  /* Present to screen buffer */
  assert(UI_FUNC(Present)(&ui, &screen) == OK);

  screen.api->Destroy(&screen);
  TERMINAL_FUNC(Canvas_Free)(&targetCanvas);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}

int main(void) {
  test_layout_helpers();
  test_event_dispatch();
  test_focus_management();
  test_scrollbar_widget();
  test_component_object();
  test_render_and_present();
  return 0;
}
