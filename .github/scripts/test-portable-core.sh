#!/bin/sh
# Portable C11 regression suite. .github/tests.txt is the only suite inventory.
# PORTABLE_RUN=0 builds and links without executing (cross-compilation).
# PORTABLE_CFLAGS adds compiler flags without changing the selected suites.
set -eu
set -f
CC=${CC:-cc}
PORTABLE_RUN=${PORTABLE_RUN:-1}
PORTABLE_CFLAGS=${PORTABLE_CFLAGS:-}
export CC
MANIFEST=.github/tests.txt
[ -f "$MANIFEST" ] || { echo "[portable] missing manifest: $MANIFEST" >&2; exit 2; }
mkdir -p .build-portable
printf '[portable] %s %s | compiler: %s | execute: %s\n' \
  "$(uname -s)" "$(uname -m)" "$CC" "$PORTABLE_RUN"

execute() {
  if [ "$PORTABLE_RUN" = 1 ]; then
    "$@"
  else
    echo "[portable] built, not executed: $1"
  fi
}

run_suite() {
  module=$1
  test_name=$2
  source="Codespace/Tests/$module/$test_name.c"
  if [ ! -f "$source" ]; then
    echo "[portable] manifest source does not exist: $source" >&2
    exit 2
  fi
  echo "[portable] $module/$test_name"
  (
    cd "Codespace/Tests/$module"
    # PORTABLE_CFLAGS is intentionally word-split into compiler arguments.
    # shellcheck disable=SC2086
    $CC -std=c11 -Wall -Wextra -Wpedantic -Werror \
      -D_POSIX_C_SOURCE=200809L $PORTABLE_CFLAGS "$test_name.c" \
      -o "../../../.build-portable/${module}_${test_name}"
    execute "../../../.build-portable/${module}_${test_name}"
  )
}

while IFS= read -r line || [ -n "$line" ]; do
  line=$(printf '%s' "$line" | tr -d '\r')
  # The manifest is whitespace-delimited; disable glob expansion above.
  set -- $line
  [ "$#" -eq 0 ] && continue
  case "$1" in
    \#*) continue ;;
  esac
  if [ "$#" -ne 2 ]; then
    echo "[portable] invalid manifest entry (expected exactly 2 columns): $line" >&2
    exit 2
  fi
  run_suite "$1" "$2"
done < "$MANIFEST"

if [ "$PORTABLE_RUN" = 1 ]; then
  echo "[portable] all selected C11 suites passed"
else
  echo "[portable] all selected C11 suites compiled and linked (not executed)"
fi
