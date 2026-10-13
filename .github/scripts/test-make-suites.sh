#!/bin/sh
# Run every make-based module suite with one compiler profile.
#
# Shared by the native POSIX runners (Ubuntu, macOS), the 32-bit profile and
# the sanitizer profile so the module list has a single source of truth.
#
#   CC            compiler (default: cc)
#   SUITE_TAG     build-directory suffix for suites with BUILD= (default: $CC)
#   SUITE_CFLAGS  replaces each Makefile's CFLAGS when non-empty
#   SUITE_SKIP    space-separated module names to skip, reported explicitly
#
# Every module runs even if a previous one failed; the script fails at the end
# and lists all failing modules, so one regression does not hide another.
set -u
CC=${CC:-cc}
SUITE_TAG=${SUITE_TAG:-$CC}
SUITE_CFLAGS=${SUITE_CFLAGS:-}
SUITE_SKIP=${SUITE_SKIP:-}
failed=
skipped=

group_begin() {
  if [ "${GITHUB_ACTIONS:-}" = true ]; then echo "::group::$1"; else echo "== $1"; fi
}

group_end() {
  if [ "${GITHUB_ACTIONS:-}" = true ]; then echo "::endgroup::"; fi
}

suite() {
  module=$1
  shift
  case " $SUITE_SKIP " in
    *" $module "*)
      echo "[make-suites] skip $module (SUITE_SKIP)"
      skipped="$skipped $module"
      return 0
      ;;
  esac
  group_begin "$module ($CC)"
  if [ -n "$SUITE_CFLAGS" ]; then
    make -C "Codespace/Tests/$module" clean "$@" CC="$CC" CFLAGS="$SUITE_CFLAGS"
  else
    make -C "Codespace/Tests/$module" clean "$@" CC="$CC"
  fi
  status=$?
  group_end
  if [ "$status" -ne 0 ]; then
    echo "::error title=$module::make suite failed with $CC (exit $status)"
    failed="$failed $module"
  fi
}

suite CLI run
suite Parsing run
suite Compression run interop
suite Database run
suite File run
suite Input run
suite Terminal run
suite GUI run
suite Audio run
suite Core run
suite Bit run
suite Chronometry run
suite Container strict multi-tu
suite Math run
suite Random run
suite Struct run
suite Text run BUILD="build-$SUITE_TAG"
suite Type run
suite Concurrency run BUILD="build-$SUITE_TAG"
suite Network strict

if [ -n "$skipped" ]; then
  echo "[make-suites] skipped by profile:$skipped"
fi
if [ -n "$failed" ]; then
  echo "[make-suites] FAILED modules:$failed" >&2
  exit 1
fi
echo "[make-suites] all module suites passed with $CC"
