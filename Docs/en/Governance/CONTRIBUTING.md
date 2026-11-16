# Contributing to Cosmeron

Contributions are welcome when they preserve Cosmeron's central goals: readable C, explicit behavior, modular architecture, portability, and code that remains useful to study.

Before changing public APIs, read the [Cosmeron Pattern](../Reference/PATTERN.md).

## Ground rules

Cosmeron targets **C11**. Public APIs should remain portable and should avoid exposing operating-system-specific vocabulary unless the API is explicitly native-facing.

Follow the established naming, namespace, ownership, status, and output-parameter conventions. Prefer existing preprocessor helpers instead of rebuilding token concatenation or namespace machinery ad hoc.

Where viable, preserve the **zero-link** philosophy: a user should not gain a new mandatory linker flag merely because a feature can be implemented without one.

## Changes

Keep changes focused. If a refactor changes an established pattern across multiple modules, document the rule first or update the Pattern in the same change.

New public behavior should include:

- tests covering success, boundary, and error behavior;
- documentation for the public API;
- examples when the intended use is not self-evident;
- consideration of GCC, Clang, MinGW, MSVC, and macOS Clang where applicable.

## Tests

The project uses strict C11 compilation as a baseline:

```text
-std=c11 -Wall -Wextra -Wpedantic -Werror
```

Module tests should remain organized by module and should exercise every public function affected by the change.

Do not weaken warnings merely to make a change pass.

## Pull requests

A pull request should explain **what changed**, **why it belongs in Cosmeron**, and any portability or compatibility consequences.

Small, coherent pull requests are easier to review than unrelated changes bundled together.

## Documentation

Canonical project documentation lives under `Docs/`.

- governance: `Docs/Governance/`
- project direction and releases: `Docs/Project/`
- cross-project technical rules: `Docs/Reference/`
- module documentation: `Docs/Modules/`
- translations: `Docs/Translations/`

Keep the repository root reserved for files that have a repository-level or machine-discovery role.

## Security

Do not open public issues for vulnerabilities that require confidential details. Follow the [Security Policy](SECURITY.md) and use GitHub Private vulnerability reporting.

## Conduct

Participation is governed by the [Code of Conduct](CODE_OF_CONDUCT.md).
