#pragma once
#ifndef GUI_DISABLE_FUNCTION_TABLE
#define GUI_FUNCTION_TABLE_STRUCT                                                                  \
  typedef struct GUI_TYPE(TFunctionTable) {                                                        \
    GUI_TYPE(TFont) (*fontASF)(const FONT_TYPE(TFont) * font);                                     \
    GUI_TYPE(TTheme) (*themeDark)(void);                                                           \
    GUI_TYPE(TTheme) (*themeLight)(void);                                                          \
    GUI_TYPE(TTheme) (*themeClassic)(void);                                                        \
    GUI_TYPE(TTheme) (*themeMonochrome)(void);                                                     \
    GUI_TYPE(TTheme) (*themeDefault)(void);                                                        \
    OPSTATUS (*themeSet)(GUI_TYPE(TContext) * context, const GUI_TYPE(TTheme) * theme);            \
    OPSTATUS (*themeGet)(const GUI_TYPE(TContext) * context, GUI_TYPE(TTheme) * outTheme);         \
    OPSTATUS (*scaleSet)(GUI_TYPE(TContext) * context, float scale);                               \
    OPSTATUS (*visualPush)(GUI_TYPE(TContext) * context, const GUI_TYPE(TVisual) * visual);        \
    OPSTATUS (*visualPop)(GUI_TYPE(TContext) * context);                                           \
    OPSTATUS (*begin)(GUI_TYPE(TContext) * context, GRAPHICS_TYPE(TSurface) * surface);            \
    OPSTATUS (*end)(GUI_TYPE(TContext) * context);                                                 \
    OPSTATUS (*event)(GUI_TYPE(TContext) * context, const INPUT_TYPE(TEvent) * event);             \
    OPSTATUS (*getSize)(const GUI_TYPE(TContext) * context, TDUAL_TYPE(float) * outSize);          \
    GUI_TYPE(WidgetId) (*getId)(const GUI_TYPE(TContext) * context, const char *label);            \
    OPSTATUS (*pushId)(GUI_TYPE(TContext) * context, const char *scope);                           \
    OPSTATUS (*popId)(GUI_TYPE(TContext) * context);                                               \
    OPSTATUS (*focusSet)(GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) widget);                 \
    OPSTATUS (*focusNext)(GUI_TYPE(TContext) * context);                                           \
    OPSTATUS (*focusPrevious)(GUI_TYPE(TContext) * context);                                       \
    bool (*isFocused)(const GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) widget);              \
    OPSTATUS (*clipPush)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region);                  \
    OPSTATUS (*clipPop)(GUI_TYPE(TContext) * context);                                             \
    OPSTATUS (*rowBegin)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float gap);       \
    OPSTATUS (*columnBegin)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float gap);    \
    OPSTATUS (*layoutNext)                                                                         \
    (GUI_TYPE(TContext) * context, float extent, TQUAD_TYPE(float) * outRegion);                   \
    OPSTATUS (*layoutEnd)(GUI_TYPE(TContext) * context);                                           \
    OPSTATUS (*layoutMeasureText)                                                                  \
    (const GUI_TYPE(TFont) * font, const char *text, float scale, TDUAL_TYPE(float) * outSize);    \
    OPSTATUS (*drawRectangle)                                                                      \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint32_t color, float radius);        \
    OPSTATUS (*drawText)                                                                           \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *text, uint32_t color,     \
     const GUI_TYPE(TFont) * font, float scale);                                                   \
    OPSTATUS (*image)                                                                              \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,                                       \
     const GRAPHICS_TYPE(TSurface) * surface);                                                     \
    OPSTATUS (*imageEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TImage) * image);             \
    OPSTATUS (*label)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *text);   \
    OPSTATUS (*labelEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TLabel) * label);             \
    OPSTATUS (*separator)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region);                 \
    OPSTATUS (*button)                                                                             \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *outPressed); \
    OPSTATUS (*buttonEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TButton) * button);                \
    OPSTATUS (*checkbox)                                                                           \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *checked);    \
    OPSTATUS (*checkboxEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TCheckbox) * checkbox);          \
    OPSTATUS (*radio)                                                                              \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, uint32_t option,   \
     uint32_t *selected);                                                                          \
    OPSTATUS (*radioEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TRadio) * radio);                   \
    OPSTATUS (*slider)                                                                             \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float minimum, float maximum,         \
     float *value);                                                                                \
    OPSTATUS (*sliderEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TSlider) * slider);                \
    OPSTATUS (*progress)(GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float fraction);  \
    OPSTATUS (*progressBar)                                                                        \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint64_t value, uint64_t maximum);    \
    OPSTATUS (*progressBarEx)                                                                      \
    (GUI_TYPE(TContext) * context, const GUI_TYPE(TProgressBar) * progress);                       \
    OPSTATUS (*inputText)                                                                          \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, char *buffer, size_t capacity,        \
     size_t *inoutLength);                                                                         \
    OPSTATUS (*inputTextEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TInputText) * input);           \
    OPSTATUS (*textArea)                                                                           \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, char *buffer, size_t capacity,        \
     size_t *inoutLength);                                                                         \
    OPSTATUS (*textAreaEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTextArea) * input);             \
    OPSTATUS (*inputTextSelect)(GUI_TYPE(TInputText) * input, size_t anchor, size_t caret);        \
    OPSTATUS (*inputTextCopy)                                                                      \
    (const GUI_TYPE(TInputText) * input, char *buffer, size_t capacity, size_t *outLength);        \
    OPSTATUS (*inputTextCut)                                                                       \
    (GUI_TYPE(TInputText) * input, char *buffer, size_t capacity, size_t *outLength);              \
    OPSTATUS (*inputTextPaste)                                                                     \
    (GUI_TYPE(TInputText) * input, const char *utf8Text, size_t byteLength);                       \
    OPSTATUS (*list)                                                                               \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items,             \
     size_t count, size_t *selected);                                                              \
    OPSTATUS (*listEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TList) * list);                      \
    OPSTATUS (*menu)                                                                               \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items,             \
     size_t count, size_t *selected, bool *outActivated);                                          \
    OPSTATUS (*menuEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TMenu) * menu);                      \
    OPSTATUS (*panelBegin)                                                                         \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *title);                   \
    OPSTATUS (*panelBeginEx)(GUI_TYPE(TContext) * context, const GUI_TYPE(TPanel) * panel);        \
    OPSTATUS (*panelEnd)(GUI_TYPE(TContext) * context);                                            \
    OPSTATUS (*modalBegin)(GUI_TYPE(TContext) * context, GUI_TYPE(WidgetId) id);                   \
    OPSTATUS (*modalEnd)(GUI_TYPE(TContext) * context);                                            \
    OPSTATUS (*confirm)                                                                            \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *message,                  \
     bool *outAccepted, bool *outRejected);                                                        \
    OPSTATUS (*confirmEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TDialog) * dialog);               \
    OPSTATUS (*dialog)                                                                             \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *title,                    \
     const char *message, bool *outDismissed);                                                     \
    OPSTATUS (*dialogEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TDialog) * dialog);                \
    OPSTATUS (*statusBar)                                                                          \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *left, const char *right); \
    OPSTATUS (*toast)                                                                              \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *message, bool visible);   \
    OPSTATUS (*spinner)                                                                            \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, uint64_t phase);   \
    OPSTATUS (*renderSoftware)                                                                     \
    (GRAPHICS_TYPE(TSurface) * surface, const GUI_TYPE(TCommand) * commands, size_t count,         \
     float scale);                                                                                 \
    OPSTATUS (*imageButton)                                                                        \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region,                                       \
     const GRAPHICS_TYPE(TSurface) * surface, bool *outPressed);                                   \
    OPSTATUS (*imageButtonEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TImageButton) * button);      \
    OPSTATUS (*comboBox)                                                                           \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items,             \
     size_t count, size_t *selected, bool *open);                                                  \
    OPSTATUS (*comboBoxEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TComboBox) * combo);             \
    OPSTATUS (*tabs)                                                                               \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *items,             \
     size_t count, size_t *selected);                                                              \
    OPSTATUS (*tabsEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTabs) * tabs);                      \
    OPSTATUS (*treeNode)                                                                           \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *label, bool *expanded,    \
     bool *selected);                                                                              \
    OPSTATUS (*treeNodeEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTreeNode) * node);              \
    OPSTATUS (*table)                                                                              \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, const char *const *cells,             \
     size_t rows, size_t columns, size_t *selectedRow);                                            \
    OPSTATUS (*tableEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TTable) * table);                   \
    OPSTATUS (*colorPicker)                                                                        \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, uint32_t *color);                     \
    OPSTATUS (*colorPickerEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TColorPicker) * picker);      \
    OPSTATUS (*number)                                                                             \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float minimum, float maximum,         \
     float step, float *value);                                                                    \
    OPSTATUS (*numberEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TNumber) * number);                \
    OPSTATUS (*scrollbar)                                                                          \
    (GUI_TYPE(TContext) * context, TQUAD_TYPE(float) region, float maximum, float visible,         \
     float *offset, bool vertical);                                                                \
    OPSTATUS (*scrollbarEx)(GUI_TYPE(TContext) * context, GUI_TYPE(TScrollbar) * scrollbar);       \
  } GUI_TYPE(TFunctionTable);

#define GUI_FUNCTION_TABLE_INSTANCE                                                                \
  static const GUI_TYPE(TFunctionTable)                                                            \
      GUI_FUNC(functions) = {.fontASF = GUI_FUNC(Font_ASF),                                        \
                             .themeDark = GUI_FUNC(Theme_Dark),                                    \
                             .themeLight = GUI_FUNC(Theme_Light),                                  \
                             .themeClassic = GUI_FUNC(Theme_Classic),                              \
                             .themeMonochrome = GUI_FUNC(Theme_Monochrome),                        \
                             .themeDefault = GUI_FUNC(Theme_Default),                              \
                             .themeSet = GUI_FUNC(Theme_Set),                                      \
                             .themeGet = GUI_FUNC(Theme_Get),                                      \
                             .scaleSet = GUI_FUNC(Scale_Set),                                      \
                             .visualPush = GUI_FUNC(Visual_Push),                                  \
                             .visualPop = GUI_FUNC(Visual_Pop),                                    \
                             .begin = GUI_FUNC(Begin),                                             \
                             .end = GUI_FUNC(End),                                                 \
                             .event = GUI_FUNC(Event),                                             \
                             .getSize = GUI_FUNC(GetSize),                                         \
                             .getId = GUI_FUNC(GetId),                                             \
                             .pushId = GUI_FUNC(PushId),                                           \
                             .popId = GUI_FUNC(PopId),                                             \
                             .focusSet = GUI_FUNC(Focus_Set),                                      \
                             .focusNext = GUI_FUNC(Focus_Next),                                    \
                             .focusPrevious = GUI_FUNC(Focus_Previous),                            \
                             .isFocused = GUI_FUNC(IsFocused),                                     \
                             .clipPush = GUI_FUNC(Clip_Push),                                      \
                             .clipPop = GUI_FUNC(Clip_Pop),                                        \
                             .rowBegin = GUI_FUNC(Row_Begin),                                      \
                             .columnBegin = GUI_FUNC(Column_Begin),                                \
                             .layoutNext = GUI_FUNC(Layout_Next),                                  \
                             .layoutEnd = GUI_FUNC(Layout_End),                                    \
                             .layoutMeasureText = GUI_FUNC(Layout_MeasureText),                    \
                             .drawRectangle = GUI_FUNC(DrawRectangle),                             \
                             .drawText = GUI_FUNC(DrawText),                                       \
                             .image = GUI_FUNC(Image),                                             \
                             .imageEx = GUI_FUNC(ImageEx),                                         \
                             .label = GUI_FUNC(Label),                                             \
                             .labelEx = GUI_FUNC(LabelEx),                                         \
                             .separator = GUI_FUNC(Separator),                                     \
                             .button = GUI_FUNC(Button),                                           \
                             .buttonEx = GUI_FUNC(ButtonEx),                                       \
                             .checkbox = GUI_FUNC(Checkbox),                                       \
                             .checkboxEx = GUI_FUNC(CheckboxEx),                                   \
                             .radio = GUI_FUNC(Radio),                                             \
                             .radioEx = GUI_FUNC(RadioEx),                                         \
                             .slider = GUI_FUNC(Slider),                                           \
                             .sliderEx = GUI_FUNC(SliderEx),                                       \
                             .progress = GUI_FUNC(Progress),                                       \
                             .progressBar = GUI_FUNC(ProgressBar),                                 \
                             .progressBarEx = GUI_FUNC(ProgressBarEx),                             \
                             .inputText = GUI_FUNC(InputText),                                     \
                             .inputTextEx = GUI_FUNC(InputTextEx),                                 \
                             .textArea = GUI_FUNC(TextArea),                                       \
                             .textAreaEx = GUI_FUNC(TextAreaEx),                                   \
                             .inputTextSelect = GUI_FUNC(InputText_Select),                        \
                             .inputTextCopy = GUI_FUNC(InputText_Copy),                            \
                             .inputTextCut = GUI_FUNC(InputText_Cut),                              \
                             .inputTextPaste = GUI_FUNC(InputText_Paste),                          \
                             .list = GUI_FUNC(List),                                               \
                             .listEx = GUI_FUNC(ListEx),                                           \
                             .menu = GUI_FUNC(Menu),                                               \
                             .menuEx = GUI_FUNC(MenuEx),                                           \
                             .panelBegin = GUI_FUNC(Panel_Begin),                                  \
                             .panelBeginEx = GUI_FUNC(Panel_BeginEx),                              \
                             .panelEnd = GUI_FUNC(Panel_End),                                      \
                             .modalBegin = GUI_FUNC(Modal_Begin),                                  \
                             .modalEnd = GUI_FUNC(Modal_End),                                      \
                             .confirm = GUI_FUNC(Confirm),                                         \
                             .confirmEx = GUI_FUNC(ConfirmEx),                                     \
                             .dialog = GUI_FUNC(Dialog),                                           \
                             .dialogEx = GUI_FUNC(DialogEx),                                       \
                             .statusBar = GUI_FUNC(StatusBar),                                     \
                             .toast = GUI_FUNC(Toast),                                             \
                             .spinner = GUI_FUNC(Spinner),                                         \
                             .renderSoftware = GUI_FUNC(RenderSoftware),                           \
                             .imageButton = GUI_FUNC(ImageButton),                                 \
                             .imageButtonEx = GUI_FUNC(ImageButtonEx),                             \
                             .comboBox = GUI_FUNC(ComboBox),                                       \
                             .comboBoxEx = GUI_FUNC(ComboBoxEx),                                   \
                             .tabs = GUI_FUNC(Tabs),                                               \
                             .tabsEx = GUI_FUNC(TabsEx),                                           \
                             .treeNode = GUI_FUNC(TreeNode),                                       \
                             .treeNodeEx = GUI_FUNC(TreeNodeEx),                                   \
                             .table = GUI_FUNC(Table),                                             \
                             .tableEx = GUI_FUNC(TableEx),                                         \
                             .colorPicker = GUI_FUNC(ColorPicker),                                 \
                             .colorPickerEx = GUI_FUNC(ColorPickerEx),                             \
                             .number = GUI_FUNC(Number),                                           \
                             .numberEx = GUI_FUNC(NumberEx),                                       \
                             .scrollbar = GUI_FUNC(Scrollbar),                                     \
                             .scrollbarEx = GUI_FUNC(ScrollbarEx)};

GUI_FUNCTION_TABLE_STRUCT
GUI_FUNCTION_TABLE_INSTANCE
#endif
