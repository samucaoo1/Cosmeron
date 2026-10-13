# Cosmeron — portability and CI matrix

_Updated: 2026-10-11. C11, header-oriented, no mandatory user linker flags._

This document separates **tested** from **plausible**. A successful cross-compile is
not a successful runtime/device test. All results must be checked in GitHub Actions;
a workflow existing does not mean it has passed.

## Automated verification

| Target | Environment | Workflow | Scope |
| --- | --- | --- | --- |
| Linux GCC/Clang | Native GitHub runners | [By Operating System](../../.github/workflows/03-by-operating-system.yml) / [Simple Test](../../.github/workflows/01-simple-test.yml) | Existing module integration suites |
| Windows MinGW/MSVC (10 & 11) | Native GitHub runners | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Existing module integration suites |
| macOS Clang (Apple Silicon & Intel) | Native GitHub runners | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Existing module integration suites |
| FreeBSD | Native FreeBSD guest VM via vmactions | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Portable + extended native suites; per-module reports distinguish zero-link profiles |
| OpenBSD / NetBSD / DragonFly BSD | Native guest VMs via vmactions | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Portable + extended native suites; per-module reports distinguish zero-link profiles |
| Fedora / Arch / Debian / Ubuntu / openSUSE / Gentoo / Alpine | Native Linux userspace containers | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Portable + native suites across diverse libc implementations |
| Void glibc / Void musl | Native Linux userspace containers | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Portable suites; glibc and musl coverage |
| Nix toolchain / NixOS VM | Pinned Nix derivation on Ubuntu and real NixOS QEMU guest | [By Operating System](../../.github/workflows/03-by-operating-system.yml) | Strict portable C11 passed as package and inside NixOS VM ([run](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787)); not full hardware certification |
| ChromeOS Crostini proxy | Debian container on Ubuntu | [Experimental](../../.github/workflows/04-experimental.yml) | Portable and native suites inside Crostini container approximation; **not** a real Chromebook |

The portable smoke suite is [test-portable-core.sh](../../.github/scripts/test-portable-core.sh).
It compiles and executes the repository's existing test sources, with
`-std=c11 -Wall -Wextra -Wpedantic -Werror` and no explicit extra link libraries.
The [BSD extended runner](../../.github/scripts/test-bsd-extended.sh)
executes Network, Concurrency, SQLite, Compression, Audio, Input and
noninteractive Terminal/Canvas. It emits a **per-module CI job summary**
distinguishing compilation, executed runtime, zero-extra-link and device scope.

The embedded SQLite uses a **single-threaded mutex-free** implementation by
default on BSD and passed zero-extra-link tests on all four BSD guests
([SQLite design](SQLITE_LINKING.md)). It must not be accessed concurrently.

Audio and Concurrency have separate, pure zero-link subsets:
[Audio/Offline.h](../../Codespace/Cosmeron/Modules/Audio/Offline.h) provides
PCM, synthesis, sequences and beep rendering without device I/O or OS threads;
[Concurrency/Cooperative.h](../../Codespace/Cosmeron/Modules/Concurrency/Cooperative.h)
provides atomics, coroutines and direct tasks **without native threading**.
These are not equivalent to the desktop Audio/Device or pthread ThreadPool
backends. On some BSDs the native threaded and audio-device backends still
require `-pthread` and/or `-lm`, so their functional compatibility tests
are explicitly **not** zero-link certifications. Physical devices and
interactive TTYs remain untested.

## Additional platform feasibility

### NixOS

**Verified for the selected portable C11 test subset on a real NixOS VM.**
`flake.nix` and `flake.lock` pin Nixpkgs, expose a derivation/check and boot a
QEMU NixOS guest which executes the packaged tests. Both the pinned package
and the real NixOS VM [passed CI](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787).
OS-specific hardware, permissions and drivers remain unverified.

### ChromeOS

**Feasible through Crostini, with restrictions.** Google's Linux development
environment exposes Debian inside a managed VM/container. The portable modules
should behave like their Linux counterparts. The Debian CI job approximates its
userspace but does *not* exercise the ChromeOS kernel/device permissions.

A real Chromebook verification script exists at [Tools/crostini/check.sh](../../Tools/crostini/check.sh);
it has not been executed on a Chromebook. Native ChromeOS apps are a different
target; terminal access to HID, evdev,
joysticks, USB and GUI/input capture may be mediated or blocked. Validate Input,
Network, Audio and Terminal on real Chromebook hardware. Consider Android-on-
ChromeOS builds separately; the Crostini job is not an Android NDK test.

Official references:
- https://developers.google.com/chromeos/app-development/develop
- https://developers.google.com/chromeos/app-development/develop/setup
- https://www.chromium.org/chromium-os/developer-library/guides/containers/containers-and-vms/

### OpenHarmony

**Native SDK 6.0 ARM64 C11 cross-build and multi-translation-unit link passed CI**:
[actual OpenHarmony SDK run](https://github.com/samucaoo1/Cosmeron/actions/runs/38141169787).
Target macros and triple are preserved in the workflow artifacts.
The [Tools/ohos](../../Tools/ohos/README.md) compiler proof does **not**
execute the code. OpenHarmony runtime, OS-backed APIs, LiteOS devices, Huawei
HarmonyOS NEXT and hardware permissions remain unverified.

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
3. **Additional coverage:** libc variants, real Chromebook, OpenHarmony device,
   full BSD zero-link backends and hardware runtime tests. NixOS VM and OHOS
   ARM64 cross-compile are already implemented and have passing runs.
4. **Per-module certification:** publish results and backend limitations.
   Distinguish compiled, unit-tested, integrated, and hardware-tested states.

This is an educational library: deliberately unusual algorithms/modules remain,
but build checks must not pass solely because tests have been silently skipped.
