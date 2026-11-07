#pragma once
#include "GUI.space"

#define GUI_FONT_ASF_PROTOTYPE                                                                     \
  static inline GUI_TYPE(TFont) GUI_FUNC(Font_ASF)(const FONT_TYPE(TFont) * font)

#define GUI_THEME_DARK_PROTOTYPE static inline GUI_TYPE(TTheme) GUI_FUNC(Theme_Dark)(void)

#define GUI_THEME_LIGHT_PROTOTYPE static inline GUI_TYPE(TTheme) GUI_FUNC(Theme_Light)(void)

#define GUI_THEME_CLASSIC_PROTOTYPE static inline GUI_TYPE(TTheme) GUI_FUNC(Theme_Classic)(void)

#define GUI_THEME_MONOCHROME_PROTOTYPE                                                             \
  static inline GUI_TYPE(TTheme) GUI_FUNC(Theme_Monochrome)(void)

#define GUI_THEME_DEFAULT_PROTOTYPE static inline GUI_TYPE(TTheme) GUI_FUNC(Theme_Default)(void)

#define GUI_THEME_SET_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Theme_Set)(GUI_TYPE(TContext) * context,                         \
                                             const GUI_TYPE(TTheme) * theme)

#define GUI_THEME_GET_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Theme_Get)(const GUI_TYPE(TContext) * context,                   \
                                             GUI_TYPE(TTheme) * outTheme)

#define GUI_SCALE_SET_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Scale_Set)(GUI_TYPE(TContext) * context, float scale)

#define GUI_VISUAL_PUSH_PROTOTYPE                                                                  \
  static inline OPSTATUS GUI_FUNC(Visual_Push)(GUI_TYPE(TContext) * context,                       \
                                               const GUI_TYPE(TVisual) * visual)

#define GUI_VISUAL_POP_PROTOTYPE                                                                   \
  static inline OPSTATUS GUI_FUNC(Visual_Pop)(GUI_TYPE(TContext) * context)

#define GUI_BEGIN_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Begin)(GUI_TYPE(TContext) * context,                             \
                                         GRAPHICS_TYPE(TSurface) * surface)

#define GUI_END_PROTOTYPE static inline OPSTATUS GUI_FUNC(End)(GUI_TYPE(TContext) * context)

#define GUI_EVENT_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Event)(GUI_TYPE(TContext) * context,                             \
                                         const INPUT_TYPE(TEvent) * event)

#define GUI_GET_SIZE_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(GetSize)(const GUI_TYPE(TContext) * context,                     \
                                           TDUAL_TYPE(float) * outSize)

#define GUI_GET_ID_PROTOTYPE                                                                       \
  static inline GUI_TYPE(WidgetId)                                                                 \
      GUI_FUNC(GetId)(const GUI_TYPE(TContext) * context, const char *label)

#define GUI_PUSH_ID_PROTOTYPE                                                                      \
  static inline OPSTATUS GUI_FUNC(PushId)(GUI_TYPE(TContext) * context, const char *scope)

#define GUI_POP_ID_PROTOTYPE static inline OPSTATUS GUI_FUNC(PopId)(GUI_TYPE(TContext) * context)

#define GUI_FOCUS_SET_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Focus_Set)(GUI_TYPE(TContext) * context,                         \
                                             GUI_TYPE(WidgetId) widget)

#define GUI_FOCUS_NEXT_PROTOTYPE                                                                   \
  static inline OPSTATUS GUI_FUNC(Focus_Next)(GUI_TYPE(TContext) * context)

#define GUI_FOCUS_PREVIOUS_PROTOTYPE                                                               \
  static inline OPSTATUS GUI_FUNC(Focus_Previous)(GUI_TYPE(TContext) * context)

#define GUI_IS_FOCUSED_PROTOTYPE                                                                   \
  static inline bool GUI_FUNC(IsFocused)(const GUI_TYPE(TContext) * context,                       \
                                         GUI_TYPE(WidgetId) widget)

#define GUI_CLIP_PUSH_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Clip_Push)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region)

#define GUI_CLIP_POP_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(Clip_Pop)(GUI_TYPE(TContext) * context)

#define GUI_ROW_BEGIN_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Row_Begin)(GUI_TYPE(TContext) * context,                         \
                                             TQUAD_TYPE(float) region, float gap)

#define GUI_COLUMN_BEGIN_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(Column_Begin)(GUI_TYPE(TContext) * context,                      \
                                                TQUAD_TYPE(float) region, float gap)

#define GUI_LAYOUT_NEXT_PROTOTYPE                                                                  \
  static inline OPSTATUS GUI_FUNC(Layout_Next)(GUI_TYPE(TContext) * context, float extent,         \
                                               TQUAD_TYPE(float) * outRegion)

#define GUI_LAYOUT_END_PROTOTYPE                                                                   \
  static inline OPSTATUS GUI_FUNC(Layout_End)(GUI_TYPE(TContext) * context)

#define GUI_LAYOUT_MEASURE_TEXT_PROTOTYPE                                                          \
  static inline OPSTATUS GUI_FUNC(Layout_MeasureText)(                                             \
      const GUI_TYPE(TFont) * font, const char *text, float scale, TDUAL_TYPE(float) * outSize)

#define GUI_DRAW_RECTANGLE_PROTOTYPE                                                               \
  static inline OPSTATUS GUI_FUNC(DrawRectangle)(                                                  \
      GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint32_t color, float radius)

#define GUI_DRAW_TEXT_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(DrawText)(                                                       \
      GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *text, uint32_t color,    \
      const GUI_TYPE(TFont) * font, float scale)

#define GUI_IMAGE_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Image)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,   \
                                         const GRAPHICS_TYPE(TSurface) * surface)

#define GUI_IMAGE_EX_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(ImageEx)(GUI_TYPE(TContext) * context,                           \
                                           const GUI_TYPE(TImage) * image)

#define GUI_LABEL_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Label)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,   \
                                         const char *text)

#define GUI_LABEL_EX_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(LabelEx)(GUI_TYPE(TContext) * context,                           \
                                           const GUI_TYPE(TLabel) * label)

#define GUI_SEPARATOR_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Separator)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region)

#define GUI_BUTTON_PROTOTYPE                                                                       \
  static inline OPSTATUS GUI_FUNC(Button)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,  \
                                          const char *label, bool *outPressed)

#define GUI_BUTTON_EX_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(ButtonEx)(GUI_TYPE(TContext) * context,                          \
                                            GUI_TYPE(TButton) * button)

#define GUI_CHECKBOX_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(Checkbox)(                                                       \
      GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *checked)

#define GUI_CHECKBOX_EX_PROTOTYPE                                                                  \
  static inline OPSTATUS GUI_FUNC(CheckboxEx)(GUI_TYPE(TContext) * context,                        \
                                              GUI_TYPE(TCheckbox) * checkbox)

#define GUI_RADIO_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Radio)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,   \
                                         const char *label, uint32_t option, uint32_t *selected)

#define GUI_RADIO_EX_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(RadioEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TRadio) * radio)

#define GUI_SLIDER_PROTOTYPE                                                                       \
  static inline OPSTATUS GUI_FUNC(Slider)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,  \
                                          float minimum, float maximum, float *value)

#define GUI_SLIDER_EX_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(SliderEx)(GUI_TYPE(TContext) * context,                          \
                                            GUI_TYPE(TSlider) * slider)

#define GUI_PROGRESS_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(Progress)(GUI_TYPE(TContext) * context,                          \
                                            TQUAD_TYPE(float) region, float fraction)

#define GUI_PROGRESS_BAR_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(ProgressBar)(                                                    \
      GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint64_t value, uint64_t maximum)

#define GUI_PROGRESS_BAR_EX_PROTOTYPE                                                              \
  static inline OPSTATUS GUI_FUNC(ProgressBarEx)(GUI_TYPE(TContext) * context,                     \
                                                 const GUI_TYPE(TProgressBar) * progress)

#define GUI_INPUT_TEXT_PROTOTYPE                                                                   \
  static inline OPSTATUS GUI_FUNC(InputText)(GUI_TYPE(TContext) * context,                         \
                                             TQUAD_TYPE(float) region, char *buffer,               \
                                             size_t capacity, size_t *inoutLength)

#define GUI_INPUT_TEXT_EX_PROTOTYPE                                                                \
  static inline OPSTATUS GUI_FUNC(InputTextEx)(GUI_TYPE(TContext) * context,                       \
                                               GUI_TYPE(TInputText) * input)

#define GUI_TEXT_AREA_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(TextArea)(GUI_TYPE(TContext) * context,                          \
                                            TQUAD_TYPE(float) region, char *buffer,                \
                                            size_t capacity, size_t *inoutLength)

#define GUI_TEXT_AREA_EX_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(TextAreaEx)(GUI_TYPE(TContext) * context,                        \
                                              GUI_TYPE(TTextArea) * input)

#define GUI_INPUT_TEXT_SELECT_PROTOTYPE                                                            \
  static inline OPSTATUS GUI_FUNC(InputText_Select)(GUI_TYPE(TInputText) * input, size_t anchor,   \
                                                    size_t caret)

#define GUI_INPUT_TEXT_COPY_PROTOTYPE                                                              \
  static inline OPSTATUS GUI_FUNC(InputText_Copy)(                                                 \
      const GUI_TYPE(TInputText) * input, char *buffer, size_t capacity, size_t *outLength)

#define GUI_INPUT_TEXT_CUT_PROTOTYPE                                                               \
  static inline OPSTATUS GUI_FUNC(InputText_Cut)(GUI_TYPE(TInputText) * input, char *buffer,       \
                                                 size_t capacity, size_t *outLength)

#define GUI_INPUT_TEXT_PASTE_PROTOTYPE                                                             \
  static inline OPSTATUS GUI_FUNC(InputText_Paste)(GUI_TYPE(TInputText) * input,                   \
                                                   const char *utf8Text, size_t byteLength)

#define GUI_LIST_PROTOTYPE                                                                         \
  static inline OPSTATUS GUI_FUNC(List)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,    \
                                        const char *const *items, size_t count, size_t *selected)

#define GUI_LIST_EX_PROTOTYPE                                                                      \
  static inline OPSTATUS GUI_FUNC(ListEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TList) * list)

#define GUI_MENU_PROTOTYPE                                                                         \
  static inline OPSTATUS GUI_FUNC(Menu)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,    \
                                        const char *const *items, size_t count, size_t *selected,  \
                                        bool *outActivated)

#define GUI_MENU_EX_PROTOTYPE                                                                      \
  static inline OPSTATUS GUI_FUNC(MenuEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TMenu) * menu)

#define GUI_PANEL_BEGIN_PROTOTYPE                                                                  \
  static inline OPSTATUS GUI_FUNC(Panel_Begin)(GUI_TYPE(TContext) * context,                       \
                                               TQUAD_TYPE(float) region, const char *title)

#define GUI_PANEL_BEGIN_EX_PROTOTYPE                                                               \
  static inline OPSTATUS GUI_FUNC(Panel_BeginEx)(GUI_TYPE(TContext) * context,                     \
                                                 const GUI_TYPE(TPanel) * panel)

#define GUI_PANEL_END_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Panel_End)(GUI_TYPE(TContext) * context)

#define GUI_MODAL_BEGIN_PROTOTYPE                                                                  \
  static inline OPSTATUS GUI_FUNC(Modal_Begin)(GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) id)

#define GUI_MODAL_END_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Modal_End)(GUI_TYPE(TContext) * context)

#define GUI_CONFIRM_PROTOTYPE                                                                      \
  static inline OPSTATUS GUI_FUNC(Confirm)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, \
                                           const char *message, bool *outAccepted,                 \
                                           bool *outRejected)

#define GUI_CONFIRM_EX_PROTOTYPE                                                                   \
  static inline OPSTATUS GUI_FUNC(ConfirmEx)(GUI_TYPE(TContext) * context,                         \
                                             GUI_TYPE(TDialog) * dialog)

#define GUI_DIALOG_PROTOTYPE                                                                       \
  static inline OPSTATUS GUI_FUNC(Dialog)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,  \
                                          const char *title, const char *message,                  \
                                          bool *outDismissed)

#define GUI_DIALOG_EX_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(DialogEx)(GUI_TYPE(TContext) * context,                          \
                                            GUI_TYPE(TDialog) * dialog)

#define GUI_STATUS_BAR_PROTOTYPE                                                                   \
  static inline OPSTATUS GUI_FUNC(StatusBar)(                                                      \
      GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *left, const char *right)

#define GUI_TOAST_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Toast)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,   \
                                         const char *message, bool visible)

#define GUI_SPINNER_PROTOTYPE                                                                      \
  static inline OPSTATUS GUI_FUNC(Spinner)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, \
                                           const char *label, uint64_t phase)

#define GUI_RENDER_SOFTWARE_PROTOTYPE                                                              \
  static inline OPSTATUS GUI_FUNC(RenderSoftware)(GRAPHICS_TYPE(TSurface) * surface,               \
                                                  const GUI_TYPE(TCommand) * commands,             \
                                                  size_t count, float scale)

#define GUI_IMAGE_BUTTON_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(ImageButton)(                                                    \
      GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,                                      \
      const GRAPHICS_TYPE(TSurface) * surface, bool *outPressed)

#define GUI_IMAGE_BUTTON_EX_PROTOTYPE                                                              \
  static inline OPSTATUS GUI_FUNC(ImageButtonEx)(GUI_TYPE(TContext) * context,                     \
                                                 GUI_TYPE(TImageButton) * button)

#define GUI_COMBO_BOX_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(ComboBox)(GUI_TYPE(TContext) * context,                          \
                                            TQUAD_TYPE(float) region, const char *const *items,    \
                                            size_t count, size_t *selected, bool *open)

#define GUI_COMBO_BOX_EX_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(ComboBoxEx)(GUI_TYPE(TContext) * context,                        \
                                              GUI_TYPE(TComboBox) * combo)

#define GUI_TABS_PROTOTYPE                                                                         \
  static inline OPSTATUS GUI_FUNC(Tabs)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,    \
                                        const char *const *items, size_t count, size_t *selected)

#define GUI_TABS_EX_PROTOTYPE                                                                      \
  static inline OPSTATUS GUI_FUNC(TabsEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTabs) * tabs)

#define GUI_TREE_NODE_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(TreeNode)(GUI_TYPE(TContext) * context,                          \
                                            TQUAD_TYPE(float) region, const char *label,           \
                                            bool *expanded, bool *selected)

#define GUI_TREE_NODE_EX_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(TreeNodeEx)(GUI_TYPE(TContext) * context,                        \
                                              GUI_TYPE(TTreeNode) * node)

#define GUI_TABLE_PROTOTYPE                                                                        \
  static inline OPSTATUS GUI_FUNC(Table)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,   \
                                         const char *const *cells, size_t rows, size_t columns,    \
                                         size_t *selectedRow)

#define GUI_TABLE_EX_PROTOTYPE                                                                     \
  static inline OPSTATUS GUI_FUNC(TableEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTable) * table)

#define GUI_COLOR_PICKER_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(ColorPicker)(GUI_TYPE(TContext) * context,                       \
                                               TQUAD_TYPE(float) region, uint32_t * color)

#define GUI_COLOR_PICKER_EX_PROTOTYPE                                                              \
  static inline OPSTATUS GUI_FUNC(ColorPickerEx)(GUI_TYPE(TContext) * context,                     \
                                                 GUI_TYPE(TColorPicker) * picker)

#define GUI_NUMBER_PROTOTYPE                                                                       \
  static inline OPSTATUS GUI_FUNC(Number)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,  \
                                          float minimum, float maximum, float step, float *value)

#define GUI_NUMBER_EX_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(NumberEx)(GUI_TYPE(TContext) * context,                          \
                                            GUI_TYPE(TNumber) * number)

#define GUI_SCROLLBAR_PROTOTYPE                                                                    \
  static inline OPSTATUS GUI_FUNC(Scrollbar)(GUI_TYPE(TContext) * context,                         \
                                             TQUAD_TYPE(float) region, float maximum,              \
                                             float visible, float *offset, bool vertical)

#define GUI_SCROLLBAR_EX_PROTOTYPE                                                                 \
  static inline OPSTATUS GUI_FUNC(ScrollbarEx)(GUI_TYPE(TContext) * context,                       \
                                               GUI_TYPE(TScrollbar) * scrollbar)

GUI_FONT_ASF_PROTOTYPE;
GUI_THEME_DARK_PROTOTYPE;
GUI_THEME_LIGHT_PROTOTYPE;
GUI_THEME_CLASSIC_PROTOTYPE;
GUI_THEME_MONOCHROME_PROTOTYPE;
GUI_THEME_DEFAULT_PROTOTYPE;
GUI_THEME_SET_PROTOTYPE;
GUI_THEME_GET_PROTOTYPE;
GUI_SCALE_SET_PROTOTYPE;
GUI_VISUAL_PUSH_PROTOTYPE;
GUI_VISUAL_POP_PROTOTYPE;
GUI_BEGIN_PROTOTYPE;
GUI_END_PROTOTYPE;
GUI_EVENT_PROTOTYPE;
GUI_GET_SIZE_PROTOTYPE;
GUI_GET_ID_PROTOTYPE;
GUI_PUSH_ID_PROTOTYPE;
GUI_POP_ID_PROTOTYPE;
GUI_FOCUS_SET_PROTOTYPE;
GUI_FOCUS_NEXT_PROTOTYPE;
GUI_FOCUS_PREVIOUS_PROTOTYPE;
GUI_IS_FOCUSED_PROTOTYPE;
GUI_CLIP_PUSH_PROTOTYPE;
GUI_CLIP_POP_PROTOTYPE;
GUI_ROW_BEGIN_PROTOTYPE;
GUI_COLUMN_BEGIN_PROTOTYPE;
GUI_LAYOUT_NEXT_PROTOTYPE;
GUI_LAYOUT_END_PROTOTYPE;
GUI_LAYOUT_MEASURE_TEXT_PROTOTYPE;
GUI_DRAW_RECTANGLE_PROTOTYPE;
GUI_DRAW_TEXT_PROTOTYPE;
GUI_IMAGE_PROTOTYPE;
GUI_IMAGE_EX_PROTOTYPE;
GUI_LABEL_PROTOTYPE;
GUI_LABEL_EX_PROTOTYPE;
GUI_SEPARATOR_PROTOTYPE;
GUI_BUTTON_PROTOTYPE;
GUI_BUTTON_EX_PROTOTYPE;
GUI_CHECKBOX_PROTOTYPE;
GUI_CHECKBOX_EX_PROTOTYPE;
GUI_RADIO_PROTOTYPE;
GUI_RADIO_EX_PROTOTYPE;
GUI_SLIDER_PROTOTYPE;
GUI_SLIDER_EX_PROTOTYPE;
GUI_PROGRESS_PROTOTYPE;
GUI_PROGRESS_BAR_PROTOTYPE;
GUI_PROGRESS_BAR_EX_PROTOTYPE;
GUI_INPUT_TEXT_PROTOTYPE;
GUI_INPUT_TEXT_EX_PROTOTYPE;
GUI_TEXT_AREA_PROTOTYPE;
GUI_TEXT_AREA_EX_PROTOTYPE;
GUI_INPUT_TEXT_SELECT_PROTOTYPE;
GUI_INPUT_TEXT_COPY_PROTOTYPE;
GUI_INPUT_TEXT_CUT_PROTOTYPE;
GUI_INPUT_TEXT_PASTE_PROTOTYPE;
GUI_LIST_PROTOTYPE;
GUI_LIST_EX_PROTOTYPE;
GUI_MENU_PROTOTYPE;
GUI_MENU_EX_PROTOTYPE;
GUI_PANEL_BEGIN_PROTOTYPE;
GUI_PANEL_BEGIN_EX_PROTOTYPE;
GUI_PANEL_END_PROTOTYPE;
GUI_MODAL_BEGIN_PROTOTYPE;
GUI_MODAL_END_PROTOTYPE;
GUI_CONFIRM_PROTOTYPE;
GUI_CONFIRM_EX_PROTOTYPE;
GUI_DIALOG_PROTOTYPE;
GUI_DIALOG_EX_PROTOTYPE;
GUI_STATUS_BAR_PROTOTYPE;
GUI_TOAST_PROTOTYPE;
GUI_SPINNER_PROTOTYPE;
GUI_RENDER_SOFTWARE_PROTOTYPE;
GUI_IMAGE_BUTTON_PROTOTYPE;
GUI_IMAGE_BUTTON_EX_PROTOTYPE;
GUI_COMBO_BOX_PROTOTYPE;
GUI_COMBO_BOX_EX_PROTOTYPE;
GUI_TABS_PROTOTYPE;
GUI_TABS_EX_PROTOTYPE;
GUI_TREE_NODE_PROTOTYPE;
GUI_TREE_NODE_EX_PROTOTYPE;
GUI_TABLE_PROTOTYPE;
GUI_TABLE_EX_PROTOTYPE;
GUI_COLOR_PICKER_PROTOTYPE;
GUI_COLOR_PICKER_EX_PROTOTYPE;
GUI_NUMBER_PROTOTYPE;
GUI_NUMBER_EX_PROTOTYPE;
GUI_SCROLLBAR_PROTOTYPE;
GUI_SCROLLBAR_EX_PROTOTYPE;

#include "Impl/GUI.impl"

#include "FunctionTable/GUI.inc"
