#!/bin/sh
# Portable C11 regression suite. The manifest is the only test-suite inventory.
# PORTABLE_RUN=0 builds and links without executing (cross-compilation).
# PORTABLE_CFLAGS adds compiler flags without changing the selected suites.
set -eu
CC=${CC:-cc}
PORTABLE_RUN=${PORTABLE_RUN:-1}
PORTABLE_CFLAGS=${PORTABLE_CFLAGS:-}
export CC
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
  test=$2
  echo "[portable] $module/$test"
  (
    cd "Codespace/Tests/$module"
    # PORTABLE_CFLAGS is intentionally word-split into compiler arguments.
    # shellcheck disable=SC2086
    $CC -std=c11 -Wall -Wextra -Wpedantic -Werror \
      -D_POSIX_C_SOURCE=200809L $PORTABLE_CFLAGS "$test.c" \
      -o "../../../.build-portable/${module}_${test}"
    execute "../../../.build-portable/${module}_${test}"
  )
}

while IFS=' ' read -r module test || [ -n "${module:-}" ]; do
  test=$(printf '%s' "${test:-}" | tr -d '\r')
  case "${module:-}" in
    ""|\#*) continue ;;
  esac
  if [ -z "$test" ]; then
    echo "[portable] invalid manifest entry: $module" >&2
    exit 2
  fi
  run_suite "$module" "$test"
done < .github/tests.txt

if [ "$PORTABLE_RUN" = 1 ]; then
  echo "[portable] all selected C11 suites passed"
else
  echo "[portable] all selected C11 suites compiled and linked (not executed)"
fi
