#!/bin/sh
# Native BSD integration tests. Uses only the existing C11 regression sources.
# Unlike the portable baseline, these probe libc, filesystem and socket APIs.
set -eu
CC=${CC:-cc}
# NetBSD 10 hides symlink() from POSIX.1-2008-only headers despite POSIX
# specifying it; explicitly request the NetBSD interfaces for test sources.
case "$(uname -s)" in
  NetBSD) BSD_FEATURE_CFLAGS=-D_NETBSD_SOURCE=1 ;;
  *) BSD_FEATURE_CFLAGS= ;;
esac
mkdir -p .build-bsd-native
echo "[bsd-native] $(uname -s) $(uname -m), compiler=$CC"

run_suite() {
  module=$1
  shift
  for test in "$@"; do
    echo "[bsd-native] $module/$test"
    (
      cd "Codespace/Tests/$module"
      "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
        -D_POSIX_C_SOURCE=200809L ${BSD_FEATURE_CFLAGS} "$test.c" \
        -o "../../../.build-bsd-native/${module}_${test}"
      "../../../.build-bsd-native/${module}_${test}"
    )
  done
}
run_multi_tu() {
  module=$1
  echo "[bsd-native] $module/MultiTU"
  (
    cd "Codespace/Tests/$module"
    "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
      -D_POSIX_C_SOURCE=200809L ${BSD_FEATURE_CFLAGS} MultiTU_A.c MultiTU_B.c MultiTU_Main.c \
      -o "../../../.build-bsd-native/${module}_MultiTU"
    "../../../.build-bsd-native/${module}_MultiTU"
  )
}

run_suite CLI CLI IncludeAll Namespace FunctionTableOff
run_multi_tu CLI

run_suite Parsing Common JSON INI CSV TOML YAML XML IncludeAll Namespace \
  FunctionTableOff Robustness Allocation
run_multi_tu Parsing

run_suite File IncludeAll Path File Namespace WindowsMacros
run_multi_tu File

# Test the OS-backed network headers, localhost binding and resolver on the
# native guest, not in a Linux container. Socket transport tests come next.
run_suite Network IncludeAll Smoke Resolve

# Lock-free atomics should work without -pthread/-latomic link flags.
run_suite Concurrency Atomic

# Compilation of the embedded codec itself must also be header-only.
run_suite Compression IncludeAll

echo "[bsd-native] all selected platform suites passed"
