# OpenHarmony and Huawei HarmonyOS NEXT

The OpenHarmony SDK native toolchain is available for Linux and can
cross-compile the portable C11 test manifest. CI uses the public SDK 6.0
native component through the pinned third-party action
`openharmony-rs/setup-ohos-sdk@eb82b94ef522b07269679195c2512f22e922ef3b`.

## Repeat the SDK check

```sh
OHOS_SDK=/path/to/ohos-sdk/native OHOS_ARCH=arm64-v8a \
  sh .github/tools/ohos/check-portability.sh
```

The CMake project reads `.github/tests.txt` directly, compiles every
selected test source with SDK Clang in strict C11 mode, and links a
three-translation-unit C11 probe. This is compile/link evidence only:
the generated test objects are not executed on the GitHub runner.

## Device verification still required

- Run tests on an OpenHarmony standard-system device/emulator; record model,
  firmware/API level, target architecture and actual results.
- Verify OS-backed modules against the SDK and device POSIX/permission model.
- Check LiteOS-derived minimal systems separately from standard OHOS.
- Huawei HarmonyOS NEXT is a separate SDK/ArkTS/native-bridge and permissions
  target. Neither Android nor OpenHarmony builds certify it automatically.

The public preprocessor exposes `OS_OHOS` when the SDK defines
`__OHOS__`. On the standard Linux-kernel OHOS profile `OS_LINUX` may
also be true; `OS_OHOS` identifies the ABI without claiming every Linux
kernel/device API is available. This macro does not prove HarmonyOS NEXT.

## Run linked probes on a real OpenHarmony target

After cross-building, a compatible device/emulator and `hdc` are required
for runtime evidence. A successful cross-build does not certify sockets,
audio, HID, threads, device permissions, LiteOS or HarmonyOS NEXT.
