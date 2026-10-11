# Third-party provenance and notices

## Historical miniz implementation (replaced)

The Cosmeron Compression module originally incorporated **miniz 3.1.0**
(commit `174573d60290f447c13a2b1b3405de2b96e27d6c`), authored by
Rich Geldreich, Tenacious Software LLC, RAD Game Tools, and Valve Software.
Source: https://github.com/richgel999/miniz/tree/3.1.0.

The former vendored engine has been superseded on branch
`compression-native` by a C11 implementation targeting RFC 1950, RFC 1951,
RFC 1952 and the public ZIP format. The native implementation preserves the
public Cosmeron API and uses internal Cosmeron code. The original component
remains credited here for provenance and historical source review.

The miniz MIT license was reproduced in
`Docs/Legal/Licenses/Miniz.MIT.txt`. Retaining this record does **not**
represent miniz as a current runtime or header dependency.

When adding future third-party sources, record their revision, origin,
copyright owners, modifications and complete license notices here and
keep the required notice files with all redistributed substantial copies.

## Numerical MPEG Layer III Huffman codebook reference

The `Audio/Codec/Impl/MP3HuffmanTables.inc` file records the numerical
codeword and length assignments required by the MPEG Layer III audio
format (ISO/IEC 11172-3 and 13818-3). The assignment matrices were
cross-checked against the FFmpeg 4.3 public documentation and source
reference at
https://github.com/FFmpeg/FFmpeg/blob/release/4.3/libavcodec/mpegaudiodectab.h .

Only format-defined numerical codebook values are represented; the
Huffman bitstream reader, escape handling, count1 parsing and granule
assembly are original C11 Cosmeron implementations. FFmpeg decoding
functions, lookup algorithms and headers have not been incorporated as
third-party code. This reference is kept outside the generated include
files in keeping with Cosmeron's consolidated provenance documentation.
