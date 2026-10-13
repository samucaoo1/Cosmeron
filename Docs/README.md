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

Canonical module and package API references. Each document includes the exported APIs, argument and return contracts, examples and portability notes, following the [Bit reference style](Modules/Bit.md).

#### Core

- [Algorithm](Modules/Algorithm.md) — three-way comparison callbacks and helpers
- [Cast](Modules/Cast.md) — type-conversion namespace infrastructure
- [Error](Modules/Error.md) — statuses, panic and errors
- [Memory](Modules/Memory.md) — allocation, arenas and memory operations
- [Preprocessor](Modules/Preprocessor.md) — token operations, expansion and iteration

#### Main modules

- [Audio](Modules/Audio.md) — audio buffers, oscillator, playback, codecs and sequencing
- [Bit](Modules/Bit.md) — flags, masks, rotations and bit fields
- [CLI](Modules/CLI.md) — arguments, schemas and help output
- [Chronometry](Modules/Chronometry.md) — clocks, duration and calendar operations
- [Compression](Modules/Compression.md) — DEFLATE, gzip, ZIP and checksums
- [Concurrency](Modules/Concurrency.md) — threads, atomics, synchronization and cooperative tasks
- [Container](Modules/Container.md) — vector, queues, stacks, trees and strings
- [Database](Modules/Database.md) — embedded SQLite, prepared statements and transactions
- [File](Modules/File.md) — paths, streams, directories and atomic file replacement
- [Input](Modules/Input.md) — keyboard, pointer and controller events
- [Math](Modules/Math.md) — numeric arithmetic and utility operations
- [Network](Modules/Network.md) — addressing, resolution, sockets and polling
- [Parsing](Modules/Parsing.md) — JSON, INI, CSV, TOML, XML and YAML
- [Random](Modules/Random.md) — entropy sources, distributions and shuffle
- [Sort](Modules/Sort.md) — comparison sorts, radix and educational algorithms
- [Struct](Modules/Struct.md) — general-purpose tuple structures
- [Text](Modules/Text.md) — Unicode, encoding, text grids and formatting
- [Type](Modules/Type.md) — fundamental typed primitives

#### Graphics packages

The Graphics umbrella aggregates packages that can also be included individually.

- [Canvas](Modules/Canvas.md) — retained terminal-cell surfaces and differential presentation
- [FIGlet](Modules/FIGlet.md) — FIGfont parsing, measuring and drawing
- [Font](Modules/Font.md) — native C11 compile-time .asf fonts
- [GUI](Modules/GUI.md) — pixel-based immediate widgets and struct variants
- [Pseudo3DRoad](Modules/Pseudo3DRoad.md) — road curves, hills, projection and renderer
- [Rasterization](Modules/Rasterization.md) — 2D primitives for Canvas
- [Raycasting](Modules/Raycasting.md) — grid DDA and depth-aware wall/sprite rendering
- [Sprite](Modules/Sprite.md) — borrowed Canvas sprites, layers and animation
- [Surface](Modules/Surface.md) — owned and borrowed ARGB pixel storage
- [Terminal](Modules/Terminal.md) — immediate streams, cursor, attributes and events
- [UI](Modules/UI.md) — terminal-based widgets and layout
- [Window](Modules/Window.md) — native window presentation and host adapters

### Documentation verification

The repository verifies the presence and reference-section consistency of
module documentation. Complete, compilable C11 examples are marked in their
Markdown source with `<!-- cosmeron-c11-test -->`, immediately before the
fenced `c` block. Other snippets are usage fragments and are not compiled as
standalone translation units.

From the repository root:

```sh
python3 Tools/check_doc_examples.py --cc gcc --min-tests 6
python3 Tools/check_doc_examples.py --cc clang --min-tests 6
```

The tool compiles each opted-in example with strict C11 flags and runs it. It
uses temporary build paths, requires no third-party Python packages, and does
not write generated files to the repository. The main CI workflow runs it on
Linux (GCC/Clang) and macOS (Clang).

### [Translations](Translations/)

Localized project documentation.

- [Português (Brasil)](Translations/Portugues-Brasil/README.md)
- [Español (Latinoamérica)](Translations/Espanol-LATAM/README.md)

## Discovery

The repository root intentionally remains small. Files with repository-level or machine-discovery roles stay there, including `README.md`, `LICENSE`, `CITATION.cff`, and `llms.txt`.

Cosmeron was previously developed as **Congro;Library**. The former name is retained only where useful for historical continuity and search discovery.
