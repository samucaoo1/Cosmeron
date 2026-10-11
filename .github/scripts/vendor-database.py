#!/usr/bin/env python3
"""Import the pinned SQLite 3.53.4 amalgamation mirrored by go-sqlite3.

Usage: python3 .github/scripts/vendor-database.py /path/to/go-sqlite3
The upstream public-domain notices, strings and OS symbol names are preserved.
"""
import hashlib
import re
import sys
from pathlib import Path

root = Path(sys.argv[1])
target = Path("Codespace/Cosmeron/Modules/Database/SQLite/Internal")
target.mkdir(parents=True, exist_ok=True)
expected = {
    "sqlite3-binding.c": "eb023455154c8da14a9920dbe44f4f9e732871ef8d8a058cafb84d18d6a2de00",
    "sqlite3-binding.h": "4e7d1523cf95991f7e4c08c576e2232e063da6f10067e5c03c9bf9f904b1cf5f",
}
tokens = re.compile(r'/\*[\s\S]*?\*/|//[^\n]*|"(?:\\[\s\S]|[^"\\\n])*"|\'(?:\\[\s\S]|[^\'\\\n])*\'|\b[A-Za-z_][A-Za-z_0-9]*\b')

def replace(match):
    token = match.group()
    if re.match(r"(?:sqlite3|SQLITE|_SQLITE|fts5|Fts5|FTS5|_FTS5)", token):
        return "cosmeron_database_vendor_" + token
    return token

for name, digest in expected.items():
    source = (root / name).read_bytes()
    if hashlib.sha256(source).hexdigest() != digest:
        raise SystemExit(f"Unexpected SQLite source: {name}")
    text = source.decode("utf-8")
    assert text.startswith("#ifndef USE_LIBSQLITE3\n")
    text = text.split("\n", 1)[1].split("#else // USE_LIBSQLITE3", 1)[0]
    text = text.replace("**    \n", "**\n")  # Empty upstream version-header line.
    text = tokens.sub(replace, text)
    # Imported implementation diagnostics are isolated from authored adapters.
    text = '#if defined(__GNUC__) || defined(__clang__)\n#pragma GCC system_header\n#endif\n' + text
    out = target / ("SQLite.inc" if name.endswith(".c") else "Declarations.inc")
    out.write_text(text, encoding="utf-8")
    print(out)
