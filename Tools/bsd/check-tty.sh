#!/bin/sh
# Run only in an interactive BSD terminal with an actual TTY.
# No CI VM without a real terminal can satisfy this check.
set -eu
CC=${CC:-cc}
report=${COSMERON_BSD_TTY_REPORT:-bsd-tty-report.txt}
case "$(uname -s)" in FreeBSD|OpenBSD|NetBSD|DragonFly) ;; *) echo "BSD target required" >&2; exit 2 ;; esac
if [ ! -t 0 ] || [ ! -t 1 ]; then echo "Interactive input/output TTY required" >&2; exit 2; fi
mkdir -p .build-bsd-tty
{
  echo "Cosmeron BSD hardware/TTY verification"
  date -u
  uname -a
  tty
  echo "TERM=${TERM:-unset}"
  echo "Input and audio hardware are NOT tested by this terminal-only probe."
} > "$report"
# Noninteractive rendering regression remains a mandatory prerequisite.
for name in Canvas UI UIAdvanced; do
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    "Codespace/Tests/Terminal/$name.c" -o ".build-bsd-tty/$name"
  ".build-bsd-tty/$name"
  echo "Terminal/$name: PASS" >> "$report"
done
echo "TTY detected and terminal/canvas regression passed" >> "$report"
echo "TTY=PASS; keyboard/mouse/input, interactive rendering and audio device require separate manual verification"
