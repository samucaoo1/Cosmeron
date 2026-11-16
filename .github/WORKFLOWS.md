# Cosmeron GitHub Actions

The CI uses four workflows. The selected C test suites have one source of
truth: `.github/tests.txt`. Every compiler and platform test runner reads
that manifest; workflow files must not maintain independent suite lists.

| # | Workflow | Responsibility | Required gate |
|---|---|---|---|
| 01 | [CI](workflows/01-simple-test.yml) | GCC/Clang portable C11 suites, standalone public headers and repository tooling | `CI / Gate` |
| 02 | [Platform Compatibility](workflows/03-by-operating-system.yml) | Linux distributions, FreeBSD, macOS, Windows MinGW and MSVC | `Platform Compatibility / Gate` |
| 03 | [Security](workflows/02-security.yml) | CodeQL, repository privacy checks, ASan and UBSan over the manifest suites | `Security / Gate` |
| 04 | [Maintenance](workflows/04-maintenance.yml) | LOC report, entropy calculation and badge artifacts | Informational |

## Test selection

Each non-specialized C test runner consumes `.github/tests.txt`:

- POSIX: `.github/scripts/test-portable-core.sh`
- MSVC: `.github/scripts/test-msvc.ps1`

Do not add module-specific test inventories to workflows or runner scripts.
To include or remove a portable test, change the manifest only after verifying
that the corresponding source exists and is in scope.

## Scope

The workflows validate the repository's current test manifest. They do not
assume that every module has been refactored or has a complete test suite.
Module-specific fuzzers, parity tests and sanitizer jobs are intentionally
not part of the general CI gate.

## Branch protection

Configure these required status checks:

1. `CI / Gate`
2. `Platform Compatibility / Gate`
3. `Security / Gate`

Maintenance artifacts are informational and do not block merges.
