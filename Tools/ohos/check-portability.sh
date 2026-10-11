#!/bin/sh
# Cross-compile the portable Cosmeron C11 surface with an installed OHOS SDK.
# Does not execute binaries or claim compatibility with Huawei HarmonyOS NEXT.
set -eu
: "${OHOS_SDK:?Set OHOS_SDK to the OpenHarmony SDK directory or its native/ subdirectory}"
OHOS_ARCH=${OHOS_ARCH:-arm64-v8a}
case "$OHOS_SDK" in
  */native) sdk_native=$OHOS_SDK ;;
  *) sdk_native=$OHOS_SDK/native ;;
esac
toolchain=$sdk_native/build/cmake/ohos.toolchain.cmake
if [ ! -f "$toolchain" ]; then
  echo "OpenHarmony SDK toolchain not found: $toolchain" >&2
  exit 2
fi
build_dir=${OHOS_BUILD_DIR:-.build-ohos}
cmake -S Tools/ohos -B "$build_dir" \
  -DCMAKE_TOOLCHAIN_FILE="$toolchain" \
  -DOHOS_ARCH="$OHOS_ARCH" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build "$build_dir" --parallel 2
echo "OHOS compile-only check passed for $OHOS_ARCH; device runtime NOT tested"
