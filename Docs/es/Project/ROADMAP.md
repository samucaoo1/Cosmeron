# Hoja de ruta de pre-lanzamiento de Cosmeron

> **Lema:** *A universe belong*

Cosmeron está entrando en la fase de **pre-lanzamiento**, con versiones más pequeñas y enfocadas. Todo el código refactorizado se encuentra en `Codespace/Cosmeron/`, mientras que los módulos pendientes de refactorización están en `Roadmap/Cosmeron/`.

## Módulos refactorizados (100%)

- `Core` — **100%** (Preprocessor, DSL, Memory, Outcome, Detect, Error, etc.)
- `Modules/Chrono` — **100%**
- `Modules/Container` — **100%**
- `Modules/Math` — **100%**
- `Modules/Numeric` — **100%**
- `Modules/Text` — **100%**
- `Modules/Tuple` — **100%**

## Refactorizaciones pendientes (`Roadmap/Cosmeron/Modules/`)

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

## Objetivos de plataforma y destino

- **WebAssembly** (`WASI` / `Emscripten`) como destino de primera clase junto con Linux, BSD, macOS y Windows.
- **C11 sin dependencias** con DSL declarativo (`MODULE`, `PACKAGE`, `OBJECT`, `STRUCT`, `TEMPLATE_*`, `ENUM`, `CALLABLE`, `DELEGATE`, `EVENT_*`).
