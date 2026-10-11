#!/bin/sh
# Real X11 presentation, event translation and the full GUI example.
set -eu
CC=${CC:-cc}
mkdir -p .build-gui-native
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L \
  Codespace/Tests/GUI/WindowNative.c -o .build-gui-native/WindowNative
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -D_POSIX_C_SOURCE=200809L \
  Codespace/Examples/GUI/Lab.c -o .build-gui-native/Lab
xvfb-run -a sh -c '
  set -eu
  COSMERON_WINDOW_NATIVE_TEST=1 .build-gui-native/WindowNative
  .build-gui-native/Lab --frames 3
'
