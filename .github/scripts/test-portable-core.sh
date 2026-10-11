#!/bin/sh
# Strict portable C11 smoke and regression suite.
# Runs on native BSD VMs, GNU/Linux containers, and a Nix-provided toolchain.
set -eu

CC=${CC:-cc}
export CC

echo "[portable] OS: $(uname -s) / $(uname -m)"
echo "[portable] compiler: $CC"
"$CC" --version 2>/dev/null | head -n 1 || true

# Each Makefile executes its test binaries, not merely compiles them.
# The subset below intentionally avoids OS-specific device and GUI services.
for module in Core Bit Struct Type Math Random Chronometry; do
  echo "[portable] testing $module"
  make -C "Codespace/Tests/$module" clean run CC="$CC"
done

echo "[portable] testing Container (strict, multiple translation units)"
make -C Codespace/Tests/Container clean strict multi-tu CC="$CC"

echo "[portable] testing Text"
make -C Codespace/Tests/Text clean run CC="$CC"

# Sort does not have a standalone Makefile; run both catalog suites explicitly.
echo "[portable] testing Sort"
mkdir -p .build-portable
for src in Sort SortCatalog; do
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -I Codespace \
    "Codespace/Tests/$src.c" -o ".build-portable/$src"
  ".build-portable/$src"
done
echo "[portable] core suites passed"
