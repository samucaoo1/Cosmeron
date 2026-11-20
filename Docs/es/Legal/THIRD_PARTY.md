# Procedencia y avisos de terceros

## Implementación histórica de miniz (sustituida)

El módulo Compression de Cosmeron incorporaba originalmente **miniz 3.1.0**
(commit `174573d60290f447c13a2b1b3405de2b96e27d6c`), cuyos autores fueron
Rich Geldreich, Tenacious Software LLC, RAD Game Tools y Valve Software.
Fuente: https://github.com/richgel999/miniz/tree/3.1.0.

El antiguo motor vendorizado fue sustituido, en la rama
`compression-native`, por una implementación C11 orientada a RFC 1950, RFC 1951,
RFC 1952 y al formato ZIP público. La implementación nativa conserva la
API pública de Cosmeron y utiliza código interno de Cosmeron. El componente
original sigue acreditado aquí para fines de procedencia y revisión histórica del código fuente.

La licencia MIT de miniz se reprodujo en
`Docs/Legal/Licenses/Miniz.MIT.txt`. Conservar este registro **no**
significa que miniz sea una dependencia actual de runtime o de headers.

Al añadir futuras fuentes de terceros, registra su revisión, origen,
titulares de los derechos de autor, modificaciones y avisos completos de licencia
en este documento, y conserva los archivos de aviso exigidos junto con todas las
copias sustanciales redistribuidas.

## Referencia del codebook numérico Huffman de MPEG Layer III

El archivo `Audio/Codec/Impl/MP3HuffmanTables.inc` registra las asignaciones numéricas de palabras de código y longitudes exigidas por el formato de audio MPEG Layer III (ISO/IEC 11172-3 y 13818-3). Las matrices de asignación se contrastaron con la documentación pública y el código fuente de FFmpeg 4.3 en
https://github.com/FFmpeg/FFmpeg/blob/release/4.3/libavcodec/mpegaudiodectab.h .

Solo se representan los valores numéricos del codebook definidos por el formato; el lector de bitstream Huffman, el tratamiento de escape, el parsing count1 y el ensamblado de granules son implementaciones C11 originales de Cosmeron. No se han incorporado como código de terceros funciones de decodificación, algoritmos de búsqueda ni headers de FFmpeg. Esta referencia se mantiene fuera de los archivos include generados, de acuerdo con la documentación consolidada de procedencia de Cosmeron.

## Referencia de la tabla numérica de síntesis MPEG Layer III

El archivo independiente `Audio/Codec/Impl/MP3SynthesisTables.inc` almacena la secuencia estandarizada de 512 entradas de la ventana de síntesis de ISO/IEC 11172-3 (Tabla B.3) y la matriz matemática de coseno de 64×32 calculada mediante la fórmula cos((16+i)(2k+1)pi/64). Los valores numéricos de la ventana se contrastaron con la secuencia entera de media ventana de FFmpeg 4.3, `libavcodec/mpegaudiodsp_data.c`:
https://github.com/FFmpeg/FFmpeg/blob/release/4.3/libavcodec/mpegaudiodsp_data.c

Las implementaciones `MP3_SynthesisSlot` y `MP3_Synthesize` de Cosmeron son rutinas C11 originales; no se importaron algoritmos de decodificación, headers ni objetos de runtime de terceros.