#!/bin/sh
# Linux distribution userspace check, executed inside the distribution's own
# container image (its libc, headers and compiler), not the Ubuntu host.
# Portable C11 baseline, real codec decoding under the native libc, POSIX
# filesystem/socket/atomic suites and the Input backends.
set -eu
CC=${CC:-cc}
export CC
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  . /etc/os-release
  echo "[linux-distro] ${PRETTY_NAME:-unknown} | $(uname -m) | $("$CC" --version | head -n 1)"
fi
sh .github/scripts/test-portable-core.sh
sh .github/scripts/test-linux-codec-minimal.sh
sh .github/scripts/test-bsd-native.sh
sh .github/scripts/test-input-posix.sh
echo "[linux-distro] all suites passed"
