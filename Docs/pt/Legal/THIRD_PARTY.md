# Proveniência e avisos de terceiros

## Implementação histórica do miniz (substituída)

O módulo Compression do Cosmeron incorporava originalmente **miniz 3.1.0**
(commit `174573d60290f447c13a2b1b3405de2b96e27d6c`), de autoria de
Rich Geldreich, Tenacious Software LLC, RAD Game Tools e Valve Software.
Fonte: https://github.com/richgel999/miniz/tree/3.1.0.

O antigo mecanismo vendorizado foi substituído, na branch
`compression-native`, por uma implementação C11 direcionada a RFC 1950, RFC 1951,
RFC 1952 e ao formato ZIP público. A implementação nativa preserva a
API pública do Cosmeron e utiliza código interno do Cosmeron. O componente
original continua creditado aqui para fins de proveniência e revisão histórica do código-fonte.

A licença MIT do miniz foi reproduzida em
`Docs/Legal/Licenses/Miniz.MIT.txt`. Manter este registro **não**
significa que miniz seja uma dependência atual de runtime ou de headers.

Ao adicionar futuras fontes de terceiros, registre sua revisão, origem,
titulares dos direitos autorais, modificações e avisos completos de licença
neste documento, mantendo os arquivos de aviso exigidos junto de todas as
cópias substanciais redistribuídas.

## Referência do codebook numérico Huffman do MPEG Layer III

O arquivo `Audio/Codec/Impl/MP3HuffmanTables.inc` registra as atribuições numéricas de palavras de código e comprimentos exigidas pelo formato de áudio MPEG Layer III (ISO/IEC 11172-3 e 13818-3). As matrizes de atribuição foram conferidas com a documentação pública e o código-fonte do FFmpeg 4.3 em
https://github.com/FFmpeg/FFmpeg/blob/release/4.3/libavcodec/mpegaudiodectab.h .

Somente os valores numéricos de codebook definidos pelo formato são representados; o leitor de bitstream Huffman, o tratamento de escape, o parsing count1 e a montagem de granules são implementações C11 originais do Cosmeron. Funções de decodificação, algoritmos de busca e headers do FFmpeg não foram incorporados como código de terceiros. Esta referência é mantida fora dos arquivos include gerados, em conformidade com a documentação consolidada de proveniência do Cosmeron.

## Referência da tabela numérica de síntese MPEG Layer III

O arquivo independente `Audio/Codec/Impl/MP3SynthesisTables.inc` armazena a sequência padronizada de 512 entradas da janela de síntese da ISO/IEC 11172-3 (Tabela B.3) e a matriz cosseno matemática de 64×32 calculada pela fórmula cos((16+i)(2k+1)pi/64). Os valores numéricos da janela foram conferidos com a sequência inteira de meia janela do FFmpeg 4.3, `libavcodec/mpegaudiodsp_data.c`:
https://github.com/FFmpeg/FFmpeg/blob/release/4.3/libavcodec/mpegaudiodsp_data.c

As implementações `MP3_SynthesisSlot` e `MP3_Synthesize` do Cosmeron são rotinas C11 originais; algoritmos de decodificação, headers e objetos de runtime de terceiros não foram importados.