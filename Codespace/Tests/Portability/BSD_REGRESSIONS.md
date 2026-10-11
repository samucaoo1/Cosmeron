# Regression coverage for BSD-specific portability fixes

The native BSD VMs execute these existing and targeted regressions. Each
case is compiled with `-std=c11 -Wall -Wextra -Wpedantic -Werror`.

| Regression | Test | What is checked |
| --- | --- | --- |
| NetBSD feature visibility for POSIX `symlink()` | `File/File.c`, run by `test-bsd-native.sh` with NetBSD-specific header visibility | Creates, queries, resolves and removes symbolic links |
| DragonFly strict C11 socket `MSG_DONTWAIT` fallback | `Network/Poller.c` and related poller tests, run by `test-bsd-extended.sh` | Sockets/poller compile under strict C11 and correctly report events without assuming the macro exists |
| FreeBSD complex-root linker regression | `Math/BSDQuadraticRegression.c`, run in `test-portable-core.sh` on all BSDs | Links without `-lm` and verifies real and imaginary quadratic roots |

**Scope:** These test paths verify narrow regressions, not full OS or device
certification. In particular, the pthread-backed Concurrency/SQLite and
Miniaudio desktop backends may still need extra linker flags on the BSDs.
