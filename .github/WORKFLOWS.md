# Cosmeron GitHub Actions Architecture

The continuous integration architecture consists of exactly five modular workflows:

| # | Workflow | Purpose | Trigger | Required Gate |
|---|---|---|---|---|
| 01 | [Simple Test](workflows/01-simple-test.yml) | Fast feedback: strict portable C11 suite (GCC & Clang), standalone public headers, doc examples, tooling tests and actionlint | Every push & PR | `Simple Test / Gate` |
| 02 | [Security](workflows/02-security.yml) | CodeQL (C/C++, Python, Actions), ASan + UBSan across all modules, TSan (Concurrency), decoder sanitizers (Compression, FLAC, MP3), stream fuzzing, and public-repository privacy scan | Push to main, PR to main, weekly, manual | `Security / Gate` |
| 03 | [By Operating System](workflows/03-by-operating-system.yml) | Linux base distros (Debian, Ubuntu, Fedora, Arch, Void glibc/musl, Alpine, openSUSE, Gentoo, 32-bit, Nix + NixOS VM), BSD family (FreeBSD, OpenBSD, NetBSD, DragonFly BSD), Windows 10 & 11 (MinGW + MSVC), macOS (Apple Silicon + Intel) | Push to main/feature branches, PR to main, weekly, manual | `By Operating System / Gate` |
| 04 | [Experimental](workflows/04-experimental.yml) | Feasibility research & diagnostic probes: OpenHarmony (OHOS SDK 6.0 ARM64), HarmonyOS NEXT (HOS), ChromeOS (Crostini proxy), OpenIndiana (Illumos VM), Oracle Solaris VM, Android NDK (aarch64), iOS (arm64) and audio parity | Push, PR, weekly, manual (non-blocking) | `Experimental / Summary` (informational) |
| 05 | [Cosmeron](workflows/05-cosmeron.yml) | Entropy calculation ($H$), badge generation (C11, platforms, security, dependencies, entropy) and automated publishing via PR squash-merge | Push to main touching code/entropy/badges | N/A (automation) |

## Reading Workflow Results

- **01 (Simple Test):** Verifies the portable C11 core, header standalone compilability and repository tooling in minutes on a standard Ubuntu host.
- **02 (Security):** Enforces static analysis, memory safety, concurrency contention safety and fuzzing invariants. Also runs `.github/scripts/check-privacy.py` to ensure local developer paths are never committed.
- **03 (By Operating System):** Each Linux distribution runs its native container image (its own libc and toolchain). BSDs boot native VM guests via `vmactions`. Windows exercises both MSVC (`cl.exe`) and MinGW (`gcc.exe`). macOS tests both ARM64 and Intel architectures.
- **04 (Experimental):** Results are non-blocking (`continue-on-error: true` and non-strict gate). They provide diagnostics and tracking for emerging or niche platform targets.
- **05 (Cosmeron):** Generates official SVG badges in `.github/badges/` and updates `.github/entropy/state.json`.

## Branch Protection

Configure the following status checks as required in repository settings:
1. `Simple Test / Gate`
2. `Security / Gate`
3. `By Operating System / Gate`

The old consolidated checks (`CI / Gate`, `Compatibility / Summary`) are replaced by these three gates.
