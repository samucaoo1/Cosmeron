# Roadmap de pré-lançamento da Cosmeron

> **Slogan:** *A universe belong*

A Cosmeron está entrando na fase de **pré-lançamento**, com versões menores e focadas. Todo o código refatorado fica em `Codespace/Cosmeron/`, enquanto os módulos que aguardam refatoração ficam em `Roadmap/Cosmeron/`.

## Módulos refatorados (100%)

- `Core` — **100%** (Preprocessor, DSL, Memory, Outcome, Detect, Error etc.)
- `Modules/Chrono` — **100%**
- `Modules/Container` — **100%**
- `Modules/Math` — **100%**
- `Modules/Numeric` — **100%**
- `Modules/Text` — **100%**
- `Modules/Tuple` — **100%**

## Refatorações pendentes (`Roadmap/Cosmeron/Modules/`)

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

## Objetivos de plataforma e destino

- **WebAssembly** (`WASI` / `Emscripten`) como destino de primeira classe, junto com Linux, BSD, macOS e Windows.
- **C11 sem dependências** com DSL declarativa (`MODULE`, `PACKAGE`, `OBJECT`, `STRUCT`, `TEMPLATE_*`, `ENUM`, `CALLBACK`, `EVENT_*`).
