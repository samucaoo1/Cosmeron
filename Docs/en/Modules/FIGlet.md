# FIGlet — arquivos FIGfont .flf

A pasta **`Codespace/Asset/FIGlet/`** contém todas as fontes FIGlet:
Cosmeron, Cosmeron-ASCII, Cosmeron-Bold, Cosmeron-Bold-ASCII e NibbleBits.

O parser e renderizador independentes estão em
`Codespace/Cosmeron/Modules/Graphics/FIGlet/FIGlet.h`.

```c
#include "Cosmeron/Modules/Graphics/FIGlet/FIGlet.h"

FIGLET_TYPE(TFont) fig={0};
if(FIGLET_FUNC(Parse)(data,length,&fig)==STATUS_CONST(SUCCESS)){
    TDUAL_TYPE(uint16) dims={0};
    (void)FIGLET_FUNC(Measure)(&fig,"COSMERON",&dims);
    (void)FIGLET_FUNC(Draw)(&canvas,
        (TDUAL_TYPE(uint16)){.x=0,.y=0},
        &fig,"COSMERON",TEXT_GRID_ATTRIBUTE_FUNC(Default)());
}
```

O aplicativo carrega os bytes `.flf` da forma que preferir e os mantém
vivos durante a utilização; o parser empresta o buffer, sem alocação
dinâmica ou dependência de bibliotecas externas.

Compatibilidade inicial: header `flf2a`, comentários, hardblank, 95 glifos
ASCII, arte Unicode UTF-8, desenhada lado a lado (sem smushing, kerning ou
layout direita-para-esquerda). Codetags extras são ignoradas.

Para fontes novas compiladas como structs C11, use `.asf` em
`Graphics/Font/Fonts/`; esse outro formato não passa pelo parser FIGlet.

---

# API reference

The entries below follow the [Bit.md](Bit.md) convention: direct and namespace-macro signatures, inputs, results, remarks and examples. Examples are illustrative fragments; variables not explicitly declared must already have the correct initialized state.

## Function summary

| Function | Return type | Description |
| --- | --- | --- |
| `FIGLET_FUNC(Parse)` | `OPSTATUS` | Parses a borrowed FIGfont .flf buffer into a TFont descriptor. |
| `FIGLET_FUNC(Measure)` | `OPSTATUS` | Measures a string rendered in the parsed FIGlet font. |
| `FIGLET_FUNC(Draw)` | `OPSTATUS` | Draws a FIGlet text string onto Canvas without terminal presentation. |

# Parse

Parses a borrowed FIGfont .flf buffer into a TFont descriptor.

### Syntax

#### Macro form

```c
OPSTATUS FIGLET_FUNC(Parse)(const char *data,size_t size,FIGLET_TYPE(TFont)*outFont);
```

#### Direct form

```c
OPSTATUS FIGlet_Parse(const char *data,size_t size,FIGlet_TFont*outFont);
```

### Parameters

- `outFont`: typed value passed by the caller.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

FIGlet parsing borrows .flf bytes. Keep them alive while measuring/drawing; unsupported FIGlet smushing or right-to-left layout is not implied.

### Example

```c
FIGLET_TYPE(TFont) font = {0};
OPSTATUS status = FIGLET_FUNC(Parse)(fontData, fontBytes, &font);
```

# Measure

Measures a string rendered in the parsed FIGlet font.

### Syntax

#### Macro form

```c
OPSTATUS FIGLET_FUNC(Measure)(const FIGLET_TYPE(TFont)*font,const char*asciiText,TDUAL_TYPE(uint16)*outSize);
```

#### Direct form

```c
OPSTATUS FIGlet_Measure(const FIGlet_TFont*font,const char*asciiText,TDual_uint16*outSize);
```

### Parameters

- `outSize`: output dimensions in cells or pixels depending on module.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

FIGlet parsing borrows .flf bytes. Keep them alive while measuring/drawing; unsupported FIGlet smushing or right-to-left layout is not implied.

### Example

```c
TDUAL_TYPE(uint16) measured = {0};
OPSTATUS status = FONT_FUNC(Measure)(&font, "COSMERON", &measured);
```

# Draw

Draws a FIGlet text string onto Canvas without terminal presentation.

### Syntax

#### Macro form

```c
OPSTATUS FIGLET_FUNC(Draw)(TERMINAL_TYPE(TCanvas)*canvas,TDUAL_TYPE(uint16)position, const FIGLET_TYPE(TFont)*font,const char*asciiText, TEXT_GRID_ATTRIBUTE_TYPE(Cell)defaultStyle);
```

#### Direct form

```c
OPSTATUS FIGlet_Draw(Terminal_TCanvas*canvas,TDual_uint16position, const FIGlet_TFont*font,const char*asciiText, Text_Grid_Attribute_CelldefaultStyle);
```

### Parameters

- `position`: zero-based cell location.
- `asciiText`: NUL-terminated ASCII text to render.
- `defaultStyle`: fallback cell attributes if the glyph has no overrides.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

FIGlet parsing borrows .flf bytes. Keep them alive while measuring/drawing; unsupported FIGlet smushing or right-to-left layout is not implied.

### Example

```c
OPSTATUS status = FONT_FUNC(Draw)(&canvas, (TDUAL_TYPE(uint16)){.x=2,.y=2}, &font, "COSMERON", TEXT_GRID_ATTRIBUTE_FUNC(Default)());
```

