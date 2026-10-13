#!/bin/sh
# Probe actual device nodes INSIDE a real Chromebook's Crostini environment.
# Do not run this on generic Ubuntu/Debian CI as ChromeOS evidence.
set -eu
report=${COSMERON_CROSTINI_HARDWARE_REPORT:-crostini-hardware-report.txt}
if [ ! -e /dev/.cros_milestone ] && [ ! -e /dev/vm_tools ] && [ ! -e /mnt/chromeos ]; then
  echo "No Crostini marker found; refusing to label generic Linux as ChromeOS." >&2
  echo "If your ChromeOS version uses different markers, document them before changing this check." >&2
  exit 2
fi
{
  echo "Cosmeron ChromeOS Crostini hardware exposure report"
  date -u
  uname -a
  if [ -r /etc/os-release ]; then cat /etc/os-release; fi
  echo "Crostini marker found; device presence is NOT functionality certification."
  for node in /dev/input /dev/snd /dev/bus/usb /dev/dri /dev/tty; do
    if [ -e "$node" ]; then
      ls -ld "$node"
      if [ -r "$node" ]; then echo "$node: readable"; else echo "$node: not readable"; fi
      if [ -w "$node" ]; then echo "$node: writable"; else echo "$node: not writable"; fi
    else
      echo "$node: not exposed"
    fi
  done
  echo "DISPLAY=${DISPLAY:-unset}"
  echo "WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-unset}"
  echo "PULSE_SERVER=${PULSE_SERVER:-unset}"
  echo "RUNTIME_TESTS=NOT_RUN"
  echo "USB_HID_JOYSTICK=UNVERIFIED"
  echo "AUDIO_OUTPUT=UNVERIFIED"
  echo "GPU_RENDERING=UNVERIFIED"
} > "$report"
echo "Hardware exposure report: $report"
