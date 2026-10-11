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
