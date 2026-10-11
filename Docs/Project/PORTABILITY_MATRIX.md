# Cosmeron — portability and CI matrix

_Updated: 2026-10-11. C11, header-oriented, no mandatory user linker flags._

This document separates **tested** from **plausible**. A successful cross-compile is
not a successful runtime/device test. All results must be checked in GitHub Actions;
a workflow existing does not mean it has passed.

## Automated verification

| Target | Environment | Workflow | Scope |
| --- | --- | --- | --- |
| Linux GCC/Clang | Native GitHub runners | [CI](../../.github/workflows/01-ci.yml) | Existing module integration suites |
| Windows MinGW/MSVC | Native GitHub runners | [CI](../../.github/workflows/01-ci.yml) | Existing module integration suites |
| macOS Clang | Native GitHub runner | [CI](../../.github/workflows/01-ci.yml) | Existing module integration suites |
| FreeBSD | Native FreeBSD guest VM via vmactions | [Continuous Integration](../../.github/workflows/01-ci.yml) | Portable C11, OS-native smoke and Input suites |
| OpenBSD / NetBSD / DragonFly BSD | Native guest VMs via vmactions | [Platform Compatibility](../../.github/workflows/02-platforms.yml) | Portable C11, OS-native smoke and Input suites |
| Fedora / Arch | Native Linux userspace containers | [Platform Compatibility](../../.github/workflows/02-platforms.yml) | Same ten portable suites |
| Void glibc / Void musl | Native Linux userspace containers | [Platform Compatibility](../../.github/workflows/02-platforms.yml) | Same ten portable suites; libc coverage |
| Nix toolchain | Nix-provided compiler on Ubuntu | [Platform Compatibility](../../.github/workflows/02-platforms.yml) | Same ten portable suites; **not** native NixOS |
| ChromeOS Crostini proxy | Debian container on Ubuntu | [Platform Compatibility](../../.github/workflows/02-platforms.yml) | Same ten suites; **not** a real Chromebook |

The portable smoke suite is [test-portable-core.sh](../../.github/scripts/test-portable-core.sh).
It compiles and executes the repository's existing test sources, with
`-std=c11 -Wall -Wextra -Wpedantic -Werror` and no explicit extra link libraries.
Network, OS-backed concurrency, terminal, file I/O, audio devices, full compression
and database paths require additional integration and/or target-specific tests.

## Additional platform feasibility

### NixOS

**Feasible, not yet natively certified.** Since Cosmeron is source-level C11, the
normal Nixpkgs GCC/Clang toolchains should be usable for its portable modules.
The Nix-toolchain job tests development-environment reproducibility, not NixOS's
kernel integration, packaging, sandboxing or system layout.

Next step: ship a `flake.nix` devShell plus derivation/check; then run a true NixOS VM
or self-hosted NixOS runner for OS-specific modules. Pin Nixpkgs for reproducibility.

### ChromeOS

**Feasible through Crostini, with restrictions.** Google's Linux development
environment exposes Debian inside a managed VM/container. The portable modules
should behave like their Linux counterparts. The Debian CI job approximates its
userspace but does *not* exercise the ChromeOS kernel/device permissions.

Native ChromeOS apps are a different target; terminal access to HID, evdev,
joysticks, USB and GUI/input capture may be mediated or blocked. Validate Input,
Network, Audio and Terminal on real Chromebook hardware. Consider Android-on-
ChromeOS builds separately; the Crostini job is not an Android NDK test.

Official references:
- https://developers.google.com/chromeos/app-development/develop
- https://developers.google.com/chromeos/app-development/develop/setup
- https://www.chromium.org/chromium-os/developer-library/guides/containers/containers-and-vms/

### OpenHarmony

**Technically feasible for the pure C11 subset; no CI or device certification yet.**
OpenHarmony offers a native C/C++ SDK with Clang, a sysroot and an
`ohos.toolchain.cmake`. Compile-time verification needs the specific target
architecture and SDK version (e.g. ARM64/standard system); running tests requires
an OpenHarmony VM, emulator, development board or device.

Sequence:
1. Acquire and pin a trusted OpenHarmony native SDK/toolchain.
2. Check all public headers using target Clang + OHOS sysroot; explicitly track
   `__OHOS__` and other predefined target macros from the **actual** compiler.
3. Start with Core/Bit/Struct/Type/Math/Sort and pure memory/containers.
4. Add POSIX compatibility probes for Chronometry, Random, File, Concurrency,
   Network, and Terminal; write platform backends only when needed.
5. Validate multi-translation-unit header-only inclusion and zero-extra-link
   contract, then execute on a representative OHOS standard-system target.
6. Treat LiteOS-based minimal products as a distinct profile from standard
   OpenHarmony; a successful ARM64 userspace build does not certify tiny MCUs.

References:
- https://github.com/openharmony/docs/blob/master/en/device-dev/subsystems/subsys-build-all.md
- https://gitee.com/openharmony/build/wikis/pages/export?doc_id=3015289&type=pdf

### Huawei HarmonyOS / HarmonyOS NEXT

**Separate platform certification; not a synonym for OpenHarmony.** Huawei
documents HarmonyOS application development through DevEco Studio/HarmonyOS SDK,
ArkTS/ArkUI and native facilities. Reusable C algorithms can be embedded via
a native bridge if supported by the target SDK/runtime. Device/permission
APIs, app packaging, and compatibility with particular HarmonyOS releases
require validation in Huawei's toolchain.

Do not treat Linux macros, POSIX availability, Android ABI compatibility, or
an OpenHarmony cross-compile as proof of HarmonyOS NEXT support. Prefer a small
ArkTS/native bridge example and device tests once a supported SDK is available.

Official developer portal:
- https://developer.huawei.com/consumer/en/harmonyos/develop/

## Staged rollout

1. **Required:** ten portable C11 suites on the four BSD guests; Fedora/Arch/Void
   glibc/Void musl; Nix-provided compiler; Debian Crostini proxy.
2. **Broader BSD integration:** CLI, Parsing, Compression, Database, File, Audio,
   Terminal/Canvas, Concurrency, Network; diagnose failures by actual BSD
   headers, syscalls and linker behavior, not by disabling warnings.
3. **Additional coverage:** libc variants and ARM64 runners; real NixOS VM,
   real Chromebook, OHOS SDK cross-compile, hardware runtime tests.
4. **Per-module certification:** publish results and backend limitations.
   Distinguish compiled, unit-tested, integrated, and hardware-tested states.

This is an educational library: deliberately unusual algorithms/modules remain,
but build checks must not pass solely because tests have been silently skipped.
