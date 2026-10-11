#!/bin/sh
# Extended, *executed* BSD regression matrix. Never adds -lm/-pthread/-lsqlite3.
# Runs in the native vmactions guest, with no simulated platform guards.
set -eu
CC=${CC:-cc}
case "$(uname -s)" in
  NetBSD) extra=-D_NETBSD_SOURCE=1 ;;
  *) extra= ;;
esac
mkdir -p .build-bsd-extended
linkflags=

suite() {
  module=$1; shift
  for test in "$@"; do
    echo "[bsd-extended] run $module/$test"
    (
      cd "Codespace/Tests/$module"
      "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
        -D_POSIX_C_SOURCE=200809L ${extra} "$test.c" ${linkflags} \
        -o "../../../.build-bsd-extended/${module}_${test}"
      "../../../.build-bsd-extended/${module}_${test}"
    )
  done
}
multitu() {
  module=$1
  echo "[bsd-extended] run $module/MultiTU"
  (
    cd "Codespace/Tests/$module"
    "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
      -D_POSIX_C_SOURCE=200809L ${extra} \
      MultiTU_A.c MultiTU_B.c MultiTU_Main.c \
      -o "../../../.build-bsd-extended/${module}_MultiTU"
    "../../../.build-bsd-extended/${module}_MultiTU"
  )
}

# Local loopback only; no Internet is required.
suite Network TCP UDP Poller NonBlocking PollerClose UDPClearPeer IPv6 \
  Lifecycle SocketRecreate PollerEmpty ConnectionNonBlocking \
  PollerDescriptorLimit PollerTimeout ConnectionArguments \
  DatagramArguments Surface IncludeOrder
multitu Network

# Thread primitives, synchronisation, tasks and thread pool, no explicit -pthread.
# Probe the public zero-extra-link contract *before* running the thread tests.
# The CI may use -pthread solely to diagnose the backend on systems with
# separately linked libpthread; it MUST report that this does not meet the
# Cosmeron zero-link policy for consumers.
if (
  cd Codespace/Tests/Concurrency
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} ModuleSurface.c \
    -o ../../../.build-bsd-extended/Concurrency_ZeroLink
); then
  echo "[bsd-extended] THREAD_ZERO_LINK=PASS"
else
  echo "[bsd-extended] THREAD_ZERO_LINK=NOT_SUPPORTED on $(uname -s) (requires native libpthread)"
  linkflags=-pthread
fi
suite Concurrency IncludeAll ModuleSurface Mutex Once RWLock Semaphore \
  Thread Coroutine Condition Barrier Contention ProducerConsumer \
  ThreadPool ThreadPoolMultiProducer ThreadPoolShutdown Future
linkflags=

# Miniz codec/ZIP and SQLite in the single-implementation translation unit.
suite Compression Codec Zip File Namespace FunctionTableOff Robustness Allocation
multitu Compression
echo "[bsd-extended] compile+run Database (one shared embedded SQLite object)"
(
  cd Codespace/Tests/Database
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} -c Backend.c \
    -o ../../../.build-bsd-extended/Database_Backend.o
  for test in IncludeAll Database Persistence Namespace; do
    echo "[bsd-extended] run Database/$test"
    "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
      -D_POSIX_C_SOURCE=200809L ${extra} "$test.c" \
      ../../../.build-bsd-extended/Database_Backend.o \
      -o "../../../.build-bsd-extended/Database_${test}"
    "../../../.build-bsd-extended/Database_${test}"
  done
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    -D_POSIX_C_SOURCE=200809L ${extra} \
    MultiTU_A.c MultiTU_B.c MultiTU_Main.c \
    ../../../.build-bsd-extended/Database_Backend.o \
    -o ../../../.build-bsd-extended/Database_MultiTU
  ../../../.build-bsd-extended/Database_MultiTU
)

# Audio runs in offline mode; no hardware access is claimed.
suite Audio Synthesis Codec Player IncludeAll
multitu Audio

# Only noninteractive graphics, canvas, text UI and font regression sources.
suite Terminal IncludeAll Rasterization Sprite Curves ProjectionSprites \
  Raycasting Pseudo3DRoad ProjectionCore Canvas UI UIAdvanced UIGameWidgets \
  UIOverlays Font FIGlet FontCosmeron FontCosmeronBold FontNibbleBits

echo "[bsd-extended] all noninteractive integration suites passed"
