# Font — somente .asf nativo C11

O módulo \`Graphics/Font\` aceita exclusivamente fontes \`.asf\` definidas
como \`static const FONT_TYPE(TFont)\`. A própria fonte é incluída em C11,
sem script, parser, leitura de arquivos ou \`#embed\`.

## Exemplo

\`\`\`c
#include "Cosmeron/Modules/Graphics/Font/Font.h"

static const FONT_TYPE(TFont) Example_Font = {
    .height = 3,
    .name = "Example",
    .author = "Example author",
    .license = "CC0-1.0",
    .letterSpacing = 1,

    /* glyphs é o último campo. */
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

Uma linha \`.art\` é UTF-8 e cada glifo tem exatamente \`.height\` linhas.
A largura é a maior linha, ou \`.columns\` caso esse campo seja informado;
neste caso, deve ser pelo menos a largura real. Entradas ASCII: 32..126.
Os espaçamentos \`.letterSpacing\` e \`.lineSpacing\` são medidos em células.
As máscaras \`.foregroundRows\`/\`.backgroundRows\` podem colorir por linha
com RGB sem inserir escapes ANSI no desenho.

## Desenho

\`\`\`c
#include "Cosmeron/Modules/Graphics/Font/Fonts/VGA8x8.asf"

TDUAL_TYPE(uint16) size={0};
(void)FONT_FUNC(Measure)(&VGA8x8_Font,"COSMERON",&size);
(void)FONT_FUNC(Draw)(&canvas,(TDUAL_TYPE(uint16)){.x=2,.y=2},
   &VGA8x8_Font,"COSMERON",TEXT_GRID_ATTRIBUTE_FUNC(Default)());
\`\`\`

Fontes fornecidas: \`Readable.asf\` (exemplo pequeno colorido) e
\`VGA8x8.asf\` (95 caracteres ASCII de bitmap 8×8, projeto original
[font8x8](https://github.com/dhepper/font8x8/blob/8e279d2d864e79128e96188a6b9526cfa3fbfef9/font8x8_basic.h)
de domínio público, conforme declaração de seus autores).

**FIGlet é separado**: os \`.flf\` residem em
\`Codespace/Asset/FIGlet/\` e são lidos por
\`Graphics/FIGlet/FIGlet.h\`; veja [FIGlet.md](FIGlet.md).

Esta limpeza removeu intencionalmente os formatos textuais ASF1/ASF2,
\`Font_ParseASF\`, \`Font_ParseASF2\`, \`Font_ParseFIGlet\` e o conversor
\`Tools/asf_compile.py\`. Permanecem somente \`Font_Measure\` e
\`Font_Draw\` para \`static const\`.
