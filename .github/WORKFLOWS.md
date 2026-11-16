# Cosmeron GitHub Actions

The CI uses five workflows. The selected portable C test suites have one
source of truth: `.github/tests.txt`. Platform and compiler runners use
that manifest wherever the target supports execution or cross-compilation.

| # | Workflow | Responsibility | Gate |
|---|---|---|---|
| 01 | [CI](workflows/01-simple-test.yml) | GCC/Clang portable C11 suites, standalone public headers and repository tooling | `CI / Gate` |
| 02 | [Platform Compatibility](workflows/03-by-operating-system.yml) | Linux distributions, Nix/NixOS, BSD family, macOS ARM64/Intel, Windows MinGW/MSVC | `Platform Compatibility / Gate` |
| 03 | [Security](workflows/02-security.yml) | CodeQL, repository privacy, ASan and UBSan over manifest suites | `Security / Gate` |
| 04 | [Experimental](workflows/04-experimental.yml) | OpenHarmony SDK, HarmonyOS NEXT boundary probe, Crostini proxy, Android NDK, iOS SDK and Redox macro probe | Informational |
| 05 | [Maintenance](workflows/04-maintenance.yml) | LOC report, entropy calculation and badge artifacts | Informational |

## Portable test selection

`.github/tests.txt` is the single inventory of portable test suites.

- POSIX and Unix-like targets: `.github/scripts/test-portable-core.sh`
- MSVC: `.github/scripts/test-msvc.ps1`
- Nix: `flake.nix`, built by the platform workflow
- BSD extended native integration: `.github/scripts/test-bsd-extended.sh`

Do not add a second copy of the portable test inventory to workflow files.
Add a test to the manifest only after confirming that its source exists and
is in scope.

## Platform coverage

### Required platform workflow

- Linux container userspaces: Debian, Ubuntu (GCC and Clang), Fedora, Arch,
  Void glibc, Void musl, Alpine/musl, openSUSE Tumbleweed and Gentoo.
- Nix flake toolchain on x86_64-linux; real NixOS QEMU guest VM on schedule
  or explicit workflow dispatch.
- FreeBSD, OpenBSD, NetBSD and DragonFly BSD native VM guests.
- macOS Apple Silicon and Intel runners.
- Windows 2022 and latest hosted runner images, each with MinGW and MSVC.
  GitHub-hosted Windows runners are Server images, not consumer Windows
  10/11 installations; this matrix does not certify those desktop editions.

All required platform jobs use the current manifest. The NixOS VM is
schedule/opt-in and is the only permitted skipped job in the required gate.

### Experimental workflow (non-blocking)

- OpenHarmony SDK 6.0: cross-compilation and multi-translation-unit link;
  this is not device/runtime certification.
- HarmonyOS NEXT: synthetic ABI-boundary probe only; no official HOS SDK
  is provisioned, so target compatibility remains unverified.
- Crostini: Debian userspace proxy only. Actual ChromeOS/Crostini testing
  requires a physical Chromebook and the procedure in
  `.github/tools/crostini/README.md`.
- Android NDK: aarch64 cross-compilation; binaries are not executed.
- iOS: arm64 SDK cross-compilation; binaries are not executed.
- Redox: synthetic target-detection macro probe; no Redox kernel/relibc
  runtime or native toolchain is provisioned.

Experimental failures are diagnostic and never block merges. Synthetic
probes, cross-compilation, and Linux userspace proxies must not be described
as native device/OS runtime certification.

## Scope and branch protection

The workflows validate the repository's current manifest and explicit
platform probes. They do not assume every module has been refactored or
has a complete test suite. Hardware access, audio output, input devices,
and physical-device permissions require separate manual evidence.

Configure these required status checks:

1. `CI / Gate`
2. `Platform Compatibility / Gate`
3. `Security / Gate`

Experimental and Maintenance workflows are informational.
