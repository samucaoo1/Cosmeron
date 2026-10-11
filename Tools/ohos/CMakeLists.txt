cmake_minimum_required(VERSION 3.16)
project(CosmeronOHOSPortability LANGUAGES C)

# This intentionally compiles objects only. A successful cross-compile does NOT
# claim runtime/device, permissions, graphics, or POSIX backend support.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "OHOS")
  message(FATAL_ERROR "Use the OpenHarmony native SDK's ohos.toolchain.cmake")
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
set_target_properties(cosmeron_ohos_c11_check PROPERTIES
  C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(cosmeron_ohos_c11_check PRIVATE
  "${COSMERON_ROOT}/Codespace")
target_compile_options(cosmeron_ohos_c11_check PRIVATE
  -Wall -Wextra -Wpedantic -Werror)

message(STATUS "OHOS ABI: ${OHOS_ARCH}")
message(STATUS "Cosmeron: compile-only portable C11 subset, runtime unverified")
