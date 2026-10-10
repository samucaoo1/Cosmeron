# Font — fontes estáticas C11 (.asf), ASF1/ASF2 e FIGlet (.flf)

Path: \`Codespace/Cosmeron/Modules/Graphics/Font/Font.h\`
Namespace: \`FONT_TYPE(TFont)\` and \`FONT_FUNC(...)\`.

The Font package accepts static C11 font definitions or borrowed source bytes,
measures ASCII banner text and renders into the **existing terminal Canvas**, preserving the same
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

## ASF nativo — \`static const struct\` C11 (recomendado)

O formato recomendado para fontes que serão **compiladas junto do programa**
é um \`.asf\` contendo uma definição C11 **diretamente**. Não é preciso
executar scripts, converter o arquivo, carregar bytes nem chamar
\`Font_ParseASF\`. É apenas um \`#include\`:

\`\`\`c
/* FonteExemplo.asf */
#pragma once
#include "../Font.h" /* ajuste conforme a localização da fonte */

static const FONT_TYPE(TFont) FonteExemplo = {
    .format = FONT_NS(FORMAT_STATIC),
    .height = 3,
    .glyphs = {
        FONT_GLYPH('A') = {
            .art = "  A\n"
                   " A A\n"
                   "AAAAA"
        },
        FONT_GLYPH('B') = {
            .art = "BBBBB\n"
                   "B   B\n"
                   "BBBBB",
            .foreground = 0xFF281EU,
            .foregroundRows = UINT32_C(1) << 0
        }
    }
};
\`\`\`

O conteúdo da fonte fica visualmente legível como FIGlet, **mas cada linha
precisa ser uma string C entre aspas**: a linguagem C11 não tem strings
multilinha brutas e um inicializador \`const struct\` exige sintaxe C.
Espaços à direita não são necessários; a largura de cada glifo é inferida
automaticamente pela maior linha. \`FONT_GLYPH('A')\` é um índice designado
C11 equivalente a \`['A'-32]\`. O suporte inicial é ASCII imprimível para
*caracteres de entrada*; os caracteres desenhados podem usar UTF-8.

Cores não atrapalham a arte: \`.foregroundRows\` é uma máscara de bits,
na qual bit 0 significa a primeira linha, bit 1 a segunda e assim por
diante. \`.foreground\` é a cor RGB compartilhada entre as linhas marcadas.
Há também \`.background\` e \`.backgroundRows\`. Para pintar todas as linhas,
use \`UINT32_MAX\`. Atributos não definidos preservam o estilo padrão
fornecido a \`Font_Draw\`.

\`\`\`c
#include "Cosmeron/Modules/Graphics/Font/Font.h"
#include "FonteExemplo.asf"

FONT_FUNC(Draw)(&canvas, (TDUAL_TYPE(uint16)){.x=1,.y=1},
    &FonteExemplo, "AB", TEXT_GRID_ATTRIBUTE_FUNC(Default)());
\`\`\`

**Vantagens:** somente dados \`static const\`, sem dependência de Python,
sem \`#embed\` C23, sem alocação dinâmica ou linkagem adicional, compilação
normal no C11, com a mesma API \`Font_Measure\`/\`Font_Draw\`.

O arquivo [Readable.asf](../../Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Readable.asf)
é um exemplo funcional. Para arquivos \`.flf\` tradicionais ou ASF1/ASF2
em texto, os leitores anteriores continuam disponíveis; o script de
conversão é **opcional e apenas para essas fontes legadas**.

---

## ASF2 — fonte legível, estilo FIGlet

**Novo formato recomendado para criação manual.** Um arquivo ASF2 mostra as
letras diretamente; nenhuma sequência ANSI é inserida no desenho. As cores
opcionais são declaradas após as linhas de arte de cada glifo.

```text
ASF2 3
# Glifos visíveis em um editor de texto comum
@A
  A
 A A
AAAAA

@B
BBBBB
B   B
BBBBB
!fg 0 #FF281E
```

- `ASF2 3`: altura de três linhas; `@A` e `@B` são glifos por caractere;
  `@space` representa o espaço (ASCII 32).
- Linhas têm largura variável dentro de um glifo; o leitor mede a maior e
  avança esse número de células, sem obrigar espaços à direita.
- `!fg 0 #FF281E`: pinta **somente a linha zero** do glifo B com RGB.
  Usar `!fg 1 #...` para a segunda, e assim por diante.
- Linhas em branco e comentários são permitidos entre blocos.
- Glifos de entrada ainda são limitados a ASCII 32–126. Os desenhos aceitam
  Unicode UTF-8, sujeitos ao suporte de largura do Canvas.
- O parser antigo `Font_ParseASF` continua lendo ASF1, sem alterações.
  Use `Font_ParseASF2` para esta sintaxe.

```c
FONT_TYPE(TFont) font={0};
FONT_FUNC(ParseASF2)((const char *)MyFont_Data,
    sizeof(MyFont_Data),&font);
FONT_FUNC(Draw)(&canvas,position,&font,"AB",
    TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

A ferramenta `Tools/asf_compile.py` também embute arquivos ASF2 em
headers C11 sem modificar os bytes. Exemplo:
`Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Readable.asf`.

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


## Bundled FIGlet fonts: Cosmeron and NibbleBits

Two distinct fonts are available in
[Graphics/Font/Fonts](../../Codespace/Cosmeron/Modules/Graphics/Font/Fonts).

### Cosmeron.flf — orbital wordmark

The new **Cosmeron.flf** is inspired by the project's wordmark: sharp,
minimal, futuristic strokes, open C/S shapes, an angular M, segmented E
and R, plus a round orbital O containing a central dot.

Because a FIGlet font maps each source character to one glyph, both O
letters in ordinary `COSMERON` share the same orbital-dot form.
Type **`COSMER0N` (zero)** to use the alternate O with a power-button
vertical bar in the penultimate position, like the reference logo.

This **UTF-8 FIGfont** uses Unicode box-drawing characters and is 9 rows
high; each printable ASCII input glyph occupies 12 terminal cells.
All 95 standard printable ASCII glyphs and seven legacy FIGlet extended
positions are present. This is a monochrome font; pass white/cyan
`TEXT_GRID_ATTRIBUTE_TYPE(Cell)` styles to `Font_Draw` to add color.

Compatibility note: Cosmeron's own UTF-8 Canvas reader supports this
FIGfont. Legacy versions of the external `figlet` program may not
measure or render UTF-8 glyph artwork correctly. For those tools, use
**Cosmeron-ASCII.flf**, the same letterforms translated to basic ASCII
strokes; its geometry is also 9 rows × 12 columns.

Example:

```sh
# Portable ASCII fallback for the classic figlet CLI:
figlet -f Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Cosmeron-ASCII.flf COSMER0N

# Embed the Unicode variant directly as C11 data, no runtime file loading:
python3 Tools/asf_compile.py \
  Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Cosmeron.flf \
  --symbol CosmeronWordmark -o CosmeronWordmark.flf.h
```

```c
#include "Cosmeron/Modules/Graphics/Font/Font.h"
#include "CosmeronWordmark.flf.h"

FONT_TYPE(TFont) wordmark={0};
OPSTATUS status=FONT_FUNC(ParseFIGlet)(
    (const char *)CosmeronWordmark_Data,
    sizeof(CosmeronWordmark_Data), &wordmark);

if(status==STATUS_CONST(SUCCESS)){
    status=FONT_FUNC(Draw)(&canvas,
        (TDUAL_TYPE(uint16)){.x=0,.y=0},
        &wordmark,"COSMER0N",TEXT_GRID_ATTRIBUTE_FUNC(Default)());
}
```

The rendering size for an eight-letter wordmark is **96 columns × 9
rows**, requiring an appropriately sized Canvas and terminal viewport.

### NibbleBits.flf — original binary font

The previous Cosmeron Binary Orbit design has been preserved as
**NibbleBits.flf** rather than overwritten. It is built from the
characters `0` and `1` only, with 7-row glyphs and 8 columns per
character. It supports the same 95 ASCII characters and seven optional
FIGlet extended positions.

```sh
figlet -f Codespace/Cosmeron/Modules/Graphics/Font/Fonts/NibbleBits.flf COSMERON
```

Test suites: `Codespace/Tests/Terminal/FontCosmeron.c`
and `FontNibbleBits.c`.

### Cosmeron Bold (.flf) — thick orbital wordmark

The **Bold** companion uses heavy Unicode box-drawing strokes (`━`, `┃`, etc.),
reinforced diagonals and rounded orbital corners, following the thick strokes
of the reference COSMERON wordmark. The two `O` characters contain the solid
core `●`; type `COSMER0N` (zero) for the power-bar version of the second ring.

- [Cosmeron-Bold.flf](../../Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Cosmeron-Bold.flf): UTF-8 heavy-line variant.
- [Cosmeron-Bold-ASCII.flf](../../Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Cosmeron-Bold-ASCII.flf): ASCII fallback, using dense `#` and `=` strokes.

Both fonts have **9 rows and 12 columns per character**, support all 95
printable ASCII inputs and the seven traditional FIGlet extended slots, and
use full-width layout (no smushing). The original regular Cosmeron and
**NibbleBits** fonts remain unchanged.

```sh
figlet -f Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Cosmeron-Bold-ASCII.flf COSMER0N
python3 Tools/asf_compile.py \\
  Codespace/Cosmeron/Modules/Graphics/Font/Fonts/Cosmeron-Bold.flf \\
  --symbol CosmeronBold -o CosmeronBold.flf.h
```

FIGlet artwork is monochrome. The bright white/cyan neon and light bloom of
the original wordmark are presentation effects: use `Font_Draw` with colored
Text/Grid attributes or convert to ASF for per-glyph colors. They are not
encoded into the `.flf` itself.

CI covers both fonts in `Codespace/Tests/Terminal/FontCosmeronBold.c`.
