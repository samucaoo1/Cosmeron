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
- [Security policy](Governance/SECURITY.md)

### [Project](Project/)

Project direction and release model.

- [Roadmap](Project/ROADMAP.md)
- [Entropy versioning](Project/ENTROPY.md)
- [Release history](Project/RELEASES.md)

### [Reference](Reference/)

Cross-project technical conventions.

- [Cosmeron Pattern](Reference/PATTERN.md)

### [Modules](Modules/)

Module-by-module API and usage documentation.

- [GUI](Modules/GUI.md) — pixel widgets, shared TUI interaction, direct and persistent APIs
- [Surface](Modules/Surface.md) — owned or borrowed ARGB pixels and clipped views
- [Window](Modules/Window.md) — native Win32/X11 presentation and host adapters
- [UI](Modules/UI.md) — terminal widgets
- [Input](Modules/Input.md) — keyboard, mouse, controllers and gamepad state without public initialization
- [Audio](Modules/Audio.md) — synthesis, beep music, codecs and playback
- [CLI](Modules/CLI.md) — arguments, declarative commands and help
- [Parsing](Modules/Parsing.md) — INI, JSON, CSV/TSV, TOML, XML and YAML profiles
- [File](Modules/File.md) — files, paths, directories and atomic content replacement
- [Compression](Modules/Compression.md) — DEFLATE, zlib, gzip, ZIP, checksums and bounded file helpers
- [Database](Modules/Database.md) — embedded SQLite, prepared SQL, typed columns and transactions
- [Bit](Modules/Bit.md)

### [Translations](Translations/)

Localized project documentation.

- [Português (Brasil)](Translations/Portugues-Brasil/README.md)
- [Español (Latinoamérica)](Translations/Espanol-LATAM/README.md)

## Discovery

The repository root intentionally remains small. Files with repository-level or machine-discovery roles stay there, including `README.md`, `LICENSE`, `CITATION.cff`, and `llms.txt`.

Cosmeron was previously developed as **Congro;Library**. The former name is retained only where useful for historical continuity and search discovery.
