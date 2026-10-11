# Embedded miniz

Source: https://github.com/richgel999/miniz/tree/3.1.0
Pinned commit: `174573d60290f447c13a2b1b3405de2b96e27d6c`.
License: MIT, copied in LICENSE.txt and retained in the generated include.

Regenerate from an unmodified checkout with:

```sh
python3 Tools/vendor_miniz.py /path/to/miniz-3.1.0
```

Local adaptations are encoded in that script: amalgamation, identifier prefixes,
internal linkage, portable reads, disabled stdio/time/compatibility aliases,
valid deterministic DOS dates and cleanup of the temporary ZIP compressor on
heap-write failure. Do not edit Miniz.inc directly.
