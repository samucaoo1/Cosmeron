#!/bin/sh
# Run INSIDE a real Chromebook's Linux development environment (Crostini).
set -eu
CC=${CC:-cc}
for command in "$CC" make python3; do
  if ! command -v "$command" >/dev/null 2>&1; then
    echo "Missing $command: install build-essential, make and python3 in Crostini" >&2
    exit 2
  fi
done
report=${COSMERON_CROSTINI_REPORT:-crostini-report.txt}
{
  echo "Cosmeron Crostini hardware environment"
  date -u
  uname -a
  cat /etc/os-release
  echo "Device paths (presence does not imply access or permissions):"
  for path in /dev/input /dev/snd /dev/bus/usb /dev/dri; do
    if [ -e "$path" ]; then ls -ld "$path"; else echo "$path: not exposed"; fi
  done
  echo "DISPLAY=${DISPLAY:-unset} WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-unset}"
} > "$report"
sh .github/scripts/test-portable-core.sh
sh .github/scripts/test-bsd-native.sh
sh .github/scripts/test-input-posix.sh
make -C Codespace/Tests/Network clean strict CC="$CC"
make -C Codespace/Tests/Audio clean run CC="$CC"
make -C Codespace/Tests/Terminal clean run CC="$CC"
echo "PASS: portable / native / network / audio-offline / terminal" >> "$report"
echo "Crostini test complete: report=$report; hardware permissions are NOT certified"
