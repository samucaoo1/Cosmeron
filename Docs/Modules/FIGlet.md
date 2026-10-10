# FIGlet — arquivos FIGfont .flf

A pasta **\`Codespace/Asset/FIGlet/\`** contém todas as fontes FIGlet:
Cosmeron, Cosmeron-ASCII, Cosmeron-Bold, Cosmeron-Bold-ASCII e NibbleBits.

O parser e renderizador independentes estão em
\`Codespace/Cosmeron/Modules/Graphics/FIGlet/FIGlet.h\`.

\`\`\`c
#include "Cosmeron/Modules/Graphics/FIGlet/FIGlet.h"

FIGLET_TYPE(TFont) fig={0};
if(FIGLET_FUNC(Parse)(data,length,&fig)==STATUS_CONST(SUCCESS)){
    TDUAL_TYPE(uint16) dims={0};
    (void)FIGLET_FUNC(Measure)(&fig,"COSMERON",&dims);
    (void)FIGLET_FUNC(Draw)(&canvas,
        (TDUAL_TYPE(uint16)){.x=0,.y=0},
        &fig,"COSMERON",TEXT_GRID_ATTRIBUTE_FUNC(Default)());
}
\`\`\`

O aplicativo carrega os bytes \`.flf\` da forma que preferir e os mantém
vivos durante a utilização; o parser empresta o buffer, sem alocação
dinâmica ou dependência de bibliotecas externas.

Compatibilidade inicial: header \`flf2a\`, comentários, hardblank, 95 glifos
ASCII, arte Unicode UTF-8, desenhada lado a lado (sem smushing, kerning ou
layout direita-para-esquerda). Codetags extras são ignoradas.

Para fontes novas compiladas como structs C11, use \`.asf\` em
\`Graphics/Font/Fonts/\`; esse outro formato não passa pelo parser FIGlet.
