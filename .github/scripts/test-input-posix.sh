#!/bin/sh
set -eu
cd Codespace/Tests/Input
mkdir -p .build-native
for name in State Application HID ControllerTransport BSDHID NativeControllers Namespace Linux Windows MacOS SDLControllers IncludeAll; do
  echo "Compile and run $name"
  cc -std=c11 -Wall -Wextra -Wpedantic -Werror "$name.c" -o ".build-native/$name"
  ".build-native/$name"
done
cc -std=c11 -Wall -Wextra -Wpedantic -Werror MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o .build-native/MultiTU
.build-native/MultiTU
cc -std=c11 -Wall -Wextra -Wpedantic -Werror -DINPUT_EXTERNAL_STORAGE MultiTU_A.c MultiTU_B.c MultiTU_Main.c -o .build-native/ExternalStorage
.build-native/ExternalStorage
