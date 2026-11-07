#pragma once
#include "UI.space"
#include "../../../Core/Module/Package.h"

/* C11 immediate-mode UI; direct and struct-based APIs share widget logic. */

#define UI_VISUAL_DEFAULT_PROTOTYPE \
  static inline UI_TYPE(TVisual) UI_FUNC(Visual_Default)( \
      void)

#define UI_VISUAL_RGB_PROTOTYPE \
  static inline TEXT_GRID_ATTRIBUTE_TYPE(Cell) UI_FUNC(Visual_RGB)( \
      TEXT_GRID_COLOR_TYPE(RGB) foreground, \
      TEXT_GRID_COLOR_TYPE(RGB) background)

#define UI_VISUAL_PUSH_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Visual_Push)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TVisual) *visual)

#define UI_VISUAL_POP_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Visual_Pop)( \
      UI_TYPE(TContext) *context)

#define UI_THEME_DEFAULT_PROTOTYPE \
  static inline UI_TYPE(TTheme) UI_FUNC(Theme_Default)( \
      void)

#define UI_THEME_DARK_PROTOTYPE \
  static inline UI_TYPE(TTheme) UI_FUNC(Theme_Dark)( \
      void)

#define UI_THEME_LIGHT_PROTOTYPE \
  static inline UI_TYPE(TTheme) UI_FUNC(Theme_Light)( \
      void)

#define UI_THEME_CLASSIC_PROTOTYPE \
  static inline UI_TYPE(TTheme) UI_FUNC(Theme_Classic)( \
      void)

#define UI_THEME_MONOCHROME_PROTOTYPE \
  static inline UI_TYPE(TTheme) UI_FUNC(Theme_Monochrome)( \
      void)

#define UI_THEME_SET_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Theme_Set)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TTheme) *theme)

#define UI_THEME_GET_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Theme_Get)( \
      const UI_TYPE(TContext) *context, UI_TYPE(TTheme) *outTheme)

#define UI_GLYPHS_ASCII_PROTOTYPE \
  static inline UI_TYPE(TGlyphs) UI_FUNC(Glyphs_ASCII)( \
      void)

#define UI_GLYPHS_UNICODE_PROTOTYPE \
  static inline UI_TYPE(TGlyphs) UI_FUNC(Glyphs_Unicode)( \
      void)

#define UI_POLL_EVENTS_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(PollEvents)( \
      UI_TYPE(TContext) *context, size_t *outCount)

#define UI_RESIZE_TAKE_PROTOTYPE \
  static inline bool UI_FUNC(Resize_Take)( \
      UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) *outSize)

#define UI_LAYOUT_SHARE_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_Share)( \
      UI_TYPE(TContext) *context, uint16_t numerator, uint16_t denominator, TQUAD_TYPE(uint16) *outRegion)

#define UI_LAYOUT_MEASURE_TEXT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_MeasureText)( \
      const char *text, TDUAL_TYPE(uint16) padding, TDUAL_TYPE(uint16) *outSize)

#define UI_INPUT_TEXT_SELECT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(InputText_Select)( \
      UI_TYPE(TInputText) *input, size_t anchor, size_t caret)

#define UI_INPUT_TEXT_COPY_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(InputText_Copy)( \
      const UI_TYPE(TInputText) *input, char *buffer, size_t capacity, size_t *outLength)

#define UI_INPUT_TEXT_CUT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(InputText_Cut)( \
      UI_TYPE(TInputText) *input, char *buffer, size_t capacity, size_t *outLength)

#define UI_INPUT_TEXT_PASTE_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(InputText_Paste)( \
      UI_TYPE(TInputText) *input, const char *utf8Text, size_t byteLength)

#define UI_BEGIN_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Begin)( \
      UI_TYPE(TContext) *context, TERMINAL_TYPE(TCanvas) *canvas)

#define UI_END_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(End)( \
      UI_TYPE(TContext) *context)

#define UI_GET_SIZE_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(GetSize)( \
      const UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) *outSize)

#define UI_KEY_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Key)( \
      UI_TYPE(TContext) *context, TERMINAL_TYPE(TKey) key)

#define UI_POINTER_MOVE_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(PointerMove)( \
      UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) position)

#define UI_POINTER_BUTTON_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(PointerButton)( \
      UI_TYPE(TContext) *context, bool pressed)

#define UI_SCROLL_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Scroll)( \
      UI_TYPE(TContext) *context, int32_t delta)

#define UI_GET_ID_PROTOTYPE \
  static inline UI_TYPE(WidgetId) UI_FUNC(GetId)( \
      const UI_TYPE(TContext) *context, const char *label)

#define UI_PUSH_ID_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(PushId)( \
      UI_TYPE(TContext) *context, const char *scope)

#define UI_POP_ID_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(PopId)( \
      UI_TYPE(TContext) *context)

#define UI_FOCUS_SET_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Focus_Set)( \
      UI_TYPE(TContext) *context, UI_TYPE(WidgetId) widget)

#define UI_FOCUS_NEXT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Focus_Next)( \
      UI_TYPE(TContext) *context)

#define UI_FOCUS_PREVIOUS_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Focus_Previous)( \
      UI_TYPE(TContext) *context)

#define UI_IS_FOCUSED_PROTOTYPE \
  static inline bool UI_FUNC(IsFocused)( \
      const UI_TYPE(TContext) *context, UI_TYPE(WidgetId) widget)

#define UI_ROW_BEGIN_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Row_Begin)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, uint16_t gap)

#define UI_COLUMN_BEGIN_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Column_Begin)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, uint16_t gap)

#define UI_LAYOUT_NEXT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_Next)( \
      UI_TYPE(TContext) *context, uint16_t extent, \
      TQUAD_TYPE(uint16) *outRegion)

#define UI_LAYOUT_REMAINING_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_Remaining)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) *outRegion)

#define UI_LAYOUT_END_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_End)( \
      UI_TYPE(TContext) *context)

#define UI_PANEL_BEGIN_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Panel_Begin)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *title)

#define UI_PANEL_BEGIN_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Panel_BeginEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TPanel) *panel)

#define UI_PANEL_END_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Panel_End)( \
      UI_TYPE(TContext) *context)

#define UI_LABEL_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Label)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *text)

#define UI_LABEL_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(LabelEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TLabel) *label)

#define UI_SEPARATOR_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Separator)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region)

#define UI_SEPARATOR_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(SeparatorEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TSeparator) *separator)

#define UI_BUTTON_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Button)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *label, bool *outPressed)

#define UI_BUTTON_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ButtonEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TButton) *button)

#define UI_CHECKBOX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Checkbox)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *label, bool *checked)

#define UI_CHECKBOX_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(CheckboxEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TCheckbox) *checkbox)

#define UI_RADIO_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Radio)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *label, uint32_t option, uint32_t *selected)

#define UI_RADIO_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(RadioEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TRadio) *radio)

#define UI_SLIDER_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Slider)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, int32_t minimum, \
      int32_t maximum, int32_t *value)

#define UI_SLIDER_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(SliderEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TSlider) *slider)

#define UI_PROGRESS_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Progress)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, float fraction)

#define UI_PROGRESS_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ProgressEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TProgress) *progress)

#define UI_MENU_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Menu)( \
      UI_TYPE(TContext) *context, \
      TQUAD_TYPE(uint16) region, \
      const char *const *items, size_t count, \
      size_t *selected, bool *outActivated)

#define UI_MENU_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(MenuEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TMenu) *menu)

#define UI_PROGRESS_BAR_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ProgressBar)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      uint64_t value, uint64_t maximum)

#define UI_PROGRESS_BAR_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ProgressBarEx)( \
      UI_TYPE(TContext) *context, \
      const UI_TYPE(TProgressBar) *progress)

#define UI_DIALOG_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Dialog)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *title, const char *message, bool *outDismissed)

#define UI_DIALOG_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(DialogEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TDialog) *dialog)

#define UI_CONFIRM_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Confirm)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *message, bool *outAccepted, bool *outRejected)

#define UI_CONFIRM_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ConfirmEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TDialog) *dialog)

#define UI_STATUS_BAR_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(StatusBar)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *left, const char *right)

#define UI_STATUS_BAR_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(StatusBarEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TStatusBar) *bar)

#define UI_TOAST_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Toast)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *message, bool visible)

#define UI_TOAST_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ToastEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TToast) *toast)

#define UI_SPINNER_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Spinner)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *label, uint64_t phase)

#define UI_SPINNER_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(SpinnerEx)( \
      UI_TYPE(TContext) *context, const UI_TYPE(TSpinner) *spinner)

#define UI_INPUT_TEXT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(InputText)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, char *buffer, \
      size_t capacity, size_t *inoutLength)

#define UI_INPUT_TEXT_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(InputTextEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TInputText) *input)

#define UI_TEXT_AREA_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(TextArea)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, char *buffer, \
      size_t capacity, size_t *inoutLength)

#define UI_TEXT_AREA_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(TextAreaEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TTextArea) *input)

#define UI_LIST_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(List)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      const char *const *items, size_t count, size_t *selected)

#define UI_LIST_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ListEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TList) *list)

#define UI_INIT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Init)(UI_TYPE(TContext) *context)

#define UI_RENDER_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Render)( \
      const UI_TYPE(TContext) *context, TERMINAL_TYPE(TCanvas) *canvas)

#define UI_PRESENT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Present)( \
      const UI_TYPE(TContext) *context, \
      TERMINAL_SCREEN_TYPE(TTerminalScreen) *screen)

#define UI_DISPATCH_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Dispatch)( \
      UI_TYPE(TContext) *context, const TERMINAL_TYPE(TEvent) *event)

#define UI_DISPATCH_INPUT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(DispatchInput)( \
      UI_TYPE(TContext) *context, const INPUT_TYPE(TEvent) *event)

#define UI_FOCUS_CLEAR_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Focus_Clear)(UI_TYPE(TContext) *context)

#define UI_LAYOUT_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout)( \
      UI_TYPE(TContext) *context, TDUAL_TYPE(uint16) viewportSize)

#define UI_LAYOUT_SPLIT_HORIZONTAL_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_SplitHorizontal)( \
      TQUAD_TYPE(uint16) parent, uint16_t topRatio, uint16_t bottomRatio, \
      uint16_t gap, TQUAD_TYPE(uint16) *outTop, TQUAD_TYPE(uint16) *outBottom)

#define UI_LAYOUT_SPLIT_VERTICAL_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_SplitVertical)( \
      TQUAD_TYPE(uint16) parent, uint16_t leftRatio, uint16_t rightRatio, \
      uint16_t gap, TQUAD_TYPE(uint16) *outLeft, TQUAD_TYPE(uint16) *outRight)

#define UI_LAYOUT_GRID_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_Grid)( \
      TQUAD_TYPE(uint16) parent, uint16_t cols, uint16_t rows, \
      uint16_t col, uint16_t row, uint16_t gapX, uint16_t gapY, \
      TQUAD_TYPE(uint16) *outRegion)

#define UI_LAYOUT_CENTER_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Layout_Center)( \
      TQUAD_TYPE(uint16) parent, TDUAL_TYPE(uint16) size, \
      TQUAD_TYPE(uint16) *outRegion)

#define UI_SCROLLBAR_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(Scrollbar)( \
      UI_TYPE(TContext) *context, TQUAD_TYPE(uint16) region, \
      int64_t minimum, int64_t maximum, int64_t pageSize, \
      bool vertical, int64_t *value)

#define UI_SCROLLBAR_EX_PROTOTYPE \
  static inline OPSTATUS UI_FUNC(ScrollbarEx)( \
      UI_TYPE(TContext) *context, UI_TYPE(TScrollbar) *scrollbar)

UI_INIT_PROTOTYPE;
UI_VISUAL_DEFAULT_PROTOTYPE;
UI_VISUAL_RGB_PROTOTYPE;
UI_VISUAL_PUSH_PROTOTYPE;
UI_VISUAL_POP_PROTOTYPE;
UI_THEME_DEFAULT_PROTOTYPE;
UI_THEME_DARK_PROTOTYPE;
UI_THEME_LIGHT_PROTOTYPE;
UI_THEME_CLASSIC_PROTOTYPE;
UI_THEME_MONOCHROME_PROTOTYPE;
UI_THEME_SET_PROTOTYPE;
UI_THEME_GET_PROTOTYPE;
UI_GLYPHS_ASCII_PROTOTYPE;
UI_GLYPHS_UNICODE_PROTOTYPE;
UI_POLL_EVENTS_PROTOTYPE;
UI_DISPATCH_PROTOTYPE;
UI_DISPATCH_INPUT_PROTOTYPE;
UI_RESIZE_TAKE_PROTOTYPE;
UI_LAYOUT_SHARE_PROTOTYPE;
UI_LAYOUT_MEASURE_TEXT_PROTOTYPE;
UI_LAYOUT_PROTOTYPE;
UI_LAYOUT_SPLIT_HORIZONTAL_PROTOTYPE;
UI_LAYOUT_SPLIT_VERTICAL_PROTOTYPE;
UI_LAYOUT_GRID_PROTOTYPE;
UI_LAYOUT_CENTER_PROTOTYPE;
UI_INPUT_TEXT_SELECT_PROTOTYPE;
UI_INPUT_TEXT_COPY_PROTOTYPE;
UI_INPUT_TEXT_CUT_PROTOTYPE;
UI_INPUT_TEXT_PASTE_PROTOTYPE;
UI_BEGIN_PROTOTYPE;
UI_END_PROTOTYPE;
UI_GET_SIZE_PROTOTYPE;
UI_RENDER_PROTOTYPE;
UI_PRESENT_PROTOTYPE;
UI_KEY_PROTOTYPE;
UI_POINTER_MOVE_PROTOTYPE;
UI_POINTER_BUTTON_PROTOTYPE;
UI_SCROLL_PROTOTYPE;
UI_GET_ID_PROTOTYPE;
UI_PUSH_ID_PROTOTYPE;
UI_POP_ID_PROTOTYPE;
UI_FOCUS_SET_PROTOTYPE;
UI_FOCUS_NEXT_PROTOTYPE;
UI_FOCUS_PREVIOUS_PROTOTYPE;
UI_FOCUS_CLEAR_PROTOTYPE;
UI_IS_FOCUSED_PROTOTYPE;
UI_ROW_BEGIN_PROTOTYPE;
UI_COLUMN_BEGIN_PROTOTYPE;
UI_LAYOUT_NEXT_PROTOTYPE;
UI_LAYOUT_REMAINING_PROTOTYPE;
UI_LAYOUT_END_PROTOTYPE;
UI_PANEL_BEGIN_PROTOTYPE;
UI_PANEL_BEGIN_EX_PROTOTYPE;
UI_PANEL_END_PROTOTYPE;
UI_LABEL_PROTOTYPE;
UI_LABEL_EX_PROTOTYPE;
UI_SEPARATOR_PROTOTYPE;
UI_SEPARATOR_EX_PROTOTYPE;
UI_BUTTON_PROTOTYPE;
UI_BUTTON_EX_PROTOTYPE;
UI_CHECKBOX_PROTOTYPE;
UI_CHECKBOX_EX_PROTOTYPE;
UI_RADIO_PROTOTYPE;
UI_RADIO_EX_PROTOTYPE;
UI_SLIDER_PROTOTYPE;
UI_SLIDER_EX_PROTOTYPE;
UI_SCROLLBAR_PROTOTYPE;
UI_SCROLLBAR_EX_PROTOTYPE;
UI_PROGRESS_PROTOTYPE;
UI_PROGRESS_EX_PROTOTYPE;
UI_MENU_PROTOTYPE;
UI_MENU_EX_PROTOTYPE;
UI_PROGRESS_BAR_PROTOTYPE;
UI_PROGRESS_BAR_EX_PROTOTYPE;
UI_DIALOG_PROTOTYPE;
UI_DIALOG_EX_PROTOTYPE;
UI_CONFIRM_PROTOTYPE;
UI_CONFIRM_EX_PROTOTYPE;
UI_STATUS_BAR_PROTOTYPE;
UI_STATUS_BAR_EX_PROTOTYPE;
UI_TOAST_PROTOTYPE;
UI_TOAST_EX_PROTOTYPE;
UI_SPINNER_PROTOTYPE;
UI_SPINNER_EX_PROTOTYPE;
UI_INPUT_TEXT_PROTOTYPE;
UI_INPUT_TEXT_EX_PROTOTYPE;
UI_TEXT_AREA_PROTOTYPE;
UI_TEXT_AREA_EX_PROTOTYPE;
UI_LIST_PROTOTYPE;
UI_LIST_EX_PROTOTYPE;

#if defined(UI_DISABLE_FUNCTION_TABLE)
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE
#endif
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

PACKAGE(LIB_PREFIX(UI_MOD),
    FUNCTION(OPSTATUS, Init, (UI_TYPE(TContext) * context)),
    FUNCTION(UI_TYPE(TVisual), Visual_Default, ()),
    FUNCTION(TEXT_GRID_ATTRIBUTE_TYPE(Cell), Visual_RGB,
             (TEXT_GRID_COLOR_TYPE(RGB) foreground,
              TEXT_GRID_COLOR_TYPE(RGB) background)),
    FUNCTION(OPSTATUS, Visual_Push,
             (UI_TYPE(TContext) * context, const UI_TYPE(TVisual) * visual)),
    FUNCTION(OPSTATUS, Visual_Pop, (UI_TYPE(TContext) * context)),
    FUNCTION(UI_TYPE(TTheme), Theme_Default, ()),
    FUNCTION(UI_TYPE(TTheme), Theme_Dark, ()),
    FUNCTION(UI_TYPE(TTheme), Theme_Light, ()),
    FUNCTION(UI_TYPE(TTheme), Theme_Classic, ()),
    FUNCTION(UI_TYPE(TTheme), Theme_Monochrome, ()),
    FUNCTION(OPSTATUS, Theme_Set,
             (UI_TYPE(TContext) * context, const UI_TYPE(TTheme) * theme)),
    FUNCTION(OPSTATUS, Theme_Get,
             (const UI_TYPE(TContext) * context, UI_TYPE(TTheme) * outTheme)),
    FUNCTION(UI_TYPE(TGlyphs), Glyphs_ASCII, ()),
    FUNCTION(UI_TYPE(TGlyphs), Glyphs_Unicode, ()),
    FUNCTION(OPSTATUS, Begin,
             (UI_TYPE(TContext) * context, TERMINAL_TYPE(TCanvas) * canvas)),
    FUNCTION(OPSTATUS, End, (UI_TYPE(TContext) * context)),
    FUNCTION(OPSTATUS, GetSize,
             (const UI_TYPE(TContext) * context, TDUAL_TYPE(uint16) * outSize)),
    FUNCTION(OPSTATUS, Render,
             (const UI_TYPE(TContext) * context, TERMINAL_TYPE(TCanvas) * canvas)),
    FUNCTION(OPSTATUS, Present,
             (const UI_TYPE(TContext) * context,
              TERMINAL_SCREEN_TYPE(TTerminalScreen) * screen)),
    FUNCTION(OPSTATUS, PollEvents,
             (UI_TYPE(TContext) * context, size_t * outCount)),
    FUNCTION(OPSTATUS, Dispatch,
             (UI_TYPE(TContext) * context, const TERMINAL_TYPE(TEvent) * event)),
    FUNCTION(OPSTATUS, DispatchInput,
             (UI_TYPE(TContext) * context, const INPUT_TYPE(TEvent) * event)),
    FUNCTION(bool, Resize_Take,
             (UI_TYPE(TContext) * context, TDUAL_TYPE(uint16) * outSize)),
    FUNCTION(OPSTATUS, Key,
             (UI_TYPE(TContext) * context, TERMINAL_TYPE(TKey) key)),
    FUNCTION(OPSTATUS, PointerMove,
             (UI_TYPE(TContext) * context, TDUAL_TYPE(uint16) position)),
    FUNCTION(OPSTATUS, PointerButton,
             (UI_TYPE(TContext) * context, bool pressed)),
    FUNCTION(OPSTATUS, Scroll, (UI_TYPE(TContext) * context, int32_t delta)),
    FUNCTION(UI_TYPE(WidgetId), GetId,
             (const UI_TYPE(TContext) * context, const char *label)),
    FUNCTION(OPSTATUS, PushId,
             (UI_TYPE(TContext) * context, const char *scope)),
    FUNCTION(OPSTATUS, PopId, (UI_TYPE(TContext) * context)),
    FUNCTION(OPSTATUS, Focus_Set,
             (UI_TYPE(TContext) * context, UI_TYPE(WidgetId) widget)),
    FUNCTION(OPSTATUS, Focus_Next, (UI_TYPE(TContext) * context)),
    FUNCTION(OPSTATUS, Focus_Previous, (UI_TYPE(TContext) * context)),
    FUNCTION(OPSTATUS, Focus_Clear, (UI_TYPE(TContext) * context)),
    FUNCTION(bool, IsFocused,
             (const UI_TYPE(TContext) * context, UI_TYPE(WidgetId) widget)),
    FUNCTION(OPSTATUS, Layout,
             (UI_TYPE(TContext) * context, TDUAL_TYPE(uint16) viewportSize)),
    FUNCTION(OPSTATUS, Layout_Share,
             (UI_TYPE(TContext) * context, uint16_t numerator,
              uint16_t denominator, TQUAD_TYPE(uint16) * outRegion)),
    FUNCTION(OPSTATUS, Layout_MeasureText,
             (const char *text, TDUAL_TYPE(uint16) padding,
              TDUAL_TYPE(uint16) * outSize)),
    FUNCTION(OPSTATUS, Layout_SplitHorizontal,
             (TQUAD_TYPE(uint16) parent, uint16_t topRatio,
              uint16_t bottomRatio, uint16_t gap, TQUAD_TYPE(uint16) * outTop,
              TQUAD_TYPE(uint16) * outBottom)),
    FUNCTION(OPSTATUS, Layout_SplitVertical,
             (TQUAD_TYPE(uint16) parent, uint16_t leftRatio,
              uint16_t rightRatio, uint16_t gap, TQUAD_TYPE(uint16) * outLeft,
              TQUAD_TYPE(uint16) * outRight)),
    FUNCTION(OPSTATUS, Layout_Grid,
             (TQUAD_TYPE(uint16) parent, uint16_t cols, uint16_t rows,
              uint16_t col, uint16_t row, uint16_t gapX, uint16_t gapY,
              TQUAD_TYPE(uint16) * outRegion)),
    FUNCTION(OPSTATUS, Layout_Center,
             (TQUAD_TYPE(uint16) parent, TDUAL_TYPE(uint16) size,
              TQUAD_TYPE(uint16) * outRegion)),
    FUNCTION(OPSTATUS, Row_Begin,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              uint16_t gap)),
    FUNCTION(OPSTATUS, Column_Begin,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              uint16_t gap)),
    FUNCTION(OPSTATUS, Layout_Next,
             (UI_TYPE(TContext) * context, uint16_t extent,
              TQUAD_TYPE(uint16) * outRegion)),
    FUNCTION(OPSTATUS, Layout_Remaining,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) * outRegion)),
    FUNCTION(OPSTATUS, Layout_End, (UI_TYPE(TContext) * context)),
    FUNCTION(OPSTATUS, Panel_Begin,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *title)),
    FUNCTION(OPSTATUS, Panel_BeginEx,
             (UI_TYPE(TContext) * context, const UI_TYPE(TPanel) * panel)),
    FUNCTION(OPSTATUS, Panel_End, (UI_TYPE(TContext) * context)),
    FUNCTION(OPSTATUS, Label,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *text)),
    FUNCTION(OPSTATUS, LabelEx,
             (UI_TYPE(TContext) * context, const UI_TYPE(TLabel) * label)),
    FUNCTION(OPSTATUS, Separator,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region)),
    FUNCTION(OPSTATUS, SeparatorEx,
             (UI_TYPE(TContext) * context,
              const UI_TYPE(TSeparator) * separator)),
    FUNCTION(OPSTATUS, Button,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *label, bool *outPressed)),
    FUNCTION(OPSTATUS, ButtonEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TButton) * button)),
    FUNCTION(OPSTATUS, Checkbox,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *label, bool *checked)),
    FUNCTION(OPSTATUS, CheckboxEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TCheckbox) * checkbox)),
    FUNCTION(OPSTATUS, Radio,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *label, uint32_t option, uint32_t * selected)),
    FUNCTION(OPSTATUS, RadioEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TRadio) * radio)),
    FUNCTION(OPSTATUS, Slider,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              int32_t minimum, int32_t maximum, int32_t * value)),
    FUNCTION(OPSTATUS, SliderEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TSlider) * slider)),
    FUNCTION(OPSTATUS, Scrollbar,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              int64_t minimum, int64_t maximum, int64_t pageSize,
              bool vertical, int64_t * value)),
    FUNCTION(OPSTATUS, ScrollbarEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TScrollbar) * scrollbar)),
    FUNCTION(OPSTATUS, Progress,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              float fraction)),
    FUNCTION(OPSTATUS, ProgressEx,
             (UI_TYPE(TContext) * context,
              const UI_TYPE(TProgress) * progress)),
    FUNCTION(OPSTATUS, ProgressBar,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              uint64_t value, uint64_t maximum)),
    FUNCTION(OPSTATUS, ProgressBarEx,
             (UI_TYPE(TContext) * context,
              const UI_TYPE(TProgressBar) * progress)),
    FUNCTION(OPSTATUS, Menu,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *const *items, size_t count, size_t * selected,
              bool *outActivated)),
    FUNCTION(OPSTATUS, MenuEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TMenu) * menu)),
    FUNCTION(OPSTATUS, Dialog,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *title, const char *message, bool *outDismissed)),
    FUNCTION(OPSTATUS, DialogEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TDialog) * dialog)),
    FUNCTION(OPSTATUS, Confirm,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *message, bool *outAccepted, bool *outRejected)),
    FUNCTION(OPSTATUS, ConfirmEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TDialog) * dialog)),
    FUNCTION(OPSTATUS, StatusBar,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *left, const char *right)),
    FUNCTION(OPSTATUS, StatusBarEx,
             (UI_TYPE(TContext) * context, const UI_TYPE(TStatusBar) * bar)),
    FUNCTION(OPSTATUS, Toast,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *message, bool visible)),
    FUNCTION(OPSTATUS, ToastEx,
             (UI_TYPE(TContext) * context, const UI_TYPE(TToast) * toast)),
    FUNCTION(OPSTATUS, Spinner,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *label, uint64_t phase)),
    FUNCTION(OPSTATUS, SpinnerEx,
             (UI_TYPE(TContext) * context, const UI_TYPE(TSpinner) * spinner)),
    FUNCTION(OPSTATUS, InputText,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              char *buffer, size_t capacity, size_t * inoutLength)),
    FUNCTION(OPSTATUS, InputTextEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TInputText) * input)),
    FUNCTION(OPSTATUS, InputText_Select,
             (UI_TYPE(TInputText) * input, size_t anchor, size_t caret)),
    FUNCTION(OPSTATUS, InputText_Copy,
             (const UI_TYPE(TInputText) * input, char *buffer,
              size_t capacity, size_t * outLength)),
    FUNCTION(OPSTATUS, InputText_Cut,
             (UI_TYPE(TInputText) * input, char *buffer, size_t capacity,
              size_t * outLength)),
    FUNCTION(OPSTATUS, InputText_Paste,
             (UI_TYPE(TInputText) * input, const char *utf8Text,
              size_t byteLength)),
    FUNCTION(OPSTATUS, TextArea,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              char *buffer, size_t capacity, size_t * inoutLength)),
    FUNCTION(OPSTATUS, TextAreaEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TTextArea) * input)),
    FUNCTION(OPSTATUS, List,
             (UI_TYPE(TContext) * context, TQUAD_TYPE(uint16) region,
              const char *const *items, size_t count, size_t * selected)),
    FUNCTION(OPSTATUS, ListEx,
             (UI_TYPE(TContext) * context, UI_TYPE(TList) * list))
);

#include "Impl/UI.impl"
#include "Component/Component.h"
