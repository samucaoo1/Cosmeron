#!/bin/sh
set -eu
: "${OHOS_SDK:?Specify installed official OpenHarmony SDK native directory}"
OHOS_ARCH=${OHOS_ARCH:-arm64-v8a}
case "$OHOS_SDK" in */native) sdk_native=$OHOS_SDK ;; *) sdk_native=$OHOS_SDK/native ;; esac
toolchain=$sdk_native/build/cmake/ohos.toolchain.cmake
if [ ! -f "$toolchain" ]; then echo "Missing SDK toolchain: $toolchain" >&2; exit 2; fi
build_dir=${OHOS_BUILD_DIR:-.build-ohos}
cmake -S Tools/ohos -B "$build_dir" \
  -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DOHOS_ARCH="$OHOS_ARCH" -DCMAKE_BUILD_TYPE=Release \
  -DCOSMERON_OHOS_SYSTEM_PROBES="${OHOS_SYSTEM_PROBES:-ON}"
cmake --build "$build_dir" --parallel 2
if [ -x "$sdk_native/llvm/bin/clang" ]; then
  case "$OHOS_ARCH" in
    arm64-v8a) triple=aarch64-linux-ohos ;;
    armeabi-v7a) triple=arm-linux-ohos ;;
    x86_64) triple=x86_64-linux-ohos ;;
    *) echo "Unrecognized OHOS ABI: $OHOS_ARCH" >&2; exit 2 ;;
  esac
  "$sdk_native/llvm/bin/clang" --target="$triple" -dM -E -x c /dev/null \
    > "$build_dir/target-macros.txt"
  "$sdk_native/llvm/bin/clang" --target="$triple" -print-target-triple \
    > "$build_dir/target-triple.txt"
  grep -E '(__OHOS__|__aarch64__|__arm__|__linux__)' "$build_dir/target-macros.txt" || :
fi
echo "OpenHarmony C11 + multi-TU cross-build complete. Runtime and Huawei HarmonyOS NEXT UNVERIFIED"
