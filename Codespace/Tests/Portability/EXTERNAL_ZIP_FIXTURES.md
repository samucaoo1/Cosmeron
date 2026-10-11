# Independent ZIP import fixtures for BSD CI

The BSD CI's `ExternalZipFixtures.c` stores **two deterministic archives
created with Python 3's standard `zipfile` implementation**, not Miniz or
Cosmeron: one `ZIP_STORED` and one `ZIP_DEFLATED`. Their single entry
`payload.bin` contains four repeated text lines plus eight binary bytes
(including NUL, 0x80 and 0xff). The expected payload is emitted independently
by the same fixture file and checked byte-for-byte using `cmp`.

The fixture builder compiles with strict C11 and needs no external ZIP tools
in the BSD VM. It does not use the Cosmeron compression implementation.

Run `sh .github/scripts/test-bsd-interop.sh` from the repository root. The
script runs the real compression library's `zipread` path on both fixtures,
and also runs system gzip and independent ZIP decoders. If the system has
`zip` or Perl's `IO::Compress::Zip`, a separately generated ZIP is tested
in addition; absence of that optional tool is still recorded explicitly.

**Limitation:** these two fixtures cover ZIP store and deflate only, not
AES encryption, ZIP64, data descriptors, multipart archives or all possible
external ZIP producers. CI reports `ZIP_IMPORT_FIXTURE=PASS` separately from
`ZIP_IMPORT=PASS_FIXTURE_AND_NATIVE`.
