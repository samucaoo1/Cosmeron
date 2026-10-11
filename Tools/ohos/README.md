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


## OS-backed compile-only probes (SDK)

The `COSMERON_OHOS_SYSTEM_PROBES` CMake option defaults to `ON`.
It compiles the existing Random, Chronometry, File, Concurrency, Network,
Input and Terminal IncludeAll tests with SDK Clang in **strict C11**.
These are object-only checks: successful compilation does not demonstrate
native socket operation, audio output, HID access or thread linkage.

Disable this supplementary target temporarily while investigating SDK
support with `OHOS_SYSTEM_PROBES=OFF`; the baseline portable tests and
multi-translation-unit link will still execute.

The public preprocessor now exposes `OS_OHOS` when the SDK defines
`__OHOS__`. On the standard Linux-kernel OHOS profile `OS_LINUX` may
also be true; `OS_OHOS` identifies the ABI without claiming every Linux
kernel/device API is available. This macro does not prove HarmonyOS NEXT.

## Integrated POSIX link probe

`cosmeron_ohos_posix_link` (three translation units) now calls
Cosmeron's `CLOCK_FUNC(Now)` and `NETWORK_RESOLVE_FUNC(Address)`
for the standard OHOS libc clock and loopback resolver interfaces.
The executable must **link without user-provided external libraries**.
The tool does **not execute** it on the GitHub runner. A linked binary
is stronger evidence than compiling headers alone, but remains short
of actual device compatibility.
