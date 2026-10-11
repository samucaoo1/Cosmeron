# OpenHarmony and Huawei HarmonyOS NEXT

The OpenHarmony SDK native toolchain is available for Linux and can
cross-compile standard-system C11 code. CI uses the public SDK 6.0 native
component through the pinned (third-party) action
`openharmony-rs/setup-ohos-sdk@eb82b94ef522b07269679195c2512f22e922ef3b`
which checks the SDK archive against the upstream SHA-256 value.

## Repeat the SDK check
```sh
OHOS_SDK=/path/to/ohos-sdk/native OHOS_ARCH=arm64-v8a \
  sh Tools/ohos/check-portability.sh
```

Objects from C11 public headers are cross-compiled with `-Werror`, and
a three-translation-unit example is **linked without user-supplied -lm,
-pthread or -lws2_32**. The target triple and actual compiler predefined
macros are recorded in `.build-ohos/`. This does not execute anything.

## Device verification still required
- Run tests on an OpenHarmony standard-system device/emulator; record model,
  firmware/API level, target architecture and actual results.
- Verify Random, Chronometry, File, Network, Concurrency, Terminal and Input
  separately against the SDK and device's POSIX/permission model.
- Check LiteOS-derived minimal systems separately from standard OHOS.
- Huawei HarmonyOS NEXT is a separate SDK/ArkTS/native-bridge and permissions
  target. Neither Android nor OpenHarmony builds certify it automatically.
