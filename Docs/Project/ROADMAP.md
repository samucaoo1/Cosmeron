# Cosmeron Roadmap

This roadmap carries forward the latest architectural direction of the project while the codebase moves into the Cosmeron repository.

The goal is not to restore historical code mechanically. Each capability must be reconsidered under the current Cosmeron Pattern, C11 baseline, ownership rules, error model, namespace system, header-oriented architecture, portability requirements, and zero-link goals.

## Migration foundation

- [ ] Import and rename the canonical source tree for Cosmeron.
- [ ] Revalidate the complete test matrix after the repository migration.
- [ ] Re-establish Entropy from the first canonical Cosmeron release.
- [ ] Preserve Hash and Graph work from the latest pre-migration branch.

## Roadmap

### 1. Foundation II

- [ ] File / Filesystem
- [ ] Attributed Text

### 2. Input

- [ ] Keyboard
- [ ] Mouse
- [ ] Events
- [ ] Portable key/button vocabulary
- [ ] Terminal input backends
- [ ] Evaluate Gamepad / Controller

### 3. Terminal Foundation

- [ ] Terminal lifecycle
- [ ] Screen
- [ ] Buffer
- [ ] Cursor
- [ ] Output
- [ ] Capabilities
- [ ] ANSI/POSIX backend
- [ ] Windows Console / VT backend

### 4. TUI

- [ ] Event
- [ ] Render
- [ ] Layout
- [ ] Focus
- [ ] Component
- [ ] Widget set
- [ ] SpeakBox / dialogue box
- [ ] Overlay / HUD primitives

### 5. Graphics

- [ ] Geometry
- [ ] Transform
- [ ] Canvas
- [ ] Raster
- [ ] Scanline fill
- [ ] Line styles
- [ ] Palette
- [ ] Gradients
- [ ] Text Grid rendering target

### 6. Raycast

- [ ] Ray2D / Ray3D
- [ ] Hit/intersection representation
- [ ] Primitive intersection tests
- [ ] Closest / first / any / all hit queries
- [ ] Maximum-distance queries
- [ ] Optional masks/layers
- [ ] Integration with Graphics / Geometry / Transform
- [ ] Immediate GUI picking support

### 7. Immediate GUI

A graphical immediate-mode UI layer inspired by the architectural role of Nuklear,
but implemented in Cosmeron's own API, naming, memory and module conventions.

- [ ] Immediate-mode GUI context and frame lifecycle
- [ ] Minimal persistent state
- [ ] Explicit Input integration
- [ ] Renderer-independent draw-command output
- [ ] Styling / skinning
- [ ] Windows / panels
- [ ] Layout
- [ ] Widget set
- [ ] Text editing
- [ ] Images
- [ ] Custom widget hook
- [ ] Software raster backend through Graphics
- [ ] Optional vertex-buffer output
- [ ] OpenGL example/backend
- [ ] Keep OS/window ownership outside the GUI core

The GUI and TUI are separate frontends.

### 8. Parsing

- [ ] Source cursor
- [ ] Tokens and spans
- [ ] Line/column tracking
- [ ] Scanner helpers
- [ ] Token stream
- [ ] Expect / accept helpers
- [ ] Parse error location
- [ ] Reusable recursive-descent helpers

### 9. Serialization

- [ ] JSON
- [ ] CSV
- [ ] INI
- [ ] Evaluate TOML
- [ ] Base64
- [ ] Hex encoding
- [ ] Structured value representation where justified
- [ ] Pretty / compact serialization
- [ ] Error location reporting

### 10. CLI

- [ ] Positional arguments
- [ ] Short/long options
- [ ] Flags
- [ ] Option values
- [ ] Repeated options
- [ ] Subcommands
- [ ] Help / usage generation
- [ ] Validation

### 11. Process and System

- [ ] Environment variables
- [ ] Current working directory
- [ ] Hostname
- [ ] CPU count
- [ ] Process ID
- [ ] Executable path
- [ ] Spawn
- [ ] Wait
- [ ] Exit status
- [ ] Termination
- [ ] Standard stream redirection
- [ ] Pipes

### 12. Event Loop

- [ ] Loop lifecycle
- [ ] Register/unregister source
- [ ] Callback dispatch
- [ ] Timers
- [ ] Socket readiness
- [ ] Wakeup mechanism
- [ ] Stop / drain semantics
- [ ] User-posted events/tasks
- [ ] Input / Terminal integration
- [ ] Optional Concurrency integration

### 13. Checksum and Digest

- [ ] CRC32
- [ ] Adler-32
- [ ] FNV-1 / FNV-1a
- [ ] Evaluate additional fast non-cryptographic hashes
- [ ] Incremental API
- [ ] One-shot API
- [ ] File/buffer helpers

### 14. UUID and Identifier

- [ ] UUID type
- [ ] Parse / format
- [ ] Equality / comparison
- [ ] UUID v4
- [ ] UUID v7
- [ ] Nil UUID
- [ ] Compact identifier helpers

### 15. Compression and Archive

- [ ] TAR reader/writer
- [ ] Archive entry metadata
- [ ] Streaming extraction/creation
- [ ] Evaluate Deflate
- [ ] Evaluate ZIP
- [ ] Incremental compression/decompression
- [ ] Buffer/file helpers

### 16. Logging

- [ ] Log levels
- [ ] Logger/context
- [ ] Message formatting
- [ ] Timestamp integration
- [ ] Console/File/User sinks
- [ ] Multiple sinks
- [ ] Compile-time filtering
- [ ] Runtime filtering
- [ ] Optional thread safety

Logging remains separate from OPSTATUS/Error semantics.

### 17. Testing

- [ ] Assertion helpers
- [ ] Equality/status helpers
- [ ] Test runner
- [ ] Setup/teardown
- [ ] Expected failure helpers
- [ ] Reporting
- [ ] Optional timing
- [ ] Optional property-style helpers later

### 18. Text Art

- [ ] FIGlet font representation
- [ ] FIGlet parser
- [ ] FIGlet rendering
- [ ] ASCII image conversion
- [ ] Luminance ramp
- [ ] Configurable palette
- [ ] Styled Text Grid output

### 19. Audio

- [ ] Audio core
- [ ] Wave generation
- [ ] Synth
- [ ] Music
- [ ] MIDI
- [ ] Output backends
- [ ] Investigate a zero-link output strategy

### 20. Compatibility Archaeology

- [ ] Audit old Debug facilities
- [ ] Assertions / diagnostics gaps
- [ ] Object/container dump helpers
- [ ] Audit old native converters
- [ ] Portable type replacements
- [ ] Backend-only native concepts
- [ ] Explicit Native escape hatches where useful

## Dependency direction

The intended dependency direction is approximately:

```text
                         File / Filesystem
                           /      |      \
                          v       v       v
                     Parsing  Logging  Archive/Compression
                        |
                        v
                  Serialization

Input ----------------------+
  |                         |
  v                         v
Terminal                Event Loop <------ Chronometry / Network
  |                         |
  v                         |
 TUI                        |
                            v
Graphics <------------ Immediate GUI
  |  \                    /
  |   \                  /
  |    v                /
  |  Raycast <---------+
  |
  +------------------> Text Art

Random + Chronometry ------> UUID / Identifier

File / Network -----------> Checksum / Digest

Process / System ---------> CLI and Event Loop integration

Testing supports every domain but is not a runtime dependency.

Audio remains mostly independent from this chain.
```

This ordering is architectural guidance, not a promise that every item must be
implemented strictly in sequence.

## Roadmap rules

- A roadmap item does not require recreating its historical implementation.
- Reuse current Cosmeron foundations before introducing new abstractions.
- Public APIs follow the current Pattern, not historical naming by default.
- Native platform details stay below portable interfaces unless an explicit
  advanced interop escape hatch is justified.
- New modules/packages should exist because they communicate a real domain,
  not merely for directory symmetry.
- Completion of a roadmap item should include tests and portability review.
