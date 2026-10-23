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

- [x] File / Filesystem
  - [x] Path manipulation, normalization, absolute check, view stems and extensions (`FileSystem / Path`)
  - [x] File metadata, permissions, modified/accessed timestamps (`FileSystem / Metadata`)
  - [x] Streaming file I/O, buffered read/write, seek and tell (`FileSystem / File`)
  - [x] Directory iteration, recursive traversal, creation, and removal (`FileSystem / Directory`)
  - [x] Cross-platform filesystem watcher and change notification (`FileSystem / Watch`: inotify, kqueue, ReadDirectoryChangesW)
  - [x] Memory-mapped files (POSIX mmap / Windows MapViewOfFile, msync, mlock/munlock)
- [x] Text & String Foundations
  - [x] C-string safety helpers and bounded string utilities (`Text / String / CString`)
  - [x] Heap-allocated dynamic string (`Text / String / TString`)
  - [x] Non-owning string slices and string views (`Text / String / TStringView`)
  - [x] String builder with amortized geometric buffer growth (`Text / String / Builder`)
- [x] Attributed Text
- [x] Human-readable byte size formatting and parsing (Format_BytesHuman, Parse_BytesHuman)

### 2. Input

- [x] Keyboard
- [x] Mouse
- [x] Events
- [x] Portable key/button vocabulary
- [x] Terminal input backends
- [x] Evaluate Gamepad / Controller

### 3. Terminal Foundation

- [x] Terminal lifecycle
- [x] Screen
- [x] Buffer
- [x] Cursor
- [x] Output
- [x] Capabilities
- [x] ANSI/POSIX backend
- [x] Windows Console / VT backend

### 4. TUI

- [x] Event
- [x] Render
- [x] Layout
- [x] Focus
- [x] Component
- [x] Widget set
- [x] SpeakBox / dialogue box
- [x] Overlay / HUD primitives

### 5. Graphics

- [x] Geometry
- [x] Transform
- [x] Canvas
- [x] Raster
- [x] Scanline fill
- [x] Line styles
- [x] Palette
- [x] Gradients
- [x] Text Grid rendering target
- [x] Graphics Extensions
  - [x] Compositor: multi-layer Canvas composition, z-ordering, alpha blending and transparency (`Graphics / Extensions / Compositor`)
  - [x] Sprite: text/cell frames, blitting, clipping, and multi-frame animation (`Graphics / Extensions / Sprite`)
  - [x] Animation: chronometry-driven easing, interpolation and transitions (`Graphics / Extensions / Animation`)
  - [x] Pixel Canvas: true-pixel drawing target and terminal half-block / block character quantization (`Graphics / Extensions / PixelCanvas`)

### 6. Raycast

- [x] Ray2D
- [ ] Ray3D
- [x] Hit/intersection representation
- [x] Primitive intersection tests
- [x] Closest / first / any / all hit queries
- [x] Maximum-distance queries
- [ ] Optional masks/layers
- [x] Integration with Graphics / Geometry / Transform
- [ ] Immediate GUI picking support

### 7. Immediate GUI

A graphical immediate-mode UI layer inspired by the architectural role of Nuklear,
but implemented in Cosmeron's own API, naming, memory and module conventions.

- [x] Immediate-mode GUI context and frame lifecycle
- [x] Minimal persistent state
- [x] Explicit Input integration
- [x] Renderer-independent draw-command output
- [x] Styling / skinning
- [x] Windows / panels
- [x] Layout
- [x] Widget set
- [x] Text editing
- [x] Images
- [ ] Custom widget hook
- [x] Software raster backend through Graphics
- [ ] Optional vertex-buffer output
- [ ] OpenGL example/backend
- [x] Keep OS/window ownership outside the GUI core

The GUI and TUI are separate frontends.

### 8. Parsing

- [x] Source cursor and byte navigation
- [ ] Lexer primitives and classification helpers (`Parsing / Lexer`)
- [ ] Tokenizer state machine and token stream emission (`Parsing / Tokenizer`)
- [ ] Scanner helpers and span slicing (`Parsing / Scanner`)
- [ ] Reusable recursive-descent parser engine and combinators (`Parsing / Parser`)
- [x] Tokens and spans
- [x] Line/column tracking
- [ ] Expect / accept helpers
- [x] Parse error location
- [ ] RFC 3986 URI/URL parser (scheme, userinfo, host, port, path, query, fragment)
- [ ] Mathematical expression parser and runtime evaluator with variable binding and standard math functions

### 9. Serialization

- [x] JSON serializer and deserializer (`Serialization / JSON`)
- [x] CSV table parser and emitter (`Serialization / CSV`)
- [x] INI configuration format parser (`Serialization / INI`)
- [x] TOML parser and serializer (`Serialization / TOML`)
- [ ] Portable binary serialization / endian-neutral packing (`Serialization / Binary`)
- [ ] Base64
- [ ] Hex encoding
- [ ] Structured value representation where justified
- [x] Pretty / compact serialization
- [x] Error location reporting

### 10. CLI

- [x] Positional arguments
- [x] Short/long options
- [x] Flags
- [x] Option values
- [x] Repeated options
- [x] Subcommands
- [x] Help / usage generation
- [x] Validation

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
- [ ] Pipes and local IPC abstractions
  - [ ] Named and anonymous process pipes with cross-platform nonblocking behavior (`IPC / Pipe`)
  - [ ] Shared memory segments with lifetime, mapping, and synchronization contracts (`IPC / SharedMemory`)
  - [ ] Structured IPC message framing and typed payload conventions (`IPC / Message`)
- [ ] Signal handling (graceful shutdown notifications via pipe, double interrupt handling)
- [ ] Async-signal-safe formatted printing
- [ ] Fatal crash signal handling with instruction backtrace output

### 12. Event Loop

- [ ] Loop lifecycle
- [ ] Register/unregister source
- [ ] Callback dispatch
- [ ] Timers
  - [ ] Hashed timing wheel (O(1) cancellation and timeout wheel polling)
- [ ] Socket readiness
  - [ ] Modern high-performance poller backends (Linux epoll, BSD/macOS kqueue, Windows WSAPoll with edge-triggered support)
  - [ ] Unix Domain Sockets (AF_UNIX stream and datagram)
- [ ] Wakeup mechanism
- [ ] Stop / drain semantics
- [ ] User-posted events/tasks
- [ ] Input / Terminal integration
- [ ] Optional Concurrency integration

### 13. Checksum and Digest

- [x] CRC32
  - [ ] Hardware-accelerated CRC-32C (Castagnoli) using CPU instructions (SSE 4.2 / ARMv8 intrinsics)
- [x] Adler-32
- [x] FNV-1 / FNV-1a
- [ ] Evaluate additional fast non-cryptographic hashes
- [x] Incremental API
- [x] One-shot API
- [x] File/buffer helpers

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
- [x] ZIP entry metadata
- [ ] Streaming extraction/creation
- [x] DEFLATE, zlib and gzip compression/decompression
- [x] ZIP memory reader/writer (Store and Deflate)
- [ ] Incremental compression/decompression
- [x] Bounded buffer/file helpers
- [x] Large-file ZIP64 (>4 GiB) local/central size and offset verification with sparse-file testing
- [ ] Corpus-guided fuzzing (libFuzzer/AFL++) for DEFLATE, zlib, gzip, ZIP headers, and ZIP64 extras
- [ ] Enhanced LZ77 match search (hash-chain/window) and optimal block splitting

### 16. Logging

- [x] Log levels
- [x] Logger/context
- [x] Message formatting
- [x] Timestamp integration
- [x] Console/File/User sinks
  - [ ] Log file rotation (size and archive count limits)
- [x] Multiple sinks
- [ ] Thread name tagging in log formatters
- [x] Compile-time filtering
- [x] Runtime filtering
- [ ] Optional thread safety

Logging remains separate from OPSTATUS/Error semantics.

### 17. Testing

- [ ] Assertion helpers
- [ ] Equality/status helpers
- [ ] Test runner (registration, setup/teardown, reporting, exit status)
- [ ] Benchmark harness (repeatable duration measurement, stats, regression comparison)
- [ ] Profiler (scoped timing, event samples, low-overhead collection and reports)
- [ ] Expected failure helpers
- [ ] Reporting
- [ ] Optional timing
- [ ] Optional property-style helpers later

### 18. Text Art

- [x] FIGlet font representation
- [x] FIGlet parser
- [x] FIGlet rendering
  - [ ] Smushing and kerning (horizontal and vertical according to fullLayout)
  - [ ] Additional codetags, right-to-left layout, and multibyte font support
- [ ] ASCII image conversion
- [ ] Luminance ramp
- [ ] Configurable palette
- [x] Styled Text Grid output

### 19. Audio

- [ ] Audio core
- [ ] Wave generation
- [ ] Synth
- [ ] Music
- [ ] MIDI
- [ ] Output backends
  - [ ] Native hardware validation across Linux (ALSA), Windows (waveOut), macOS (AudioQueue/CoreAudio), FreeBSD (OSS), OpenBSD (sndio), NetBSD (audioio)
  - [ ] Driver validation: disconnect, underrun, format negotiation, and hotplug behavior
  - [ ] Retirement of miniaudio device fallback after native validation coverage
- [ ] Native Audio Codecs
  - [x] Native C11 FLAC decoder (memory, file, and streaming reader with exact-integer STREAMINFO MD5 verification)
  - [x] Native C11 MP3 Layer III decoder (Xing/LAME gapless, indexed seek, LSF, MS+intensity stereo)
  - [ ] MP3/FLAC decoder parity completion on remaining rare variants (8-kHz mixed/intensity, unusual subframes, third-party encoders, SEEKTABLE ingestion)
- [ ] Investigate a zero-link output strategy

### 20. Compatibility Archaeology

- [ ] Audit old Debug facilities
- [ ] Assertions / diagnostics gaps
- [ ] Object/container dump helpers
- [ ] Audit old native converters
- [ ] Portable type replacements
- [ ] Backend-only native concepts
- [ ] Explicit Native escape hatches where useful

### 21. Advanced Containers and Data Structures

- [x] Flat Heap / Priority Queue (`Container/Array/Heap`) with Min-Heap, Max-Heap, custom comparators, and Floyd's heapify
- [ ] Disjoint Set / Union-Find (`Container/Disjoint`) with path compression and rank-based union
- [ ] Intrusive Doubly Linked List (`Container/Intrusive/List`) for zero-allocation node embedding
- [x] In-memory B-Tree (`Container/Tree/BTree`) cache-friendly multiway search tree for large ordered collections
- [ ] Range Minimum Query / Order Statistics Tree (`Container/Tree/OrderTree`) augmented balanced search tree for O(log n) range minimum and rank queries
- [x] Unordered Hash Set (`Container/Hash`) with `HASH_SET_IMPLEMENT_ALL` for keys-only O(1) set operations without value field overhead
- [ ] Single-pass `GetOrInsert` / Entry API (`Container/Hash`, `Container/Tree`) to eliminate duplicate hash computations, probing cycles, and redundant tree descent
- [ ] Ordered range boundary queries (`Container/Tree`) with `LowerBound` (key >= target) and `UpperBound` (key > target) across AVL, BST, and Red-Black trees
- [ ] Doubly linked list `Splice` (`Container/Linked/Double`) for O(1) node and sub-list relocation between list instances without heap allocation
- [ ] Vector bulk insertions (`Container/Array/Vector`) with `PushBackN` and `InsertN` using single-pass capacity reservation and block memory transfer
- [ ] Standardized container copy/clone operations (`Container/*`) with `*_Copy` / `*_Clone` prototypes returning `OPSTATUS` across all container families
- [ ] Cache-conscious Hash Table layout (`Container/Hash`) with separate control/metadata array (inspired by Verstable / Swiss Table) to reduce cache misses during collision resolution
- [ ] Optional element lifecycle cleanup hooks (`Container/*`) via `ElementDestroy` callbacks in container generation macros for managing dynamic elements and nested containers safely
- [x] Non-owning Contiguous Span (`Container/Array/Span`) zero-allocation buffer and vector slicing, element access, and boundary checks
- [ ] Flat Map and Flat Set (`Container/Array/FlatMap`, `Container/Array/FlatSet`) cache-friendly sorted contiguous vector-backed associative containers with binary search lookups
- [ ] Multi-word BitSet / BitVector (`Container/BitSet`) arbitrary-size compact bit array with word-level bitwise operations, popcount, and first-set searches
- [ ] Segmented / Chunked Deque (`Container/Array/SegmentedDeque`) two-level fixed-size chunked array providing O(1) push/pop at both ends without reallocating existing elements or chasing node pointers
- [ ] Associative MultiMap and MultiSet (`Container/Tree`, `Container/Hash`) supporting duplicate key entries and equal-range queries
- [ ] Safe cursor-based iterators (`Container/*`) with `IterInit` and `IterNext` for non-contiguous containers (Hash, Tree, List) without exposing private internal node or bucket layouts
- [ ] Functional `ForEach` visitor helpers (`Container/*`) with early-exit `OPSTATUS` return across all container families
- [ ] Multi-parameter support for universal template engine (`Core/Module/Common/Template.impl`), expanding `TEMPLATE_IMPLEMENT(...)` and `TEMPLATE_DECLARE(...)` to support variadic argument lists for multi-parameter templates such as maps (e.g. `TEMPLATE_IMPLEMENT(TBTREE_MAP, int, int, int, int)`)

### 22. Advanced Algorithms and Selection

- [ ] QuickSelect / k-smallest element selection (`Sort/Selection`) in expected O(n) time and median finding without full array sorting
- [ ] Suffix Array construction (`Text/Index/SuffixArray`) via linear-time O(n) SAIS (Suffix Array Induced Sorting) algorithm for full-text indexing

### 23. Memory Management

- [x] Fixed-size chunk memory pool allocator (`Core/Memory/Pool`) for uniform object/node recycling without heap fragmentation

### 24. Concurrency Pipelines

- [ ] Streaming parallel pipeline (`Concurrency/Pipeline`) for multi-stage concurrent processing of data batches

### 25. Numerical and Scientific Math

- [ ] Matrix diagonalization, eigenvalue and eigenvector solvers (`Math/Matrix`)
- [ ] Derivative-free nonlinear optimization (Hooke-Jeeves pattern search, Brent's 1D minimizer)
- [ ] Special statistical distribution functions (Incomplete Gamma, Digamma, Kolmogorov-Smirnov)

### 26. Database & Persistence

- [ ] Embedded Key-Value Data Store (`Database/KVStore`): iteration, serialization, and persistence
- [ ] Storage engine primitives (`Database/Storage`): pages, b-tree indices, memory durability, and encoding formats
- [ ] Record filtering and query layer (`Database/Query`)
- [ ] ACID Transactions and crash durability (`Database/Transactions`): atomic write-ahead/rollback log, isolation, and recovery

### 27. Network & Protocols

- [ ] Application-level HTTP/1.1 client (`Network/HTTP/Client`)
- [ ] Lightweight embedded HTTP server (`Network/HTTP/Server`)
- [ ] HTTP Headers and message parser (`Network/HTTP/Headers`)
- [ ] RFC 3986 URL/URI builder and parser (`Network/HTTP/URL`)
- [ ] WebSocket framing and protocol adapter (`Network/HTTP/WebSocket`)

### 28. Cryptography & Security

- [ ] Cryptographically secure random number generator (`Security/Crypto/SecureRandom`)
- [x] SHA-2 cryptographic digest family (`Security/Crypto/SHA2`: SHA-224, SHA-256, SHA-384, SHA-512)
- [ ] HMAC (Hash-based Message Authentication Code) message integrity verification (`Security/Crypto/HMAC`)
- [ ] Symmetric encryption primitives (`Security/Crypto/Encryption`: AES, ChaCha20-Poly1305)

### 29. Text & Localization

- [ ] Locale abstraction and culture tagging (`Text/Localization/Locale`)
- [ ] Number and percentage formatting per locale (`Text/Localization/NumberFormat`)
- [ ] Currency and monetary formatting per locale (`Text/Localization/Currency`)
- [ ] Pluralization and parameterized message localization (`Text/Localization/MessageFormat`)

### 30. Configuration Management

- [ ] Multi-source configuration loader (`Configuration/Load`: files, environment variables, CLI overrides)
- [ ] Configuration schema specification (`Configuration/Schema`)
- [ ] Semantic validation and type checking of configuration values (`Configuration/Validate`)
- [ ] Layered hierarchy merging with precedence rules (`Configuration/Merge`)

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
