#!/bin/sh
# Extended *executed* BSD regression matrix in native vmactions guests.
# First probe the zero-extra-link contract, then optionally use -pthread
# *in CI diagnostics only* to test otherwise unverified native thread backends.
# Reports must NOT present a -pthread test as satisfying zero-link.
set -eu
CC=${CC:-cc}
case "$(uname -s)" in
  NetBSD) extra=-D_NETBSD_SOURCE=1 ;;
  *) extra= ;;
esac
mkdir -p .build-bsd-extended
linkflags=
coverage=.build-bsd-extended/coverage.md
stage=Initialization
printf '%s\n' "### BSD native integration — $(uname -s)" "" \
  '| Module | C11 compile | Runtime | Zero-extra-link | Hardware/device |' \
  '|---|---|---|---|---|' > "$coverage"
record() {
  printf '| %s | PASS | PASS | %s | %s |\n' "$1" "$2" "$3" >> "$coverage"
}
trap 'rc=$?; if [ "$rc" -ne 0 ]; then printf "| %s | FAIL | NOT COMPLETED | UNVERIFIED | NOT TESTED |\\n" "$stage" >> "$coverage"; fi' EXIT

suite() {
  module=$1; shift
  module_features=-D_POSIX_C_SOURCE=200809L
  if [ "$module" = Audio ]; then
    # Native sound headers use BSD u_char/u_short; choose matching extensions
    # without weakening -std=c11 or -Werror.
    case "$(uname -s)" in
      DragonFly|FreeBSD) module_features= ;;
      OpenBSD) module_features="-D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE=1" ;;
    esac
    echo "[bsd-extended] Audio header profile: ${module_features:-BSD native defaults}"
  fi
  for test in "$@"; do
    echo "[bsd-extended] run $module/$test"
    (
      cd "Codespace/Tests/$module"
      "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
        ${module_features} ${extra} "$test.c" ${linkflags} \
        -o "../../../.build-bsd-extended/${module}_${test}"
      "../../../.build-bsd-extended/${module}_${test}"
    )
  done
}
multitu() {
  module=$1
  module_features=-D_POSIX_C_SOURCE=200809L
  if [ "$module" = Audio ]; then
    case "$(uname -s)" in
      DragonFly|FreeBSD) module_features= ;;
      OpenBSD) module_features="-D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE=1" ;;
    esac
  fi
  echo "[bsd-extended] run $module/MultiTU"
  (
    cd "Codespace/Tests/$module"
    "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
      ${module_features} ${extra} \
      MultiTU_A.c MultiTU_B.c MultiTU_Main.c ${linkflags} \
      -o "../../../.build-bsd-extended/${module}_MultiTU"
    "../../../.build-bsd-extended/${module}_MultiTU"
  )
}

# Local loopback only; no Internet is required.
stage=Network
suite Network TCP UDP Poller NonBlocking PollerClose UDPClearPeer IPv6 \
  Lifecycle SocketRecreate PollerEmpty ConnectionNonBlocking \
  PollerDescriptorLimit PollerTimeout ConnectionArguments \
  DatagramArguments Surface IncludeOrder
multitu Network
record Network PASS 'localhost sockets only'

# Cooperative mode is intentionally independent of pthread and libm.
stage=ConcurrencyCooperative
suite Concurrency Cooperative
echo "[bsd-extended] CONCURRENCY_COOPERATIVE_ZERO_LINK=PASS"
record ConcurrencyCooperative PASS 'single-thread cooperative tasks, not OS threads'

# Evaluate the alternative C11 thrd_* ABI without hiding a library load.
# thrd_create + thrd_join must LINK and RUN without -pthread/-lstdthreads.
# This is an independent feasibility probe, not a replacement for the existing
# pthread-based public Thread/Mutex/ThreadPool implementation.
stage=C11ThreadProbe
if (
  cd Codespace/Tests/Concurrency
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} C11ThreadsProbe.c \
    -o ../../../.build-bsd-extended/C11ThreadsProbe >/dev/null 2>&1
) && .build-bsd-extended/C11ThreadsProbe; then
  echo "[bsd-extended] C11_THREAD_NATIVE_ZERO_LINK=PASS"
  record C11ThreadsProbe PASS 'C11 native threads executed; separate from pthread API'
else
  echo "[bsd-extended] C11_THREAD_NATIVE_ZERO_LINK=NOT_SUPPORTED"
  printf '%s\n' '| C11ThreadsProbe | NOT SUPPORTED | NOT CERTIFIED | NOT SUPPORTED | no device needed |' >> "$coverage"
fi

# Thread primitives, synchronisation, tasks and thread pool, no explicit -pthread.
# Probe the public zero-extra-link contract *before* running the thread tests.
# The CI may use -pthread solely to diagnose the backend on systems with
# separately linked libpthread; it MUST report that this does not meet the
# Cosmeron zero-link policy for consumers.
stage=Concurrency
thread_zero_link=PASS
if (
  cd Codespace/Tests/Concurrency
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} ModuleSurface.c \
    -o ../../../.build-bsd-extended/Concurrency_ZeroLink
); then
  echo "[bsd-extended] THREAD_ZERO_LINK=PASS"
else
  echo "[bsd-extended] THREAD_ZERO_LINK=NOT_SUPPORTED on $(uname -s) (requires native libpthread)"
  thread_zero_link=NOT_SUPPORTED
  linkflags=-pthread
fi
suite Concurrency NativeStandalone IncludeAll ModuleSurface Mutex Once RWLock Semaphore \
  Thread Coroutine Condition Barrier Contention ProducerConsumer \
  ThreadPool ThreadPoolMultiProducer ThreadPoolShutdown Future
record Concurrency "$thread_zero_link" 'threads tested in VM (with -pthread if required)'
linkflags=

# Miniz codec/ZIP and SQLite in the single-implementation translation unit.
stage=Compression
suite Compression Codec Zip File Namespace FunctionTableOff Robustness Allocation
multitu Compression
sh .github/scripts/test-bsd-interop.sh
zip_external=$(sed -n 's/^ZIP_EXTERNAL=//p' .build-bsd-extended/interop-status.txt)
zip_import=$(sed -n 's/^ZIP_IMPORT=//p' .build-bsd-extended/interop-status.txt)
record Compression PASS "gzip external: PASS; ZIP external: $zip_external; ZIP import: $zip_import"
stage=Database
echo "[bsd-extended] compile+run Database (one shared embedded SQLite object)"
(
  cd Codespace/Tests/Database
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} -c Backend.c \
    -o ../../../.build-bsd-extended/Database_Backend.o
  # BSD SQLite defaults to mutex-free single-thread mode. This is a
  # mandatory zero-extra-link contract test, not a diagnostic fallback.
  db_linkflags=
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} IncludeAll.c \
    ../../../.build-bsd-extended/Database_Backend.o \
    -o ../../../.build-bsd-extended/Database_ZeroLink
  echo "[bsd-extended] DATABASE_ZERO_LINK=PASS (mutex-free SQLite)"
  for test in IncludeAll Database Persistence Namespace SQLiteThreadMode; do
    echo "[bsd-extended] run Database/$test"
    "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
      -D_POSIX_C_SOURCE=200809L ${extra} "$test.c" \
      ../../../.build-bsd-extended/Database_Backend.o ${db_linkflags} \
      -o "../../../.build-bsd-extended/Database_${test}"
    "../../../.build-bsd-extended/Database_${test}"
  done
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} \
    MultiTU_A.c MultiTU_B.c MultiTU_Main.c \
    ../../../.build-bsd-extended/Database_Backend.o ${db_linkflags} \
    -o ../../../.build-bsd-extended/Database_MultiTU
  ../../../.build-bsd-extended/Database_MultiTU
)
record Database PASS 'SQLite single-thread profile; no parallel callers'

# Audio offline: strict zero-extra-link, multiple translation units.
stage=AudioManual
suite Audio Manual
echo "[bsd-extended] AUDIO_MANUAL_ZERO_LINK=PASS (compile+codec conversion; no device opened)"
record AudioManual PASS 'OSS/audio4 build and PCM conversion; hardware not opened'
stage=AudioOffline
suite Audio Synthesis OfflineStandalone OfflineCompat
echo "[bsd-extended] run Audio/OfflineMultiTU (zero-link)"
(
  cd Codespace/Tests/Audio
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    OfflineTU_A.c OfflineTU_B.c OfflineTU_Main.c \
    -o ../../../.build-bsd-extended/Audio_OfflineMultiTU
  ../../../.build-bsd-extended/Audio_OfflineMultiTU
)
echo "[bsd-extended] AUDIO_OFFLINE_ZERO_LINK=PASS"
record AudioOffline PASS 'synthesis only; no device backend'

# Audio runs in offline mode; no hardware access is claimed.
# Some BSD libc implementations do not export sin/exp without libm. A strict
# zero-link *probe* must be recorded before enabling a diagnostic workaround.
stage=Audio
audio_zero_link=PASS
audio_features=-D_POSIX_C_SOURCE=200809L
case "$(uname -s)" in
  DragonFly|FreeBSD) audio_features= ;;
  OpenBSD) audio_features="-D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE=1" ;;
esac
# Probe codec and desktop entrypoints independently. Diagnostic link flags
# are used only for the functional suite, never to label a backend zero-link.
codec_zero_link=PASS
device_zero_link=PASS
if (
  cd Codespace/Tests/Audio
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    ${audio_features} ${extra} CodecStandalone.c \
    -o ../../../.build-bsd-extended/Audio_CodecZeroLink 2>/dev/null
); then
  echo "[bsd-extended] AUDIO_CODEC_ZERO_LINK=PASS"
else
  echo "[bsd-extended] AUDIO_CODEC_ZERO_LINK=NOT_SUPPORTED on $(uname -s)"
  codec_zero_link=NOT_SUPPORTED
  linkflags="-lm -pthread"
fi
if (
  cd Codespace/Tests/Audio
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    ${audio_features} ${extra} DesktopStandalone.c \
    -o ../../../.build-bsd-extended/Audio_DeviceZeroLink 2>/dev/null
); then
  echo "[bsd-extended] AUDIO_DEVICE_ZERO_LINK=PASS"
else
  echo "[bsd-extended] AUDIO_DEVICE_ZERO_LINK=NOT_SUPPORTED on $(uname -s)"
  device_zero_link=NOT_SUPPORTED
  linkflags="-lm -pthread"
fi
if [ "$codec_zero_link" = PASS ] && [ "$device_zero_link" = PASS ]; then
  echo "[bsd-extended] AUDIO_ZERO_LINK=PASS"
else
  echo "[bsd-extended] AUDIO_ZERO_LINK=NOT_SUPPORTED"
  audio_zero_link=NOT_SUPPORTED
fi
suite Audio CodecStandalone DesktopStandalone Codec Player IncludeAll
multitu Audio
record AudioCodec "$codec_zero_link" 'WAV/MP3/FLAC offline decoding; no hardware'
record AudioDevice "$device_zero_link" 'public device API exercised without physical device'
record Audio "$audio_zero_link" 'Miniaudio desktop integration; no physical device'
linkflags=

# Only noninteractive graphics, canvas, text UI and font regression sources.
stage=Terminal
suite Terminal IncludeAll Rasterization Sprite Curves ProjectionSprites \
  Raycasting Pseudo3DRoad ProjectionCore Canvas UI UIAdvanced UIGameWidgets \
  UIOverlays Font FIGlet FontCosmeron FontCosmeronBold FontNibbleBits
record Terminal PASS 'noninteractive rendering, no real TTY'
stage=Complete

echo "[bsd-extended] all noninteractive integration suites passed"
