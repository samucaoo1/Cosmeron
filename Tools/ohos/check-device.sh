#!/bin/sh
# Execute *linked* Cosmeron OpenHarmony SDK probes on a real OHOS target.
# This does not certify Huawei HarmonyOS NEXT or hardware-permission APIs.
set -eu
HDC=${HDC:-hdc}
build_dir=${OHOS_BUILD_DIR:-.build-ohos}
remote_dir=${OHOS_REMOTE_DIR:-/data/local/tmp/cosmeron}
report=${OHOS_DEVICE_REPORT:-ohos-device-report.txt}
if ! command -v "$HDC" >/dev/null 2>&1; then
  echo "OpenHarmony hdc client not found: $HDC" >&2
  exit 2
fi
for binary in cosmeron_ohos_multi_tu cosmeron_ohos_posix_link; do
  if [ ! -f "$build_dir/$binary" ]; then
    echo "Missing $build_dir/$binary; run Tools/ohos/check-portability.sh first" >&2
    exit 2
  fi
done
# Require an explicit target selection if more than one device is connected.
# HDC_TARGET is passed through hdc -t; this prevents testing the wrong device.
if [ -n "${HDC_TARGET:-}" ]; then
  hdc() { "$HDC" -t "$HDC_TARGET" "$@"; }
else
  hdc() { "$HDC" "$@"; }
fi
{
  echo "Cosmeron OpenHarmony on-device linked-probe report"
  date -u
  echo "target=${HDC_TARGET:-default}"
  echo "build_dir=$build_dir"
  echo "This is NOT a Huawei HarmonyOS NEXT certification."
} > "$report"
hdc shell "mkdir -p '$remote_dir'"
for binary in cosmeron_ohos_multi_tu cosmeron_ohos_posix_link; do
  hdc file send "$build_dir/$binary" "$remote_dir/$binary"
  # hdc propagates the shell command failure on supported SDK versions.
  # The marker independently verifies the program's own exit status.
  token="COSMERON_EXIT_"
  output=$(hdc shell "chmod 700 '$remote_dir/$binary' && '$remote_dir/$binary'; rc=\$?; echo ${token}\$rc") || {
    printf '%s\n' "$binary: HDC_TRANSPORT_FAILURE" >> "$report"
    exit 1
  }
  printf '%s\n' "$output" >> "$report"
  exit_code=$(printf "%s\n" "$output" | sed -n "s/.*${token}\([0-9][0-9]*\).*/\1/p" | tail -n 1)
  if [ "$exit_code" = 0 ]; then
    echo "$binary: PASS" >> "$report"
  else
    echo "$binary: FAIL (exit=${exit_code:-missing})" >> "$report"
    exit 1
  fi
done
echo "PASS: linked C11 multi-TU and native POSIX probe executed on OHOS target" >> "$report"
echo "Device report: $report"
