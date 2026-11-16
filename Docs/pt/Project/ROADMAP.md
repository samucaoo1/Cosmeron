# Roadmap de Pré-Lançamento do Cosmeron

> **Slogan:** *A universe belong*

O Cosmeron está entrando em **pré-lançamento** com entregas menores e focadas. Todo o código refatorado vive em `Codespace/Cosmeron/`, enquanto os módulos que aguardam refatoração ficam em `Roadmap/Cosmeron/`.

## Módulos Refatorados (100%)

- `Core` — **100%** (Preprocessor, DSL, Memory, Outcome, Detect, Error, etc.)
- `Modules/Container` — **100%**
- `Modules/Math` — **100%**
- `Modules/Numeric` — **100%**
- `Modules/Text` — **100%**
- `Modules/Tuple` — **100%**

## Aguardando Refatoração (`Roadmap/Cosmeron/Modules/`)

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

## Metas de Plataforma e Arquitetura

- **WebAssembly** (`WASI` / `Emscripten`) como alvo de primeira classe ao lado de Linux, BSD, macOS e Windows.
- **C11 sem dependências** com DSL declarativa (`MODULE`, `PACKAGE`, `OBJECT`, `STRUCT`, `TEMPLATE_*`, `ENUM`, `CALLBACK`, `EVENT_*`).
