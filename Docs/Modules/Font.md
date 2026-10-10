# Font — FIGlet (.flf) and ASF1 (.asf)

Path: \`Codespace/Cosmeron/Modules/Graphics/Font/Font.h\`
Namespace: \`FONT_TYPE(TFont)\` and \`FONT_FUNC(...)\`.

The Font package borrows font source bytes, checks them, measures ASCII banner
text and renders into the **existing terminal Canvas**, preserving the same
Text/Grid style cells. It never prints ANSI escape sequences to stdout and
does not create separate color, geometry or frame-buffer types.

This first iteration deliberately supports text characters U+0020 through
U+007E (printable ASCII) and a maximum glyph height of 32 rows. More Unicode
glyphs and full FIGlet horizontal smushing are follow-up work; basic
side-by-side composition is supported. No additional linker flags.

## Public API

\`\`\`c
OPSTATUS FONT_FUNC(ParseASF)(
    const char *data, size_t size, FONT_TYPE(TFont) *outFont);
OPSTATUS FONT_FUNC(ParseFIGlet)(
    const char *data, size_t size, FONT_TYPE(TFont) *outFont);
OPSTATUS FONT_FUNC(Measure)(
    const FONT_TYPE(TFont) *font, const char *asciiText,
    TDUAL_TYPE(uint16) *outSize);
OPSTATUS FONT_FUNC(Draw)(
    TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position,
    const FONT_TYPE(TFont) *font, const char *asciiText,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) defaultStyle);
\`\`\`

Default symbol names are \`Font_ParseASF\`, \`Font_ParseFIGlet\`,
\`Font_Measure\` and \`Font_Draw\`. Initialize fonts as \`{0}\`;
\`Parse*\` fills them without allocation. **Do not discard, overwrite, or free
the input bytes before your last Draw/Measure call**. A parsed font contains
offsets into that source memory.

## ASF1 specification (version 1)

ASF means *ANSI/ASCII Font*. Version 1 is a **plain UTF-8 text** file with
the extension \`.asf\`. It is independent of existing unrelated ASF multimedia
formats: check the leading \`ASF1\` signature instead of relying on extension.

\`\`\`text
ASF1 3
# Optional comment between glyph blocks
@32
     
     
     
@65
  A
 A A
AAAAA
@66
\e[38;2;255;40;30mBBBBB\e[0m
B   B
BBBBB
\`\`\`

The first line is \`ASF1 <height>\`; height must be 1–32. Each glyph begins
with \`@<decimal ASCII code>\` (32..126), followed by **exactly height**
lines of cells. **All the rows of a glyph must have the same rendered cell
width** (use spaces to pad); widths may differ between glyphs. Do not trim
leading/trailing spaces. Empty lines and \`#\` comments are accepted **only
between glyph blocks**, not during a glyph's rows. Files can use LF or CRLF.

The row contents may contain printable UTF-8 glyphs (including box-drawing)
and supported ANSI SGR styles. ANSI SGR may be written as actual ESC bytes
or the portable two-character \`\e\` escape followed by \`[\` and parameters,
e.g. \`\e[31m\`, \`\e[1m\`, \`\e[38;2;255;0;0m\`,
\`\e[48;5;27m\`, \`\e[0m\`. These are treated as **style metadata**, never
as terminal output. Supported modifiers include bold/faint, italic,
underline, inverse, 16-color, 256-color and RGB foreground/background.
Unknown CSI sequences are rejected during render; \`Font_Draw\` will not
execute arbitrary terminal commands. Each row starts with the caller's
\`defaultStyle\`; reset returns to that style.

## Compile a font into your application

Use the dependency-free Python 3 **build-time converter**:

\`\`\`bash
python3 Tools/asf_compile.py MyFont.asf \
    --symbol MyFont -o MyFont.asf.h
\`\`\`

It emits an ordinary immutable C11 byte array. The generated header can be
included alongside your application and built normally by GCC, Clang,
MSVC, or MinGW; no \`#embed\` (C23), \`incbin\`, runtime file IO, or
\`-l...\` flags required. You can similarly compile \`.flf\` files.

\`\`\`c
#include "Cosmeron/Modules/Graphics/Font/Font.h"
#include "MyFont.asf.h" /* static const unsigned char MyFont_Data[] */

FONT_TYPE(TFont) font={0};
TERMINAL_TYPE(TCanvas) canvas={0};

/* After creating a Canvas, e.g. 80x25: */
if(FONT_FUNC(ParseASF)((const char *)MyFont_Data,
                       sizeof(MyFont_Data), &font)==STATUS_CONST(SUCCESS)){
    (void)FONT_FUNC(Draw)(&canvas,(TDUAL_TYPE(uint16)){.x=2,.y=2},
         &font,"AB",TEXT_GRID_ATTRIBUTE_FUNC(Default)());
}
(void)TERMINAL_FUNC(Canvas_Update)(&canvas);
\`\`\`

Build-time conversion is optional. If loading a \`.asf\` or \`.flf\` file
manually, read its bytes into an application-owned buffer and pass its actual
byte length to \`ParseASF\`/\`ParseFIGlet\`. A final NUL is not required.

## FIGlet support

\`ParseFIGlet\` accepts FIGfont 2 \`flf2a\` headers, comment lines and
the **95 required printable ASCII glyphs**. It strips FIGlet endmarks and
turns font hardblanks into ordinary canvas spaces. It does not currently
implement optional Unicode codetag mappings, FIGlet control files,
right-to-left layouts, or FIGlet's vertical/horizontal smushing algorithms.

FIGlet compatibility here means **font loading + rectangular drawing**,
not pixel-identical output with every option of the \`figlet\` executable.

## Ownership and limits

- Parser state is a value type; no \`Font_Init/Free\` required.
- \`Measure\` and \`Draw\` accept multiline ASCII text (\`\\n\`). The latter
  preflights the bounding box to avoid writing outside its Canvas.
- A missing glyph returns \`NOT_FOUND\`, an unsupported input text codepoint
  returns \`NOT_SUPPORTED\`, and malformed sources return \`INVALID_SEQUENCE\`.
- ANSI SGR style data is parsed into Text/Grid attributes; the terminal
  presentation happens **only** on \`Canvas_Update\`.
- Existing \`UI_Label\`/menus can integrate a banner via an explicit
  \`Font_Draw\` call before/after their layout. No independent TUI event loop.
- Fonts from third parties may have separate licenses; the converter does
  **not** grant usage rights to those fonts.
