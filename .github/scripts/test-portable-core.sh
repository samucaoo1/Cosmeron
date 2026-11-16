#!/bin/sh
# Portable C11 regression suite: cc + POSIX shell only. No GNU make required.
# Deliberately uses the real module tests, not fabricated compile-only probes.
# PORTABLE_RUN=0 builds and links every suite without executing it; used by
# cross-compilation targets (Android, iOS) whose binaries run elsewhere.
set -eu
CC=${CC:-cc}
PORTABLE_RUN=${PORTABLE_RUN:-1}
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
  shift
  for test in "$@"; do
    echo "[portable] $module/$test"
    (
      cd "Codespace/Tests/$module"
      $CC -std=c11 -Wall -Wextra -Wpedantic -Werror \
        -D_POSIX_C_SOURCE=200809L "$test.c" \
        -o "../../../.build-portable/${module}_${test}"
      execute "../../../.build-portable/${module}_${test}"
    )
  done
}

run_suite Core Namespace Allocator Bit PackageNoFunctionTable
run_suite Container Array DynamicBitset IncludeAll Iterator Linked NoFunctionTable Tree
run_suite Tuple Tuple
if [ "$PORTABLE_RUN" = 1 ]; then
  echo "[portable] all selected C11 suites passed"
else
  echo "[portable] all selected C11 suites compiled and linked (not executed)"
fi
