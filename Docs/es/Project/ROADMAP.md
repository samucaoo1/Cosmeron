# Hoja de Ruta de Prelanzamiento de Cosmeron

> **Eslogan:** *A universe belong*

Cosmeron está entrando en **prelanzamiento** con entregas más pequeñas y enfocadas. Todo el código refactorizado vive en `Codespace/Cosmeron/`, mientras que los módulos pendientes de refactorización están en `Roadmap/Cosmeron/`.

## Módulos Refactorizados (100%)

- `Core` — **100%** (Preprocessor, DSL, Memory, Outcome, Detect, Error, etc.)
- `Modules/Container` — **100%**
- `Modules/Math` — **100%**
- `Modules/Numeric` — **100%**
- `Modules/Text` — **100%**
- `Modules/Tuple` — **100%**

## Pendientes de Refactorización (`Roadmap/Cosmeron/Modules/`)

- `Audio`
- `Chronometry`
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

## Metas de Plataforma y Arquitectura

- **WebAssembly** (`WASI` / `Emscripten`) como objetivo de primera clase junto con Linux, BSD, macOS y Windows.
- **C11 sin dependencias** con DSL declarativa (`MODULE`, `PACKAGE`, `OBJECT`, `STRUCT`, `TEMPLATE_*`, `ENUM`, `CALLBACK`, `EVENT_*`).
