$ErrorActionPreference = "Stop"

function Invoke-CosmeronTest {
  param(
    [string]$Directory,
    [string]$Name,
    [string[]]$Sources,
    [string[]]$Libraries = @()
  )
  $root = (Resolve-Path $Directory).Path
  $build = Join-Path $root ".msvc"
  New-Item -ItemType Directory -Force -Path $build | Out-Null
  $exe = Join-Path $build "$Name.exe"
  Push-Location $root
  try {
    $args = @("/nologo", "/std:c11", "/W4", "/WX", "/D_CRT_SECURE_NO_WARNINGS")
    $args += $Sources
    $args += "/Fe:$exe"
    $args += $Libraries
    & cl @args
    if ($LASTEXITCODE -ne 0) { throw "cl failed: $Directory/$Name" }
    & $exe
    if ($LASTEXITCODE -ne 0) { throw "test failed: $Directory/$Name ($LASTEXITCODE)" }
  } finally {
    Pop-Location
  }
}

$core = @("Namespace","Cast","Preprocessor","Detect","Error","Memory","MemoryOperations","MemoryUtilities","Algorithm","IncludeAll")
foreach ($name in $core) {
  Invoke-CosmeronTest "Codespace/Tests/Core" $name @("$name.c")
}

foreach ($pair in @(
  @("bit","Bit.c"), @("widths","Widths.c"), @("include-all","IncludeAll.c")
)) { Invoke-CosmeronTest "Codespace/Tests/Bit" $pair[0] @($pair[1]) }

foreach ($pair in @(
  @("chronometry","Chronometry.c"), @("duration","Duration.c"), @("instant","Instant.c"), @("calendar","Calendar.c"), @("datetime","DateTime.c"), @("epoch-timezone-format","EpochTimezoneFormat.c"), @("clock-timer","ClockTimer.c"), @("include-all","IncludeAll.c")
)) { Invoke-CosmeronTest "Codespace/Tests/Chronometry" $pair[0] @($pair[1]) }

foreach ($pair in @(
  @("functional","Functional.c"),
  @("linear-extra","LinearExtra.c"),
  @("linear-all-types","LinearAllTypes.c"),
  @("tree-all-functions","TreeAllFunctions.c"),
  @("hash","Hash.c"),
  @("graph","Graph.c"),
  @("off","FunctionTableOff.c"),
  @("custom-type","CustomType.c"),
  @("include-all","IncludeAll.c"),
  @("include-all-off","IncludeAllOff.c")
)) {
  Invoke-CosmeronTest "Codespace/Tests/Container" $pair[0] @($pair[1])
}
Invoke-CosmeronTest "Codespace/Tests/Container" "multi-tu" @("MultiTU_A.c","MultiTU_B.c","MultiTU_Main.c")

foreach ($pair in @(
  @("math","Math.c"), @("arithmetic","Arithmetic.c"), @("value","Value.c"), @("equations","Equations.c"), @("all-types","AllTypes.c"), @("include-all","IncludeAll.c")
)) { Invoke-CosmeronTest "Codespace/Tests/Math" $pair[0] @($pair[1]) }

foreach ($pair in @(
  @("random","Random.c"), @("engines","Engines.c"), @("distribution","Distribution.c"), @("include-all","IncludeAll.c")
)) { Invoke-CosmeronTest "Codespace/Tests/Random" $pair[0] @($pair[1]) }

foreach ($pair in @(
  @("struct","Struct.c"), @("variants","Variants.c"), @("all-types","AllTypes.c"), @("include-all","IncludeAll.c")
)) { Invoke-CosmeronTest "Codespace/Tests/Struct" $pair[0] @($pair[1]) }

foreach ($pair in @(
  @("include-all","IncludeAll.c"), @("text","Text.c"), @("namespace","Namespace.c"), @("unicode","Unicode.c"), @("all-functions","AllFunctions.c")
)) {
  Invoke-CosmeronTest "Codespace/Tests/Text" $pair[0] @($pair[1])
}

foreach ($pair in @(
  @("type","Type.c"), @("type-no-function-table","TypeNoFunctionTable.c"), @("all-functions","AllFunctions.c"), @("include-all","IncludeAll.c")
)) { Invoke-CosmeronTest "Codespace/Tests/Type" $pair[0] @($pair[1]) }

$concurrency = @{
  "include-all"="IncludeAll.c"; "module-surface"="ModuleSurface.c";
  "atomic"="Atomic.c"; "mutex"="Mutex.c"; "once"="Once.c"; "rwlock"="RWLock.c";
  "semaphore"="Semaphore.c"; "thread"="Thread.c"; "coroutine"="Coroutine.c";
  "condition"="Condition.c"; "barrier"="Barrier.c"; "contention"="Contention.c";
  "producer-consumer"="ProducerConsumer.c"; "thread-pool"="ThreadPool.c";
  "thread-pool-multi-producer"="ThreadPoolMultiProducer.c";
  "thread-pool-shutdown"="ThreadPoolShutdown.c"; "future"="Future.c"
}
foreach ($name in $concurrency.Keys) {
  Invoke-CosmeronTest "Codespace/Tests/Concurrency" $name @($concurrency[$name])
}

$network = @{
  "include-all"="IncludeAll.c"; "smoke"="Smoke.c"; "tcp"="TCP.c"; "udp"="UDP.c";
  "poller"="Poller.c"; "resolve"="Resolve.c"; "nonblocking"="NonBlocking.c";
  "poller-close"="PollerClose.c"; "udp-clear-peer"="UDPClearPeer.c"; "ipv6"="IPv6.c";
  "lifecycle"="Lifecycle.c"; "socket-recreate"="SocketRecreate.c";
  "poller-empty"="PollerEmpty.c"; "connection-nonblocking"="ConnectionNonBlocking.c";
  "poller-descriptor-limit"="PollerDescriptorLimit.c"; "poller-timeout"="PollerTimeout.c";
  "connection-arguments"="ConnectionArguments.c"; "datagram-arguments"="DatagramArguments.c";
  "surface"="Surface.c"; "include-order"="IncludeOrder.c"
}
foreach ($name in $network.Keys) {
  Invoke-CosmeronTest "Codespace/Tests/Network" $name @($network[$name])
}
Invoke-CosmeronTest "Codespace/Tests/Network" "multi-tu" @("MultiTU_Main.c","MultiTU_A.c","MultiTU_B.c")

foreach ($name in @("Synthesis", "Codec", "Player", "IncludeAll")) {
  Invoke-CosmeronTest "Codespace/Tests/Audio" $name @("$name.c")
}
Invoke-CosmeronTest "Codespace/Tests/Audio" "multi-tu" @("MultiTU_A.c", "MultiTU_B.c", "MultiTU_Main.c")

foreach ($name in @("State", "Application", "HID", "ControllerTransport", "BSDHID", "MacHID", "NativeControllers", "Namespace", "Linux", "Windows", "MacOS", "SDLControllers", "IncludeAll")) {
  Invoke-CosmeronTest "Codespace/Tests/Input" $name @("$name.c")
}
Invoke-CosmeronTest "Codespace/Tests/Input" "multi-tu" @("MultiTU_A.c", "MultiTU_B.c", "MultiTU_Main.c")

foreach ($name in @("Terminal", "Canvas", "UI", "UIAdvanced", "UIGameWidgets", "IncludeAll")) {
  Invoke-CosmeronTest "Codespace/Tests/Terminal" $name @("$name.c")
}
