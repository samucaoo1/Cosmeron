<p align="center">
  <img src="Assets/Brand/Cosmeron.png" alt="Cosmeron" width="192">
</p>

# Cosmeron Documentation

This directory contains the canonical project documentation that does not need to occupy the repository root.

## Structure

### [Governance](Governance/)

Project-level policies and legal or community documents.

- [Authors and contributors](Governance/AUTHORS.md)
- [Contributing](Governance/CONTRIBUTING.md)
- [Support](Governance/SUPPORT.md)
- [Code of Conduct](Governance/CODE_OF_CONDUCT.md)
- [Licensing rationale](Governance/LICENSING.md)
- [EUPL 1.2 — official licence text](LICENSE.md)
- [Security policy](Governance/SECURITY.md)

### [Project](Project/)

Project direction and release model.

- [Roadmap](Project/ROADMAP.md)
- [Entropy versioning](Project/ENTROPY.md)
- [Release history](Project/RELEASES.md)

### [Reference](Reference/)

Cross-project technical conventions.

- [Cosmeron Pattern](Reference/PATTERN.md)

### [Refactored Modules](Modules/)

Reference documentation for the refactored C11 DSL modules in `Codespace/Cosmeron/`:

- **Core**: [Algorithm](Modules/Algorithm.md) · [Bit](Modules/Bit.md) · [Condition](Modules/Condition.md) · [Diagnostic](Modules/Diagnostic.md) · [Memory](Modules/Memory.md) · [Module](Modules/Module.md) · [Preprocessor](Modules/Preprocessor.md) · [Types](Modules/Types.md) · [Cast](Modules/Cast.md)
- **Modules**: [Chronometry](Modules/Chronometry.md) · [Container](Modules/Container.md) · [Math](Modules/Math.md) · [Numeric](Modules/Numeric.md) · [Text](Modules/Text.md) · [Tuple](Modules/Tuple.md)

### [Translations](Translations/)

Localized project documentation.

- [Português (Brasil)](../pt/README.md)
- [Español (Latinoamérica)](../es/README.md)

## Discovery

The repository root intentionally remains small. Files with repository-level or machine-discovery roles stay there, including `README.md`, `LICENSE`, `CITATION.cff`, and `llms.txt`.

Cosmeron was previously developed as **Congro;Library**. The former name is retained only where useful for historical continuity and search discovery.
