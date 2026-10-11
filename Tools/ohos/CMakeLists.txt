cmake_minimum_required(VERSION 3.16)
project(CosmeronOHOSPortability LANGUAGES C)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "OHOS")
  message(FATAL_ERROR "OpenHarmony native ohos.toolchain.cmake is required")
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
add_executable(cosmeron_ohos_multi_tu
  MultiTU_A.c MultiTU_B.c MultiTU_Main.c)
foreach(target cosmeron_ohos_c11_check cosmeron_ohos_multi_tu)
  set_target_properties(${target} PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
  target_include_directories(${target} PRIVATE "${COSMERON_ROOT}/Codespace")
  # The official SDK injects --gcc-toolchain into Clang, although Clang's\n  # native cross compiler does not consume that option. Suppress only this\n  # toolchain noise; preserve -Werror for actual Cosmeron diagnostics.\n  target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror -Qunused-arguments)
endforeach()
message(STATUS "OHOS target architecture: ${OHOS_ARCH}; compile and link only, no device")
