#!/bin/sh
# Run INSIDE a real Chromebook Linux development environment (Crostini).
set -eu
CC=${CC:-cc}
if ! command -v "$CC" >/dev/null 2>&1; then
  echo "Missing compiler '$CC': install build-essential in Crostini" >&2
  exit 2
fi
report=${COSMERON_CROSTINI_REPORT:-crostini-report.txt}
{
  echo "Cosmeron Crostini environment"
  date -u
  uname -a
  cat /etc/os-release
  echo "Device paths (presence does not imply access or permissions):"
  for path in /dev/input /dev/snd /dev/bus/usb /dev/dri; do
    if [ -e "$path" ]; then ls -ld "$path"; else echo "$path: not exposed"; fi
  done
  echo "DISPLAY=${DISPLAY:-unset} WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-unset}"
} > "$report"
CC="$CC" sh .github/scripts/test-portable-core.sh
echo "PASS: portable C11 manifest suites" >> "$report"
echo "Crostini test complete: report=$report; hardware permissions are NOT certified"
