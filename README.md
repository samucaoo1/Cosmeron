<p align="center">
  <img src="Docs/Assets/Brand/Cosmeron-Horizontal.png" alt="Cosmeron" width="600">
</p>

> **A universe belong**

![C11](.github/badges/c11.svg)
![Platforms](.github/badges/platforms.svg)
![License](.github/badges/license.svg)
![Entropy](.github/badges/entropy.svg)
![Lines of Code](.github/badges/loc.svg)

Cosmeron is a zero-dependency **C11** ecosystem and standard library built around a **declarative DSL** for safe, modular, and portable systems programming—targeting native platforms (Linux, BSD, macOS, Windows) as well as **WebAssembly** (`WASI` / `Emscripten`).

---

## Documentation

- [English (Default)](Docs/en/README.md)
- [Português](Docs/pt/README.md)
- [Español (LATAM)](Docs/es/README.md)

---

## The Declarative C11 DSL

Cosmeron replaces ad-hoc boilerplate with a clean, type-safe declarative DSL in pure C11:

- **Modules & Packages (`MODULE`, `PACKAGE`)**: Unified $O(1)$ namespacing (`MODULE_NS`, `MODULE_FUNC`, `MODULE_TYPE`) with zero global namespace pollution.
- **Objects & Pure POD Structs (`OBJECT`, `STRUCT`)**:
  - `STRUCT` defines pure Plain Old Data containers with zero overhead and full C-ABI compliance.
  - `OBJECT` / `OBJECT_EXTENDS` defines encapsulated types with `FunctionTable` dispatch (`OBJECT_CALL`) and explicit contracts.
- **Strict Function Contracts**: Every member declares its exact semantic contract:
  - `OPERATION` (fallible, returns `OPSTATUS`)
  - `PREDICATE` (proposition, returns `bool`)
  - `FUNCTION` (value-producing)
  - `PROCEDURE` (infallible `void` mutation)
  - `COMPARISON` (three-way `CMPOUT`)
- **Dual-Mode Templates (`TEMPLATE_OBJECT`, `TEMPLATE_PACKAGE`, `TEMPLATE_STRUCT`)**: Single-source-of-truth `.def` templates supporting both macro-export batch instantiation (`TEMPLATE_INSTANTIATE`) and direct include instantiation.
- **X-Macro Enums, Callbacks & Events (`ENUM`, `CALLBACK`, `EVENT_PROCEDURE`, `EVENT_OPERATION`)**: Zero-heap-allocation reactive events and type-safe callbacks.

---

## Targets & Pre-Release

Cosmeron is entering **pre-release** with smaller, focused releases:

- **Native**: Linux, FreeBSD, OpenBSD, NetBSD, macOS, Windows.
- **WebAssembly**: `WASI` and `Emscripten` as first-class targets.
- **Refactored vs. Pending**: See [Roadmap (EN)](Docs/en/Project/ROADMAP.md) | [Roadmap (PT)](Docs/pt/Project/ROADMAP.md) | [Hoja de Ruta (ES)](Docs/es/Project/ROADMAP.md).

---

## Repository Structure

```text
Cosmeron/
├── Codespace/
│   ├── Cosmeron/     # Refactored core and modules (100% DSL-compliant)
│   ├── Tests/        # Unit and integration tests
│   └── Examples/     # Usage examples
├── Roadmap/          # Modules awaiting refactoring and future plans
├── Docs/
│   ├── en/           # English documentation (default)
│   ├── pt/           # Portuguese documentation
│   └── es/           # Latin American Spanish documentation
└── .github/          # CI workflows, entropy, LOC counter, badges, governance links
```

---

## Governance

- [Code of Conduct](.github/CODE_OF_CONDUCT.md)
- [Contributing](.github/CONTRIBUTING.md)
- [Security Policy](.github/SECURITY.md)
