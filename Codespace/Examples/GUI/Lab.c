/* cc -std=c11 -D_POSIX_C_SOURCE=200809L -I Codespace Codespace/Examples/GUI/Lab.c -o lab
 * ./lab                         native window
 * ./lab --frames 3              native smoke run
 * ./lab --ppm /tmp/cosmeron.ppm  software-only preview (no display required)
 */
#include "../../Cosmeron/Modules/Graphics/GUI/GUI.h"
#include "../../Cosmeron/Modules/Graphics/Window/Window.h"
#include "../../Cosmeron/Modules/Concurrency/Thread/Sleep.h"
#include "../../Cosmeron/Modules/Chronometry/Timer/Timer.h"
#include <stdio.h>
#include <locale.h>
static TQUAD_TYPE(float) region(float x, float y, float width, float height) {
  TQUAD_TYPE(float) r;
  r.value1 = x;
  r.value2 = y;
  r.value3 = width;
  r.value4 = height;
  return r;
}
#define DEMO_TRY(OP)                                                                               \
  do {                                                                                             \
    OPSTATUS demoStatus = (OP);                                                                    \
    if (demoStatus != STATUS_CONST(SUCCESS))                                                       \
      return demoStatus;                                                                           \
  } while (0)
static OPSTATUS draw(GUI_TYPE(TContext) * gui) {
  static char name[64] = "Okarin";
  static GUI_TYPE(TInputText) input = {.id = 100,
                                       .buffer = name,
                                       .capacity = sizeof(name),
                                       .length = 6,
                                       .caret = 6,
                                       .selectionAnchor = 6};
  static GUI_TYPE(TButton) save = {.id = 101, .label = "Save"};
  static bool enabled = true, expanded = true, selectedNode = false, comboOpen = false;
  static float volume = 62, number = 3;
  static uint32_t choice = 0, color = UINT32_C(0xFF4F91E8);
  static size_t tab = 0, selected = 1, row = 0;
  static const char *tabs[] = {"Controls", "Drawing", "Settings"};
  static const char *items[] = {"Time machine", "Phone microwave", "Divergence meter"};
  static const char *cells[] = {"Module", "C11", "Input", "Ready", "GUI", "Ready"};
  DEMO_TRY(GUI_FUNC(Label)(gui, region(20, 12, 580, 38), "COSMERON / GADGET LAB"));
  DEMO_TRY(GUI_FUNC(Tabs)(gui, region(20, 52, 580, 28), tabs, 3, &tab));
  DEMO_TRY(GUI_FUNC(Panel_Begin)(gui, region(20, 92, 280, 300), "Direct API"));
  DEMO_TRY(GUI_FUNC(Checkbox)(gui, region(34, 126, 250, 28), "Enable experiment", &enabled));
  DEMO_TRY(GUI_FUNC(Radio)(gui, region(34, 164, 120, 26), "Mode A", 0, &choice));
  DEMO_TRY(GUI_FUNC(Radio)(gui, region(164, 164, 120, 26), "Mode B", 1, &choice));
  DEMO_TRY(GUI_FUNC(Label)(gui, region(34, 200, 250, 20), "Power"));
  DEMO_TRY(GUI_FUNC(Slider)(gui, region(34, 224, 250, 22), 0, 100, &volume));
  DEMO_TRY(GUI_FUNC(Progress)(gui, region(34, 258, 250, 18), volume / 100));
  DEMO_TRY(GUI_FUNC(TreeNode)(gui, region(34, 288, 250, 24), "Devices", &expanded, &selectedNode));
  if (expanded)
    DEMO_TRY(GUI_FUNC(List)(gui, region(44, 320, 230, 54), items, 3, &selected));
  DEMO_TRY(GUI_FUNC(Panel_End)(gui));
  DEMO_TRY(GUI_FUNC(Panel_Begin)(gui, region(320, 92, 280, 300), "Struct API"));
  input.region = region(334, 126, 250, 28);
  DEMO_TRY(GUI_FUNC(InputTextEx)(gui, &input));
  save.region = region(334, 164, 118, 28);
  save.disabled = !enabled;
  DEMO_TRY(GUI_FUNC(ButtonEx)(gui, &save));
  DEMO_TRY(GUI_FUNC(Number)(gui, region(466, 164, 118, 28), 0, 10, 1, &number));
  DEMO_TRY(GUI_FUNC(ComboBox)(gui, region(334, 206, 250, 26), items, 3, &selected, &comboOpen));
  DEMO_TRY(GUI_FUNC(ColorPicker)(gui, region(334, 246, 110, 96), &color));
  DEMO_TRY(GUI_FUNC(DrawRectangle)(gui, region(462, 246, 120, 96), color, 14));
  DEMO_TRY(GUI_FUNC(Table)(gui, region(334, 352, 250, 28), cells, 3, 2, &row));
  DEMO_TRY(GUI_FUNC(Panel_End)(gui));
  DEMO_TRY(GUI_FUNC(StatusBar)(gui, region(20, 408, 580, 24), save.pressed ? "Saved" : "Ready",
                               "C11 / header-only"));
  return STATUS_CONST(SUCCESS);
}
static OPSTATUS savePPM(const GRAPHICS_TYPE(TSurface) * surface, const char *path) {
  FILE *file = fopen(path, "wb");
  uint32_t x, y;
  if (!file)
    return STATUS_CONST(GENERIC_ERROR);
  if (fprintf(file, "P6\n%u %u\n255\n", surface->size.x, surface->size.y) < 0) {
    fclose(file);
    return STATUS_CONST(GENERIC_ERROR);
  }
  for (y = 0; y < surface->size.y; ++y)
    for (x = 0; x < surface->size.x; ++x) {
      uint32_t p = surface->pixels[(size_t)y * surface->stride + x];
      unsigned char rgb[3] = {(unsigned char)(p >> 16), (unsigned char)(p >> 8), (unsigned char)p};
      if (fwrite(rgb, 1, 3, file) != 3) {
        fclose(file);
        return STATUS_CONST(GENERIC_ERROR);
      }
    }
  return fclose(file) == 0 ? STATUS_CONST(SUCCESS) : STATUS_CONST(GENERIC_ERROR);
}
int main(int argc, char **argv) {
  GRAPHICS_TYPE(TSurface) surface = {0};
  GUI_TYPE(TContext) gui = {0};
  WINDOW_TYPE(TWindow) window = {0};
  CHRONOMETRY_TYPE(FrameLimiter) limiter = {0};
  unsigned long frames = 0, limit = 0;
  const char *ppm = NULL;
  OPSTATUS status;
  setlocale(LC_CTYPE, "");
  if (argc == 3 && strcmp(argv[1], "--ppm") == 0)
    ppm = argv[2];
  else if (argc == 3 && strcmp(argv[1], "--frames") == 0)
    limit = strtoul(argv[2], NULL, 10);
  status = GRAPHICS_FUNC(Surface_Create)(&surface, (TDUAL_TYPE(uint32)){.x = 640, .y = 448});
  if (status != STATUS_CONST(SUCCESS))
    return 1;
  if (!ppm) {
    status = WINDOW_FUNC(Open)(&window, "Cosmeron GUI", surface.size);
    if (status != STATUS_CONST(SUCCESS))
      goto cleanup;
  }
  status = TIMER_FUNC(FrameLimiterFromFPS)(60, &limiter);
  if (status != STATUS_CONST(SUCCESS))
    goto cleanup;
  do {
    INPUT_TYPE(TEvent) inputEvents[GUI_MAX_EVENTS];
    size_t inputCount = 0, i;
    WINDOW_TYPE(TEvent) event = {0};
    CHRONOMETRY_TYPE(Duration) remaining;
    status = TIMER_FUNC(FrameLimiterBegin)(&limiter);
    if (status != STATUS_CONST(SUCCESS))
      break;
    if (!ppm)
      while (inputCount < GUI_MAX_EVENTS) {
        status = WINDOW_FUNC(Poll)(&window, &event);
        if (status == STATUS_CONST(WOULD_BLOCK)) {
          status = STATUS_CONST(SUCCESS);
          break;
        }
        if (status != STATUS_CONST(SUCCESS))
          goto cleanup;
        if (event.type == WINDOW_CONST(EVENT_CLOSE))
          break;
        if (event.type == WINDOW_CONST(EVENT_RESIZE)) {
          status = GRAPHICS_FUNC(Surface_Resize)(&surface, event.size);
          if (status != STATUS_CONST(SUCCESS))
            goto cleanup;
        } else if (event.type == WINDOW_CONST(EVENT_INPUT))
          inputEvents[inputCount++] = event.input;
      }
    if (!ppm && !WINDOW_FUNC(IsOpen)(&window))
      break;
    status = GRAPHICS_FUNC(Surface_Clear)(&surface, UINT32_C(0xFF0D121C));
    if (status != STATUS_CONST(SUCCESS))
      break;
    status = GUI_FUNC(Begin)(&gui, &surface);
    if (status != STATUS_CONST(SUCCESS))
      break;
    for (i = 0; i < inputCount; ++i) {
      status = GUI_FUNC(Event)(&gui, &inputEvents[i]);
      if (status != STATUS_CONST(SUCCESS))
        break;
    }
    if (status == STATUS_CONST(SUCCESS))
      status = draw(&gui);
    {
      OPSTATUS endStatus = GUI_FUNC(End)(&gui);
      if (status == STATUS_CONST(SUCCESS))
        status = endStatus;
    }
    if (status != STATUS_CONST(SUCCESS))
      break;
    if (ppm) {
      status = savePPM(&surface, ppm);
      break;
    }
    status = WINDOW_FUNC(Update)(&window, &surface);
    if (status != STATUS_CONST(SUCCESS))
      break;
    ++frames;
    if (limit && frames >= limit)
      break;
    status = TIMER_FUNC(FrameLimiterRemaining)(&limiter, &remaining);
    if (status != STATUS_CONST(SUCCESS))
      break;
    if (remaining.nanoseconds > 0) {
      status = THREAD_FUNC(SleepFor)(remaining);
      if (status != STATUS_CONST(SUCCESS))
        break;
    }
  } while (WINDOW_FUNC(IsOpen)(&window));
cleanup:
  WINDOW_FUNC(Close)(&window);
  GRAPHICS_FUNC(Surface_Free)(&surface);
  if (status != STATUS_CONST(SUCCESS))
    fprintf(stderr, "GUI status: %d\n", (int)status);
  return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
