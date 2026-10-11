cmake_minimum_required(VERSION 3.16)
project(CosmeronOHOSPortability LANGUAGES C)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "OHOS")
  message(FATAL_ERROR "Use the OpenHarmony native SDK ohos.toolchain.cmake")
endif()

set(COSMERON_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
set(COSMERON_TARGETS
  "${COSMERON_ROOT}/Codespace/Tests/Core/Namespace.c"
  "${COSMERON_ROOT}/Codespace/Tests/Core/Preprocessor.c"
  "${COSMERON_ROOT}/Codespace/Tests/Core/Algorithm.c"
  "${COSMERON_ROOT}/Codespace/Tests/Bit/IncludeAll.c"
  "${COSMERON_ROOT}/Codespace/Tests/Struct/IncludeAll.c"
  "${COSMERON_ROOT}/Codespace/Tests/Type/IncludeAll.c"
  "${COSMERON_ROOT}/Codespace/Tests/Math/IncludeAll.c"
  "${COSMERON_ROOT}/Codespace/Tests/Container/IncludeAll.c"
  "${COSMERON_ROOT}/Codespace/Tests/Sort.c"
  "${COSMERON_ROOT}/Codespace/Tests/SortCatalog.c"
)
add_library(cosmeron_ohos_c11_check OBJECT ${COSMERON_TARGETS})
add_executable(cosmeron_ohos_multi_tu MultiTU_A.c MultiTU_B.c MultiTU_Main.c)

# Expand the compile-only probe to the SDK's POSIX and device API surface.
# Object libraries do not prove link availability, permissions or runtime.
option(COSMERON_OHOS_SYSTEM_PROBES "Compile OS-backed Cosmeron headers" ON)
set(COSMERON_OHOS_CHECK_TARGETS
  cosmeron_ohos_c11_check
  cosmeron_ohos_multi_tu
)
if(COSMERON_OHOS_SYSTEM_PROBES)
  set(COSMERON_OS_SOURCES
    "${COSMERON_ROOT}/Codespace/Tests/Random/IncludeAll.c"
    "${COSMERON_ROOT}/Codespace/Tests/Chronometry/IncludeAll.c"
    "${COSMERON_ROOT}/Codespace/Tests/File/IncludeAll.c"
    "${COSMERON_ROOT}/Codespace/Tests/Concurrency/IncludeAll.c"
    "${COSMERON_ROOT}/Codespace/Tests/Network/IncludeAll.c"
    "${COSMERON_ROOT}/Codespace/Tests/Input/IncludeAll.c"
    "${COSMERON_ROOT}/Codespace/Tests/Terminal/IncludeAll.c"
  )
  add_library(cosmeron_ohos_system_headers OBJECT ${COSMERON_OS_SOURCES})
  # Expose the POSIX clock/socket declarations in OHOS libc headers.
  target_compile_definitions(cosmeron_ohos_system_headers PRIVATE
    _POSIX_C_SOURCE=200809L)
  list(APPEND COSMERON_OHOS_CHECK_TARGETS cosmeron_ohos_system_headers)
endif()

foreach(target IN LISTS COSMERON_OHOS_CHECK_TARGETS)
  set_target_properties(${target} PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
  target_include_directories(${target} PRIVATE "${COSMERON_ROOT}/Codespace")
  # OHOS SDK injects --gcc-toolchain, which is unused by its native Clang.
  # Ignore only unused driver flags; retain -Werror for source diagnostics.
  # IncludeAll/test objects contain assertions: never optimize their
  # dependent variables away with NDEBUG, even in a release-configured SDK.
  target_compile_options(${target} PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Qunused-arguments -UNDEBUG)
endforeach()

message(STATUS "OHOS target ABI: ${OHOS_ARCH}")
message(STATUS "System header probes: ${COSMERON_OHOS_SYSTEM_PROBES}")
message(STATUS "Compile/link verification only; runtime and device permissions unverified")
