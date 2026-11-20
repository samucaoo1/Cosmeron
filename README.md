<p align="center">
  <img src="Docs/Assets/Brand/Cosmeron_1.0_Nebula.jpg" alt="Cosmeron — A Universe Beyond" width="100%">
</p>

<p align="center">
  <strong>A Universe Beyond</strong><br>
  <sub> Version 1.0.0 · Nebula </sub>
</p>

![C11](.github/badges/c11.svg)
![Platforms](.github/badges/platforms.svg)
![License](.github/badges/license.svg)
![Entropy](.github/badges/entropy.svg)
![Lines of Code](.github/badges/loc.svg)

Cosmeron is a zero-dependency **C11** ecosystem and standard
library built around of **Cosmic DSL** for safe, modular,
and portable systems programming—targeting native platforms
(Linux, BSD, macOS, Windows) as well as **WebAssembly**
(`WASI` / `Emscripten`).

## Development Progress

**Overall Progress: 28.6% (6/21 modules)**

```text
MODULES
├── Core              [####################] 100%
├── Container         [####################] 100%
├── Math              [####################] 100%
├── Numeric           [####################] 100%
├── Text              [####################] 100%
└── Tuple             [####################] 100%

ROADMAP
├── Audio             [....................]   0%
├── Chronometry       [....................]   0%
├── CLI               [....................]   0%
├── Compression       [....................]   0%
├── Concurrency       [....................]   0%
├── Database          [....................]   0%
├── File              [....................]   0%
├── Graphics          [....................]   0%
├── Input             [....................]   0%
├── Logging           [....................]   0%
├── Network           [....................]   0%
├── Parsing           [....................]   0%
├── Random            [....................]   0%
├── Security          [....................]   0%
└── Sort              [....................]   0%
```

Progress represents the implementation and refactoring
status of each module, not test coverage.

---

## Documentation

- [English (Default)](Docs/en/README.md)
- [Português](Docs/pt/README.md)
- [Español (LATAM)](Docs/es/README.md)

---

## License

Cosmeron is licensed under the **European Union Public
Licence 1.2 (EUPL-1.2)**. The canonical licence is available
in the repository root as [`LICENSE`](LICENSE), with official
language copies in [English](Docs/en/LICENSE.md),
[Português](Docs/pt/LICENSE.md), and
[Español](Docs/es/LICENSE.md).

---

## Cosmic DSL - The brand new Declarative C11 DSL

Cosmeron replaces ad-hoc boilerplate with a clean,
type-safe declarative DSL in pure C11:

- **Modules & Packages (`MODULE`, `PACKAGE`)**:
  Unified namespacing with zero global namespace pollution.
- **Objects & Pure POD Structs (`OBJECT`, `STRUCT`)**:
  Encapsulated types and zero-overhead data containers.
- **Strict Function Contracts**:
  `OPERATION`, `PREDICATE`, `FUNCTION`, `PROCEDURE`
  and `COMPARISON`.
- **Dual-Mode Templates**:
  `TEMPLATE_OBJECT`, `TEMPLATE_PACKAGE`, `TEMPLATE_STRUCT`.
- **X-Macro Enums, Callbacks & Events**:
  Zero-heap-allocation reactive events and type-safe callbacks.

---

## Targets & Pre-Release

- **Native**: Linux, FreeBSD, OpenBSD, NetBSD, macOS, Windows.
- **WebAssembly**: `WASI` and `Emscripten`.
- **Refactored vs. Pending**:
  See the [Roadmap](Docs/en/Project/ROADMAP.md).

---

## Repository Structure

```text
Cosmeron/
├── Codespace/
│   ├── Cosmeron/
│   ├── Tests/
│   └── Examples/
├── Roadmap/
├── Docs/
│   ├── en/
│   ├── pt/
│   └── es/
└── .github/
```

---

## Governance

- [Code of Conduct](.github/CODE_OF_CONDUCT.md)
- [Contributing](.github/CONTRIBUTING.md)
- [Security Policy](.github/SECURITY.md)
