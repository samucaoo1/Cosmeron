# Cosmeron Pre-Release Roadmap

> **Slogan:** *A universe belong*

Cosmeron is entering **pre-release** with smaller, focused releases. All refactored code lives in `Codespace/Cosmeron/`, while modules awaiting refactoring live in `Roadmap/Cosmeron/`.

## Refactored Modules (100%)

- `Core` — **100%** (Preprocessor, DSL, Memory, Outcome, Detect, Error, etc.)
- `Modules/Chrono` — **100%**
- `Modules/Container` — **100%**
- `Modules/Math` — **100%**
- `Modules/Numeric` — **100%**
- `Modules/Text` — **100%**
- `Modules/Tuple` — **100%**

## Pending Refactoring (`Roadmap/Cosmeron/Modules/`)

- `Audio`
- `CLI`
- `Compression`
- `Concurrency`
- `Database`
- `File`
- `Graphics`
- `Input`
- `Logging`
- `Network`
- `Parsing`
- `Random`
- `Security`
- `Sort`

## Platform & Target Goals

- **WebAssembly** (`WASI` / `Emscripten`) as a first-class target alongside Linux, BSD, macOS, and Windows.
- **Zero-dependency C11** with declarative DSL (`MODULE`, `PACKAGE`, `OBJECT`, `STRUCT`, `TEMPLATE_*`, `ENUM`, `CALLABLE`, `DELEGATE`, `EVENT_*`).
